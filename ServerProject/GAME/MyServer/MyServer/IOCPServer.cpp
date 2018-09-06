#include "IOCPServer.h"
#include "ClientManager.h"
#include "Process.h"
#include "NPCManager.h"
IOCPServer::IOCPServer()
{
}

IOCPServer::~IOCPServer()
{
}

void IOCPServer::StartServer()
{
	printf("StartServer : \n");
	g_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); // 의미없는 파라메터, 마지막은 알아서 쓰레드를 만들어준다.
	CreateWord();
	CreateSocket();
	Bind_listen();

	printf("Thread : \n");
	thread t_th{ TimerThread ,this };

	vector<thread> Wthreads;

	for (int i = 0; i < 4; ++i)
		Wthreads.push_back(thread{ WorkThread,this });
	thread a_thread{ AcceotThread,this };
	for (auto& p : Wthreads)
		p.join();
	a_thread.join();
	t_th.join();

}
void IOCPServer::CloseSocket(int id)
{
	
}
void IOCPServer::EndServer()
{
	CloseServer();
	CloseHandle(g_hIocp);
}
void IOCPServer::WorkThread(IOCPServer* server)
{
	NPCManager& npc = NPCManager::getInstance();
	IOCPServer* iocp = server;
	ClientManager& p = ClientManager::getInstance();
	Process process;
	while (1)
	{
		unsigned long io_size;
		unsigned long long iocp_key; // 64 비트 integer , 우리가 64비트로 컴파일해서 64비트
		WSAOVERLAPPED *over;
		BOOL ret = GetQueuedCompletionStatus(iocp->g_hIocp, &io_size, &iocp_key, &over, INFINITE);
		int key = static_cast<int>(iocp_key);
		//cout << "WT::Network I/O with Client [" << key << "]\n";
		if (0 == io_size) {
			p.ClientClose(key);
			continue;
		}
		if (FALSE == ret) {
			cout << "Error in GQCS\n";
			p.ClientClose(key);

			continue;
		}
		EXOVER *p_over = reinterpret_cast<EXOVER *>(over);
		if (EXOVER_EVENT::RECV == p_over->overEvent) {
			//cout << "WT:Packet From Client [" << key << "]\n";
			
			int work_size = io_size;
			char *wptr = p_over->m_iobuf;
			char* RecvPacketData = p.RecvPacket(key, work_size, wptr);
			if (RecvPacketData != NULL)
				process.ProcessPacket(key, RecvPacketData);
			p.StartRecv(key);
		}
		else if (EXOVER_EVENT::MONSTER_ATTACK == p_over->overEvent)
		{
			npc.MonsterAttack(key, p_over->ohterid);
			delete p_over;
		}
		else if (EXOVER_EVENT::SEND == p_over->overEvent) {  // Send 후처리
															 //cout << "WT:A packet was sent to Client[" << key << "]\n";
			delete p_over;
		}
		else
		{
			npc.Update(key, p_over->overEvent);
			delete p_over;
		}
	}
}
void IOCPServer::AcceotThread(IOCPServer* server)
{
	IOCPServer* iocp = server;
	ClientManager& p = ClientManager::getInstance();

	while (1)
	{
		SOCKADDR_IN c_addr;
		ZeroMemory(&c_addr, sizeof(SOCKADDR_IN));
		c_addr.sin_family = AF_INET;
		c_addr.sin_port = htons(MY_SERVER_PORT);
		c_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.
		int addr_size = sizeof(sockaddr);
		SOCKET cs = WSAAccept(iocp->serverS, reinterpret_cast<sockaddr *>(&c_addr), &addr_size, NULL, NULL);
		if (INVALID_SOCKET == cs) {
			ERRORLOG::ErrorDisplay("In Accept Thread:WSAAccept()");
			continue;
		}

		cout << "New Client Connected!\n";
		int newid = p.getAcceptId();
		if (newid == -1)
		{
			closesocket(cs);
			cout << "MAX USER Exceeded\n";
			continue;
		}
		cout << "ID of new Client is [" << newid << "]";
		p.AcceptSetting(newid, cs);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(cs), iocp->g_hIocp, newid, 0);
		p.StartRecv(newid);
	}	
}
void IOCPServer::TimerThread(IOCPServer* server)
{
	NPCManager& npc = NPCManager::getInstance();
	IOCPServer* iocp = server;
	while (true)
	{
		this_thread::sleep_for(10us);
		while (false == npc.timerq.empty())
		{
			if (npc.timerq.top().executeTime > high_resolution_clock::now())
				break;
			TimerEvent it = npc.timerq.top();
			npc.timerq.pop();
			EXOVER *ex = new EXOVER;
			ex->ohterid = it.aid;
			if (it.timerEvent == TIMEEVENTLIST::TIMER_NPC_MOVE)
			{
				ex->overEvent = EXOVER_EVENT::NPC_MOVE;
			}
			else if (it.timerEvent == TIMEEVENTLIST::TIMER_CLIENT_RESPAWN)
			{
				ex->overEvent = EXOVER_EVENT::RESPAWN_CLIENT;
			}
			else if (it.timerEvent == TIMEEVENTLIST::TIMER_MONSTER_ATTACK)
			{
				ex->overEvent = EXOVER_EVENT::MONSTER_ATTACK;
			}
			else if (it.timerEvent == TIMEEVENTLIST::TIMER_HERO_HPUP)
			{
				ex->overEvent = EXOVER_EVENT::HERO_HPUP;
			}
			else if (it.timerEvent == TIMEEVENTLIST::TIMER_MONSTER_RETURN)
			{
				ex->overEvent = EXOVER_EVENT::MONSTER_RETURN;
			}
			PostQueuedCompletionStatus(iocp->g_hIocp, 1, it.id, &ex->m_over);
		}
	}
}