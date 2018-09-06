#pragma once
#include "Server.h"
class IOCPServer : public Server
{
public:
	HANDLE g_hIocp;
public:
	IOCPServer();
	~IOCPServer();

public:
	void StartServer();
	void EndServer();

public:
	
	void CloseSocket(int id);

	static void WorkThread(IOCPServer* server);
	static void AcceotThread(IOCPServer* server);
	static void TimerThread(IOCPServer* server);

};
