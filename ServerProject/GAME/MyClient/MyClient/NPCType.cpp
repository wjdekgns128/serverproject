#include "NPCType.h"
NPCType::NPCType(int _x, int _y, int _type)
{

	x = _x;
	y = _y;
	type = _type;
}
NPCType::~NPCType()
{
}
void NPCType::Update(float tick)
{

}
void NPCType::Render(const HANDLE& handle, int mapLeft, int mapTop)
{
	if (x - mapLeft >= 20 || y - mapTop >= 20)
		return;
	if (x - mapLeft < 0 || y - mapTop < 0)
		return;

	BaseSprite->Render(handle, " ", (x - mapLeft) * 4, (y - mapTop) * 2, COLOR::BLACK, COLOR::JADE);
	BaseSprite->Render(handle, " ", (x - mapLeft) * 4+3, (y - mapTop) * 2, COLOR::BLACK, COLOR::JADE);
	BaseSprite->Render(handle, "  ", (x - mapLeft) * 4 + 1, (y - mapTop) * 2, COLOR::BLACK, COLOR::PURPLE);

	BaseSprite->Render(handle, " ", (x - mapLeft) * 4, (y - mapTop) * 2+1, COLOR::BLACK, COLOR::JADE);
	BaseSprite->Render(handle, " ", (x - mapLeft) * 4 + 3, (y - mapTop) * 2+1, COLOR::BLACK, COLOR::JADE);
	BaseSprite->Render(handle, "  ", (x - mapLeft) * 4 + 1, (y - mapTop) * 2+1, COLOR::BLACK, COLOR::PURPLE);
	
}
