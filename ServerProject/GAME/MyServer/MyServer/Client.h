#pragma once
#include "DB.h"
#include "MyStruct.h"
#include "GameData.h"
struct Aggressive
{
	int id;
	int Damage;

	Aggressive(int _id, int _Damage)
	{
		id = _id;
		Damage = _Damage;
	}
	Aggressive()
	{

	}
};
enum OBJECT_STATE
{
	NORMAL = 0,
	RESPAWN = 1,
	RETURN = 2,
	ATTACKS = 3
};
class Client {
public:
	lua_State * L;
	mutex	lualock;
	vector<Aggressive>		aggres;
	mutex					aggresLock;
	int						tar;
	bool					logining;
	mutex					m_ClinetLock;
	USERDATA				Data;
	unordered_set<int>		m_viewList;
	mutex			        m_viewLock;
	chrono::high_resolution_clock::time_point executeTime;
	chrono::high_resolution_clock::time_point moveTime;
	chrono::high_resolution_clock::time_point AttackTime;


	SOCKET m_s;
	bool m_isconnected;
	bool m_islife;
	bool m_isactive;
	EXOVER m_over;
	int m_packet_size;  // 지금 조립하고 있는 패킷의 크기
	int	m_prev_packet_size; // 지난번 recv에서 완성되지 않아서 저장해 놓은 패킷의 앞부분의 크기
	char m_packet[MAX_PACKET_SIZE];
	int type;

	SP				GameData;

	int				Slot[16]; // 슬롯에 아이템번호
	int				SlotCount[16];// 아이템 갯수
	int				SlotEM[3]; // 장착중인 아이템 (슬롯번호) //16이면 아무것도 장착하지않음.

	int				plueHp;

	int				RespawnX;
	int				RespawnY;
	int				nowstate;
public:
	Client();
	~Client();
	void Init();
	void ClientClose();

	void StartRecv();


	char* RecvPacket(int io_size,char* ptr);
};