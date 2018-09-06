#pragma once
#include "MyInclude.h"
#include "protocol.h"
enum EXOVER_EVENT
{
	RECV,
	SEND,
	NPC_MOVE,
	RESPAWN_CLIENT,
	MONSTER_RETURN,
	MONSTER_ATTACK,
	HERO_HPUP,
};
enum TIMEEVENTLIST
{
	TIMER_NPC_MOVE,
	TIMER_CLIENT_RESPAWN,
	TIMER_MONSTER_ATTACK,
	TIMER_MONSTER_RETURN,
	TIMER_HERO_HPUP,
};
struct EXOVER {
	WSAOVERLAPPED m_over;
	char m_iobuf[MAX_BUFF_SIZE];
	WSABUF m_wsabuf;
	EXOVER_EVENT overEvent;
	int ohterid;
};
typedef struct UserData
{
public:
	int id;
	int x;
	int y;
	int Hp;
	int Exp;
	int Lev;
	int money;
	char Name[MAX_STR_SIZE];
}USERDATA;