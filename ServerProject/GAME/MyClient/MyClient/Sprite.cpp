#include "Sprite.h"

void Sprite::Render(const HANDLE& han, const WCHAR* text, int x, int y, COLOR FontColor, COLOR BackColor)
{

	COORD Coor = { 0, 0 }; //세번째 줄의 함수를 위한 구조체
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleTextAttribute(han, FontColor | (BackColor << 4));
	SetConsoleCursorPosition(han, CursorPosition);
	WriteFile(han, text, wcslen(text), &dw, NULL);


}

void Sprite::Render(const HANDLE& han,const char* text, int x, int y, COLOR FontColor,COLOR BackColor)
{

	COORD Coor = { 0, 0 }; //세번째 줄의 함수를 위한 구조체
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleTextAttribute(han, FontColor | (BackColor << 4));
	SetConsoleCursorPosition(han, CursorPosition);
	WriteFile(han, text, strlen(text), &dw, NULL);

	
}
void Sprite::Render(const HANDLE& han,int index, int x, int y)
{
	if (Jcs.size() <= 0)
		return;
	vector<JcsData> TempV = Jcs.at(index);
	COORD Coor = { 0, 0 }; //세번째 줄의 함수를 위한 구조체
	DWORD dw;
	COORD CursorPosition = { 0, 0 };

	for (auto p = TempV.begin(); p != TempV.end(); ++p)
	{
		CursorPosition.X = (*p).DrawX +x;
		CursorPosition.Y = (*p).DrawY + y;
		SetConsoleTextAttribute(han, (*p).FontColor| ((*p).BackColor << 4));
		SetConsoleCursorPosition(han, CursorPosition);
		WriteFile(han, " ", strlen(" "), &dw, NULL);
	}
}
void Sprite::Render(const HANDLE& han, int index)
{
	if (Jcs.size() <= 0)
		return;
	vector<JcsData> TempV = Jcs.at(index);
	COORD Coor = { 0, 0 }; //세번째 줄의 함수를 위한 구조체
	DWORD dw;
	COORD CursorPosition = { 0, 0 };

	for (auto p = TempV.begin(); p != TempV.end(); ++p)
	{
		CursorPosition.X = (*p).DrawX + (*p).StartX;
		CursorPosition.Y = (*p).DrawY + (*p).StartY;
		SetConsoleTextAttribute(han, (*p).FontColor | ((*p).BackColor << 4));
		SetConsoleCursorPosition(han, CursorPosition);
		WriteFile(han, " ", strlen(" "), &dw, NULL);
	}
}
void Sprite::Entry(int index, const char * path)
{
	Load(index, path);
}

void Sprite::Load(int index, const char * path)
{
	ifstream inFile(path);
	int StartX;
	int StartY;
	int X;
	int Y;
	int FontColor;
	int BackColor;
	JcsData TempData;
	vector<JcsData> TempV;
	inFile >> StartX >> StartY;

	while (inFile >> X >> Y >> FontColor >> BackColor)
	{
		TempData.StartX = StartX;
		TempData.StartY = StartY;

		TempData.DrawX = X;
		TempData.DrawY = Y;
		TempData.FontColor = (COLOR)FontColor;
		TempData.BackColor = (COLOR)BackColor;
		TempV.push_back(TempData);
	}
	Jcs.insert(make_pair(index, TempV));
	inFile.close();
}
