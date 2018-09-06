#include "FWMain.h"

FWMain::FWMain() :FWWindows("MY GAME",110,50)
{
}
FWMain::~FWMain()
{
}
void FWMain::Init()
{
	Entry(0, new Game);
	Trans(0);
}