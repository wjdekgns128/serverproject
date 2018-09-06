#pragma once
#include "MyInclude.h"
#include "FWMain.h"

class FWServer : public Singleton<FWServer>
{
public:
	SOCKET	sock;
	char	MyName[60];
private:
	int index;
	WSAEVENT hEvent;
	WSANETWORKEVENTS netEvent;
	WSADATA wsa;

	WSABUF	send_wsabuf;
	char 	send_buffer[BUFSIZ];
	WSABUF	recv_wsabuf;
	char	recv_buffer[BUFSIZ];

	char	packet_buffer[BUFSIZ];
	DWORD		in_packet_size;
	int		saved_packet_size;
public:
	FWServer();
	virtual ~FWServer();
public:
	bool ServerInit();
	void ServerClose();

	bool ServerRun();
	void SendPacket(void* ptr);
};