#pragma once
#include "Singleton.h"
#include "protocol.h"
struct SP
{
	int MaxHp;
	int MaxExp;
	int ATK;
	int DEF;
};
typedef struct ShopData
{
	string name;
	string type;
	string level;
	string effect;
	string price;
}SHOPDATA;
class GameData : public Singleton<GameData>
{
public:
	char name[60];
	int NowSlot[16];
	int NowSlotCount[16];
	int EM[3];
	int QSlot[3];
	map<int, SHOPDATA> itemTable;
	SP p[100];
	bool c;
public:
	void Init()
	{
		c = false;
		QSlot[0] = -1;
		QSlot[1] = -1;
		QSlot[2] = -1;
		EM[0] = 16;
		EM[1] = 16;
		EM[2] = 16;
		for (int i = 0; i < 16; ++i)
		{
			NowSlot[i] = 0;
			NowSlotCount[i] = 0;
		}
		for (int i = 0; i < 100; ++i)
		{
			p[i].ATK = 10 + (i - 1);
			p[i].DEF = 5 + (i - 1);
			if (i == 0)
				p[i].MaxExp = 100 + (i * 200);
			else
				p[i].MaxExp = 100 + ((i * 200)) + (100 + (i - 1 * 200));
			p[i].MaxHp = 100 + ((i - 1) * 30);
		}
		TableInit();
	}
	void QSlotInit()
	{
		c = true;
		char d[256];
		sprintf(d, "res/qslot/slot%s.txt", name);
		ifstream  fp(d);
		if (!fp)
		{
			fp.close();
			ofstream f(d);
			f << -1 << " " << -1 << " " << -1;
			f.close();
		}
		else
		{
			int a;
			int b;
			int c;
			while (fp >> a >> b >> c)
			{
				QSlot[0] = a;
				QSlot[1] = b;
				QSlot[2] = c;

			}
		}
		fp.close();
	}
	void QSlotSave()
	{
		if (c)
		{
			char d[256];
			sprintf(d, "res/qslot/slot%s.txt", name);
			ofstream f(d);

			f << QSlot[0] << " " << QSlot[1] << " " << QSlot[2];
			f.close();
		}
		
	}
	void TableInit()
	{
		ifstream fp("res/item/itmetable.txt");
		char readData[300];
		while (!fp.eof())
		{
			fp.getline(readData, 300);

			string Temp = readData;

			auto p = Temp.find(',');
			string id = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());
			p = Temp.find(',');
			string name = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());
			p = Temp.find(',');
			string level = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());
			p = Temp.find(',');
			string type = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());
			p = Temp.find(',');
			string effect = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());
			p = Temp.find(',');
			string price = Temp.substr(0, p);
			Temp = Temp.substr(p + 1, Temp.length());

			SHOPDATA TempData;
			TempData.name = name;
			TempData.level = level;
			TempData.type = type;
			TempData.effect = effect;
			TempData.price = price;
			itemTable.insert(make_pair(atoi(id.c_str()) - 1, TempData));
		}
		fp.close();
	}
};