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

	///////부품
	void MonsterMoveUpdate(int id);
	void RespawnUpdate(int id);
	void MonsterMove(int id,int movex= 0,int  movey = 0);
	void MonsterAttack(int id, int otherid);

	void StartMonsterAttack(int id,int otherid);

	void HeroHpUp(int id);

	void MonsterReturn(int id);

	 int MonsterFirstAttackCheck(int monsterid); // 리턴값으로 가장가까운 히어로 배열 id 리턴 ,-1라면 없음
	////

	void MonsterBTypeUpdate(int id);
	void MonsterCTypeUpdate(int id);
	void MonsterDTypeUpdate(int id);


	///////루아 스크립트에서 사용, 평화적인 몬스터는 단순하기떄문에 생략
	static int API_MonsterNoHeroFind(lua_State* L); // 선공형 몬스터가 찾지못했을떄
	static int API_MonsterYesHeroFind(lua_State* L); // 선공형 몬스터가 찾았을떄
};