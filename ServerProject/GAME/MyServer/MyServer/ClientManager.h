#pragma once
#include "SendManager.h"
class ClientManager : public SingeTon<ClientManager>
{
private:
	SendManager sendMsg;
public:
	ClientManager();
public:
	array <Client, NUM_OF_NPC> clients;
	unordered_set<int> Sector[(BOARD_WIDTH / 20)+1][(BOARD_HEIGHT / 20)+1];
	mutex SectorLock[(BOARD_WIDTH / 20)+1][(BOARD_HEIGHT / 20)+1];
public: // Accpet 관련 처리

	int getAcceptId();
	void AcceptSetting(int id, SOCKET cs);
	void StartRecv(int id);
	void AcceptToSendMsg(int id);
	//

public: // 유틸 ( 시야,NPC인지 확인등등)
	bool isColl(int id,int x, int y,const unordered_set<int>& list,int other = -1) // 좌표에 애들이있는지 판단
	{
		for (auto p : list)
		{
			if (p == id)
				continue;
			if (other != -1)
			{
				if (other == p)
					continue;
			}
			if (!IsNpc(p) && clients[p].m_isconnected == false)
				continue;
			if (x == clients[p].Data.x && y == clients[p].Data.y)
			{
				return true;//있다면 트루
			}
		}
		return false; //겹치는 애들이없으면 펄스
		// x,y좌표에 겹치는 애들이있는지 판단
	}
	bool CanSee(int id, int otherid,int view = VIEW_RADIUS)
	{
	
		int dist_x = clients[id].Data.x - clients[otherid].Data.x;
		int dist_y = clients[id].Data.y - clients[otherid].Data.y;
		int dist = dist_x * dist_x + dist_y * dist_y;
		return ((view * view) >= dist);
	}
	bool RangeOver(int x, int y, int initX, int initY, int view,int* outdist = NULL)
	{
		int dist_x = x - initX;
		int dist_y = y - initY;
		int dist = dist_x * dist_x + dist_y * dist_y;
		if(outdist != NULL)
		*outdist = dist;
		return ((view * view) >= dist);
	}
	bool IsNpc(int id)
	{
		return ((id >= NPC_START) && (id < NUM_OF_NPC));
	}
	bool IsEmpty(int id)
	{
		return clients[id].type == OBJECTTYPE::EMPTY;
	}
public: // Recv확인
	char* RecvPacket(int id, int io_size, char* ptr);


public:
	void ClientClose(int id);
	void MonsterSetting(int id);
public:
	void getSctor(int x, int y, unordered_set<int>& returnTemp)
	{
		int px = x - (20 * (x / 20));
		int py = y - (20 * (y / 20));
		int m = 0;
		if (px < 10 && py < 10)
			m = 1;
		else if (px >= 10 && py < 10)
			m = 2;
		else if (px < 10 && py >= 10)
			m = 3;
		else
			m = 4;

		int SectorX = x / 20;
		int SectorY = y / 20;
		SectorLock[SectorX][SectorY].lock();
		returnTemp = Sector[SectorX][SectorY];
		SectorLock[SectorX][SectorY].unlock();

		if (m == 1) // y -1 , x-1,x-1,y-1
		{
			if (SectorY - 1 >= 0)
			{
				SectorLock[SectorX][SectorY - 1].lock();
				for (auto p : Sector[SectorX][SectorY - 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX][SectorY - 1].unlock();

			}
			if (SectorX - 1 >= 0)
			{
				SectorLock[SectorX-1][SectorY].lock();
				for (auto p : Sector[SectorX-1][SectorY])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX-1][SectorY].unlock();
			}
			if (SectorX - 1 >= 0 && SectorY - 1 >= 0)
			{
				SectorLock[SectorX - 1][SectorY-1].lock();
				for (auto p : Sector[SectorX - 1][SectorY-1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX - 1][SectorY-1].unlock();
			}
		}
		else if (m == 2) // x+1 ,y -1 , y-1,x+1
		{
			if (SectorY - 1 >= 0)
			{
				SectorLock[SectorX][SectorY - 1].lock();
				for (auto p : Sector[SectorX][SectorY - 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX][SectorY - 1].unlock();

			}
			if (SectorX + 1 < (BOARD_WIDTH / 20))
			{
				SectorLock[SectorX + 1][SectorY].lock();
				for (auto p : Sector[SectorX + 1][SectorY])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX + 1][SectorY].unlock();
			}
			if (SectorX + 1 < (BOARD_WIDTH / 20) && SectorY - 1 >= 0)
			{
				SectorLock[SectorX + 1][SectorY - 1].lock();
				for (auto p : Sector[SectorX + 1][SectorY - 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX + 1][SectorY - 1].unlock();
			}
		}
		else if (m == 3)
		{
			// x-1,y+1,y+1,x-1
			if (SectorX - 1 >= 0)
			{
				SectorLock[SectorX - 1][SectorY].lock();
				for (auto p : Sector[SectorX - 1][SectorY])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX - 1][SectorY].unlock();

			}
			if (SectorY + 1 < (BOARD_HEIGHT / 20))
			{
				SectorLock[SectorX][SectorY+1].lock();
				for (auto p : Sector[SectorX][SectorY+1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX][SectorY+1].unlock();
			}
			if (SectorY + 1 < (BOARD_HEIGHT / 20) && SectorX - 1 >= 0)
			{
				SectorLock[SectorX - 1][SectorY + 1].lock();
				for (auto p : Sector[SectorX - 1][SectorY + 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX - 1][SectorY + 1].unlock();
			}
		}
		else
		{
			// x +1,y+1,y+1,x+1

			if (SectorX + 1  < (BOARD_WIDTH / 20))
			{
				SectorLock[SectorX + 1][SectorY].lock();
				for (auto p : Sector[SectorX + 1][SectorY])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX + 1][SectorY].unlock();

			}
			if (SectorY + 1 < (BOARD_HEIGHT / 20))
			{
				SectorLock[SectorX][SectorY + 1].lock();
				for (auto p : Sector[SectorX][SectorY + 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX][SectorY + 1].unlock();
			}
			if (SectorY + 1 < (BOARD_HEIGHT / 20) && SectorX + 1  < (BOARD_WIDTH / 20))
			{
				SectorLock[SectorX + 1][SectorY + 1].lock();
				for (auto p : Sector[SectorX + 1][SectorY + 1])
				{
					returnTemp.insert(p);
				}
				SectorLock[SectorX + 1][SectorY + 1].unlock();
			}
		}

	}
	void PrintData(int id)
	{
		Client& p = clients[id];
		printf("실제 아이디 : %d\n", p.Data.id);
		printf("이름 : %s\n", p.Data.Name);
		printf("레벨 : %d\n", p.Data.Lev);
		printf("소지금 : %d\n", p.Data.money);
		printf("HP  : %d\n", p.Data.Hp);
		printf("exp  : %d\n", p.Data.Exp);

		printf("최대 hp : %d\n", p.GameData.MaxHp);
		printf("최대 exp : %d\n", p.GameData.MaxExp);
		printf("ATK : %d\n", p.GameData.ATK);
		printf("DEF hp : %d\n", p.GameData.DEF);

		printf("슬롯 정보\n");
		for (int i = 0; i < 16; ++i)
		{
			printf("%d  : ", p.Slot[i]);
			if (p.Slot[i] == 0)
				printf("존재하지않음\n");
			else
			{
				ItemDatas d = ItemData::getInstance().Items[p.Slot[i] - 1];

				printf("%d %d %d %d %d\n", d.ItmeID, d.ItmeLevel, d.ItmePrice, d.ItmeType, d.ItmeEffect);
			}
			
		}
		printf("\n");
		printf("슬롯 갯수 정보\n");
		for(int i= 0 ; i<16;++i)
			printf("%d ", p.SlotCount[i]);
		printf("\n");

		printf("장착중인 아이템 정보\n");
		for (int i = 0; i < 3; ++i)
		{
			printf("%d : ", p.SlotEM[i]);

			if (p.SlotEM[i] == 16)
				printf("장착하지않음\n");
			else
			{
				ItemDatas d = ItemData::getInstance().Items[p.Slot[p.SlotEM[i]] - 1];
				printf("%d %d %d %d %d\n", d.ItmeID, d.ItmeLevel, d.ItmePrice, d.ItmeType, d.ItmeEffect);
			}
			
		}
		printf("\n");

	}
};

