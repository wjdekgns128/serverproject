#pragma once
#include "MyInclude.h"
#include "protocol.h"
class Server
{
public:
	SOCKET serverS;
	SOCKADDR_IN bind_addr;
	WSADATA wsadata;
public:
	Server();
	virtual ~Server();
protected:
	virtual void CreateWord();
	virtual void CreateSocket();
	virtual void Bind_listen();
	virtual void CloseServer();
};