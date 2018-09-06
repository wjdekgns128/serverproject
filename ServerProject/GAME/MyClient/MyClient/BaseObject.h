#pragma once
#include "Sprite.h"

class BaseObject
{
public:
	static Sprite* BaseSprite;
	int				x;
	int				y;
	int				type;
protected:

	
public:
	BaseObject()
	{
		if (BaseSprite == NULL)
			BaseSprite = new Sprite;
	}
	virtual ~BaseObject()
	{
		SAFE_DELETE(BaseSprite);

	}

	virtual void Update(float tick) = 0;
	virtual void Render(const HANDLE& handle, int mapLeft, int mapTop) = 0;
	virtual void KeyBoard(int key) = 0;
	void SetPosPlayer(const sc_packet_pos* p)
	{
		x = p->x;
		y = p->y;
	}

};
