#include "FWSceneManager.h"

FWSceneManager::FWSceneManager()
{
	index = -1;
	for (int i = 0; i < 10; ++i)
		pScene[i] = NULL;
}
FWSceneManager::~FWSceneManager()
{
	for (int i = 0; i < 10; ++i)
		SAFE_DELETE(pScene[i]);
}
void FWSceneManager::Entry(int index, FWScene* scene)
{
	pScene[index] = scene;
}
void FWSceneManager::Trans(int index)
{
	if (this->index != -1)
		pScene[this->index]->Destory();
	this->index = index;
	pScene[this->index]->Init();
}