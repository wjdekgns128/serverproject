#pragma once

#include "SingeTon.h"

struct ItemDatas
{
	int ItmeID;
	int ItmeType;
	int ItmeLevel;
	int ItmePrice;
	int ItmeEffect;

public:
	ItemDatas()
	{
		ItmeID = 0;
		ItmeType = 0;
		ItmeLevel = 0;
		ItmePrice = 0;
		ItmeEffect = 0;
	}
};

class ItemData : public SingeTon<ItemData>
{
public:
	ItemDatas Items[30];

public:
};