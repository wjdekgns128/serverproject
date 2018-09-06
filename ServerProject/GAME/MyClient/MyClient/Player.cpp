#include "Player.h"
#include "FWServer.h"
#include "Debug.h"
COLOR  Player::m_Color[2];
Player::Player()
{

}
Player::Player(int id, int x, int y, int serverid, int type)
{

	m_Color[0] = COLOR::LIGHT_PURPLE; // »ó´ë¹æ
	m_Color[1] = COLOR::JADE;// ³»²¨

	this->id = id;
	this->x = x;
	this->y = y;
	this->server_id = serverid;
	this->type = type;
}
Player::~Player()
{
}
void Player::Update(float tick)
{

}
void Player::Render(const HANDLE& handle, int mapLeft, int mapTop)
{
	if (x - mapLeft >= 20 || y - mapTop >= 20)
		return;
	if (x - mapLeft < 0 || y - mapTop < 0)
		return;

	if (type == HERO)
	{
		BaseSprite->Render(handle, "    ", (x - mapLeft) * 4, (y - mapTop) * 2, COLOR::BLACK, m_Color[server_id == id]);
		BaseSprite->Render(handle, "    ", (x - mapLeft) * 4, (y - mapTop) * 2 + 1, COLOR::BLACK, m_Color[server_id == id]);
	}
	
}
void Player::KeyBoard(int key)
{
	if (key == 'w' || key == 'W')
	{
		cs_packet_move p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.dir = 0;
		FWServer::GetInstance()->SendPacket(&p);
	}
	if (key == 's' || key == 'S')
	{
		cs_packet_move p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.dir = 1;
		FWServer::GetInstance()->SendPacket(&p);
	}
	if (key == 'a' || key == 'A')
	{
		cs_packet_move p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.dir = 2;
		FWServer::GetInstance()->SendPacket(&p);
	}
	if (key == 'd' || key == 'D')
	{
		cs_packet_move p;
		p.size = sizeof(p);
		p.type = CS_MOVE;
		p.dir = 3;
		FWServer::GetInstance()->SendPacket(&p);
	}
}
