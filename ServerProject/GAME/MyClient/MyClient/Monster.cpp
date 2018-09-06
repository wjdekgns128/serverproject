#include "Monster.h"
Monster::Monster(int _x,int _y ,int _type)
{

	x = _x;
	y = _y;
	type = _type;

	switch (type)
	{
	case OBJECTTYPE::MONA:
		HP = 90;
		AcTk = 10;
		DEF = 0;
		EXP = 30;
		break;
	case OBJECTTYPE::MONB:
		HP = 200;
		AcTk = 35;
		DEF = 10;
		EXP = 60;
		break;
	case OBJECTTYPE::MONC:
		HP = 400;
		AcTk = 75;
		DEF = 25;
		EXP = 100;
		break;
	case OBJECTTYPE::MOND:
		HP = 750;
		AcTk = 110;
		DEF = 50;
		EXP = 200;
		break;
	}
}
Monster::~Monster()
{
}
void Monster::Update(float tick)
{

}
void Monster::Render(const HANDLE& handle, int mapLeft, int mapTop)
{
	if (x - mapLeft >= 20 || y - mapTop >= 20)
		return;
	if (x - mapLeft < 0 || y - mapTop < 0)
		return;
	switch (type)
	{
	case OBJECTTYPE::MONA:
		BaseSprite->Render(handle, "  ", (x - mapLeft) * 4 + 1, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, "    ", (x - mapLeft) * 4, (y - mapTop) * 2 + 1, COLOR::BLACK, COLOR::LIGHT_RED);
		break;
	case OBJECTTYPE::MONB:
		BaseSprite->Render(handle, " ", (x - mapLeft) * 4, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, " ", (x - mapLeft) * 4 + 3, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, "  ", (x - mapLeft) * 4+1, (y - mapTop) * 2 + 1, COLOR::BLACK, COLOR::LIGHT_RED);
		break;
	case OBJECTTYPE::MONC:

		BaseSprite->Render(handle, " ", (x - mapLeft) * 4, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, " ", (x - mapLeft) * 4 + 3, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, "    ", (x - mapLeft) * 4, (y - mapTop) * 2 + 1, COLOR::BLACK, COLOR::LIGHT_RED);
		break;
	case OBJECTTYPE::MOND:
		BaseSprite->Render(handle, " ", (x - mapLeft) * 4, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, " ", (x - mapLeft) * 4 + 3, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_RED);
		BaseSprite->Render(handle, "  ", (x - mapLeft) * 4 + 1, (y - mapTop) * 2, COLOR::BLACK, COLOR::LIGHT_JADE);
		BaseSprite->Render(handle, "  ", (x - mapLeft) * 4 + 1, (y - mapTop) * 2 + 1, COLOR::BLACK, COLOR::LIGHT_RED);
		break;
	}
}
