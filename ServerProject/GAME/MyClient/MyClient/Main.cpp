#include "Sprite.h"
#include <stdio.h>
#include "FWMain.h"
#include <crtdbg.h>
#include <Windows.h>
#include "FWFrameTimer.h"
#include "FWServer.h"
#include "Debug.h"
#include <conio.h>
#include <thread>
#include "GameData.h"
void MainFrame();
BOOL CtrlHandler(DWORD fdwCtrlType);
FWMain*  p = FWMain::GetInstance();
FWServer*  server = FWServer::GetInstance();
int getKey()
{
	if (kbhit())
	{
		return getch();
	}
	return '\0';
}
void MainFrame()
{
	FWFrameTimer fwtimer;
	fwtimer.setFPS(p->fps);
	DWORD startTime = GetTickCount();

	while (!p->Exit)
	{

		float t = (GetTickCount() - startTime) * 0.001f;
	
		/*   빠르게 실행*/
		if (fwtimer.FWFrameUpdate(t))
		{
			// 고정 프레임으로 실행
			p->nowTimer = t;
			int key = getKey();
			if (key != '\0')
				p->getNowScene()->KeyBoard(key);
			if (p->Exit)
				break;
			p->getNowScene()->Update(t);
			startTime = GetTickCount();

		}

		if (fwtimer.FWFrameSkip())
		{
			// 가끔가다가 스킵.
			p->Clear();
			p->getNowScene()->Render(p->getNowHandle());
			p->SwapBuffer();

		}


	}
	GameData::GetInstance()->QSlotSave();
	cs_packet_logout p;
	p.size = sizeof(cs_packet_logout);
	p.type = CS_LOGINOUT;
	server->SendPacket(&p);
}
void ServerFrame()
{
	while (!p->Exit)
	{
		if (!server->ServerRun())
		{
			p->Exit = true;
			break;
		}
	}
}
int main()
{
	p->Init();
	GameData::GetInstance()->Init();
	BOOL fSuccess = SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	p->Exit = !server->ServerInit();
	thread m{ MainFrame };
	thread m1{ ServerFrame };
	m.join();
	m1.join();
	
	server->ServerClose();
}
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL+C signal. 
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT: // CTRL+CLOSE: confirm! that the user wants to exit. 
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		GameData::GetInstance()->QSlotSave();
		cs_packet_logout p;
		p.size = sizeof(cs_packet_logout);
		p.type = CS_LOGINOUT;
		server->SendPacket(&p);
	default:
		return FALSE; //TRUE로하면 에러가나고 FALSE하면 걍 잘꺼지고잘됨.
	}
}
