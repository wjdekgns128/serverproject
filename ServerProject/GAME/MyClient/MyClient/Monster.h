#pragma once
#include "MyDefine.h"
#include "MyInclude.h"

#include "BaseObject.h"
class Monster :public BaseObject
{
public:
	int HP;
	int AcTk;
	int DEF;
	int EXP;
private:

public:
	~Monster();
	Monster(int _x, int _y, int _type);
public:
	void Update(float tick);
	void Render(const HANDLE& handle, int mapLeft, int mapTop);
	void KeyBoard(int key)
	{

	}
};