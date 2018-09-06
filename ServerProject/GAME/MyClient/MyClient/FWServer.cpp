#include "FWServer.h"
#include "Debug.h"
#include "GameData.h"
FWServer::FWServer()
{
	in_packet_size = 0;
	saved_packet_size = 0;
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUFSIZ;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUFSIZ;
	index = 0;
	sock = NULL;
}

FWServer::~FWServer()
{
	

}

bool FWServer::ServerInit()
{
	char c[150];
	printf("IP 입력(-1입력시 127.0.0.1):");
	scanf("%s", c);
	if (!strcmp(c, "-1"))
		strcpy(c, "127.0.0.1");
	WSAStartup(MAKEWORD(2, 2), &wsa);
	hEvent = WSACreateEvent();
	sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (sock == INVALID_SOCKET)
	{
		printf("socket error\n");
		printf("sleep 100 -> exit\n");
		Sleep(100);
		return false;
	}
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(c);
	serveraddr.sin_port = htons(MY_SERVER_PORT);

	if (WSAConnect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		printf("CONNECT ERROR\n");
		printf("sleep 100 -> exit\n");
		Sleep(100);
		return false;
	}
	if (WSAEventSelect(sock, hEvent, FD_READ | FD_CLOSE) == SOCKET_ERROR)
	{
		printf("WSAEventSelect ERROR\n");
		printf("sleep 100 -> exit\n");
		Sleep(100);
		return false;
	}

	char c1[256] = { 0, };
	printf("접속...\n사용할 아이디 입력해주세요 : (20자 미만, 영문+숫자만 가능\n");
	while (1)
	{
		int a = 0;
		scanf(" %s", c1);
		if (strlen(c1) > 20)
		{
			a = 1;
		}
		if (a == 0)
		{
			for (int i = 0; i < strlen(c1); ++i)
			{
				if (!((c1[i] >= 'A' && c1[i] <= 'Z') || (c1[i] >= 'a' && c1[i] <= 'z') || (c1[i] >= '0' && c1[i] <= '9')))
				{
					a = 1;
					break;
				}
			}
		}
		
		if (a == 1)
			printf("다시입력해주세요 : ");
		else
			break;

	}
	cs_packet_login p;
	p.size = sizeof(cs_packet_login);
	p.type = CS_LOGIN;
	ZeroMemory(p.Userid, MAX_STR_SIZE);
	int len = strlen(c1);
	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)c1, -1, p.Userid, len);
	p.Userid[len] = NULL;
	SendPacket(&p);
	strcpy(GameData::GetInstance()->name, c1);
	strcpy(MyName, c1);
	return true;
}
bool FWServer::ServerRun()
{
	index = WSAWaitForMultipleEvents(1, &hEvent, FALSE, WSA_INFINITE, FALSE);

	if (((index == WSA_WAIT_FAILED) || (WSA_WAIT_TIMEOUT == index)))
		return true;
	else
	{
		WSAEnumNetworkEvents(sock, hEvent, &netEvent);
		if (netEvent.lNetworkEvents & FD_READ)
		{
			if (netEvent.iErrorCode[FD_READ_BIT] != 0)
			{
				Debug::Log("Error\n");
				return true;
			}
			DWORD iobyte, ioflag = 0;
			int ret = WSARecv(sock, &recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
			if (ret) {
				int err_code = WSAGetLastError();
				printf("Recv Error [%d]\n", err_code);
			}
			BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

			while (0 != iobyte) {
				if (0 == in_packet_size) in_packet_size = ptr[0];
				if (iobyte + saved_packet_size >= in_packet_size) {
					memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
					FWMain::GetInstance()->getNowScene()->ServerRecvPacket(packet_buffer);
					ptr += in_packet_size - saved_packet_size;
					iobyte -= in_packet_size - saved_packet_size;
					in_packet_size = 0;
					saved_packet_size = 0;
				}
				else {
					memcpy(packet_buffer + saved_packet_size, ptr, iobyte);
					saved_packet_size += iobyte;
					iobyte = 0;
				}
			}
		}
		if (netEvent.lNetworkEvents & FD_CLOSE)
		{
			Debug::Log("FD_CLOSE\n");
			Debug::Log("sleep 100 -> exit\n");
			Sleep(100);
			return false;
		}
	}
	return true;

}
void FWServer::ServerClose()
{
	WSACloseEvent(hEvent);
	WSACleanup();
	if (sock != NULL)
		closesocket(sock);
}
void FWServer::SendPacket(void* ptr)
{
	char *packet = reinterpret_cast<char *>(ptr);
	memcpy(send_buffer, packet, packet[0]);
	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = send_buffer[0];
	DWORD iobyte;
	WSASend(sock, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
}