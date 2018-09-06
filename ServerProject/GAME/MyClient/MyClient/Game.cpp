#include "Game.h"
#include "FWMain.h"
#include "Debug.h"
#include "GameData.h"
Game::Game()
{
	Loading = false;
	serverid = -1;
	pSprite = NULL;
	inventory.Init(&datasystem);
	shop.Init(&datasystem);
	executeTime = chrono::high_resolution_clock::now();
}
Game::~Game()
{
	SAFE_DELETE(pSprite);
}
void Game::Init()
{
	die = false;
	if (pSprite == NULL)
	{
		pSprite = new Sprite;

	}
	g_left_x = 0;
	g_top_y = 0;
	serverid = -1;
	tt[0] = COLOR::LIGHT_YELLOW;
	tt[1] = COLOR::YELLOW;
	tt[2] = COLOR::GREEN;
	tt[3] = COLOR::LIGHT_GREEN;
	tt[4] = COLOR::LIGHT_WHITE;
	tt[5] = COLOR::WHITE;
	tt[6] = COLOR::BLUE;
	tt[7] = COLOR::LIGHT_BLUE;
	ifstream fp("res/map/cl_map.txt");
	for (int i = 0; i < 300; ++i)
	{
		for (int j = 0; j < 300; ++j)
			fp >> tiles[i][j];

	}
	fp.close();
}
void Game::Destory()
{
	SAFE_DELETE(pSprite);
	if (objects.size() > 0)
	{
		for (auto it = objects.begin(); it != objects.end(); it++)
		{
			delete it->second;
		}
		objects.clear();
	}
}
void Game::Render(const HANDLE& handle)
{
	if (Loading == false)
	{
		pSprite->Render(handle, "로딩중입니다.......", 30, 30, COLOR::LIGHT_WHITE, COLOR::BLACK);

	}
	else
	{

		for (int i = 0; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				int tx = i + g_left_x;
				int ty = j + g_top_y;
				if (tx < 0 || ty < 0 || tx >= BOARD_WIDTH || ty >= BOARD_HEIGHT) continue;
				pSprite->Render(handle, "    ", i * 4, j * 2, COLOR::BLACK, tt[tiles[tx][ty]]);
				pSprite->Render(handle, "    ", i * 4, (j * 2) + 1, COLOR::BLACK, tt[tiles[tx][ty]]);


			}
		}
		for (auto& p : objects)
		{
			p.second->Render(handle, g_left_x, g_top_y);
		}

		pSprite->Render(handle, "-------------상태-------------", 80, 28, COLOR::LIGHT_WHITE, COLOR::JADE);

		char c[100] = { 0, };

		sprintf(c, "이름 : %s", FWServer::GetInstance()->MyName);
		pSprite->Render(handle, c, 81, 29, COLOR::LIGHT_WHITE, COLOR::BLACK);

		sprintf(c, "레벨 : %d", level);
		pSprite->Render(handle, c, 81, 30, COLOR::LIGHT_WHITE, COLOR::BLACK);

		sprintf(c, "Hp : %d / %d", hp, maxhp);
		pSprite->Render(handle, c, 81, 31, COLOR::LIGHT_WHITE, COLOR::BLACK);

		sprintf(c, "경험치: %d / %d", exp, GameData::GetInstance()->p[level].MaxExp);
		pSprite->Render(handle, c, 81, 32, COLOR::LIGHT_WHITE, COLOR::BLACK);

		sprintf(c, "공격력: %d", atk);
		pSprite->Render(handle, c, 81, 33, COLOR::LIGHT_WHITE, COLOR::BLACK);

		sprintf(c, "방어력: %d", def);
		pSprite->Render(handle, c, 81, 34, COLOR::LIGHT_WHITE, COLOR::BLACK);
		sprintf(c, "소지금: %d", money);
		pSprite->Render(handle, c, 81, 35, COLOR::LIGHT_WHITE, COLOR::BLACK);


	

		SlotDraw(handle);
		chatting.Render(handle);
		inventory.Render(handle);
		shop.Render(handle);
		datasystem.Render(handle);
	}
}
string Game::FindChar(const string& a)
{
	auto p = a.find("소", 0);
	if (p != string::npos)
		return "소";
	auto p1 = a.find("중", 0);
	if (p1 != string::npos)
		return "중";
	auto p2 = a.find("대", 0);
	if (p2 != string::npos)
		return "대";

	return "?";
}
void Game::SlotDraw(const HANDLE& handle)
{
	pSprite->Render(handle, "-------------슬롯-------------", 80, 36, COLOR::LIGHT_WHITE, COLOR::JADE);
	int count = GameData::GetInstance()->NowSlotCount[GameData::GetInstance()->QSlot[0]];
	int count1 = GameData::GetInstance()->NowSlotCount[GameData::GetInstance()->QSlot[1]];
	int count2 = GameData::GetInstance()->NowSlotCount[GameData::GetInstance()->QSlot[2]];

	int slotx = GameData::GetInstance()->NowSlot[GameData::GetInstance()->QSlot[0]]-1;
	int slotx1 = GameData::GetInstance()->NowSlot[GameData::GetInstance()->QSlot[1]]-1;
	int slotx2 = GameData::GetInstance()->NowSlot[GameData::GetInstance()->QSlot[2]]-1;

	string p1;
		string p2;
		string p3;
		if (GameData::GetInstance()->QSlot[0] != -1)
		{
			p1 = (GameData::GetInstance()->itemTable[slotx].name);
			p1 = FindChar(p1);
		}
		if (GameData::GetInstance()->QSlot[1] != -1)
		{
			p2 = GameData::GetInstance()->itemTable[slotx1].name;
			p2 = FindChar(p2);

		}
		if (GameData::GetInstance()->QSlot[2] != -1)
		{
			p3 = GameData::GetInstance()->itemTable[slotx2].name;
			p3 = FindChar(p3);

		}

	if (GameData::GetInstance()->QSlot[0] == -1)
	{
		pSprite->Render(handle, "    ", 87, 37, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "    ", 87, 38, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "1", 87, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);
	}
	else
	{
		pSprite->Render(handle, "    ", 87, 37, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "    ", 87, 38, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "1", 87, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);

		char c1[100];
		sprintf(c1, "%s", p1.c_str());
		pSprite->Render(handle, c1, 89, 38, COLOR::BLACK, COLOR::LIGHT_WHITE);

		char c[100];
		sprintf(c, "%d개", count);
		pSprite->Render(handle, c, 87, 39, COLOR::BLACK, COLOR::LIGHT_WHITE);

	}
	if (GameData::GetInstance()->QSlot[1] == -1)
	{
		pSprite->Render(handle, "    ", 93, 37, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "    ", 93, 38, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "2", 93, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);

	}
	else
	{
		pSprite->Render(handle, "    ", 93, 37, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "    ", 93, 38, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "2", 93, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);
		char c1[100];
		sprintf(c1, "%s", p2.c_str());
		pSprite->Render(handle, c1, 95, 38, COLOR::BLACK, COLOR::LIGHT_WHITE);
		char c[100];
		sprintf(c, "%d개", count1);
		pSprite->Render(handle, c, 93, 39, COLOR::BLACK, COLOR::LIGHT_WHITE);

	}
	if (GameData::GetInstance()->QSlot[2] == -1)
	{
		pSprite->Render(handle, "    ", 99, 37, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "    ", 99, 38, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, "3", 99, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);
	}
	else
	{
		char c[100];
		sprintf(c, "%d개", count2);
		pSprite->Render(handle, "    ", 99, 37, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "    ", 99, 38, COLOR::LIGHT_WHITE, COLOR::RED);
		pSprite->Render(handle, "3", 99, 37, COLOR::BLACK, COLOR::LIGHT_WHITE);
		char c1[100];
		sprintf(c1, "%s", p3.c_str());
		pSprite->Render(handle, c1, 101, 38, COLOR::BLACK, COLOR::LIGHT_WHITE);
		pSprite->Render(handle, c, 99, 39, COLOR::BLACK, COLOR::LIGHT_WHITE);

	}
}
void Game::Update(float tick)
{

}
bool Game::isNpc(int x, int y)
{
	for (auto m : objects)
	{
		if (m.second->type != OBJECTTYPE::NPC)
			continue;
		int dist_x = x - m.second->x;
		int dist_y = y - m.second->y;
		int dist = dist_x * dist_x + dist_y * dist_y;
		if ((6 >= dist) == true)
			return  true;
	}
	return false;
}
void Game::KeyBoard(int key)
{
	if (!Loading)
		return;
	if (die)
	{
		if (key == 27)
		{
			FWMain::GetInstance()->Exit = true;
			return;
		}
		else
			return;
	}
	if (shop.open)
	{
		if ((key == 'l' || key == 'L') || key == 27)
		{

			shop.Tab = 0;
			shop.nowId = 0;
			shop.open = false;
		}
		else
		{
			if (key == 13)
			{
				if (shop.Tab == 0)
				{
					cs_packet_buyitem p;
					p.size = sizeof(cs_packet_buyitem);
					p.type = CS_ITEM_BUY;
					p.itemIndex = shop.nowId;

					FWServer::GetInstance()->SendPacket(&p);
				}
				else
				{

					if (GameData::GetInstance()->NowSlot[shop.nowId] == 0)
						return;
					for (int i = 0; i < 3; ++i)
					{
						if (GameData::GetInstance()->EM[i] == shop.nowId)
						{
							datasystem.AddMessage("장착중..판매에 실패하였습니다.");
							return;
						}
					}
					cs_packet_saleitem p;
					p.size = sizeof(cs_packet_saleitem);
					p.type = CS_ITEM_SALE;
					p.saleslotindex = shop.nowId;
					FWServer::GetInstance()->SendPacket(&p);
				}
			}
			else
				shop.KeyBoard(key);
		}

	}
	else
	{
		if (inventory.open)
		{
			if (key == 'i' || key == 'I' || key == 27)
			{
				inventory.open = false;
				inventory.nowindex = 0;
			}
			else
			{

				if (key == 13)
				{
					if (GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[inventory.nowindex] - 1].type == "물약")
					{
						if (!inventory.UseHp(inventory.nowindex))
							datasystem.AddMessage("물약 쿨타임입니다.", COLOR::LIGHT_RED);
					}
					else 
					{
						for (int i = 0; i < 3; ++i)
						{
							if (GameData::GetInstance()->EM[i] == inventory.nowindex)
							{
								cs_packet_release p1;
								p1.size = sizeof(cs_packet_release);
								p1.type = CS_RELEASE;
								p1.slotIndex = inventory.nowindex;
								FWServer::GetInstance()->SendPacket(&p1);

								return;
							}
						}
						cs_packet_apply p;
						p.size = sizeof(cs_packet_apply);
						p.type = CS_APPLY;
						p.slotIndex = inventory.nowindex;
						FWServer::GetInstance()->SendPacket(&p);

					}

				}
				else
				inventory.KeyBoard(key);
			}
		}
		else
		{
			if (key == 27)
				FWMain::GetInstance()->Exit = true;
			if ((key == 'i' || key == 'I') && chatting.open == false)
				inventory.open = true;
			if ((key == 'l' || key == 'L') && chatting.open == false)
			{
				if (isNpc(objects[serverid]->x, objects[serverid]->y))
				{
					shop.open = true;
				}
				else // 아니라면 어택메세지.
				{
					
						cs_packet_attack p;
						p.size = sizeof(cs_packet_attack);
						p.type = CS_ATTACK;
						FWServer::GetInstance()->SendPacket(&p);
			
				}
			}
			else
			{
				if (key == '1')
				{
					if (GameData::GetInstance()->QSlot[0] != -1)
					{
						if(!inventory.UseHp(GameData::GetInstance()->QSlot[0]))
							datasystem.AddMessage("물약 쿨타임입니다.",COLOR::LIGHT_RED);


					}
				}
				else if (key == '2')
				{
					if (GameData::GetInstance()->QSlot[1] != -1)
					{
						if (!inventory.UseHp(GameData::GetInstance()->QSlot[1]))
							datasystem.AddMessage("물약 쿨타임입니다.", COLOR::LIGHT_RED);

					}
				}
				else if (key == '3')
				{
					if (GameData::GetInstance()->QSlot[2] != -1)
					{
						if (!inventory.UseHp(GameData::GetInstance()->QSlot[2]))
							datasystem.AddMessage("물약 쿨타임입니다.", COLOR::LIGHT_RED);


					}
				}
				if (chatting.open == false)
				{
					objects[serverid]->KeyBoard(key);
					datasystem.KeyBoard(key);
				}
				chatting.KeyBoard(key);
			}

		}
	}



}
int Game::EffectToSting(const SHOPDATA& a)
{
	if (a.type == "무기")
	{
		if (a.effect == "공격력 15증가")
		{
			return 15;
		}
		else if (a.effect == "공격력 30증가")
		{
			return 30;
		}
		else if (a.effect == "공격력 65증가")
		{
			return 65;
		}
		else if (a.effect == "공격력 120증가")
		{
			return 120;
		}
	}
	else if (a.type == "갑옷")
	{
		if (a.effect == "방어력 10증가")
		{
			return 10;
		}
		else if (a.effect == "방어력 25증가")
		{
			return 25;
		}
		else if (a.effect == "방어력 45증가")
		{
			return 45;
		}
		else if (a.effect == "방어력 80증가")
		{
			return 80;
		}
	}
	else if (a.type == "귀걸이")
	{
		if (a.effect == "최대 체력 100증가")
		{
			return 100;
		}
		else if (a.effect == "최대 체력 300증가")
		{
			return 300;
		}
		else if (a.effect == "최대 체력 600증가")
		{
			return 600;
		}
		else if (a.effect == "최대 체력 1000증가")
		{
			return 1000;
		}
	}
	return 0;
}
string Game::getName(int type) // 몬스터 타입에 따른 스트
{
	switch (type)
	{
	case OBJECTTYPE::MONA:
		return "A_MON";
	case OBJECTTYPE::MONB:
		return "B_MON";

	case OBJECTTYPE::MONC:
		return "C_MON";

	case OBJECTTYPE::MOND:
		return "D_MON";

	}
	return "";
}
void Game::ServerRecvPacket(char* packet_buffer)
{
	switch (packet_buffer[1])
	{
	case SC_HERO_AUTO_HPUP:
	{
		sc_packet_hero_autohp * pa = (sc_packet_hero_autohp*)packet_buffer;
		hp = pa->hp;
		datasystem.AddMessage("자체적으로 HP를 10만큼 회복했습니다.");
		break;
	}
	case SC_MONSET_ATTACK: // 몬스터가 공격했을때
	{
		sc_packet_monster_attack * pa = (sc_packet_monster_attack*)packet_buffer;
		Monster* a = (Monster*)objects[pa->monsterid];
		string name = getName(a->type);
		int realattack = a->AcTk - def;
		if (realattack <= 0)
			realattack = 1;
		char c[256] = { 0, };
		itoa(realattack, c, 10);
		hp -= realattack;
		datasystem.AddMessage(name + "에게 " + c + "데미지를 입었습니다", COLOR::LIGHT_RED);
		break;
	}
	case SC_STAT_CHANGE:
	{
		sc_packet_stat_change* cp = (sc_packet_stat_change*)packet_buffer;

		money = cp->money;
		exp = cp->exp;
		if (cp->level != level)
		{
			SP data = GameData::GetInstance()->p[level];


			int upatk = atk - data.ATK;
			int updef = def - data.DEF;
			int updatahp = maxhp - data.MaxHp;

			level = cp->level;
			SP data1 = GameData::GetInstance()->p[level];

			atk = upatk + data1.ATK;
			def = updef + data1.DEF;
			maxhp = updatahp + data1.MaxHp;
			hp = maxhp;
		}
	
	}
		break;
	case SC_ATTACK_OK:
	{
		sc_packet_ATTACK_OK* pa = (sc_packet_ATTACK_OK*)packet_buffer;
		Monster* a = (Monster*)objects[pa->moneyID];
		int real = atk - a->DEF;
		
		string name = getName(a->type);
		char c[256] = { 0, };
		if (real <= 0)
			real = 1;
		itoa(real, c, 10);
		datasystem.AddMessage(name + "에게 " + c + "데미지를 입혔습니다",COLOR::LIGHT_WHITE);
		if (pa->hp > 0)
		{
			itoa(pa->hp, c, 10);
			datasystem.AddMessage(name + "의 남은 체력 :" + c, COLOR::LIGHT_WHITE);
		}
		else
		{
			datasystem.AddMessage(name + "가 사망하였습니다.", COLOR::LIGHT_WHITE);
		}
		a->HP = pa->hp;

 	}
		break;
	case SC_APPLY_FAIL:
	{
		sc_packet_EM_FALI* p = (sc_packet_EM_FALI*)packet_buffer;
		SHOPDATA temp = GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[p->applysolt] - 1];

		datasystem.AddMessage(temp.name + "의 장착을 실패했습니다.(레벨제한)", COLOR::LIGHT_YELLOW);
	}
	break;
	case SC_APPLY_OK:
	{
		sc_packet_EM_OK* p = (sc_packet_EM_OK*)packet_buffer;
		SHOPDATA temp = GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[p->applyslot] - 1];
		if (p->check == 0)
		{
			if (temp.type == "무기")
			{
				if (GameData::GetInstance()->EM[0] != 16)
				{
					SHOPDATA atemp = GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[0]]-1];
					atk -= EffectToSting(atemp);
				}

				int eff = EffectToSting(temp);
				atk += eff;
				GameData::GetInstance()->EM[0] = p->applyslot;
			}
			else if (temp.type == "갑옷")
			{
				if (GameData::GetInstance()->EM[1] != 16)
				{
					SHOPDATA atemp = GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[1]]-1];
					def -= EffectToSting(atemp);
				}
				int eff = EffectToSting(temp);
				def += eff;
				GameData::GetInstance()->EM[1] = p->applyslot;

			}
			else if (temp.type == "귀걸이")
			{
				if (GameData::GetInstance()->EM[2] != 16)
				{
					SHOPDATA atemp = GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[2]]-1];
					maxhp -= EffectToSting(atemp);
				}
				int eff = EffectToSting(temp);
				maxhp += eff;
				GameData::GetInstance()->EM[2] = p->applyslot;
				if (maxhp < hp)
					hp = maxhp;
			}
		}
		else if (p->check == 1)
		{
			if (temp.type == "무기")
			{
				int eff = EffectToSting(temp);
				atk -= eff;

				GameData::GetInstance()->EM[0] = 16;
			}
			else if (temp.type == "갑옷")
			{
				int eff = EffectToSting(temp);
				def -= eff;
				GameData::GetInstance()->EM[1] = 16;

			}
			else if (temp.type == "귀걸이")
			{
				int eff = EffectToSting(temp);
				maxhp -= eff;
				if (maxhp < hp)
					hp = maxhp;
				GameData::GetInstance()->EM[2] = 16;

			}
		}
	
	}
		break;
	case SC_EM:
	{
		sc_packet_EM * p = (sc_packet_EM*)packet_buffer;
		for (int i = 0; i < 3; ++i)
			GameData::GetInstance()->EM[i] = p->em[i];
		

		if (GameData::GetInstance()->EM[0] != 16)
		{
			int eff = EffectToSting(GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[0]] - 1]);
			atk += eff;
		}
		 if (GameData::GetInstance()->EM[1] != 16)
		{
			int eff = EffectToSting(GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[1]] - 1]);

			def += eff;
		}
		 if (GameData::GetInstance()->EM[2] != 16)
		{
			int eff = EffectToSting(GameData::GetInstance()->itemTable[GameData::GetInstance()->NowSlot[GameData::GetInstance()->EM[2]] - 1]);

			maxhp += eff;

		}
		break;
	}
	case SC_USE_HPOK:
	{
		sc_packet_hpok* p = (sc_packet_hpok*)packet_buffer;
		int realSlot = GameData::GetInstance()->NowSlot[p->Slot] - 1;
		if (GameData::GetInstance()->itemTable[realSlot].name == "체력 포션(소)")
		{

			hp = hp + 100;
		}
		else if (GameData::GetInstance()->itemTable[realSlot].name == "체력 포션(중)")
		{
			hp = hp + 500;
		}
		else if (GameData::GetInstance()->itemTable[realSlot].name == "체력 포션(대)")
		{
			hp = hp + 900;
		}

		GameData::GetInstance()->NowSlotCount[p->Slot]--;
		if (GameData::GetInstance()->NowSlotCount[p->Slot] <= 0)
		{
			GameData::GetInstance()->NowSlotCount[p->Slot] = 0;
			GameData::GetInstance()->NowSlot[p->Slot] = 0;
			for (int i = 0; i < 3; ++i)
			{
				if (GameData::GetInstance()->QSlot[i] == p->Slot)
					GameData::GetInstance()->QSlot[i] = -1;
			}
		}
		if (maxhp < hp)
			hp = maxhp;

		datasystem.AddMessage(GameData::GetInstance()->itemTable[realSlot].name + "을 사용하였습니다", COLOR::LIGHT_YELLOW);

		break;

	}
	case SC_ITEM_SALEOK:
	{
		sc_packet_itemsaleok* p = (sc_packet_itemsaleok*)packet_buffer;
		money = p->money;
		int count = GameData::GetInstance()->NowSlotCount[p->saleslot];
		count -= 1;
		if (count <= 0)
		{
			count = 0;
			GameData::GetInstance()->NowSlot[p->saleslot] = 0;
			for (int i = 0; i < 3; ++i)
			{
				if (GameData::GetInstance()->QSlot[i] == p->saleslot)
				{
					GameData::GetInstance()->QSlot[i] = -1;
				}
			}
		}
		GameData::GetInstance()->NowSlotCount[p->saleslot] = count;
		datasystem.AddMessage(GameData::GetInstance()->itemTable[p->saleitemID - 1].name + "을 판매하여 " + GameData::GetInstance()->itemTable[p->saleitemID - 1].price + "획득했습니다.", COLOR::LIGHT_YELLOW);

		break;
	}
	case SC_ITEM_BUYOK:
	{
		sc_packet_itembuyok* p = (sc_packet_itembuyok*)packet_buffer;
		money = p->money;
		GameData::GetInstance()->NowSlot[p->SlotNumber] = p->buyitmeID;
		GameData::GetInstance()->NowSlotCount[p->SlotNumber] += 1;

		datasystem.AddMessage(GameData::GetInstance()->itemTable[p->buyitmeID - 1].name + "을 구매했습니다.", COLOR::LIGHT_YELLOW);
		break;
	}
	case SC_ITEM_BUYFAIL:
	{
		sc_packet_itembuyFail* p = (sc_packet_itembuyFail*)packet_buffer;
		datasystem.AddMessage(GameData::GetInstance()->itemTable[p->buyitmeID - 1].name + "의 구매를 실패했습니다.", COLOR::LIGHT_YELLOW);

		break;
	}
	case SC_ITEM_INDEX:
	{
		sc_packet_itemslotid *my_packet = (sc_packet_itemslotid*)packet_buffer;
		for (int i = 0; i < 16; ++i)
		{
			GameData::GetInstance()->NowSlot[i] = my_packet->index[i];
		}
		break;
	}
	case SC_ITEM_COUNT:
	{
		sc_packet_itemCount* my_packet = (sc_packet_itemCount*)packet_buffer;
		for (int i = 0; i < 16; ++i)
		{
			GameData::GetInstance()->NowSlotCount[i] = my_packet->count[i];
		}
			break;
	}
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = (sc_packet_put_player*)packet_buffer;
		if (die == false)
		{
			if (serverid == -1)
				serverid = my_packet->id;
		}
		else
		{
			if (serverid == my_packet->id)
			{
				die = false;
				hp = maxhp;
				my_packet->x = 10;
				my_packet->y = 10;
				g_top_y = my_packet->y - 10;
				g_left_x = my_packet->x - 10;
				objectLock.lock();
				for (auto temp : objects)
					SAFE_DELETE(temp.second);
				objects.clear();
				objectLock.unlock();

				datasystem.AddMessage("부활하였습니다.");
			}
		}
		objectLock.lock();
		if (my_packet->objecttype == HERO)
		{
			objects.insert(make_pair(my_packet->id, new Player(my_packet->id, my_packet->x, my_packet->y, serverid, my_packet->objecttype)));
		}
		else if (my_packet->objecttype != NPC)
		{
			objects.insert(make_pair(my_packet->id, new Monster(my_packet->x, my_packet->y, my_packet->objecttype)));
		}
		else
		{
			objects.insert(make_pair(my_packet->id, new NPCType(my_packet->x, my_packet->y, my_packet->objecttype)));
		}
		objectLock.unlock();

	}
	break;
	case SC_POS:
	{
		sc_packet_pos *my_packet = (sc_packet_pos*)packet_buffer;

		objectLock.lock();
		if (objects.count(my_packet->id) != 0)
		{
			objects[my_packet->id]->SetPosPlayer(my_packet);
		}
		objectLock.unlock();

		if (my_packet->id == serverid)
		{
			if (die == false)
			{
				g_top_y = my_packet->y - 10;
				g_left_x = my_packet->x - 10;
			}
		}
	}
	break;

	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player * my_packet = (sc_packet_remove_player*)packet_buffer;
		if (my_packet->id == serverid)
		{
			die = true;
			hp = 0;
			datasystem.AddMessage("사망하였습니다. 30초후에 부활합니다.");
		}
		objectLock.lock();
		if (objects.count(my_packet->id) != 0)
		{
			SAFE_DELETE(objects[my_packet->id]);
			objects.erase(my_packet->id);
		}
		objectLock.unlock();


		break;
	}
	case SC_CHAT:
	{
		chatting.RecvMessage(packet_buffer);
		break;
	}

	case SC_LOGIN_OK:
	{
		GameData::GetInstance()->QSlotInit();
		Loading = true;
		sc_packet_loginok * my_packet = (sc_packet_loginok*)packet_buffer;
		if (serverid == -1)
			serverid = my_packet->id;
		objectLock.lock();
		objects.insert(make_pair(my_packet->id, new Player(my_packet->id, my_packet->x, my_packet->y, serverid, OBJECTTYPE::HERO)));
		objectLock.unlock();
		if (my_packet->id == serverid)
		{
			exp = my_packet->exp;
			hp = my_packet->hp;
			level = my_packet->level;
			money = my_packet->money;
			g_top_y = my_packet->y - 10;
			g_left_x = my_packet->x - 10;
			maxhp = GameData::GetInstance()->p[level].MaxHp;
			atk = GameData::GetInstance()->p[level].ATK;
			def = GameData::GetInstance()->p[level].DEF;
		}

		break;
	}
	case SC_LOGIN_FAIL:
	{

		printf("이미 사용중인 아이디....\n");
		Sleep(100);
		FWMain::GetInstance()->Exit = true;
		break;
	}
	}
}

