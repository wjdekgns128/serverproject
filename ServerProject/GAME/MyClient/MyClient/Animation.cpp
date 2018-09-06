#include "Animation.h"
#include "FWMain.h"
Animation::Animation()
{
	pSprite = new Sprite;
	type = ANIMATIONPLAYTYPE::BASE;
	tempType = ANIMATIONPLAYTYPE::BASE;

	frametime.clear();
	NowIndex = 0;
	AnimationTimer = 0;
	PlayIng = true;
}

Animation::~Animation()
{
	SAFE_DELETE(pSprite);
}
void Animation::Update(float ft)
{
	AnimationTimer += FWMain::GetInstance()->nowTimer;
}
void Animation::Entry(int id, const char * path, float ftime)
{
	pSprite->Entry(id, path);
	frametime.push_back(ftime);
	MaxIndex = frametime.size();
}
void Animation::Entry(const char* path)
{

	if (!LoadFile(path))
	{
		return;
	}
}
bool Animation::LoadFile(const char* path)
{
	ifstream f(path);
	if (!f)
		return false;
	frametime.clear();
	MaxIndex = 0;
	int in = 0;
	int type1;
	f >> type1;
	tempType = (ANIMATIONPLAYTYPE)type1;
	string pa;
	DWORD t;
	while (f >> pa >> t)
	{
		frametime.push_back((float)t / 1000.0f);
		string d = ("res/Sprite/" + pa );
		pSprite->Entry(in++,d.c_str() );
	}
	f.close();
	MaxIndex = frametime.size();
	AnimationTimer = 0;
	PlayIng = true;
	NowIndex = 0;
	return true;
}
void Animation::Render(HANDLE handle)
{
	TypeCheck();

	if (type == ANIMATIONPLAYTYPE::BASE)
	{
		RenderTypeBase(handle);
	}
	else if (type == ANIMATIONPLAYTYPE::LOOP)
	{
		RenderTypeLoop(handle);
	}
	else if (type == ANIMATIONPLAYTYPE::REVERSEBASE)
	{
		RenderTypeReversebase(handle);
	}
	else if (type == ANIMATIONPLAYTYPE::REVERSELOOP)
	{
		RenderTypeReverseLoop(handle);
	}
}
void Animation::RenderTypeBase(HANDLE handle)
{
	pSprite->Render(handle,NowIndex);
	if (PlayIng)
	{
		if (AnimationTimer >= frametime[NowIndex])
		{
			NowIndex++;
			AnimationTimer = 0.0f;
			if (NowIndex == MaxIndex)
			{
				NowIndex = 0;
				PlayIng = false;
			}
		}
	}
}
void Animation::RenderTypeLoop(HANDLE handle)
{
	pSprite->Render(handle, NowIndex);
	if (PlayIng)
	{
		if (AnimationTimer >= frametime[NowIndex])
		{
			NowIndex++;
			AnimationTimer = 0.0f;
			if (NowIndex == MaxIndex)
			{
				NowIndex = 0;
			}
		}
	}
}
void Animation::RenderTypeReversebase(HANDLE handle)
{
	pSprite->Render(handle, (MaxIndex-1) - NowIndex);
	if (PlayIng)
	{
		if (AnimationTimer >= frametime[(MaxIndex - 1) - NowIndex])
		{
			NowIndex++;
			AnimationTimer = 0.0f;
			if (NowIndex == MaxIndex)
			{
				NowIndex = 0;
				PlayIng = false;

			}
		}
	}
}
void Animation::RenderTypeReverseLoop(HANDLE handle)
{
	pSprite->Render(handle, (MaxIndex - 1) - NowIndex);
	if (PlayIng)
	{
		if (AnimationTimer >= frametime[(MaxIndex - 1) - NowIndex])
		{
			NowIndex++;
			AnimationTimer = 0.0f;
			if (NowIndex == MaxIndex)
			{
				NowIndex = 0;

			}
		}
	}
}
void Animation::TypeCheck()
{
	if (NowIndex != 0)
		return;
	type = tempType;
}
