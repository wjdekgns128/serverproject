#include "NPCManager.h"
#include "MapData.h"
NPCManager::NPCManager()
{

	NPCProcess[EXOVER_EVENT::NPC_MOVE] = &NPCManager::MonsterMoveUpdate;
	NPCProcess[EXOVER_EVENT::RESPAWN_CLIENT] = &NPCManager::RespawnUpdate;
	NPCProcess[EXOVER_EVENT::HERO_HPUP] = &NPCManager::HeroHpUp;
	NPCProcess[EXOVER_EVENT::MONSTER_RETURN] = &NPCManager::MonsterReturn;


}
void NPCManager::MonsterReturn(int id)
{
	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].nowstate == RESPAWN)
		return;
	p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;
	if (p.clients[id].Data.x == p.clients[id].RespawnX && p.clients[id].Data.y == p.clients[id].RespawnY)
	{
		p.clients[id].nowstate = NORMAL;
		NPCManager::getInstance().timerQLock.lock();
		NPCManager::getInstance().timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() +2s,TIMEEVENTLIST::TIMER_NPC_MOVE });
		NPCManager::getInstance().timerQLock.unlock();
	}
	else
	{
		int movex;
		int movey;
		myAStar.FindAStar(id, id, &movex, &movey);
		MonsterMove(id, movex, movey);
		timerQLock.lock();
		timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 500ms,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); //다시 넣기
		timerQLock.unlock();
	}

}
void NPCManager::MonsterBTypeUpdate(int id)
{
	MonsterMove(id);

}
int NPCManager::MonsterFirstAttackCheck(int monsterid)
{

	ClientManager& p = ClientManager::getInstance();

	
	int returnid = -1;
	int len = 999999;
	int getlen = 0;

	int	monsterX = p.clients[monsterid].Data.x;
	int monsterY = p.clients[monsterid].Data.y;
	int monsterRespawnX = p.clients[monsterid].RespawnX;
	int monsterRespawnY = p.clients[monsterid].RespawnY;

	unordered_set<int> SectorSet;
	p.getSctor(monsterRespawnX, monsterRespawnY, SectorSet);
	for (auto d : SectorSet)
	{
		if (p.clients[d].type != HERO)
			continue;
		if (p.clients[d].m_isconnected == false)
			continue;
		if (p.clients[d].nowstate == RESPAWN)
			continue;
		int herox = p.clients[d].Data.x;
		int heroy = p.clients[d].Data.y;

		if (p.RangeOver(herox, heroy , monsterRespawnX, monsterRespawnY, 13,&getlen))
		{
			// 몬스터가 히어로한테 접근 할 수 있는 거리인지 판단.
			if (len > getlen) // 가장가까운 히어로 아이디 얻기
			{
				len = getlen;
				returnid = d;
			}
		}

	}
	//printf("m :%d h :%d\n", monsterid, returnid);
	return returnid;
}

