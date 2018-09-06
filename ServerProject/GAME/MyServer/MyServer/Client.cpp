#include "Client.h"

Client::Client()
{
	L = NULL;
	logining = false;
}
Client::~Client()
{
	if (L != NULL)
		lua_close(L);
}

void Client::Init()
{
	aggres.reserve(10);
	tar = -1;
	plueHp = 0;
	for (int i = 0; i < 3; ++i)
		SlotEM[i] = 16;
	for (int i = 0; i < 16; ++i)
	{
		Slot[i] = 0;
		SlotCount[i] = 0;
	}
	RespawnX = 10;
	RespawnY = 10;
	nowstate = NORMAL;
	Data.x = 10;
	Data.y = 10;
	Data.Exp = 0;
	Data.Hp = 100;
	Data.money = 0;
	Data.id = 0;
	Data.Lev = 1;
	strcpy(Data.Name, "");
	m_prev_packet_size = 0;
	m_packet_size = 0;
	m_over.overEvent = RECV;
	ZeroMemory(&m_over.m_over, sizeof(WSAOVERLAPPED));
	m_over.m_wsabuf.buf = m_over.m_iobuf;
	m_over.m_wsabuf.len = sizeof(m_over.m_wsabuf.buf);
	m_isconnected = false;
	m_islife = false;
	m_isactive = false;
	m_viewLock.lock();
	m_viewList.clear();
	m_viewLock.unlock();
}
void Client::ClientClose()
{
	tar = -1;
	logining = false;
	strcpy(Data.Name, "");
	nowstate = NORMAL;

	m_isconnected = false;
	m_islife = false;
	m_isactive = false;
	m_viewLock.lock();
	m_viewList.clear();
	m_viewLock.unlock();
	aggresLock.lock();
	aggres.clear();
	aggresLock.unlock();
	closesocket(m_s);
}
void Client::StartRecv()
{
	unsigned long r_flag = 0;
	ZeroMemory(&m_over.m_over, sizeof(WSAOVERLAPPED));
	int ret = WSARecv(m_s, &m_over.m_wsabuf, 1,
		NULL, &r_flag, &m_over.m_over, NULL);
	if (0 != ret) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no) ERRORLOG::error_display("Recv Error", err_no);
	}
}
char* Client::RecvPacket(int io_size, char* ptr)
{
	int work_size = io_size;
	char *wptr = ptr;

	while (0 < work_size) {
		int p_size;
		if (0 != m_packet_size)
			p_size = m_packet_size;
		else {
			p_size = wptr[0];
			m_packet_size = p_size;
		}
		int need_size = p_size - m_prev_packet_size;
		if (need_size <= work_size) {
			memcpy(m_packet
				+ m_prev_packet_size, wptr, need_size);
			m_prev_packet_size = 0;
			m_packet_size = 0;
			work_size -= need_size;
			wptr += need_size;

			return m_packet;
		
		}
		else {
			memcpy(m_packet + m_prev_packet_size, wptr, work_size);
			m_prev_packet_size += work_size;
			wptr += work_size;
			work_size = 0;

		}
	}
	return NULL;
}