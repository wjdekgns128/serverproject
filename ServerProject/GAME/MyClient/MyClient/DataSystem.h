#pragma once
#include "MyInclude.h"
#include <vector>
#include <string>
#include "Sprite.h"

struct show
{
	string p;
	COLOR font;
	show()
	{

	}
	show(string p1, COLOR font1)
	{
		p = p1;
		font = font1;
	}
};
class DataSystem
{
public:
	int now;
private:
	Sprite*		pSprite;
private:
	vector<show> vChattingList;
public:
	DataSystem();
	~DataSystem();

public:
	void Render(const HANDLE& handle);
	void KeyBoard(int key);


	void RecvMessage(char* pakcet);

public:
	void AddMessage(const string& msg,COLOR color = LIGHT_WHITE)
	{
		if (vChattingList.size() - 8 == now)
			now++;

		vChattingList.push_back(show(msg,color));
	}
};