int NPCManager::API_MonsterNoHeroFind(lua_State* L)
{
	int id = (int)lua_tointeger(L, -1);

	lua_pop(L, 2);
	id -= 10000;
	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].type == MONC)
	{
		p.clients[id].m_isactive = false;

	}
	else if (p.clients[id].type == MOND)
	{
		NPCManager::getInstance().timerQLock.lock();
		NPCManager::getInstance().timerq.push(TimerEvent{ id,-999,high_resolution_clock::now()+1s,TIMEEVENTLIST::TIMER_NPC_MOVE });
		NPCManager::getInstance().timerQLock.unlock();
	}
	p.clients[id].nowstate = NORMAL;
	p.clients[id].aggresLock.lock();
	p.clients[id].aggres.clear();
	p.clients[id].aggresLock.unlock();
	return 0;
}
int NPCManager::API_MonsterYesHeroFind(lua_State* L)
{
	int id = (int)lua_tointeger(L, -2);
	int heroid = (int)lua_tointeger(L, -1);
	id -= 10000;

	ClientManager& p = ClientManager::getInstance();

	lua_pop(L, 3);


	p.clients[id].aggresLock.lock();
	auto findid = find_if(p.clients[id].aggres.begin(), p.clients[id].aggres.end(), [&](const Aggressive& a)
	{
		return a.id == heroid;
	});
	if (findid == p.clients[id].aggres.end())
	{
		p.clients[id].aggres.emplace_back(heroid, 0);
	}
	sort(p.clients[id].aggres.begin(), p.clients[id].aggres.end(), [](const Aggressive& a, const Aggressive& b)
	{
		return a.Damage > b.Damage;
	});
	p.clients[id].tar = p.clients[id].aggres[0].id;
	p.clients[id].aggresLock.unlock();
	p.clients[id].nowstate = ATTACKS;
	NPCManager::getInstance().timerQLock.lock();
	NPCManager::getInstance().timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now(),TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
	NPCManager::getInstance().timerQLock.unlock();

	return 0;
}
void NPCManager::MonsterCTypeUpdate(int id)
{
	ClientManager& p = ClientManager::getInstance();
	lua_State *L = p.clients[id].L;
	// 이부분 루아스크립트 호출로변경해야함.
	// 이하 내용들이 루아스크립트에 들어가야함.

	int getid = MonsterFirstAttackCheck(id);
	p.clients[id].lualock.lock();
	if (getid == -1)
	{
		lua_getglobal(L, "event_MonsterC_TypeNo");
		if (0 != lua_pcall(L, 0, 1, 0))
		{
			cout <<
				"LUA error calling [event_MonsterC_TypeNo] : "
				<< (char *)lua_tostring(L, -1) << endl;
		}
		lua_pop(L, 1);
	}
	else
	{
		lua_getglobal(L, "event_MonsterC_TypeYes");
		lua_pushnumber(L, getid);
		if (0 != lua_pcall(L, 1, 1, 0))
		{
			cout <<
				"LUA error calling [event_MonsterC_TypeYes] : "
				<< (char *)lua_tostring(L, -1) << endl;
		}
		lua_pop(L, 1);
	}
	p.clients[id].lualock.unlock();

}
void NPCManager::MonsterDTypeUpdate(int id)
{
	ClientManager& p = ClientManager::getInstance();
	int getid = MonsterFirstAttackCheck(id);
	lua_State *L = p.clients[id].L;
	p.clients[id].lualock.lock();
	if (getid == -1)
	{
		lua_getglobal(L, "event_MonsterD_TypeNo");
		if (0 != lua_pcall(L, 0, 1, 0))
		{
			cout <<
				"LUA error calling [event_MonsterD_TypeNo] : "
				<< (char *)lua_tostring(L, -1) << endl;
		}
		lua_pop(L, 1);
	}
	else
	{
		lua_getglobal(L, "event_MonsterD_TypeYes");
		lua_pushnumber(L, getid);
		if (0 != lua_pcall(L, 1, 1, 0))
		{
			cout <<
				"LUA error calling [event_MonsterD_TypeYes] : "
				<< (char *)lua_tostring(L, -1) << endl;
		}
		lua_pop(L, 1);
	}
	p.clients[id].lualock.unlock();

}
void NPCManager::MonsterMoveUpdate(int id)
{

	ClientManager& p = ClientManager::getInstance();
	switch (p.clients[id].type)
	{
	case OBJECTTYPE::MONA:
		break;
	case OBJECTTYPE::MONB:
		MonsterBTypeUpdate(id);
		break;
	case OBJECTTYPE::MONC: // 선공 
		MonsterCTypeUpdate(id);
		break;
	case OBJECTTYPE::MOND: // 선공 
		MonsterDTypeUpdate(id);
		break;
	}
}
void NPCManager::RespawnUpdate(int id)
{
	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].nowstate != RESPAWN)
		return;
	int x = p.clients[id].Data.x;
	int y = p.clients[id].Data.y;

	if (p.IsNpc(id))
	{
		p.MonsterSetting(id);
	}
	else
	{
		p.SectorLock[x / 20][y / 20].lock(); // 히어로 섹터에서 삭제후 좌표바꾸고 다시넣어야함.
		p.Sector[x / 20][y/ 20].erase(id);
		p.SectorLock[x / 20][y / 20].unlock();
		p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;
		msg.SendPutObjectPacket(p.clients[id], p.clients[id]);
		//리스폰됨을 알려줘야함 (자기 , 자신에게)
	}

	x = p.clients[id].RespawnX;
	y = p.clients[id].RespawnY;
	p.SectorLock[x / 20][y / 20].lock(); // 리스폰시 섹터에 다시 넣어주기
	p.Sector[x / 20][y / 20].insert(id);
	p.SectorLock[x/ 20][y / 20].unlock();
	p.clients[id].nowstate = NORMAL;
	p.clients[id].Data.x = x;
	p.clients[id].Data.y = y;

	unordered_set<int> SectorSet;
	p.getSctor(x, y, SectorSet);

	int canseecount = 0;
	for (auto temp : SectorSet) // 애들에게 부활했다고 알리기
	{
		if (temp == id)
			continue;
		if (p.clients[temp].m_isconnected == false)
			continue;
		if (p.clients[temp].type != OBJECTTYPE::HERO)
			continue;

		if (p.CanSee(temp, id))
		{
			msg.SendPutObjectPacket(p.clients[temp], p.clients[id]);
			canseecount++;
		}
		
	}
	if (!p.IsNpc(id))
	{
		for (auto temp : SectorSet) // 다시 살아난 히어로한테 주변애들 보내주기
		{
			if (temp == id)
				continue;
			if (p.clients[temp].nowstate == RESPAWN)
				continue;
			if (p.CanSee(id, temp))
			{
				msg.SendPutObjectPacket(p.clients[id], p.clients[temp]);
			}
		}
	}
	if (canseecount != 0 && p.IsNpc(id)) // npc이고 주변에 한명이라도 있다면 다시 무브시작.
	{
		NPCManager::getInstance().StartNPC(id);

	}
}
void NPCManager::MonsterMove(int id,int movex ,int movey)
{
	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].nowstate == RESPAWN)
		return;

	unordered_set<int> old_vl;
	//for (int i = 0; i < MAX_USER; ++i)
	//{
	//	if (p.clients[i].m_isconnected == false) continue;
	//	if (p.CanSee(id, i)) old_vl.insert(i);
	//}
	int monsterX = p.clients[id].Data.x;
	int monsterY = p.clients[id].Data.y;

	unordered_set<int> Temp;

	p.getSctor(monsterX, monsterY,Temp);
	for (auto p1 : Temp)
	{
		
		if (p.IsNpc(p1)) continue;
		if (p.clients[p1].m_isconnected == false) continue;
		if (p.CanSee(p1, id)) old_vl.insert(p1);
	}
	p.SectorLock[monsterX / 20][monsterY / 20].lock();
	if(p.Sector[monsterX / 20][monsterY / 20].count(id) != 0)
	p.Sector[monsterX / 20][monsterY / 20].erase(id);
	p.SectorLock[monsterX / 20][monsterY / 20].unlock();
	if (movex == 0 && movey == 0)
	{
		int x = monsterX;
		int y = monsterY;
		switch (rand() % 4)
		{
		case 0: if (y > 0) y--; break;
		case 1: if (y < BOARD_HEIGHT - 1) y++; break;
		case 2: if (x  > 0)x--; break;
		case 3: if (x  < BOARD_WIDTH - 1) x++; break;
		}
		if (p.RangeOver(x, y, p.clients[id].RespawnX, p.clients[id].RespawnY, 7)) // 범위안에있으면
		{
			if (!p.isColl(id,x,y, Temp) && (MapData::getInstance().Map[x][y] == 0)) // 갈수있는 맵이라면
			{
				monsterX = x;
				monsterY = y;
			}

		}
	}
	else
	{
		monsterX = movex;
		monsterY = movey;


	}
	 p.SectorLock[monsterX / 20][monsterY / 20].lock();
	 if (p.Sector[monsterX / 20][monsterY / 20].count(id) == 0)
	 p.Sector[monsterX / 20][monsterY / 20].insert(id);
	 p.SectorLock[monsterX / 20][monsterY / 20].unlock();

	 p.clients[id].Data.x = monsterX;
	 p.clients[id].Data.y = monsterY;


	unordered_set<int> new_vl;
	/*for (int i = 0; i < MAX_USER; ++i)
	{
		if (p.IsEmpty(i)) continue;
		if (p.clients[i].m_isconnected == false) continue;
		if (p.CanSee(id, i)) new_vl.insert(i);
	}*/
	unordered_set<int> Temp1;
	p.getSctor(p.clients[id].Data.x, p.clients[id].Data.y,Temp1);
	for (auto p1 : Temp1)
	{
		if (p.IsNpc(p1)) continue;
		if (p.clients[p1].m_isconnected == false) continue;
		if (p.CanSee(p1, id)) new_vl.insert(p1);
	}

	for (auto i : old_vl)
	{
		if (new_vl.count(i) == 0)
		{
			p.clients[i].m_viewLock.lock();
			if (p.clients[i].m_viewList.count(id) != 0)
			{
				p.clients[i].m_viewList.erase(id);
				p.clients[i].m_viewLock.unlock();
				msg.SendReMoveObjectPacket(p.clients[i], p.clients[id]);

			}
			else
				p.clients[i].m_viewLock.unlock();
		}
		else
		{
			p.clients[i].m_viewLock.lock();
			if (p.clients[i].m_viewList.count(id) != 0)
			{
				p.clients[i].m_viewLock.unlock();
				msg.SendPosObjectPacket(p.clients[i], p.clients[id]);

			}
			else
			{
				p.clients[i].m_viewList.insert(id);
				p.clients[i].m_viewLock.unlock();
				msg.SendPutObjectPacket(p.clients[i], p.clients[id]);
			}
		}
	}

	for (auto i : new_vl)
	{
		if (old_vl.count(i) == 0)
		{
			p.clients[i].m_viewLock.lock();
			if (p.clients[i].m_viewList.count(id) == 0)
			{
				p.clients[i].m_viewList.insert(id);
				p.clients[i].m_viewLock.unlock();
				msg.SendPutObjectPacket(p.clients[i], p.clients[id]);
			}
			else
			{
				p.clients[i].m_viewLock.unlock();
				msg.SendPosObjectPacket(p.clients[i], p.clients[id]);

			}
		}
	}
	if (movex == 0 && movey == 0 && p.clients[id].nowstate == NORMAL)
	{
		if (new_vl.empty() == false)
		{
			timerQLock.lock();
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_NPC_MOVE });
			timerQLock.unlock();
		}
		else
		{
			p.clients[id].m_isactive = false;
		}
	}
}
void NPCManager::Update(int id, EXOVER_EVENT myevent)
{
	ClientManager& p = ClientManager::getInstance();

	(this->*NPCProcess[myevent])(id);

}
void NPCManager::HeroHpUp(int id)
{

	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].m_isconnected == false)
		return;
	int hp = p.clients[id].Data.Hp;
	int maxhp = p.clients[id].GameData.MaxHp;
	if (p.clients[id].nowstate != RESPAWN && hp <maxhp)
	{
		hp += 10;
		if (hp > maxhp)
			hp = maxhp;
		p.clients[id].Data.Hp = hp;
		sc_packet_hero_autohp p1;
		p1.type = SC_HERO_AUTO_HPUP;
		p1.size = sizeof(sc_packet_hero_autohp);
		p1.hp = p.clients[id].Data.Hp;
		msg.SendPacket(p.clients[id], &p1);
	}
	timerQLock.lock();
	timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 25s,TIMEEVENTLIST::TIMER_HERO_HPUP });
	timerQLock.unlock();
}
void NPCManager::MonsterAttack(int id, int otherid)
{
	if (otherid < 0 || otherid >= MAX_USER)
	{
		return;
	}
	ClientManager& p = ClientManager::getInstance();

	bool Heroconnected = p.clients[otherid].m_isconnected;
	int  Heronowstate = p.clients[otherid].nowstate;
	if (Heroconnected == false)
	{
		p.clients[id].aggresLock.lock();
		if(p.clients[id].aggres.size() != 0)
		p.clients[id].aggres.erase(p.clients[id].aggres.begin());
		if (p.clients[id].aggres.size() == 0)
		{
			p.clients[id].nowstate = RETURN;
			p.clients[id].tar = -1;
			p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // 몬스터 자기 위치로 메세지
			timerQLock.unlock();
		}
		else
		{
			p.clients[id].tar = p.clients[id].aggres[0].id;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
			timerQLock.unlock();
		}
		p.clients[id].aggresLock.unlock();
		return;

	}
	if (Heronowstate == RESPAWN)
	{
		p.clients[id].aggresLock.lock();
		if (p.clients[id].aggres.size() != 0)
		p.clients[id].aggres.erase(p.clients[id].aggres.begin());
		if (p.clients[id].aggres.size() == 0)
		{
			p.clients[id].nowstate = RETURN;
			p.clients[id].tar = -1;
			p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // 몬스터 자기 위치로 메세지
			timerQLock.unlock();
		}
		else
		{
			p.clients[id].tar = p.clients[id].aggres[0].id;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
			timerQLock.unlock();
		}
		p.clients[id].aggresLock.unlock();
		return;
	}
	if (p.clients[id].nowstate == RESPAWN)
		return;
	int herox = p.clients[otherid].Data.x;
	int heroy = p.clients[otherid].Data.y;

	if (p.CanSee(id, otherid, 1))
	{
		int herohp = p.clients[otherid].Data.Hp;
		int heroDef = p.clients[otherid].GameData.DEF;
		int MonsterAtk = p.clients[id].GameData.ATK;
		int realattack = MonsterAtk - heroDef;
		if (realattack <= 0)
			realattack = 1;

		herohp -= realattack;

		if (herohp > 0)
		{
			sc_packet_monster_attack p1;
			p1.monsterid = p.clients[id].Data.id;
			p1.size = sizeof(sc_packet_monster_attack);
			p1.type = SC_MONSET_ATTACK;
			msg.SendPacket(p.clients[otherid], &p1);
			timerQLock.lock();
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
			timerQLock.unlock();

		}
		else // 사망임
		{
			unordered_set<int> SectorSet;
			p.getSctor(herox, heroy, SectorSet);

		

			for (auto temp : SectorSet)
			{
				if (p.IsNpc(temp))
					continue;
				if (p.IsEmpty(temp))
					continue;
				if (p.clients[temp].m_isconnected == false)
					continue;
				if (p.CanSee(otherid, temp)) // 없어졌다고 보내줘야함
				{
					msg.SendReMoveObjectPacket(p.clients[temp], p.clients[otherid]);
				}
			}
			p.clients[id].aggresLock.lock();
			if (p.clients[id].aggres.size() != 0)
			p.clients[id].aggres.erase(p.clients[id].aggres.begin());
			if (p.clients[id].aggres.size() == 0)
			{
				p.clients[id].aggresLock.unlock();
				p.clients[id].nowstate = RETURN;
				p.clients[id].tar = -1;
				p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;

				timerQLock.lock();
				timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // 몬스터 자기 위치로 메세지
				timerQLock.unlock();

			}
			else
			{
				p.clients[id].tar = p.clients[id].aggres[0].id;
				p.clients[id].aggresLock.unlock();

				timerQLock.lock();
				timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
				timerQLock.unlock();
			}

		

			p.clients[otherid].nowstate = RESPAWN;
			timerQLock.lock();
			timerq.push(TimerEvent{ otherid,-999,high_resolution_clock::now() + 30s,TIMEEVENTLIST::TIMER_CLIENT_RESPAWN }); // 리스폰 메세지
			timerQLock.unlock();

		}
		p.clients[otherid].Data.Hp = herohp;

	}//공격가능
	else // 찾아이동해야함
	{
		int DataX = p.clients[id].Data.x;
		int DataY = p.clients[id].Data.y;

		int movex = 0;
		int movey = 0;
		if (p.RangeOver(DataX, DataY, p.clients[id].RespawnX, p.clients[id].RespawnY, 13)) // 범위안에있으면
		{
			myAStar.FindAStar(id, otherid, &movex, &movey);
			MonsterMove(id, movex, movey);
			timerQLock.lock();
			timerq.push(TimerEvent{ id,otherid,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //다시 넣기
			timerQLock.unlock();
		}
		else // 너무많이 나갔으면 다시 원래 좌표로 리턴
		{
			p.clients[id].nowstate = RETURN;
			p.clients[id].tar = -1;
			p.clients[id].aggresLock.lock();
			p.clients[id].aggres.clear();
			p.clients[id].aggresLock.unlock();

			p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;

			timerQLock.lock();
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // 몬스터 자기 위치로 메세지
			timerQLock.unlock();
		}
	}


}
void NPCManager::StartMonsterAttack(int id,int otherid)
{

	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].nowstate != NORMAL)
		return;
	p.clients[id].nowstate = ATTACKS; // 몬스터 공격상태로 변경
	timerQLock.lock();
	timerq.push(TimerEvent{ id,otherid,high_resolution_clock::now()+1s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //공격 넣기
	timerQLock.unlock();
}
void NPCManager::StartNPC(int id)
{
	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].m_isactive)
		return;
	if (p.clients[id].nowstate == RESPAWN)
		return;
	p.clients[id].m_isactive = true;
	timerQLock.lock();
	timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_NPC_MOVE });
	timerQLock.unlock();

}
