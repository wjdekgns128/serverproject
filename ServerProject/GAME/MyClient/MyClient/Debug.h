#pragma once
#include <stdio.h>
#include <stdlib.h>

namespace Debug
{
	
	static void Log(const char* c)
	{
		OutputDebugStringA(c);

	}
};