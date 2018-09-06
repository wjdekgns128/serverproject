#pragma once
#include "MyInclude.h"
#include "MyStruct.h"
#include "SingeTon.h"
#include "ClientManager.h"
#include "AStar.h"
#include <map>
struct TimerEvent
{
	int id;
	int aid;
	chrono::high_resolution_clock::time_point executeTime;
	TIMEEVENTLIST	timerEvent;
};
class comparison
{
public:
	bool operator () (const TimerEvent first, const TimerEvent second) const
	{
		return first.executeTime > second.executeTime;
	}
};
class NPCManager : public SingeTon<NPCManager>
{
public:
	AStar myAStar;
private:
	typedef void (NPCManager::*NPCUpdate)(int);
	typedef std::map<int, NPCUpdate>  NPCUpdateMap;
	NPCUpdateMap NPCProcess;
public:
	SendManager msg;
	priority_queue <TimerEvent, vector <TimerEvent>, comparison> timerq;
	mutex timerQLock;
	
public:
	NPCManager();
	~NPCManager()
	{
	}
public:
	void Update(int id, EXOVER_EVENT myevent);
	void StartNPC(int id);

	///////��ǰ
	void MonsterMoveUpdate(int id);
	void RespawnUpdate(int id);
	void MonsterMove(int id,int movex= 0,int  movey = 0);
	void MonsterAttack(int id, int otherid);

	void StartMonsterAttack(int id,int otherid);

	void HeroHpUp(int id);

	void MonsterReturn(int id);

	 int MonsterFirstAttackCheck(int monsterid); // ���ϰ����� ���尡��� ����� �迭 id ���� ,-1��� ����
	////

	void MonsterBTypeUpdate(int id);
	void MonsterCTypeUpdate(int id);
	void MonsterDTypeUpdate(int id);


	///////��� ��ũ��Ʈ���� ���, ��ȭ���� ���ʹ� �ܼ��ϱ⋚���� ����
	static int API_MonsterNoHeroFind(lua_State* L); // ������ ���Ͱ� ã����������
	static int API_MonsterYesHeroFind(lua_State* L); // ������ ���Ͱ� ã������
};