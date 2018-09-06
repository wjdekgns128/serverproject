#pragma once
#include "BaseObject.h"
#include "protocol.h"
#include <chrono>
using namespace std;
using namespace chrono;
class Player : public BaseObject
{
public:
	 int				server_id; // 서버로부터받은아이디
private:
	static COLOR   m_Color[2];
	int				id;
	high_resolution_clock::time_point last_move_time;

	
public:
	Player();
	Player(int id,int x, int y,int serverid,int type);


	~Player();
public:
	void Update(float tick);
	void Render(const HANDLE& handle, int mapLeft, int mapTop);
	void KeyBoard(int key);
public:
public:
	int getId() const
	{
		return id;
	}
public:
};