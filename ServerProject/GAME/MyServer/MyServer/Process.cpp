#include "Process.h"
#include "NPCManager.h"
#include "MapData.h"
#include "DB.h"
#include "GameData.h"
#include "ItemData.h"
Process::Process()
{
	packetprocess[CS_MOVE] = &Process::PlayerMove;
	packetprocess[CS_CHAT] = &Process::PlayerChat;
	packetprocess[CS_LOGIN] = &Process::LoginPlayer;
	packetprocess[CS_LOGINOUT] = &Process::LoginOutPlayer;
	packetprocess[CS_ITEM_BUY] = &Process::BuyItem;
	packetprocess[CS_ITEM_SALE] = &Process::SaleItem;
	packetprocess[CS_USE_HP] = &Process::USEHp;
	packetprocess[CS_APPLY] = &Process::ApplyItem;
	packetprocess[CS_RELEASE] = &Process::ReleseItem;
	packetprocess[CS_ATTACK] = &Process::PlayerAttack;

}
Process::~Process()
{ 
}
void Process::PlayerAttack(int id, char* packet)
{
	ClientManager& p = ClientManager::getInstance();
	if (chrono::high_resolution_clock::now() - p.clients[id].AttackTime < 0.5s)return;
	p.clients[id].AttackTime = chrono::high_resolution_clock::now();
	int myX = p.clients[id].Data.x;
	int myY = p.clients[id].Data.y;
	if (p.clients[id].nowstate == RESPAWN)
		return;
	p.clients[id].nowstate = ATTACKS;
	int check = 0;
	unordered_set<int> AttackOkSet;
	unordered_set<int> SectorSet;
	p.getSctor(myX,myY, SectorSet);
	for (auto temp : SectorSet)
	{
		if (temp == id)
			continue;
		if (p.clients[temp].type == OBJECTTYPE::EMPTY || p.clients[temp].type == OBJECTTYPE::HERO || p.clients[temp].type == OBJECTTYPE::NPC)
			continue; 
		if (p.clients[temp].nowstate == RESPAWN)
			continue;
		if (p.clients[temp].m_isactive == false)
			continue;
		if (p.CanSee(id, temp, 1))
		{
			AttackOkSet.insert(temp); // 어택에 성공한 몬스터들은 넣기
		}
	}
	int myAtk = p.clients[id].GameData.ATK;
	int myDef = p.clients[id].GameData.DEF;
	int myMoney = p.clients[id].Data.money;
	int myExp = p.clients[id].Data.Exp;
	int myLevel = p.clients[id].Data.Lev;
	int myhp = p.clients[id].Data.Hp;
	int mymaxhp = p.clients[id].GameData.MaxHp;
	int mymaxExp = p.clients[id].GameData.MaxExp;
	for (auto temp : AttackOkSet)
	{
		if (p.clients[temp].nowstate == RETURN)
			continue;
		int monsterHp = p.clients[temp].Data.Hp;
		int monsterDef = p.clients[temp].GameData.DEF;
		int monsterMoney = p.clients[temp].Data.money;
		int monsterExp = p.clients[temp].Data.Exp;
		int RealAtk = myAtk - monsterDef;
		if (RealAtk <= 0)
			RealAtk = 1;
		monsterHp -= RealAtk;
		if (monsterHp <= 0)
			monsterHp = 0;

		p.clients[temp].aggresLock.lock();
		auto findid = find_if(p.clients[temp].aggres.begin(), p.clients[temp].aggres.end(), [&](const Aggressive& a)
		{
			return a.id == id;
		});
		if (findid == p.clients[temp].aggres.end())
		{
			p.clients[temp].aggres.emplace_back(id, RealAtk);
		}
		else
		{
			findid->Damage += RealAtk;
		}

		sort(p.clients[temp].aggres.begin(), p.clients[temp].aggres.end(), [](const Aggressive& a, const Aggressive& b)
		{
			return a.Damage > b.Damage;
		});
		p.clients[temp].tar = p.clients[temp].aggres[0].id;
		p.clients[temp].aggresLock.unlock();

		sc_packet_ATTACK_OK pa;
		pa.size = sizeof(sc_packet_ATTACK_OK);
		pa.type = SC_ATTACK_OK;
		pa.moneyID = p.clients[temp].Data.id;
		pa.hp = monsterHp;

		msg.SendPacket(p.clients[id], &pa);
		
		p.clients[temp].Data.Hp = monsterHp;

		if (monsterHp <= 0) // 사망시
		{
			p.SectorLock[p.clients[temp].Data.x / 20][p.clients[temp].Data.y / 20].lock(); // 몬스터 사망시 섹터에서 삭제.
			p.Sector[p.clients[temp].Data.x / 20][p.clients[temp].Data.y / 20].erase(temp);
			p.SectorLock[p.clients[temp].Data.x / 20][p.clients[temp].Data.y / 20].unlock();
			p.clients[temp].nowstate = RESPAWN;
			p.clients[temp].m_isactive = false;
			myMoney += p.clients[temp].Data.money;
			myExp += p.clients[temp].Data.Exp;
			if (myLevel >= 99)
				myExp = GameData::getInstance().p[myLevel].MaxExp;
			p.clients[temp].tar = -1;
			if (myExp >= GameData::getInstance().p[myLevel].MaxExp  && myLevel < 99)
			{
			

				int updatahp = mymaxhp - GameData::getInstance().p[myLevel].MaxHp;
				int updataATK = myAtk - GameData::getInstance().p[myLevel].ATK;
				int updataDEF = myDef - GameData::getInstance().p[myLevel].DEF;

				myLevel++;
				myAtk = GameData::getInstance().p[myLevel].ATK + updataATK;
				myDef = GameData::getInstance().p[myLevel].DEF + updataDEF;
				mymaxhp = GameData::getInstance().p[myLevel].MaxHp + updatahp;
				myhp = mymaxhp;

				p.clients[id].GameData.ATK = myAtk;
				p.clients[id].GameData.DEF = myDef;
				p.clients[id].GameData.MaxHp = mymaxhp;
				p.clients[id].GameData.MaxHp = GameData::getInstance().p[myLevel].MaxExp;
				p.clients[id].Data.Hp = myhp;
				p.clients[id].Data.Lev = myLevel;
			}
			p.clients[id].Data.money = myMoney;
			p.clients[id].Data.Exp = myExp;

			sc_packet_stat_change cp;
			cp.size = sizeof(sc_packet_stat_change);
			cp.type = SC_STAT_CHANGE;
			cp.money = myMoney;
			cp.exp = myExp;
			cp.level = myLevel;
			msg.SendPacket(p.clients[id], &cp);
			p.clients[temp].tar = -1;
			
			for (auto t : SectorSet) // 몬스터 사망시 remove 메세지
			{
				if (p.clients[t].type == OBJECTTYPE::EMPTY)
					continue;
				if (p.clients[t].m_isconnected == false)
					continue;
				if (p.IsNpc(t))
					continue;

				if (p.CanSee(t,temp)) // 유저들중에 몬스터가 보였다면
				{
					msg.SendReMoveObjectPacket(p.clients[t], p.clients[temp]);
				}
				
			}
			NPCManager::getInstance().timerQLock.lock();
			NPCManager::getInstance().timerq.push(TimerEvent{temp,-999,high_resolution_clock::now() + 40s,TIMEEVENTLIST::TIMER_CLIENT_RESPAWN }); // 리스폰 메세지
			NPCManager::getInstance().timerQLock.unlock();
			check++;
		}
		else // 사망x
		{
			if(p.clients[temp].type == MONA || p.clients[temp].type == MONB)
			NPCManager::getInstance().StartMonsterAttack(temp, id);
		}
	}
}
void Process::ReleseItem(int id, char* packet)
{
	cs_packet_release* p = (cs_packet_release*)packet;
	int nowATK = ClientManager::getInstance().clients[id].GameData.ATK;
	int nowDEF = ClientManager::getInstance().clients[id].GameData.DEF;
	int nowMaxHp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	ItemDatas temp = ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[p->slotIndex] - 1];
	
	int Effect = temp.ItmeEffect;

	if (temp.ItmeType == 0) // 무기
	{
		nowATK -= Effect;
	}
	else if (temp.ItmeType == 1) // 갑옷
	{
		nowDEF -= Effect;
	}
	else if (temp.ItmeType == 2) // 귀걸이
	{
		nowMaxHp -= Effect;
	}
	ClientManager::getInstance().clients[id].SlotEM[temp.ItmeType] = 16;
	ClientManager::getInstance().clients[id].GameData.ATK = nowATK;
	ClientManager::getInstance().clients[id].GameData.DEF = nowDEF;
	ClientManager::getInstance().clients[id].GameData.MaxHp = nowMaxHp;
	if (ClientManager::getInstance().clients[id].GameData.MaxHp < ClientManager::getInstance().clients[id].Data.Hp)
		ClientManager::getInstance().clients[id].Data.Hp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	sc_packet_EM_OK p1;
	p1.size = sizeof(sc_packet_EM_OK);
	p1.type = SC_APPLY_OK;
	p1.applyslot = p->slotIndex;
	p1.check = 1;
	msg.SendPacket(ClientManager::getInstance().clients[id], &p1);
	//ClientManager::getInstance().PrintData(id);

}
void Process::ApplyItem(int id, char* packet)
{
	cs_packet_apply* p = (cs_packet_apply*)packet;
	int nowLev = ClientManager::getInstance().clients[id].Data.Lev;
	int nowATK = ClientManager::getInstance().clients[id].GameData.ATK;
	int nowDEF = ClientManager::getInstance().clients[id].GameData.DEF;
	int nowMaxHp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	ItemDatas temp = ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[p->slotIndex]-1];
	
	int NeedLev = temp.ItmeLevel;
	int Effect = temp.ItmeEffect;
	if (NeedLev > nowLev) // 실패 
	{
		sc_packet_EM_FALI d;
		d.size = sizeof(sc_packet_EM_FALI);
		d.type = SC_APPLY_FAIL;
		d.applysolt = p->slotIndex;
		msg.SendPacket(ClientManager::getInstance().clients[id], &d);


		return;
	}
	if (ClientManager::getInstance().clients[id].SlotEM[temp.ItmeType] != 16)
	{
		ItemDatas atemp = ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[ClientManager::getInstance().clients[id].SlotEM[temp.ItmeType]] - 1];
		if (temp.ItmeType == 0)
			nowATK -= atemp.ItmeEffect;
		else if (temp.ItmeType == 1) // 갑옷
		{
			nowDEF -= atemp.ItmeEffect;
		}
		else if (temp.ItmeType == 2) // 귀걸이
		{
			nowMaxHp -= atemp.ItmeEffect;
		}
	}
	if (temp.ItmeType == 0) // 무기
	{
		nowATK += Effect;
	}
	else if (temp.ItmeType == 1) // 갑옷
	{
		nowDEF += Effect;
	}
	else if (temp.ItmeType == 2) // 귀걸이
	{
		nowMaxHp += Effect;
	}
	ClientManager::getInstance().clients[id].SlotEM[temp.ItmeType] = p->slotIndex;
	ClientManager::getInstance().clients[id].GameData.ATK = nowATK;
	ClientManager::getInstance().clients[id].GameData.DEF = nowDEF;
	ClientManager::getInstance().clients[id].GameData.MaxHp = nowMaxHp;
	if (ClientManager::getInstance().clients[id].GameData.MaxHp < ClientManager::getInstance().clients[id].Data.Hp)
		ClientManager::getInstance().clients[id].Data.Hp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	sc_packet_EM_OK p1;
	p1.size = sizeof(sc_packet_EM_OK);
	p1.type = SC_APPLY_OK;
	p1.applyslot = p->slotIndex;
	p1.check = 0;
	msg.SendPacket(ClientManager::getInstance().clients[id], &p1);
	
}
void Process::USEHp(int id, char* packet)
{
	cs_packet_useHp* p1 = (cs_packet_useHp*)packet;
	int realID= ClientManager::getInstance().clients[id].Slot[p1->slotIndex] - 1;
	ClientManager::getInstance().clients[id].Data.Hp += ItemData::getInstance().Items[realID].ItmeEffect;
	if (ClientManager::getInstance().clients[id].GameData.MaxHp < ClientManager::getInstance().clients[id].Data.Hp)
	{
		ClientManager::getInstance().clients[id].Data.Hp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	}
	ClientManager::getInstance().clients[id].SlotCount[p1->slotIndex]--;
	if (ClientManager::getInstance().clients[id].SlotCount[p1->slotIndex] <= 0)
	{
		ClientManager::getInstance().clients[id].SlotCount[p1->slotIndex] = 0;
		ClientManager::getInstance().clients[id].Slot[p1->slotIndex] = 0;
	}
	sc_packet_hpok p;
	p.size = sizeof(sc_packet_hpok);
	p.type = SC_USE_HPOK;
	p.Slot = p1->slotIndex;
	msg.SendPacket(ClientManager::getInstance().clients[id], &p);
	//ClientManager::getInstance().PrintData(id);

}
void Process::SaleItem(int id, char* packet)
{
	cs_packet_saleitem* p1 = (cs_packet_saleitem*)packet;
	int saleid = ClientManager::getInstance().clients[id].Slot[p1->saleslotindex];
	int getmoney = ItemData::getInstance().Items[saleid-1].ItmePrice;
	int count = ClientManager::getInstance().clients[id].SlotCount[p1->saleslotindex];
	count -= 1;
	if (count <= 0)
	{
		count = 0;
		ClientManager::getInstance().clients[id].Slot[p1->saleslotindex] = 0;

	}
	ClientManager::getInstance().clients[id].SlotCount[p1->saleslotindex] = count;

	ClientManager::getInstance().clients[id].Data.money += getmoney;

	sc_packet_itemsaleok p;
	p.size = sizeof(sc_packet_itemsaleok);
	p.type = SC_ITEM_SALEOK;
	p.saleitemID = saleid;
	p.saleslot = p1->saleslotindex;
	p.money = ClientManager::getInstance().clients[id].Data.money;

	msg.SendPacket(ClientManager::getInstance().clients[id], &p);
	//ClientManager::getInstance().PrintData(id);

}
void Process::BuyItem(int id, char* packet)
{
	cs_packet_buyitem* p1 = (cs_packet_buyitem*)packet;
	int buyid = p1->itemIndex;
	int itemtype = ItemData::getInstance().Items[buyid].ItmeType;
	int needMoney = ItemData::getInstance().Items[buyid].ItmePrice;
	int RealID = ItemData::getInstance().Items[buyid].ItmeID;
	int MyMoney = ClientManager::getInstance().clients[id].Data.money;

	int emptySlot = -1;
	

		for (int i = 0; i < 16; ++i)
		{
			if (ClientManager::getInstance().clients[id].Slot[i] == 0)
			{
				emptySlot = i;
				break;
			}
		}

		if (itemtype == 3)
		{
			for (int i = 0; i < 16; ++i)
			{
				if (ClientManager::getInstance().clients[id].Slot[i] == buyid + 1)
				{
					emptySlot = i;
					break;
				}
			}
		}
		if (ClientManager::getInstance().clients[id].SlotCount[emptySlot] >= 200)
		{
			sc_packet_itembuyFail p1;
			p1.size = sizeof(sc_packet_itembuyFail);
			p1.type = SC_ITEM_BUYFAIL;
			p1.buyitmeID = RealID;
			msg.SendPacket(ClientManager::getInstance().clients[id], &p1);
			return;
		}

		if (emptySlot == -1)
		{
			sc_packet_itembuyFail p1;
			p1.size = sizeof(sc_packet_itembuyFail);
			p1.type = SC_ITEM_BUYFAIL;
			p1.buyitmeID = RealID;
			msg.SendPacket(ClientManager::getInstance().clients[id], &p1);
			return;
		}
		if (needMoney > MyMoney)
		{
			sc_packet_itembuyFail p1;
			p1.size = sizeof(sc_packet_itembuyFail);
			p1.type = SC_ITEM_BUYFAIL;
			p1.buyitmeID = RealID;
			msg.SendPacket(ClientManager::getInstance().clients[id], &p1);
			return;
		}
		MyMoney -= needMoney;
		ClientManager::getInstance().clients[id].Data.money = MyMoney;
		ClientManager::getInstance().clients[id].Slot[emptySlot] = RealID;
		ClientManager::getInstance().clients[id].SlotCount[emptySlot] += 1;
		sc_packet_itembuyok p;
		p.size = sizeof(sc_packet_itembuyok);
		p.type = SC_ITEM_BUYOK;
		p.money = MyMoney;
		p.buyitmeID = RealID;
		p.SlotNumber = emptySlot;
		msg.SendPacket(ClientManager::getInstance().clients[id], &p);
		//ClientManager::getInstance().PrintData(id);

}
void Process::LoginOutPlayer(int id, char* packet)
{
	ClientManager::getInstance().ClientClose(id);

	if (ClientManager::getInstance().clients[id].nowstate == RESPAWN || ClientManager::getInstance().clients[id].Data.Hp <= 0)
	{
		ClientManager::getInstance().clients[id].Data.x = 10;
		ClientManager::getInstance().clients[id].Data.y = 10;
		ClientManager::getInstance().clients[id].Data.Hp = ClientManager::getInstance().clients[id].GameData.MaxHp;
	}
	DB::getInstance().UpdateUserData(ClientManager::getInstance().clients[id].Data);
	DB::getInstance().UpdateUserCount(ClientManager::getInstance().clients[id].Data.id,ClientManager::getInstance().clients[id].SlotCount);
	DB::getInstance().UpdateUserSlot(ClientManager::getInstance().clients[id].Data.id,ClientManager::getInstance().clients[id].Slot);
	DB::getInstance().UpdateUserSM(ClientManager::getInstance().clients[id].Data.id, ClientManager::getInstance().clients[id].SlotEM);
}
void Process::LoginPlayer(int id, char* packet)
{

	ClientManager& p = ClientManager::getInstance();
	cs_packet_login* p1 = (cs_packet_login*)packet;

	char pStr[30] = { 0, };

	int strSize = WideCharToMultiByte(CP_ACP, 0, p1->Userid, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, p1->Userid, -1, pStr, strSize, 0, 0);
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (p.clients[i].m_isconnected == false) continue;
		if (!strcmp(p.clients[i].Data.Name, pStr))
		{
			msg.SendLoginFaliPacket(p.clients[id], p.clients[id]);
			p.ClientClose(id);
			return;
		}
	}
	bool c;
	int ArraySlotID[16] = { 0, };
	int ArrayCount[16] = { 0, };
	int	ArrayEm[3] = { 16, };
	UserData Temp =  DB::getInstance().getUserData(pStr,&c);
	if (c == false)
	{
	

		DB::getInstance().getInventorySlotId(pStr, ArraySlotID);
		DB::getInstance().getInventoryCount(pStr, ArrayCount);
		DB::getInstance().getInventoryEM(pStr, ArrayEm);
		for (int i = 0; i < 16; ++i)
		{
			p.clients[id].Slot[i] = ArraySlotID[i];
			p.clients[id].SlotCount[i] = ArrayCount[i];
			dp << p.clients[id].Slot[i];
		}
		for (int i = 0; i < 3; ++i)
		{
			p.clients[id].SlotEM[i] = ArrayEm[i];

		}

	
	}
	/*testdp.lock();
	dp.open("test.txt",ios::app);
	dp << id << endl;
	testdp.unlock();

	testdp.lock();
	for (int i = 0; i < 16; ++i)
	{
		dp << p.clients[id].Slot[i] << ", ";
	}
	dp << endl;
	for (int i = 0; i < 16; ++i)
	{
		dp << p.clients[id].SlotCount[i] << ", ";
	}
	dp << endl;
	for (int i = 0; i < 3; ++i)
	{
		dp << p.clients[id].SlotEM[i] << ", ";

	}
	dp << endl;
	testdp.unlock();
	testdp.lock();
	dp.close();
	testdp.unlock();*/



	strcpy(p.clients[id].Data.Name, Temp.Name);
	p.clients[id].Data.Exp = Temp.Exp;
	p.clients[id].Data.Hp = Temp.Hp;
	p.clients[id].Data.id = Temp.id;
	p.clients[id].Data.Lev = Temp.Lev;
	p.clients[id].Data.x = Temp.x;
	p.clients[id].Data.y = Temp.y;
	p.clients[id].Data.money = Temp.money;

	p.clients[id].GameData.ATK = GameData::getInstance().p[Temp.Lev].ATK;
	p.clients[id].GameData.DEF = GameData::getInstance().p[Temp.Lev].DEF;
	p.clients[id].GameData.MaxExp = GameData::getInstance().p[Temp.Lev].MaxExp;
	p.clients[id].GameData.MaxHp = GameData::getInstance().p[Temp.Lev].MaxHp;

	if (p.clients[id].SlotEM[0] != 16)
	{
		p.clients[id].GameData.ATK += ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[p.clients[id].SlotEM[0]] - 1].ItmeEffect;
	}
	if (p.clients[id].SlotEM[1] != 16)
	{
		p.clients[id].GameData.DEF += ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[p.clients[id].SlotEM[1]] - 1].ItmeEffect;
	}
	if (p.clients[id].SlotEM[2] != 16)
	{
		p.clients[id].GameData.MaxHp += ItemData::getInstance().Items[ClientManager::getInstance().clients[id].Slot[p.clients[id].SlotEM[2]] - 1].ItmeEffect;
	}
	//////////////////////테스트용으로들어감
	/*p.clients[id].RespawnX = Temp.x;
	p.clients[id].RespawnY = Temp.y;
	p.clients[id].Data.Hp = p.clients[id].GameData.MaxHp;*/
	//////////////////////////

	p.clients[id].m_isconnected = true;
	p.clients[id].m_islife = true;
	p.clients[id].m_isactive = true;
	ClientManager::getInstance().AcceptToSendMsg(id);
	NPCManager::getInstance().timerQLock.lock();
	NPCManager::getInstance().timerq.push(TimerEvent{ id,-999,high_resolution_clock::now() + 25s,TIMEEVENTLIST::TIMER_HERO_HPUP });
	NPCManager::getInstance().timerQLock.unlock();
	//ClientManager::getInstance().PrintData(id);

}
void Process::ProcessPacket(int id, char *packet)
{
	if (packetprocess.count(packet[1]) != 0)
		(this->*packetprocess[packet[1]])(id, packet);
}
void Process::PlayerChat(int id, char* packet)
{
	ClientManager& p = ClientManager::getInstance();
	cs_packet_chat* p1 = (cs_packet_chat*)packet;
	msg.SendChatPacket(p.clients[id], p.clients[id], p1->message);
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (p.clients[i].m_isconnected == false)
			continue;
		if (i == id)
			continue;
		msg.SendChatPacket(p.clients[i], p.clients[id], p1->message);

	}
}

