#pragma once
#include "SingeTon.h"
#include <iostream>
#include <fstream>
using namespace std;
class MapData : public SingeTon<MapData>
{
public:
	bool Map[300][300];
public:
	void Init()
	{
		ifstream fp("res/map/se_map.txt");
		for (int i = 0; i < 300; ++i)
		{
			for (int j = 0; j < 300; ++j)
				fp >> Map[i][j];

		}
		fp.close();
	}
};