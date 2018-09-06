#include "DataSystem.h"
#include "FWServer.h"

DataSystem::DataSystem()
{
	pSprite = new Sprite;
	
	vChattingList.clear();
	now = 0;

}
DataSystem::~DataSystem()
{
	SAFE_DELETE(pSprite);
}
void DataSystem::Render(const HANDLE& handle)
{

	pSprite->Render(handle, "정   보   창", 80, 40, COLOR::LIGHT_WHITE, COLOR::PURPLE);
	for (int i = 41 + now; i < 49 + now; ++i)
	{
		if (vChattingList.size() + 41 <= i)
			break;
		pSprite->Render(handle, vChattingList[i - 41].p.c_str(), 57, i - now, vChattingList[i - 41].font, COLOR::BLACK);

	}

}
void DataSystem::RecvMessage(char* pakcet)
{
	
}
void DataSystem::KeyBoard(int key)
{
	//77 오른쪽
	//75 왼쪽
	if (key == 75) // 업
	{
		now--;
		if (now < 0)
			now = 0;
	}
	if (key == 77)//다운
	{
		if (vChattingList.size() >= 8)
			if (vChattingList.size() - 8 > now)
				now++;
	}
}