void Process::PlayerMove(int id, char* packet)
{
	ClientManager& p = ClientManager::getInstance();
	if (chrono::high_resolution_clock::now() - p.clients[id].moveTime < 0.3s)
		return;

	p.clients[id].moveTime = chrono::high_resolution_clock::now();
	int x = p.clients[id].Data.x;
	int y = p.clients[id].Data.y;
	p.SectorLock[x / 20][y / 20].lock();
	p.Sector[x / 20][y / 20].erase(id);
	p.SectorLock[x / 20][y / 20].unlock();
	switch (packet[2])
	{
	case 0: if (y > 0) y--; break;
	case 1: if (y < BOARD_HEIGHT-1) y++; break;
	case 2: if (x > 0) x--; break;
	case 3: if (x < BOARD_WIDTH-1) x++; break;
	default:
		cout << "Unkown Packet Type from Client [" << id << "]\n";
		return;
	}
	

	unordered_set<int> newvl;
	unordered_set<int> SectorSet;
	p.getSctor(x, y, SectorSet);

	if (MapData::getInstance().Map[x][y] == 0 && !p.isColl(id,x,y, SectorSet))
	{
	
	}
	else
	{
		x = p.clients[id].Data.x;
		y = p.clients[id].Data.y;
	}


	p.SectorLock[x /20][y/20].lock();
	p.Sector[x /20][y / 20].insert(id);
	p.SectorLock[x/ 20][y / 20].unlock();
	p.clients[id].Data.x = x;
	p.clients[id].Data.y = y;


	/*for (int i = 0; i < MAX_USER; ++i) {
		if (i == id) continue;
		if (false == p.clients[i].m_isconnected) continue;
		if (true == p.CanSee(id, i)) newvl.insert(i);
	}
	for (int i = NPC_START; i < NUM_OF_NPC; ++i) {
		if (true == p.CanSee(id, i)) newvl.insert(i);
	}*/

	
	for (auto p1 : SectorSet)
	{
		if (p1 == id) continue;
		if (p.IsEmpty(p1)) continue;
		
		if (p.CanSee(id, p1) == TRUE)
		{
			if (p.IsNpc(p1))
			{
				if (p.clients[p1].nowstate == RESPAWN)
					continue;
				newvl.insert(p1);
			}
			else
			{
				if (p.clients[p1].m_isconnected == false) continue;
				newvl.insert(p1);

			}
		}
	}
	msg.SendPosObjectPacket(p.clients[id], p.clients[id]);

	// id lock
	// 새로운 set = 내꺼 set;
	// id unlock

	// newvl에는 있는데 old_vl에 없는 경우 (보이지 않다가 보이게된 객체처리)
	for (auto ob : newvl)
	{
		p.clients[id].m_viewLock.lock();
		if (p.clients[id].m_viewList.count(ob) == 0)
		{
			p.clients[id].m_viewList.insert(ob);
			p.clients[id].m_viewLock.unlock();
			if (p.clients[ob].nowstate != RESPAWN)
			msg.SendPutObjectPacket(p.clients[id], p.clients[ob]);

			if (p.IsNpc(ob) == true)
			{
				NPCManager::getInstance().StartNPC(ob);
				continue;
			}
			p.clients[ob].m_viewLock.lock();
			if (p.clients[ob].m_viewList.count(id) == 0)
			{
				p.clients[ob].m_viewList.insert(id);
				p.clients[ob].m_viewLock.unlock();
				msg.SendPutObjectPacket(p.clients[ob], p.clients[id]);
			}
			else
			{
				p.clients[ob].m_viewLock.unlock();
				msg.SendPosObjectPacket(p.clients[ob], p.clients[id]);
			}
		}
		else
		{
			p.clients[id].m_viewLock.unlock();
			if (p.IsNpc(ob) == true)
				continue;
			p.clients[ob].m_viewLock.lock();
			if (p.clients[ob].m_viewList.count(id) != 0)
			{
				p.clients[ob].m_viewLock.unlock();
				msg.SendPosObjectPacket(p.clients[ob], p.clients[id]);
			}
			else
			{
				p.clients[ob].m_viewList.insert(id);
				p.clients[ob].m_viewLock.unlock();
				msg.SendPutObjectPacket(p.clients[ob], p.clients[id]);
			}
		}
		
	}
	// new_vi에는 없는데 , old_vl에 있는 경우. (보이다가 안보일때)
	vector <int> to_remove;
	p.clients[id].m_viewLock.lock();
	unordered_set<int> vl_copy = p.clients[id].m_viewList;
	p.clients[id].m_viewLock.unlock();
	for (auto ob : vl_copy) {
		if (0 == newvl.count(ob)) {
			to_remove.push_back(ob);
			p.clients[ob].m_viewLock.lock();
			if (0 != p.clients[ob].m_viewList.count(id)) {
				p.clients[ob].m_viewList.erase(id);
				p.clients[ob].m_viewLock.unlock();
				msg.SendReMoveObjectPacket(p.clients[ob], p.clients[id]);
			}
			else {
				p.clients[ob].m_viewLock.unlock();
			}
		}
	}
	p.clients[id].m_viewLock.lock();
	for (auto ob : to_remove) p.clients[id].m_viewList.erase(ob);
	p.clients[id].m_viewLock.unlock();
	for (auto ob : to_remove) {
		if (p.clients[ob].nowstate != RESPAWN)
		msg.SendReMoveObjectPacket(p.clients[id], p.clients[ob]);
	}



}
