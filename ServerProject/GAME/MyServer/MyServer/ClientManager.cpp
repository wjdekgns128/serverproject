#include "ClientManager.h"
#include "NPCManager.h"
#include "NPCMonsterData.h"
#include "MapData.h"
ClientManager::ClientManager()
{
	for (int i = 0; i < NUM_OF_NPC; ++i)
	{
		
		clients[i].Init();
	}
	for (int i = NPC_START; i < NUM_OF_NPC; ++i)
	{
	
		clients[i].type = NPCMonsterData::getInstance().objectArray[i- NPC_START].type;
		MonsterSetting(i);
		clients[i].Data.x = NPCMonsterData::getInstance().objectArray[i - NPC_START].x;
		clients[i].Data.y = NPCMonsterData::getInstance().objectArray[i - NPC_START].y;
		clients[i].RespawnX = NPCMonsterData::getInstance().objectArray[i - NPC_START].x;
		clients[i].RespawnY = NPCMonsterData::getInstance().objectArray[i - NPC_START].y;
		clients[i].Data.id = i + 10000;
		clients[i].nowstate = NORMAL;

		if (clients[i].type != EMPTY)
		{
			Sector[clients[i].Data.x / 20][clients[i].Data.y / 20].insert(i);
			clients[i].m_isconnected = false;
			clients[i].m_islife = true;
			clients[i].m_isactive = false;
		}
		if (clients[i].type == MONC)
		{
			lua_State* L = clients[i].L = luaL_newstate();
			luaL_openlibs(L);
			luaL_loadfile(L, "LuaFile/MonsterCType.lua");
			int a= lua_pcall(L, 0, 0, 0);
			lua_getglobal(L, "set_uid");
			lua_pushnumber(L, clients[i].Data.id);
			int b = lua_pcall(L, 1, 1, 0);
			lua_pop(L, 1);

			lua_register(L, "API_MonsterNoHeroFind", NPCManager::API_MonsterNoHeroFind);
			lua_register(L, "API_MonsterYesHeroFind", NPCManager::API_MonsterYesHeroFind);

		}
		if (clients[i].type == MOND)
		{
			lua_State* L = clients[i].L = luaL_newstate();
			luaL_openlibs(L);
			luaL_loadfile(L, "LuaFile/MonsterDType.lua");
			lua_pcall(L, 0, 0, 0);
			lua_getglobal(L, "set_uid");
			lua_pushnumber(L, clients[i].Data.id);
			lua_pcall(L, 1, 1, 0);
			lua_pop(L, 1);

			lua_register(L, "API_MonsterNoHeroFind", NPCManager::API_MonsterNoHeroFind);
			lua_register(L, "API_MonsterYesHeroFind", NPCManager::API_MonsterYesHeroFind);
		}
	}
	for(int i= 0 ;i < MAX_USER;++i)
		clients[i].type = HERO;
	
}
void ClientManager::MonsterSetting(int id)
{
	switch (clients[id].type)
	{
	case OBJECTTYPE::MONA:
		clients[id].Data.Hp = 90;
		clients[id].Data.money = (rand() % 50)+1;
		clients[id].GameData.ATK = 10;
		clients[id].GameData.DEF = 0;

		clients[id].Data.Exp = 30;
		break;
	case OBJECTTYPE::MONB:
		clients[id].Data.Hp = 200;
		clients[id].Data.money = (rand() % 120) + 30;
		clients[id].GameData.ATK = 35;
		clients[id].GameData.DEF = 10;
		clients[id].Data.Exp = 60;
		break;
	case OBJECTTYPE::MONC:
		clients[id].Data.Hp = 400;
		clients[id].Data.money = (rand() % 160) + 50;
		clients[id].GameData.ATK = 75;
		clients[id].GameData.DEF = 25;
		clients[id].Data.Exp = 100;
		break;
	case OBJECTTYPE::MOND:
		clients[id].Data.Hp = 750;
		clients[id].Data.money = (rand() % 310) + 120;
		clients[id].GameData.ATK = 110;
		clients[id].GameData.DEF = 50;
		clients[id].Data.Exp = 200;
		break;
	}
	clients[id].GameData.MaxHp = clients[id].Data.Hp;

}				
int ClientManager::getAcceptId()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].logining == true)
			continue;
		if (false == clients[i].m_isconnected)
		{
			clients[i].logining = true;
			return i;
			break;
		}
	}
	return -1;
}
void ClientManager::ClientClose(int id)
{

	SectorLock[clients[id].Data.x / 20][clients[id].Data.y / 20].lock();
	Sector[clients[id].Data.x / 20][clients[id].Data.y / 20].erase(id);
	SectorLock[clients[id].Data.x / 20][clients[id].Data.y / 20].unlock();

	for (int i = 0; i < MAX_USER; ++i) {
		if (false == clients[i].m_isconnected) continue;
		if (i == id) continue;
		clients[i].m_viewLock.lock();
		if (0 != clients[i].m_viewList.count(id)) {
			clients[i].m_viewList.erase(id);
			clients[i].m_viewLock.unlock();
			sendMsg.SendReMoveObjectPacket(clients[i], clients[id]);
		}
		else {
			clients[i].m_viewLock.unlock();
		}
	}
	clients[id].ClientClose();
}
char* ClientManager::RecvPacket(int id, int io_size, char* ptr)
{
	char* name = clients[id].RecvPacket(io_size, ptr);
	return name;
}
void ClientManager::AcceptSetting(int id, SOCKET cs)
{
	clients[id].Init();
	clients[id].m_s = cs;
	clients[id].m_viewLock.lock();
	clients[id].m_viewList.clear();
	clients[id].m_viewLock.unlock();
	clients[id].m_isactive = true;
}
void ClientManager::StartRecv(int id)
{
	if (clients[id].m_isactive == false)
		return;
	clients[id].StartRecv();
}
void ClientManager::AcceptToSendMsg(int id)
{

	sendMsg.SendLoginOkPacket(clients[id], clients[id]);

	sendMsg.SendItemSlotIndex(clients[id], clients[id]);
	sendMsg.SendItemSlotCount(clients[id], clients[id]);
	sendMsg.SendItemSlotEM(clients[id], clients[id]);


	SectorLock[clients[id].Data.x / 20][clients[id].Data.y / 20].lock();
	Sector[clients[id].Data.x / 20][clients[id].Data.y / 20].insert(id);
	SectorLock[clients[id].Data.x / 20][clients[id].Data.y / 20].unlock();

	unordered_set<int> Temp;
	getSctor(clients[id].Data.x, clients[id].Data.y,Temp);

	for (auto p : Temp)
	{
		if (clients[p].nowstate == RESPAWN)
			continue;
		if (p == id) continue;
		if (false == CanSee(p, id)) continue;
		if (IsEmpty(p)) continue;
		if (IsNpc(p))
		{
			clients[id].m_viewLock.lock();
			clients[id].m_viewList.insert(p);
			clients[id].m_viewLock.unlock();
			NPCManager::getInstance().StartNPC(p);
			sendMsg.SendPutObjectPacket(clients[id], clients[p]);
		}
		else
		{
			if (clients[p].m_isconnected == false) continue;

			clients[p].m_viewLock.lock();
			clients[p].m_viewList.insert(id);
			clients[p].m_viewLock.unlock();
			sendMsg.SendPutObjectPacket(clients[p], clients[id]);


			clients[id].m_viewLock.lock();
			clients[id].m_viewList.insert(p);
			clients[id].m_viewLock.unlock();
			sendMsg.SendPutObjectPacket(clients[id], clients[p]);
		}

	}

	//for (int i = 0; i<MAX_USER; ++i)
	//	if (true == clients[i].m_isconnected) {
	//		if (false == CanSee(i, id)) continue;
	//		if (i == id) continue;
	//		clients[i].m_viewLock.lock();
	//		clients[i].m_viewList.insert(id);
	//		clients[i].m_viewLock.unlock();
	//		sendMsg.SendPutObjectPacket(clients[i], clients[id]);

	//	}

	//// 나에게 이미 접속해 있는 플레이어들의 정보를 알려준다.
	//for (int i = 0; i < MAX_USER; ++i) {
	//	if (false == clients[i].m_isconnected) continue;
	//	if (i == id) continue;
	//	if (false == CanSee(i, id)) continue;
	//	clients[id].m_viewLock.lock();
	//	clients[id].m_viewList.insert(i);
	//	clients[id].m_viewLock.unlock();
	//	sendMsg.SendPutObjectPacket(clients[id], clients[i]);

	//}
	//// 주위에 있는 NPC 정보를 알려 준다.

	//for (int i = NPC_START; i < NUM_OF_NPC; ++i) {
	//	if (false == CanSee(id, i)) continue;
	//	if (IsEmpty(i)) continue;
	//	clients[id].m_viewLock.lock();
	//	clients[id].m_viewList.insert(i);
	//	clients[id].m_viewLock.unlock();
	//	NPCManager::getInstance().StartNPC(i);
	//	sendMsg.SendPutObjectPacket(clients[id], clients[i]);


	//}
}