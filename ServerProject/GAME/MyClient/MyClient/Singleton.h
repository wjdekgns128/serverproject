#pragma once
#include <stdio.h>


template<typename T>

class Singleton
{
public:
	static T* GetInstance()
	{
		static T ins;
		return &ins;
	}
private:
};