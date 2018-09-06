#pragma once
#include <algorithm>
#include <map>
#include "SendManager.h"
#include "ClientManager.h"
#include <fstream>
class Process
{
private:
	SendManager msg;
	mutex		testdp;
	ofstream		dp;
private:
	typedef void (Process::*packetfunction)(int, char* packet);
	typedef std::map<char, packetfunction>  PakcetMap;
	PakcetMap packetprocess;
public:
public:
	Process();
	~Process();
public:
	
	void ProcessPacket(int id, char *packet);
	void PlayerMove(int id, char* packet);
	void PlayerChat(int id, char* packet);
	void LoginPlayer(int id, char* packet);
	void LoginOutPlayer(int id, char* packet);
	void BuyItem(int id, char* packet);
	void SaleItem(int id, char* packet);

	void ApplyItem(int id, char* packet);
	void USEHp(int id, char* packet);

	void ReleseItem(int id, char* packet);


	void PlayerAttack(int id, char* packet); // 어택패킷
};