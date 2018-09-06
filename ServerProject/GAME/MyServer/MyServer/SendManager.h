#pragma once
#include "Client.h"
#include "SingeTon.h"
#include "ErrorLog.h"
class SendManager
{
public:

	void SendPacket(const Client& client, void* ptr)
	{
		if (client.m_isconnected == false)
			return;
		char *packet = reinterpret_cast<char *>(ptr);
		EXOVER *s_over = new EXOVER;
		s_over->overEvent = EXOVER_EVENT::SEND;
		memcpy(s_over->m_iobuf, packet, packet[0]);
		s_over->m_wsabuf.buf = s_over->m_iobuf;
		s_over->m_wsabuf.len = s_over->m_iobuf[0];


		sc_packet_pos* p = reinterpret_cast<sc_packet_pos*>(ptr);
		ZeroMemory(&s_over->m_over, sizeof(WSAOVERLAPPED));
		int res = WSASend(client.m_s, &s_over->m_wsabuf, 1, NULL, 0,&s_over->m_over, NULL);
		if (0 != res) {
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no) ERRORLOG::error_display("Send Error! ", err_no);
		}
	}
	void SendPutObjectPacket(const Client& to, const Client& from) // 앞에꺼에게 뒤에있는 녀석을
	{
		sc_packet_put_player p;
		p.id = from.Data.id;
		p.size = sizeof(p);
		p.type = SC_PUT_PLAYER;
		p.x = from.Data.x;
		p.y = from.Data.y;
		p.objecttype = from.type;
		SendPacket(to, &p);
	}
	void SendReMoveObjectPacket(const Client& to, const Client& from)
	{
		sc_packet_remove_player p;
		p.id = from.Data.id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		SendPacket(to, &p);
	}
	void SendPosObjectPacket(const Client& to, const Client& from)
	{
		sc_packet_pos p;
		p.id = from.Data.id;
		p.size = sizeof(sc_packet_pos);
		p.type = SC_POS;
		p.x = from.Data.x;
		p.y = from.Data.y;
		SendPacket(to, &p);
	}
	void SendChatPacket(const Client& to,const Client& from, wchar_t* msg)
	{


		sc_packet_chat p;
		p.id = from.Data.id;
		ZeroMemory(p.message, MAX_STR_SIZE);
		wcscpy(p.message, msg);
		p.size = sizeof(sc_packet_chat);
		p.type = SC_CHAT;


		SendPacket(to, &p);

	}
	void SendLoginOkPacket(const Client& to, const Client& from)
	{
		sc_packet_loginok p;
		p.id = from.Data.id;
		p.hp = from.Data.Hp;
		p.exp = from.Data.Exp;
		p.level = from.Data.Lev;
		p.x = from.Data.x;
		p.y = from.Data.y;
		p.money = from.Data.money;
		p.size = sizeof(sc_packet_loginok);
		p.type = SC_LOGIN_OK;

		SendPacket(to, &p);

	}
	void SendLoginFaliPacket(const Client& to, const Client& from)
	{
		sc_packet_loginfail p;
		p.size = sizeof(sc_packet_loginfail);
		p.type = SC_LOGIN_FAIL;

		SendPacket(to, &p);

	}
	void SendItemSlotIndex(const Client& to, const Client& from)
	{
		sc_packet_itemslotid p;
		p.size = sizeof(sc_packet_itemslotid);
		p.type = SC_ITEM_INDEX;

		for (int i = 0; i < 16; ++i)
			p.index[i] = from.Slot[i];

		SendPacket(to, &p);
	}
	void SendItemSlotCount(const Client& to, const Client& from)
	{
		sc_packet_itemCount p;
		p.size = sizeof(sc_packet_itemCount);
		p.type = SC_ITEM_COUNT;

		for (int i = 0; i < 16; ++i)
			p.count[i] = from.SlotCount[i];

		SendPacket(to, &p);
		
	}
	void SendItemSlotEM(const Client& to, const Client& from)
	{
		sc_packet_EM p;
		p.size = sizeof(sc_packet_EM);
		p.type = SC_EM;

		for (int i = 0; i < 3; ++i)
			p.em[i] = from.SlotEM[i];

		SendPacket(to, &p);

	}


};