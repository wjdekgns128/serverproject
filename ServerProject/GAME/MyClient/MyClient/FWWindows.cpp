#include "FWWindows.h"

FWWindows::FWWindows(const char* title, const int x, const int y, const float fps)
{
	this->fps = fps;
	Exit = false;
	name = new char[strlen(title) + 1];
	sizeX = x;
	sizeY = y;
	COORD size = { x,y };
	SMALL_RECT rect; //â ũ�� ���� ����ü
	rect.Left = 0;
	rect.Right = x - 1;
	rect.Top = 0;
	rect.Bottom = y - 1;
	strcpy_s(name, strlen(title) + 1, title);
	SetConsoleTitleA(name);
	/*SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), size);
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);*/
	CreateBuffer(x, y);
}
FWWindows::~FWWindows()
{
	SAFE_DELETE_ARRAY(name);
	CloseHandle(hBuffer[0]);    //���� ����
	CloseHandle(hBuffer[1]);
	hBuffer[1] = NULL;
	hBuffer[0] = NULL;
}

void FWWindows::CreateBuffer(int x, int y)
{
	nScreenIndex = 0;
	COORD size = { x,y }; //������ ũ�� ���� ����ä
	SMALL_RECT rect; //â ũ�� ���� ����ü
	rect.Left = 0;
	rect.Right = sizeX - 1;
	rect.Top = 0;
	rect.Bottom = sizeY - 1;
	//ù��° ���� ����
	hBuffer[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[0], size);
	SetConsoleWindowInfo(hBuffer[0], TRUE, &rect);
	hBuffer[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleScreenBufferSize(hBuffer[1], size);
	SetConsoleWindowInfo(hBuffer[1], TRUE, &rect);
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(hBuffer[0], &cci); // 1�� ���ۿ� �����̴� Ŀ�� ����
	SetConsoleCursorInfo(hBuffer[1], &cci); // 2�� ���ۿ� �����̴� Ŀ�� ����
	//EnableMenuItem(GetSystemMenu(GetConsoleWindow(), FALSE), SC_SIZE, MF_DISABLED | MF_GRAYED);
	//DrawMenuBar(GetConsoleWindow());
}
