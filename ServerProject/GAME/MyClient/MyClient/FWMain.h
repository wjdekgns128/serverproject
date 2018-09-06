#pragma once
#include <stdio.h>
#include "Singleton.h"
#include "FWServer.h"
#include "FWWindows.h"
#include "Game.h"
class FWMain : public Singleton<FWMain>, public FWWindows
{
public:
	FWMain();
	~FWMain();
public:
	void Init();
};	