#include "Inventory.h"
#include "FWServer.h"
Inventory::Inventory()
{
	open = false;
	pSprite = new Sprite;

	gData = GameData::GetInstance();
}
Inventory::~Inventory()
{
	SAFE_DELETE(pSprite);
}
void Inventory::Render(const HANDLE& handle)
{
	if (!open)
		return;
	pSprite->Render(handle, "-----------아이템창-----------", 80, 0,COLOR::LIGHT_WHITE,COLOR::JADE);

	
	for (int i = 0; i < 3; ++i)
	{
		if (gData->EM[i] == 16)
		{
			pSprite->Render(handle, "    ", 87 + (5 * i), 3, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
			pSprite->Render(handle, "    ", 87 + (5 * i), 4, COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		}
		else
		{
			if (gData->itemTable[gData->NowSlot[gData->EM[i]] - 1].type == "무기")
			{
				pSprite->Render(handle, "    ", 87 + (5 * i), 3, COLOR::LIGHT_WHITE, COLOR::WHITE);
				pSprite->Render(handle, "    ", 87 + (5 * i), 4, COLOR::LIGHT_WHITE, COLOR::WHITE);
			}
			else if (gData->itemTable[gData->NowSlot[gData->EM[i]] - 1].type == "갑옷")
			{
				pSprite->Render(handle, "    ", 87 + (5 * i), 3, COLOR::LIGHT_WHITE, COLOR::LIGHT_BLUE);
				pSprite->Render(handle, "    ", 87 + (5 * i), 4, COLOR::LIGHT_WHITE, COLOR::LIGHT_BLUE);
			}
			else
			{
				pSprite->Render(handle, "    ", 87 + (5 * i), 3, COLOR::LIGHT_WHITE, COLOR::GREEN);
				pSprite->Render(handle, "    ", 87 + (5 * i), 4, COLOR::LIGHT_WHITE, COLOR::GREEN);
			}
		}
	
	}


	for (int i = 0; i < 16; ++i)
	{
		if (gData->NowSlot[i] == 0)
		{
			pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
			pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::LIGHT_WHITE, COLOR::LIGHT_WHITE);
		}

		else
		{
			if (gData->itemTable[gData->NowSlot[i] - 1].type == "무기")
			{
				if (gData->EM[0] == i)
				{
					pSprite->Render(handle, "E   ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::LIGHT_YELLOW, COLOR::WHITE);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::WHITE);
				}
				else
				{
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::RED, COLOR::WHITE);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::WHITE);
				}
				

			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "갑옷")
			{
				if (gData->EM[1] ==i)
				{
					pSprite->Render(handle, "E   ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::LIGHT_YELLOW, COLOR::LIGHT_BLUE);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::LIGHT_BLUE);
				}
				else
				{
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::RED, COLOR::LIGHT_BLUE);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::LIGHT_BLUE);
				}

			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "귀걸이")
			{
				if (gData->EM[2] == i)
				{
					pSprite->Render(handle, "E   ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::LIGHT_YELLOW, COLOR::GREEN);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::GREEN);
				}
				else
				{
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::RED, COLOR::GREEN);
					pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::GREEN);
				}

			}
			else if (gData->itemTable[gData->NowSlot[i] - 1].type == "물약")
			{
				pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 6 + (3 * (i / 4)), COLOR::RED, COLOR::RED);
				pSprite->Render(handle, "    ", 85 + (5 * (i % 4)), 7 + (3 * (i / 4)), COLOR::RED, COLOR::RED);

			}
		}
	}
	pSprite->Render(handle, "    ", 85 + (5 * (nowindex % 4)), 6 + (3 * (nowindex / 4)), COLOR::LIGHT_JADE, COLOR::LIGHT_JADE);
	pSprite->Render(handle, "    ", 85 + (5 * (nowindex % 4)), 7 + (3 * (nowindex / 4)), COLOR::LIGHT_JADE, COLOR::LIGHT_JADE);
	ShowData(handle);
}
void Inventory::ShowData(const HANDLE& handle)
{
	if (gData->NowSlot[nowindex] == 0)
	{
		pSprite->Render(handle, "없음", 80, 18, COLOR::LIGHT_WHITE);

		return;
	}
	char c[400];
	sprintf(c, "이름 : %s", gData->itemTable[gData->NowSlot[nowindex] - 1].name.c_str());
	pSprite->Render(handle, c, 80, 18, COLOR::LIGHT_WHITE);

	sprintf(c, "종류 : %s", gData->itemTable[gData->NowSlot[nowindex] - 1].type.c_str());
	pSprite->Render(handle, c, 80, 19, COLOR::LIGHT_WHITE);

	sprintf(c, "착용 레벨 : %s", gData->itemTable[gData->NowSlot[nowindex] - 1].level.c_str());
	pSprite->Render(handle, c, 80, 20, COLOR::LIGHT_WHITE);

	sprintf(c, "가격 : %s", gData->itemTable[gData->NowSlot[nowindex] - 1].price.c_str());
	pSprite->Render(handle, c, 80, 21, COLOR::LIGHT_WHITE);

	sprintf(c, "효과 : %s", gData->itemTable[gData->NowSlot[nowindex] - 1].effect.c_str());
	pSprite->Render(handle, c, 80, 22, COLOR::LIGHT_WHITE);

	sprintf(c, "수량 : %d", gData->NowSlotCount[nowindex]);
	pSprite->Render(handle, c, 80, 24, COLOR::LIGHT_WHITE);
}
void Inventory::RecvMessage(char* pakcet)
{

}
bool Inventory::UseHp(int index)
{
	if (chrono::high_resolution_clock::now() - hptime> 1s)
	{
		hptime = chrono::high_resolution_clock::now();
		cs_packet_useHp p;
		p.size = sizeof(cs_packet_useHp);
		p.type = CS_USE_HP;
		p.slotIndex = index;

		FWServer::GetInstance()->SendPacket(&p);
		return true;
	}
	return false;
}
void Inventory::KeyBoard(int key)
{

	if (key == 77) // <-
	{
		nowindex++;
		if (nowindex > 15)
			nowindex = 0;
	}
	if (key == 75)// ->
	{
		nowindex--;
		if (nowindex < 0)
			nowindex = 15;
	}
	if (key == 72)
	{
		nowindex -= 4;
		if (nowindex < 0)
			nowindex = 16 + nowindex;
	}
	if (key == 80)
	{
		nowindex += 4;
		if (nowindex > 15)
			nowindex = nowindex % 16;
	}
	if (key == '1')
	{
		if (gData->itemTable[gData->NowSlot[nowindex]-1].type == "물약")
		{
			gData->GetInstance()->QSlot[0] = nowindex;

			for (int i = 1; i <= 2; ++i)
			{
				if (gData->GetInstance()->QSlot[i] == nowindex)
				{
					gData->GetInstance()->QSlot[i] = -1;
				}
			}
		}
	}
	else if (key == '2')
	{
		if (gData->itemTable[gData->NowSlot[nowindex]-1].type == "물약")
		{
			
				gData->GetInstance()->QSlot[1] = nowindex;
				if (gData->GetInstance()->QSlot[0] == nowindex)
				{
					gData->GetInstance()->QSlot[0] = -1;
				}
				if (gData->GetInstance()->QSlot[2] == nowindex)
				{
					gData->GetInstance()->QSlot[2] = -1;
				}

		}
	}
	else if (key == '3')
	{
		if (gData->itemTable[gData->NowSlot[nowindex]-1].type == "물약")
		{
			
				gData->GetInstance()->QSlot[2] = nowindex;

				if (gData->GetInstance()->QSlot[0] == nowindex)
				{
					gData->GetInstance()->QSlot[0] = -1;
				}
				if (gData->GetInstance()->QSlot[1] == nowindex)
				{
					gData->GetInstance()->QSlot[1] = -1;
				}
		}
	}

}
