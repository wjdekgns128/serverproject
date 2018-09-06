#include "Shop.h"
#include "FWServer.h"

Shop::Shop()
{
	Tab = 0;
	gData = GameData::GetInstance();
	open = false;
	pSprite = new Sprite;
	
	nowId = 0;
}
Shop::~Shop()
{
	SAFE_DELETE(pSprite);
}
void Shop::Render(const HANDLE& handle)
{
	if (!open)
		return;
	pSprite->Render(handle, "-------------상점-------------", 80, 0, COLOR::LIGHT_WHITE, COLOR::JADE);
	pSprite->Render(handle, "  ", 80, 2, COLOR::RED, COLOR::WHITE);
	pSprite->Render(handle, "  ", 83, 2, COLOR::RED, COLOR::WHITE);
	pSprite->Render(handle, "  ", 86, 2, COLOR::RED, COLOR::WHITE);
	pSprite->Render(handle, "  ", 89, 2, COLOR::RED, COLOR::WHITE);

	pSprite->Render(handle, "  ", 92, 2, COLOR::RED, COLOR::LIGHT_BLUE);
	pSprite->Render(handle, "  ", 95, 2, COLOR::RED, COLOR::LIGHT_BLUE);
	pSprite->Render(handle, "  ", 98, 2, COLOR::RED, COLOR::LIGHT_BLUE);
	pSprite->Render(handle, "  ", 101, 2, COLOR::RED, COLOR::LIGHT_BLUE);

	pSprite->Render(handle, "  ", 104, 2, COLOR::RED, COLOR::GREEN);
	pSprite->Render(handle, "  ",107, 2, COLOR::RED, COLOR::GREEN);
	pSprite->Render(handle, "  ", 80, 4, COLOR::RED, COLOR::GREEN);
	pSprite->Render(handle, "  ", 83, 4, COLOR::RED, COLOR::GREEN);


	pSprite->Render(handle, "  ", 86, 4, COLOR::RED, COLOR::RED);
	pSprite->Render(handle, "  ", 89, 4, COLOR::RED, COLOR::RED);
	pSprite->Render(handle, "  ", 92, 4, COLOR::RED, COLOR::RED);
	//pSprite->Render(handle, "▼▼", 80 + ((nowId%6)*5), 2 + ((nowId / 6) * 4),COLOR::LIGHT_JADE,COLOR::BLACK);
	pSprite->Render(handle, "-----------인벤토리-----------", 80, 6, COLOR::LIGHT_WHITE, COLOR::JADE);
	RenderMyItem(handle);
	if(Tab == 0)
	pSprite->Render(handle, "  ", 80 + (3 * (nowId % 10)), (2 + (2 * (nowId / 10))), COLOR::LIGHT_JADE, COLOR::LIGHT_JADE);
	else if(Tab == 1)
		pSprite->Render(handle, "  ", 89 + (3 * (nowId % 4)), (8 + (2 * (nowId / 4))), COLOR::LIGHT_JADE, COLOR::LIGHT_JADE);
	ShowData(handle);
}
void Shop::RenderMyItem(const HANDLE& handle)
{
	for (int i = 0; i < 16; ++i)
	{
		if(gData->NowSlot[i] == 0)
		pSprite->Render(handle, "  ", 89 + (3 * (i % 4)), 8 + (2 * (i / 4)), COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		else
		{
			if (gData->itemTable[gData->NowSlot[i] - 1].type == "무기")
			{
				pSprite->Render(handle, "  ", 89 + (3 * (i % 4)), 8 + (2 * (i / 4)), COLOR::RED, COLOR::WHITE);
			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "갑옷")
			{
				pSprite->Render(handle, "  ", 89 + (3 * (i % 4)), 8 + (2 * (i / 4)), COLOR::RED, COLOR::LIGHT_BLUE);

			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "귀걸이")
			{
				pSprite->Render(handle, "  ", 89 + (3 * (i % 4)), 8 + (2 * (i / 4)), COLOR::RED, COLOR::GREEN);

			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "물약")
			{
				pSprite->Render(handle, "  ", 89 + (3 * (i % 4)), 8 + (2 * (i / 4)), COLOR::RED, COLOR::RED);

			}
		}
	}
}
void Shop::ShowData(const HANDLE& handle)
{
	int a = 0;
	if (Tab == 0)
		a = nowId;
	else if (Tab == 1)
	{
		a = gData->NowSlot[nowId] -1;
	}
	pSprite->Render(handle, "-------------정보-------------", 80, 16, COLOR::LIGHT_WHITE, COLOR::JADE);
	if(a == -1)
		pSprite->Render(handle, "없음", 80, 18, COLOR::LIGHT_WHITE);
	else
	{
		char c[400];
		sprintf(c, "이름 : %s", gData->itemTable[a].name.c_str());
		pSprite->Render(handle, c, 80, 18, COLOR::LIGHT_WHITE);

		sprintf(c, "종류 : %s", gData->itemTable[a].type.c_str());
		pSprite->Render(handle, c, 80, 19, COLOR::LIGHT_WHITE);

		sprintf(c, "착용 레벨 : %s", gData->itemTable[a].level.c_str());
		pSprite->Render(handle, c, 80, 20, COLOR::LIGHT_WHITE);

		sprintf(c, "가격 : %s", gData->itemTable[a].price.c_str());
		pSprite->Render(handle, c, 80, 21, COLOR::LIGHT_WHITE);

		sprintf(c, "효과 : %s", gData->itemTable[a].effect.c_str());
		pSprite->Render(handle, c, 80, 22, COLOR::LIGHT_WHITE);
		if (Tab == 1)
		{
			sprintf(c, "수량 : %d", gData->NowSlotCount[nowId]);
			pSprite->Render(handle, c, 80, 24, COLOR::LIGHT_WHITE);
		}

	}
	
}
void Shop::RecvMessage(char* pakcet)
{
	
}
void Shop::KeyBoard(int key)
{
	if (Tab == 0)
	{
		
		if (key == 77) // <-
		{
			nowId++;
			if (nowId > gData->itemTable.size() - 1)
				nowId = 0;
		}
		if (key == 75)// ->
		{
			nowId--;
			if (nowId < 0)
				nowId = gData->itemTable.size() - 1;
		}
	}
	else if (Tab == 1)
	{
		
		if (key == 77) // <-
		{
			nowId++;
			if (nowId > 15)
				nowId = 0;
		}
		if (key == 75)// ->
		{
			nowId--;
			if (nowId < 0)
				nowId = 15;
		}
		if (key == 72)
		{
			nowId -= 4;
			if (nowId < 0)
				nowId = 16 + nowId;
		}
		if (key == 80)
		{
			nowId += 4;
			if (nowId > 15)
				nowId = nowId % 16;
		}
	}
	if (key == 9) // tab
	{
		nowId = 0;
		Tab = !Tab;
	}
}
