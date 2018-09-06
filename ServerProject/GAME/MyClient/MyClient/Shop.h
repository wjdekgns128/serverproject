#pragma once
#include "MyInclude.h"
#include <vector>
#include <string>
#include "Sprite.h"
#include <iostream>
#include <map>
#include <fstream>
#include "GameData.h"
#include "DataSystem.h"
using namespace std;

class Shop
{
public:
	int Tab;
	GameData* gData;
	int nowId;
public:
	DataSystem* system;
	int open;
private:
	Sprite * pSprite;
public:
	Shop();
	~Shop();
public:
	void Init(DataSystem* p)
	{
		system = p;
	}
public:
	void Render(const HANDLE& handle);
	void KeyBoard(int key);


	void RecvMessage(char* pakcet);
	void ShowData(const HANDLE& handle);
	void RenderMyItem(const HANDLE& handle);
};