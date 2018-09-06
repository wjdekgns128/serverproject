#include <WinSock2.h>
#include <winsock.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <queue>
#include <array>
#include <unordered_map>
#include "Network.h"
#include "protocol.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace chrono;
const static int MAX_TEST = MAX_USER;
HANDLE g_hiocp;
bool a = true;

enum OPTYPE { OP_SEND, OP_RECV };
high_resolution_clock::time_point last_connect_time;
struct OverlappedEx {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
	int event_target;
};

struct CLIENT {
	int state;
	int type;
	int id;
	int x;
	int y;
	high_resolution_clock::time_point last_move_time;
	high_resolution_clock::time_point last_attack_time;
	high_resolution_clock::time_point last_buy_time;


	bool connect;
	bool login;
	SOCKET client_socket;
	OverlappedEx recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packet_data;
	int curr_packet_size;
};

array<CLIENT, NUM_OF_NPC> g_clients;
atomic_int num_connections;

vector <thread *> worker_threads;
thread test_thread;
void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1])
	{
	case SC_ATTACK_OK:
	{
		//printf("%d번째 공격\n", ci);
	}
		break;
	case SC_ITEM_BUYOK:
		//printf("%d번째 BUY 성공\n", ci);
		break;
	case SC_ITEM_BUYFAIL:
		//printf("%d번째 BUY 실패\n", ci);
		break;
	case SC_LOGIN_OK:
	{
		g_clients[ci].login = true;
		sc_packet_loginok * my_packet = (sc_packet_loginok*)packet;
		g_clients[ci].x = my_packet->x;
		g_clients[ci].y = my_packet->y;
		if(g_clients[ci].id == -1)
		g_clients[ci].id = my_packet->id;
		break;
	}

	case SC_POS:
	{
		sc_packet_pos * my_packet = (sc_packet_pos*)packet;
		if (my_packet->id < 10000)
		{
			if (g_clients[ci].id == my_packet->id)
			{
				g_clients[ci].x = my_packet->x;
				g_clients[ci].y = my_packet->y;
			}
		}
		else
		{
			if (g_clients[my_packet->id - 10000].id == my_packet->id)
			{
				g_clients[my_packet->id - 10000].x = my_packet->x;
				g_clients[my_packet->id - 10000].y = my_packet->y;
				g_clients[my_packet->id - 10000].connect = true;

			}
		}
		break;
	}
	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player * my_packet = (sc_packet_remove_player*)packet;
		if (my_packet->id < 10000)
		{
			if (my_packet->id == g_clients[ci].id)
			{
				g_clients[ci].state = 2;
				printf("%d 사망함\n", my_packet->id);
			}
		}
		else
		{
			if (g_clients[my_packet->id - 10000].id == my_packet->id)
			{
				if (g_clients[my_packet->id - 10000].connect)
				{
					g_clients[my_packet->id - 10000].connect = false;
				}
			}
		}
	}
		break;
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player * my_packet = (sc_packet_put_player*)packet;
		if (my_packet->id < 10000)
		{
			if (my_packet->id == g_clients[ci].id)
			{
				g_clients[ci].x = my_packet->x;
				g_clients[ci].y = my_packet->y;

				g_clients[ci].state = 0;
				printf("%d 부활함\n", my_packet->id);

			}
		}
		else
		{
			if (g_clients[my_packet->id - 10000].id == my_packet->id)
			{
				if (g_clients[my_packet->id - 10000].connect == false)
				{

					g_clients[my_packet->id - 10000].connect = true;
				}
			}
		}
		break;

	}
	}
}
void DisconnectClient(int ci)
{
	closesocket(g_clients[ci].client_socket);
	g_clients[ci].connect = false;
	g_clients[ci].login = false;
	cout << "Client [" << ci << "] Disconnected!\n";
}
void error_display(const char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;
	LocalFree(lpMsgBuf);
	//while (true);
}

