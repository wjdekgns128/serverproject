#pragma once
#include "MyInclude.h"
#include <vector>
#include <string>
#include "Sprite.h"
#include "GameData.h"
#include "DataSystem.h"
#include <chrono>
class Inventory
{
public:
	int open;
public:
	chrono::high_resolution_clock::time_point hptime;
	GameData*	gData;
	DataSystem*		system;
	int nowindex;
private:
	Sprite * pSprite;
public:
	Inventory();
	~Inventory();
public:
	void Init(DataSystem* p)
	{
		system = p;
	}
public:
	void Render(const HANDLE& handle);
	void KeyBoard(int key);


	void RecvMessage(char* pakcet);
public:
	void ShowData(const HANDLE& handle);
	bool UseHp(int index);
};