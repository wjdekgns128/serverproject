#pragma once
#include "MyDefine.h"
#include "MyInclude.h"
#include "FWScene.h"
#include "Sprite.h"
#include "Player.h"
#include <vector>
#include <map>
#include <mutex>
#include "Monster.h"
#include "BaseObject.h"
#include "NPCType.h"
#include "ChattingSystem.h"
#include "Inventory.h"
#include "Shop.h"
#include "DataSystem.h"
#include <chrono>
using namespace std;
class Game :public FWScene
{
private:
	bool die;
	chrono::high_resolution_clock::time_point executeTime;
	bool			Loading;
	DataSystem		datasystem;
	ChattingSystem	chatting;
	Inventory		inventory;
	Shop			shop;
	mutex objectLock;
	int serverid;
private:
	int tiles[300][300];
	
	COLOR		 tt[8];
	map<int, BaseObject*> objects;

	Sprite*		pSprite;

	mutex d;
	int			g_left_x;
	int			g_top_y;


	int			maxhp;
	int			atk;
	int			def;
private:
	int exp;
	int hp;
	int level;
	int money;
public:
	Game();
	~Game();
public:
	void Init();
	void Destory();
	void Update(float tick);
	void Render(const HANDLE& handle);
	void KeyBoard(int key);
	void ServerRecvPacket(char* packet_buffer);

public:
	void SlotDraw(const HANDLE& handle);
	bool isNpc(int x, int y);
	string FindChar(const string& a);
public:
	int EffectToSting(const SHOPDATA& a);


	string getName(int type); // 몬스터 타입에 따른 스트링
};