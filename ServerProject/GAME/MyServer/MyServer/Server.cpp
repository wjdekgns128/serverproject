#include "Server.h"
#include "protocol.h"
Server::Server()
{
	std::wcout.imbue(std::locale("korean"));
	ZeroMemory(&bind_addr, sizeof(SOCKADDR_IN));
}
Server::~Server()
{
	CloseServer();
}

void Server::CreateWord()
{
	WSAStartup(MAKEWORD(2, 2), &wsadata);
}

void Server::CreateSocket()
{
	serverS = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

void Server::Bind_listen()
{
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_port = htons(MY_SERVER_PORT);
	bind_addr.sin_addr.s_addr = INADDR_ANY;	// 0.0.0.0  아무대서나 오는 것을 다 받겠다.

	::bind(serverS, reinterpret_cast<sockaddr *>(&bind_addr), sizeof(bind_addr));
	listen(serverS, 1000);
}

void Server::CloseServer()
{
	WSACleanup();
	closesocket(serverS);
}
