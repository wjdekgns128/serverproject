#pragma once
#include "MyInclude.h"
#include "SingeTon.h"
#include "protocol.h"
#include "ItemData.h"
#include "Client.h"
#include "MyStruct.h"
#include <mutex>
class DB : public SingeTon<DB>
{
public:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLRETURN retcode;
public:
	DB();
	virtual ~DB();
public:
	void getInventorySlotId(char* name, int* temp);
	void getInventoryCount(char* name, int* temp);
	void getInventoryEM(char* name, int* temp);
public:
	bool Init();
	bool ConnectDB(const wchar_t* DBName, const wchar_t* userName, const wchar_t* password);
	 UserData getUserData(char* name,bool* check);
	void UpdateUserData(const UserData& updatedata);

	void LoadItmeTableData(); // 디비에있는 아이템 테이블 로드
	void UpdateUserSlot(int id,int* arr);
	void UpdateUserCount(int id,int* arr);
	void UpdateUserSM(int id, int* arr);

private:
	void InitInventory(char* name);
	
};