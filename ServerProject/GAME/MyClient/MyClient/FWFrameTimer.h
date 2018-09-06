#pragma once

class FWFrameTimer
{
public:
	float timer; // �ð�
private:
	float FrameTime; // �� �����Ӵ� �ð�
	float fps; // �Է� fps

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
	bool FWFrameUpdate(float t) // ������ �ƿ� ����. 
	{
		timer += t;
		if (timer < 0)
			return false;
		timer -= FrameTime;
		return true;
	}
	bool FWFrameSkip() // ��ŵ�ؾ��ϴ���.
	{
		return !(timer >= 0);
	}
};