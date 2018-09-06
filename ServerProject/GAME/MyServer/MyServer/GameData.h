#pragma once
#include "SingeTon.h"
#include "protocol.h"
struct SP
{
	int MaxHp;
	int MaxExp;
	int ATK;
	int DEF;
};
class GameData : public SingeTon<GameData>
{
public:
	SP p[100];
public:
	void Init()
	{
		for (int i = 0; i < 100; ++i)
		{
			p[i].ATK = 10 + (i-1);
			p[i].DEF = 5 + (i-1);
			if(i == 0)
			p[i].MaxExp = 100 + (i * 200);
			else
				p[i].MaxExp = 100 + ((i * 200)) + (100 + (i-1 * 200));

			p[i].MaxHp = 100 + ((i - 1) * 30);
		}
	}
};