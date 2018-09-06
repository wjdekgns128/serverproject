#include "ChattingSystem.h"
#include "FWServer.h"

ChattingSystem::ChattingSystem()
{
	pSprite = new Sprite;
	open = false;
	vChattingList.clear();
	now = 0;
	strcpy(Name, FWServer::GetInstance()->MyName);
}
ChattingSystem::~ChattingSystem()
{
	SAFE_DELETE(pSprite);
}
void ChattingSystem::Render(const HANDLE& handle)
{
	for(int i = 40 ; i < 50;++i)
	pSprite->Render(handle, "|", 56, i, COLOR::LIGHT_WHITE, COLOR::PURPLE);

	pSprite->Render(handle, "채   팅   창", 22, 40,COLOR::LIGHT_WHITE,COLOR::PURPLE);
	for (int i = 41 + now; i < 49 + now; ++i)
	{
		if (vChattingList.size() + 41 <= i)
			break;
		pSprite->Render(handle, vChattingList[i - 41].vS.c_str(), 0, i-now, COLOR::LIGHT_WHITE, COLOR::BLACK);

	}

	if (open)
	{
		pSprite->Render(handle, ":", 0, 49, COLOR::LIGHT_WHITE, COLOR::LIGHT_RED);
		pSprite->Render(handle, NowChatting.c_str(), 1, 49, COLOR::WHITE, COLOR::BLACK);
	}
}
void ChattingSystem::RecvMessage(char* pakcet)
{
	sc_packet_chat * my_packet = (sc_packet_chat*)pakcet;
	if (vChattingList.size() - 8 == now)
	now++;

	int len = WideCharToMultiByte(CP_ACP, 0, my_packet->message, -1, NULL, 0, NULL, NULL);
	char p[MAX_STR_SIZE] = { 0, };
	WideCharToMultiByte(CP_ACP, 0, my_packet->message, -1, p, len, NULL, NULL);
	vChattingList.emplace_back(my_packet->id, p);
}
void ChattingSystem::KeyBoard(int key)
{
	if (key == 13)
	{
		if (open && NowChatting.size() != 0)
		{
			char c[MAX_STR_SIZE] = { 0, };
			sprintf(c, "%s : ", FWServer::GetInstance()->MyName);

			cs_packet_chat p;
			p.size = sizeof(cs_packet_chat);
			p.type = CS_CHAT;
			ZeroMemory(p.message, MAX_STR_SIZE);
			strcat(c, NowChatting.c_str());
			int len = strlen(c);
			MultiByteToWideChar(CP_ACP, 0, (LPCSTR)c, -1, p.message, len);
			p.message[len] = NULL;
			FWServer::GetInstance()->SendPacket((char*)&p);
			NowChatting.clear();
		}
		open = !open;

	}
	else if (open == false)
	{
		if (key == 72) // 업
		{
			now--;
			if (now < 0)
				now = 0;
		}
		if (key == 80)//다운
		{
			if(vChattingList.size() >= 8)
			if (vChattingList.size() -8 > now)
			now++;
		}
	}
	else
	{
		
		
		if (key == 8)
		{
			if(NowChatting.size() != 0)
			NowChatting.erase(NowChatting.size()-1);
		}
		else
		{
			if (NowChatting.size() < 30)
			{
				NowChatting.push_back(key);
			}
		}
	}
}
