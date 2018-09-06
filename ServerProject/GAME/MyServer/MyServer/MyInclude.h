#pragma once
#include <WinSock2.h>
#include <windows.h>   // include important windows stuff
#include <sqlext.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <thread>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <array>
#include <queue>
#include <chrono>
#include <set>
#include "ErrorLog.h"

extern "C"
{
#include "lua/include/lua.h"
#include "lua/include/lauxlib.h"
#include "lua/include/lualib.h"

}
using namespace std;
using namespace chrono;