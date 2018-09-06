#pragma once

class FWFrameTimer
{
public:
	float timer; // 시간
private:
	float FrameTime; // 한 프레임당 시간
	float fps; // 입력 fps

public:
	FWFrameTimer()
	{
		timer = 0;
		fps = 0;
	}
	~FWFrameTimer()
	{

	}
public:
	void Init()
	{
		timer = 0.0f;
	}
	void setFPS(float fps)
	{
		this->fps = fps;
		FrameTime = 1.0f / fps;
	}

public:
	bool FWFrameUpdate(float t) // 프레임 아예 고정. 
	{
		timer += t;
		if (timer < 0)
			return false;
		timer -= FrameTime;
		return true;
	}
	bool FWFrameSkip() // 스킵해야하는지.
	{
		return !(timer >= 0);
	}
};