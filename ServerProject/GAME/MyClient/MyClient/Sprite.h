#pragma once
#include "MyInclude.h"
#include "MyDefine.h" 
#include <string>
#include <fstream>
#include <vector>
#include <map>
using namespace std;
struct JcsData
{
	int StartX;
	int StartY;
	int DrawX; // 실제 그려질 위치
	int DrawY; // 실제 그려질 위치
	COLOR FontColor;
	COLOR BackColor;
};
using namespace std;
class Sprite
{
	map<int, vector<JcsData>> Jcs;
	int windowsX;
	int windowsY;

public:
	Sprite()
	{
	}
	~Sprite()
	{
		

	}
public:
	

public:  //jcs
	void Entry(int index, const char* path);
public:
	void Render(const HANDLE& han,const char* text, int x, int y, COLOR FontColor = COLOR::WHITE, COLOR BackColor = COLOR::BLACK);
	void Render(const HANDLE& han, const WCHAR* text, int x, int y, COLOR FontColor = COLOR::WHITE, COLOR BackColor = COLOR::BLACK);

	void Render(const HANDLE& han,int index, int x, int y);
	void Render(const HANDLE& han, int index);

private:
	void Load(int index, const char* path);
};