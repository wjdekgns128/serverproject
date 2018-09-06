#pragma once
#include "MyInclude.h"
#include "MyDefine.h"
#include "FWSceneManager.h"
class FWWindows : public FWSceneManager
{
public:
	bool Exit;
	float fps;
	float nowTimer;
	HANDLE hBuffer[2];  //버퍼 핸들

private:
	int nScreenIndex;

	char* name;
	int   sizeX;
	int   sizeY;
public:
	FWWindows(const char* title = "test", const int x = 90, const int y = 40,const float fps = 60.0f);
	virtual ~FWWindows();
public:
#pragma region get
	char* getName() const
	{
		return name;
	}
	int getSizeX() const
	{
		return sizeX;
	}
	int getSizeY() const
	{
		return sizeY;
	}
#pragma endregion
	void CreateBuffer(int x, int y);
	void SwapBuffer()
	{
		if (nScreenIndex != 0 && nScreenIndex != 1)
			return;
		SetConsoleActiveScreenBuffer(hBuffer[nScreenIndex]);
		nScreenIndex = !nScreenIndex;
	}

	void Clear()
	{
		if (hBuffer[nScreenIndex] == NULL)
			return;
		if (nScreenIndex != 0 && nScreenIndex != 1)
			return;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		COORD Coor = { 0, 0 }; //세번째 줄의 함수를 위한 구조체
		DWORD dw; //세번째 줄의 함수를 위한 구조체
		DWORD dw1; //세번째 줄의 함수를 위한 구조체

		SetConsoleTextAttribute(hBuffer[nScreenIndex], COLOR::WHITE | (COLOR::BLACK << 4));
		FillConsoleOutputCharacterW(hBuffer[nScreenIndex], ' ', (sizeX ) * (sizeY ), Coor, &dw);
		GetConsoleScreenBufferInfo(hBuffer[nScreenIndex], &csbi);
		FillConsoleOutputAttribute(hBuffer[nScreenIndex], csbi.wAttributes,
			(sizeX ) * (sizeY ), Coor, &dw1);
		SetConsoleCursorPosition(hBuffer[nScreenIndex], Coor);
	}

	HANDLE getNowHandle() const {
		return hBuffer[nScreenIndex];
	}
};