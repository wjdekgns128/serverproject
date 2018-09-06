#include "IOCPServer.h"
#include "ClientManager.h"
#include "MapData.h"
#include "NPCMonsterData.h"
#include "DB.h"
#include "Dump.h"
int main()
{
	Dump::Begin();
	bool c = DB::getInstance().Init();
	if (c)
		printf("DB init..\n");
	else
		printf("DB init 실패..\n");
	if (c)
	{
		bool d = DB::getInstance().ConnectDB(L"2012180041_jdh", L"mywork", L"wjdekgns0061");
		if (d == true)
		{
			printf("DB connect..\n");
		}
		else
			printf("DB connect 실패..\n");
	}
	GameData::getInstance().Init();
	MapData::getInstance().Init();
	NPCMonsterData::getInstance().Init();
	ClientManager::getInstance();
	DB::getInstance().LoadItmeTableData();
	IOCPServer server;

	server.StartServer();
	server.EndServer();
	Dump::End();
	return 0;
}