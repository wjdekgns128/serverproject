#pragma once
#include "MyInclude.h"
#include "MyDefine.h"
#include "Sprite.h"
enum ANIMATIONPLAYTYPE
{
	BASE = 0,
	LOOP,
	REVERSEBASE,
	REVERSELOOP,
};
class Animation
{
private:
	int			  NowIndex;
	int			  MaxIndex;
	vector<float> frametime;
	Sprite*		  pSprite;
	float		  AnimationTimer;
	ANIMATIONPLAYTYPE type;
	ANIMATIONPLAYTYPE tempType;
	bool		 PlayIng;
public:
	Animation();
	~Animation();
public:
	void Entry(int id, const char* path,float ftime);
	void Entry(const char* path);
public:
	void Update(float ft);
	void Render(HANDLE handle);
private:
	void RenderTypeBase(HANDLE handle);
	void RenderTypeLoop(HANDLE handle);
	void RenderTypeReversebase(HANDLE handle);
	void RenderTypeReverseLoop(HANDLE handle);
	void TypeCheck();
public:
#pragma region set
	void setType(ANIMATIONPLAYTYPE newtype)
	{
		AnimationTimer = 0.0f;
		tempType = newtype;
	}
	void setNowIndex(int index)
	{
		NowIndex = index;
	}
	void Stop()
	{
		AnimationTimer = 0.0f;
		PlayIng = false;
	}
	void ReStart()
	{
		AnimationTimer = 0.0f;
		PlayIng = true;
	}
	void Start()
	{
		NowIndex = 0;
		AnimationTimer = 0.0f;
		PlayIng = true;
	}
	void setFrameTimer(int id,float t)
	{
		frametime[id] = t;
	}
#pragma endregion


	
#pragma region get
	ANIMATIONPLAYTYPE getType() const
	{
		return type;
	}
	int getNowIndex()
	{
		return NowIndex ;
	}
	int getMaxIndex()
	{
		return  MaxIndex;
	}
#pragma endregion
private:
	bool LoadFile(const char* path);


};