#pragma once
#include "Sprite.h"
class FWScene
{
public:
	FWScene()
	{

	}
	virtual ~FWScene()
	{

	}

public:
	virtual void Init() = 0;
	virtual void Destory() = 0;
	virtual void Update(float tick) = 0;
	virtual void Render(const HANDLE& handle) = 0;
	virtual void KeyBoard(int key) = 0;
	virtual void ServerRecvPacket(char* packet_buffer) = 0;
};