void SendPacket(int cl, void *packet)
{
	int psize = reinterpret_cast<unsigned char *>(packet)[0];
	int ptype = reinterpret_cast<unsigned char *>(packet)[1];
	OverlappedEx *over = new OverlappedEx;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0,
		&over->over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
			error_display("Error in SendPacket:", err_no);
	}
	//std::cout << "Send Packet [" << ptype << "] To Client : " << cl << std::endl;
}
void LoginOut(int c1)
{
	if (g_clients[c1].login == false)
		return;
	cs_packet_logout p;
	p.size = sizeof(cs_packet_logout);
	p.type = CS_LOGINOUT;

	SendPacket(c1, &p);
}
void Adjust_Number_Of_Client()
{
	if (num_connections >= MAX_TEST) return;
	if (high_resolution_clock::now() < last_connect_time + 200ms) return;
	last_connect_time = high_resolution_clock::now();
	g_clients[num_connections].client_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int Result = WSAConnect(g_clients[num_connections].client_socket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	g_clients[num_connections].curr_packet_size = 0;
	g_clients[num_connections].prev_packet_data = 0;
	ZeroMemory(&g_clients[num_connections].recv_over, sizeof(g_clients[num_connections].recv_over));
	g_clients[num_connections].recv_over.event_type = OP_RECV;
	g_clients[num_connections].recv_over.wsabuf.buf =
		reinterpret_cast<CHAR *>(g_clients[num_connections].recv_over.IOCP_buf);
	g_clients[num_connections].recv_over.wsabuf.len = sizeof(g_clients[num_connections].recv_over.IOCP_buf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(g_clients[num_connections].client_socket), g_hiocp, num_connections, 0);
	WSARecv(g_clients[num_connections].client_socket, &g_clients[num_connections].recv_over.wsabuf, 1,
		NULL, &recv_flag, &g_clients[num_connections].recv_over.over, NULL);
	g_clients[num_connections].connect = true;
	char c[256] = { 0, };
	cs_packet_login p;
	p.size = sizeof(cs_packet_login);
	p.type = CS_LOGIN;\
	ZeroMemory(p.Userid, MAX_STR_SIZE);
	itoa(num_connections, c, 10);
	int len = strlen(c);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)c, -1, p.Userid, len);
	p.Userid[len] = NULL;
	SendPacket(num_connections,&p);
	num_connections++;
}
void Test_Thread()
{
	srand(time(NULL));
	while (a) {
		Sleep(10);
		Adjust_Number_Of_Client();
		for (int i = 0; i < num_connections; ++i)
		{
			if (false == g_clients[i].connect) continue;

			if (g_clients[i].login == false) continue;
			if (g_clients[i].state == 2) continue;
			

			int random = rand() % 1000;
			if (random < 770)
			{
				if (g_clients[i].last_move_time + 0.3s > high_resolution_clock::now()) continue;
				g_clients[i].last_move_time = high_resolution_clock::now();

				cs_packet_move p;
				p.size = sizeof(cs_packet_move);
				p.type = CS_MOVE;
				p.dir = rand() % 4;
				SendPacket(i, &p);

			}
			else if(random < 950 )
			{
				if (g_clients[i].last_attack_time + 0.5s > high_resolution_clock::now()) continue;
				g_clients[i].last_attack_time = high_resolution_clock::now();
				cs_packet_attack p;
				p.size = sizeof(cs_packet_attack);
				p.type = CS_ATTACK;
				SendPacket(i, &p);


			}
			else
			{
				if (g_clients[i].last_buy_time + 15s > high_resolution_clock::now()) continue;
				g_clients[i].last_buy_time = high_resolution_clock::now();
				int ra = rand() % 15;
				cs_packet_buyitem p;
				p.size = sizeof(cs_packet_buyitem);
				p.type = CS_ITEM_BUY;
				p.itemIndex = ra;
				SendPacket(i, &p);


			}
		}
		
	}
	
}

void Worker_Thread()
{
	while (a) {
		DWORD io_size;
		unsigned long long ci;
		OverlappedEx *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, &ci,
			reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);
		// std::cout << "GQCS :";
		if (FALSE == ret) {
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(ci);
			else error_display("GQCS : ", WSAGetLastError());
		}
		if (0 == io_size) {
			DisconnectClient(ci);
			continue;
		}
		if (OP_RECV == over->event_type) {
			
			unsigned char *buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned psize = g_clients[ci].curr_packet_size;
			unsigned pr_size = g_clients[ci].prev_packet_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 지금 패킷 완성 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(static_cast<int>(ci), packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;
				}
			}
			g_clients[ci].curr_packet_size = psize;
			g_clients[ci].prev_packet_data = pr_size;
			DWORD recv_flag = 0;
			WSARecv(g_clients[ci].client_socket,
				&g_clients[ci].recv_over.wsabuf, 1,
				NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
		}
		else if (OP_SEND == over->event_type) {
			if (io_size != over->wsabuf.len) {
				std::cout << "Send Incomplete Error!\n";
				closesocket(g_clients[ci].client_socket);
				g_clients[ci].connect = false;
			}
			delete over;
		}
		else {
			std::cout << "Unknown GQCS event!\n";
			while (true);
		}
	}
}
void ShutdownNetwork()
{
	for (int i = 0; i < num_connections; ++i)
		LoginOut(i);
	a = false;
	test_thread.join();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
}

void InitializeNetwork()
{
	for (int i = 0; i < MAX_USER; ++i) {
		g_clients[i].connect = false;
		g_clients[i].login = false;
		g_clients[i].id = -1;
		g_clients[i].state = 0;
		g_clients[i].type = 1;
	}

	ifstream fp("res/MonsterNPC.txt");
	int c = NPC_START;
	int x = 0;
	int y = 0;
	int type = 0;
	while (fp >> x >> y >> type)
	{
		g_clients[c].x = x;
		g_clients[c].y = y;
		g_clients[c].id = c + 10000;
		g_clients[c].state = 0;
		g_clients[c].login = true;

		g_clients[c].type = type;

		c++;

	}
	fp.close();
	num_connections = 0;
	last_connect_time = high_resolution_clock::now();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	for (int i = 0; i < 4; ++i)
		worker_threads.push_back(new std::thread{ Worker_Thread });

	test_thread = thread{ Test_Thread };

}
void getpos(int id, int* x, int * y,int* type)
{
	*x = g_clients[id].x;
	*y = g_clients[id].y;
	*type = g_clients[id].type;
	if (g_clients[id].login == false || g_clients[id].state == 2  || g_clients[id].connect == false)
		*x = -10000;
}