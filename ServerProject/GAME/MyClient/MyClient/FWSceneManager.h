#pragma once

#include "FWScene.h"
#include "MyDefine.h"
#include "MyInclude.h"

class FWSceneManager
{
private:
	int						index;
	FWScene*				pScene[10];
public:
	FWSceneManager();
	~FWSceneManager();

public:
	void Entry(int index,FWScene* scene);
	void Trans(int index);

public:
	FWScene* getNowScene() { return pScene[index]; }
};