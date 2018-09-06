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
		timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 500ms,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); //�ٽ� �ֱ�
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
			// ���Ͱ� ��������� ���� �� �� �ִ� �Ÿ����� �Ǵ�.
			if (len > getlen) // ���尡��� ����� ���̵� ���
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
	NPCManager::getInstance().timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now(),TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
	NPCManager::getInstance().timerQLock.unlock();

	return 0;
}
void NPCManager::MonsterCTypeUpdate(int id)
{
	ClientManager& p = ClientManager::getInstance();
	lua_State *L = p.clients[id].L;
	// �̺κ� ��ƽ�ũ��Ʈ ȣ��κ����ؾ���.
	// ���� ������� ��ƽ�ũ��Ʈ�� ������.

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
	case OBJECTTYPE::MONC: // ���� 
		MonsterCTypeUpdate(id);
		break;
	case OBJECTTYPE::MOND: // ���� 
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
		p.SectorLock[x / 20][y / 20].lock(); // ����� ���Ϳ��� ������ ��ǥ�ٲٰ� �ٽó־����.
		p.Sector[x / 20][y/ 20].erase(id);
		p.SectorLock[x / 20][y / 20].unlock();
		p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;
		msg.SendPutObjectPacket(p.clients[id], p.clients[id]);
		//���������� �˷������ (�ڱ� , �ڽſ���)
	}

	x = p.clients[id].RespawnX;
	y = p.clients[id].RespawnY;
	p.SectorLock[x / 20][y / 20].lock(); // �������� ���Ϳ� �ٽ� �־��ֱ�
	p.Sector[x / 20][y / 20].insert(id);
	p.SectorLock[x/ 20][y / 20].unlock();
	p.clients[id].nowstate = NORMAL;
	p.clients[id].Data.x = x;
	p.clients[id].Data.y = y;

	unordered_set<int> SectorSet;
	p.getSctor(x, y, SectorSet);

	int canseecount = 0;
	for (auto temp : SectorSet) // �ֵ鿡�� ��Ȱ�ߴٰ� �˸���
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
		for (auto temp : SectorSet) // �ٽ� ��Ƴ� ��������� �ֺ��ֵ� �����ֱ�
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
	if (canseecount != 0 && p.IsNpc(id)) // npc�̰� �ֺ��� �Ѹ��̶� �ִٸ� �ٽ� �������.
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
		if (p.RangeOver(x, y, p.clients[id].RespawnX, p.clients[id].RespawnY, 7)) // �����ȿ�������
		{
			if (!p.isColl(id,x,y, Temp) && (MapData::getInstance().Map[x][y] == 0)) // �����ִ� ���̶��
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
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // ���� �ڱ� ��ġ�� �޼���
			timerQLock.unlock();
		}
		else
		{
			p.clients[id].tar = p.clients[id].aggres[0].id;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
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
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // ���� �ڱ� ��ġ�� �޼���
			timerQLock.unlock();
		}
		else
		{
			p.clients[id].tar = p.clients[id].aggres[0].id;
			timerQLock.lock();
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
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
			timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
			timerQLock.unlock();

		}
		else // �����
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
				if (p.CanSee(otherid, temp)) // �������ٰ� ���������
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
				timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // ���� �ڱ� ��ġ�� �޼���
				timerQLock.unlock();

			}
			else
			{
				p.clients[id].tar = p.clients[id].aggres[0].id;
				p.clients[id].aggresLock.unlock();

				timerQLock.lock();
				timerq.push(TimerEvent{ id,p.clients[id].tar,high_resolution_clock::now() + 2s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
				timerQLock.unlock();
			}

		

			p.clients[otherid].nowstate = RESPAWN;
			timerQLock.lock();
			timerq.push(TimerEvent{ otherid,-999,high_resolution_clock::now() + 30s,TIMEEVENTLIST::TIMER_CLIENT_RESPAWN }); // ������ �޼���
			timerQLock.unlock();

		}
		p.clients[otherid].Data.Hp = herohp;

	}//���ݰ���
	else // ã���̵��ؾ���
	{
		int DataX = p.clients[id].Data.x;
		int DataY = p.clients[id].Data.y;

		int movex = 0;
		int movey = 0;
		if (p.RangeOver(DataX, DataY, p.clients[id].RespawnX, p.clients[id].RespawnY, 13)) // �����ȿ�������
		{
			myAStar.FindAStar(id, otherid, &movex, &movey);
			MonsterMove(id, movex, movey);
			timerQLock.lock();
			timerq.push(TimerEvent{ id,otherid,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //�ٽ� �ֱ�
			timerQLock.unlock();
		}
		else // �ʹ����� �������� �ٽ� ���� ��ǥ�� ����
		{
			p.clients[id].nowstate = RETURN;
			p.clients[id].tar = -1;
			p.clients[id].aggresLock.lock();
			p.clients[id].aggres.clear();
			p.clients[id].aggresLock.unlock();

			p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;

			timerQLock.lock();
			timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 1s,TIMEEVENTLIST::TIMER_MONSTER_RETURN }); // ���� �ڱ� ��ġ�� �޼���
			timerQLock.unlock();
		}
	}


}
void NPCManager::StartMonsterAttack(int id,int otherid)
{

	ClientManager& p = ClientManager::getInstance();
	if (p.clients[id].nowstate != NORMAL)
		return;
	p.clients[id].nowstate = ATTACKS; // ���� ���ݻ��·� ����
	timerQLock.lock();
	timerq.push(TimerEvent{ id,otherid,high_resolution_clock::now()+1s,TIMEEVENTLIST::TIMER_MONSTER_ATTACK }); //���� �ֱ�
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
