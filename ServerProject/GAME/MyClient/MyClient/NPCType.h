#pragma once
#include "MyDefine.h"
#include "MyInclude.h"

#include "BaseObject.h"
class NPCType :public BaseObject
{
private:

public:
	~NPCType();
	NPCType(int _x, int _y, int _type);
public:
	void Update(float tick);
	void Render(const HANDLE& handle, int mapLeft, int mapTop);
	void KeyBoard(int key)
	{

	}
};