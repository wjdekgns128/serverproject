#include "DB.h"
#include <stdio.h>
DB::DB()
{
}
DB::~DB()
{

	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	SQLFreeHandle(SQL_HANDLE_ENV, henv);
}
void DB::getInventoryEM(char* name, int* temp)
{
retry:

	SQLHSTMT ds;

	SQLINTEGER szSlot[3];
	SQLLEN Slot[3];
	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.getinventoryEM %S", name);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

	
		for (int i = 0; i < 3; ++i)
		{
			retcode = SQLBindCol(ds, i + 1, SQL_INTEGER, &szSlot[i], 100, &Slot[i]);
		}
		for (int i = 0; ; i++) {
			retcode = SQLFetch(ds);
		
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				goto retry;
			}
			else if(retcode == SQL_NO_DATA)
			{
				break;
			}
		}

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	
	for (int i = 0; i < 3; ++i)
	{
		temp[i] = szSlot[i];
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);

}

bool DB::Init()
{
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	// Set the ODBC version environment attribute  
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
		// Allocate connection handle  
		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
			// Set login timeout to 5 seconds  
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)20, 0);
				return true;
				// Connect to data source  
			}
			SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		}
	}
	SQLFreeHandle(SQL_HANDLE_ENV, henv);

	return false;
}
void DB::UpdateUserSM(int id, int* arr)
{
retry:

	SQLHSTMT ds;

	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.UpdateUserSM %d,%d,%d,%d", id,arr[0], arr[1], arr[2]);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

	}
	
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);


}
void DB::UpdateUserSlot(int id,int* arr)
{
retry:

	SQLHSTMT ds;

	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.UpdateUserSlot %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",id,
		arr[0], arr[1], arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9], arr[10], arr[11], arr[12], arr[13], arr[14], arr[15]);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);

}
void DB::UpdateUserCount(int id, int* arr)
{
retry:

	SQLHSTMT ds;
	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.UpdateUserCount %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",id,
		arr[0],arr[1],arr[2],arr[3],arr[4],arr[5],arr[6],arr[7],arr[8],arr[9],arr[10],arr[11],arr[12],arr[13],arr[14],arr[15]);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);

}
void DB::UpdateUserData(const UserData& updatedata)
{
retry:

	SQLHSTMT ds;

	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.UpdateUserData %d,%d,%d,%d,%d,%d,%d", updatedata.Hp,updatedata.Exp,updatedata.Lev,updatedata.x,updatedata.y, updatedata.id, updatedata.money);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
	
	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);

}
void DB::InitInventory(char* name)
{
retry:

	SQLHSTMT ds;

	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.InitInventory %S", name);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);

}
void DB::getInventoryCount(char* name, int* temp)
{
retry:

	SQLHSTMT ds;

	SQLINTEGER szSlot[16];
	SQLLEN Slot[16];
	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.getinventroycount %S", name);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		for (int i = 0; i < 16; ++i)
		{
			retcode = SQLBindCol(ds, i + 1, SQL_INTEGER, &szSlot[i], 100, &Slot[i]);
			if (retcode == SQL_ERROR || retcode == SQL_STILL_EXECUTING)
			{
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				goto retry;

			}
		}
		for (int i = 0; ; i++) {
			retcode = SQLFetch(ds);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				goto retry;

			}
			else if(retcode == SQL_NO_DATA)
				break;
			else
			{

			}
		}

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	for (int i = 0; i < 16; ++i)
		temp[i] = szSlot[i];
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);
}
void DB::getInventorySlotId(char* name,int* temp)
{
	retry:
	SQLHSTMT ds;

	SQLINTEGER szSlot[16];
	SQLLEN Slot[16];
	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.getinventorySlot %S", name);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		for (int i = 0; i < 16; ++i)
		{
			retcode = SQLBindCol(ds, i + 1, SQL_INTEGER, &szSlot[i], 100, &Slot[i]);
			if (retcode == SQL_ERROR || retcode == SQL_STILL_EXECUTING)
			{
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				goto retry;

			}
		}
		for (int i = 0; ; i++) {
			retcode = SQLFetch(ds);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			{
				printf("getInventorySlotId\n");
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				goto retry;
			}
			else if (retcode == SQL_NO_DATA)
			{
				break;
			}
			else
			{

			}
		}

	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	for (int i = 0; i < 16; ++i)
		temp[i] = szSlot[i];
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);
}
void DB::LoadItmeTableData() // 디비에있는 아이템 테이블 로
{

	SQLHSTMT ds;

	SQLINTEGER szID, szType, szLevel, szPrice, szEffect;
	SQLLEN id, type, level, price, effect;
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)L"EXEC dbo.getItems", SQL_NTS);
	retcode = SQLExecute(ds);
	int c = 0;
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {

		retcode = SQLBindCol(ds, 1, SQL_INTEGER, &szID, 100, &id);
		retcode = SQLBindCol(ds, 2, SQL_INTEGER, &szType, 100, &type);
		retcode = SQLBindCol(ds, 3, SQL_INTEGER, &szLevel, 100, &level);
		retcode = SQLBindCol(ds, 4, SQL_INTEGER, &szPrice, 100, &price);
		retcode = SQLBindCol(ds, 5, SQL_INTEGER, &szEffect, 100, &effect);
		for (int i = 0; ; i++) {
			retcode = SQLFetch(ds);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			{
				printf("??? 아이템 테이블 로드 뭔가 실패함\n");
			}
			else if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
				ItemData::getInstance().Items[c].ItmeID = szID;
				ItemData::getInstance().Items[c].ItmeType = szType;
				ItemData::getInstance().Items[c].ItmeLevel = szLevel;
				ItemData::getInstance().Items[c].ItmePrice = szPrice;
				ItemData::getInstance().Items[c].ItmeEffect = szEffect;
				c++;
				printf("%d번쨰 아이템 로드 성공.\n", c);
			}
			else
				break;
		}

	}

	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);
}
bool DB::ConnectDB(const wchar_t * DBName, const wchar_t * userName, const wchar_t * password)
{
	retcode = SQLConnect(hdbc, (SQLWCHAR*)DBName, SQL_NTS, (SQLWCHAR*)userName, SQL_NTS, (SQLWCHAR*)password, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		return true;
	}
	else
		return false;
}

 UserData DB::getUserData(char* name,bool* check)
{
 retry:
	 SQLHSTMT ds;

	 *check = false;
	UserData data;
	SQLLEN id, namecd, hp, lev, exp, x, y,Money;
	SQLWCHAR szName[45];
	SQLINTEGER szLevel, szID,szX,szY,szHp,szExe,szMoney;

	WCHAR c[256] = { NULL, };
	wsprintf(c, L"EXEC dbo.getUser %S", name);
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
	retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
	retcode = SQLExecute(ds);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
		retcode = SQLBindCol(ds, 1, SQL_INTEGER, &szID, 100, &id);
	
		retcode = SQLBindCol(ds, 2, SQL_C_CHAR, szName, 100, &namecd);
	
		retcode = SQLBindCol(ds, 3, SQL_INTEGER, &szLevel, 100, &lev);
	
		retcode = SQLBindCol(ds, 4, SQL_INTEGER, &szExe, 100, &exp);
		retcode = SQLBindCol(ds, 5, SQL_INTEGER, &szX, 100, &x);
		retcode = SQLBindCol(ds, 6, SQL_INTEGER, &szY, 100, &y);
		retcode = SQLBindCol(ds, 7, SQL_INTEGER, &szHp, 100, &hp);
		retcode = SQLBindCol(ds, 8, SQL_INTEGER, &szMoney, 100, &Money);
		for (int i = 0; ; i++) {
			retcode = SQLFetch(ds);
			if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
			{
				SQLCancel(ds);
				SQLFreeHandle(SQL_HANDLE_STMT, ds);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &ds);
				retcode = SQLPrepare(ds, (SQLWCHAR*)c, SQL_NTS);
				retcode = SQLExecute(ds);

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
					retcode = SQLBindCol(ds, 1, SQL_INTEGER, &szID, 100, &id);
					retcode = SQLBindCol(ds, 2, SQL_C_CHAR, szName, 100, &namecd);
					retcode = SQLBindCol(ds, 3, SQL_INTEGER, &szLevel, 100, &lev);
					retcode = SQLBindCol(ds, 4, SQL_INTEGER, &szExe, 100, &exp);
					retcode = SQLBindCol(ds, 5, SQL_INTEGER, &szX, 100, &x);
					retcode = SQLBindCol(ds, 6, SQL_INTEGER, &szY, 100, &y);
					retcode = SQLBindCol(ds, 7, SQL_INTEGER, &szHp, 100, &hp);
					retcode = SQLBindCol(ds, 8, SQL_INTEGER, &szMoney, 100, &Money);

					for (int i = 0; ; i++) {
						retcode = SQLFetch(ds);
						if (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO)
						{
						}
						else
						{
							*check = true;
							break;

						}
					
					}
				}
			}
			else
			{
				break;
			}
		}
	
	}
	else
	{
		SQLCancel(ds);
		SQLFreeHandle(SQL_HANDLE_STMT, ds);
		goto retry;
	}
	SQLCancel(ds);
	SQLFreeHandle(SQL_HANDLE_STMT, ds);
	if (*check == true)
	{
		InitInventory(name);
	}

	data.id = szID;
	//data.Lev = rand()%50;
	data.Lev = szLevel;
	data.Exp = szExe;
	//data.x = rand()%300;
	//data.y = rand()%300;
	data.Hp = szHp;
	data.x = szX;
	data.y = szY;
	data.money = szMoney;
	//data.money = rand()%100000;
	strcpy(data.Name, name);
	return data;
}
