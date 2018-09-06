#pragma once
#include "MyInclude.h"
#include <vector>
#include <string>
#include "Sprite.h"
struct ChattingData
{
	int id;
	string vS;

	ChattingData(int id, string vS)
	{
		this->id = id;
		this->vS = vS;
	}
};
class ChattingSystem
{
public:
	bool open;
	int now;
	char		Name[30];
private:
	Sprite*		pSprite;
private:
	vector<ChattingData> vChattingList;
	string			NowChatting;
	wstring d;
public:
	ChattingSystem();
	~ChattingSystem();

public:
	void Render(const HANDLE& handle);
	void KeyBoard(int key);


	void RecvMessage(char* pakcet);
};