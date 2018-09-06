#pragma once
#pragma once
#include "SingeTon.h"
#include <iostream>
#include <fstream>
#include "protocol.h"
using namespace std;
struct NPCMonster
{
	int x;
	int y;
	int type;
	NPCMonster()
	{
		x = 0;
		y = 0;
		type = OBJECTTYPE::EMPTY;
	}
};
class NPCMonsterData : public SingeTon<NPCMonsterData>
{
public:
	NPCMonster objectArray[NUM_OF_NPC];
public:
	void Init()
	{
		ifstream fp("res/monster/MonsterNPC.txt");
		
			NPCMonster temp;
			int c = 0;
			while (fp >> temp.x >> temp.y >> temp.type)
			{
				objectArray[c].x = temp.x;
				objectArray[c].y = temp.y;
				objectArray[c].type = temp.type;
				c++;

			}
		fp.close();
	}
};