#pragma once
enum OBJECTTYPE
{
	EMPTY = 0,
	HERO,
	MONA,
	MONB,
	MONC,
	MOND,
	NPC,
};
enum MY_ITME_TYPE // 디비 타입에 따른 아이템
{
	ATK = 0,
	DEF,
	HPUP,
	PLUEHP,
};
#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   300
#define BOARD_HEIGHT  300

#define VIEW_RADIUS   12

#define MAX_USER 500

#define NPC_START  500
#define NUM_OF_NPC  1200

#define MY_SERVER_PORT  4000

#define MAX_STR_SIZE  60

#define CS_ATTACK 24
#define CS_LOGIN 1
#define CS_LOGINOUT 2
#define CS_MOVE 3
#define CS_CHAT 5
#define CS_ITEM_BUY 11
#define CS_ITEM_SALE 12
#define CS_USE_HP 13
#define CS_APPLY 20
#define CS_RELEASE 21
#define SC_LOGIN_OK 1
#define SC_LOGIN_FAIL 2
#define SC_POS           3
#define SC_PUT_PLAYER    7
#define SC_REMOVE_PLAYER 6
#define SC_CHAT		4

#define SC_ITEM_INDEX 9
#define SC_ITEM_COUNT 10

#define SC_ITEM_BUYFAIL 16
#define SC_ITEM_BUYOK 15
#define SC_ITEM_SALEOK 17
#define SC_USE_HPOK 18
#define SC_EM 19
#define SC_APPLY_OK 21
#define SC_APPLY_FAIL 22
#define SC_ATTACK_OK 23
#define SC_STAT_CHANGE 25
#define SC_MONSET_ATTACK 29
#define SC_HERO_AUTO_HPUP 31
#pragma pack (push, 1)
struct cs_packet_attack
{
	BYTE size;
	BYTE type;
};
struct cs_packet_release
{
	BYTE size;
	BYTE type;
	BYTE slotIndex; // 몇번째 슬롯에있는 아이템을 해제할건지?
};
struct cs_packet_apply
{
	BYTE size;
	BYTE type;
	BYTE slotIndex; // 몇번째 슬롯에 있는 아이템 장착했는지?
};
struct cs_packet_useHp
{
	BYTE size;
	BYTE type;
	BYTE slotIndex; // 몇번째 슬롯에 있는 포션을 사용했는지?
};
struct cs_packet_buyitem
{
	BYTE size;
	BYTE type;
	BYTE itemIndex;
};
struct cs_packet_saleitem
{
	BYTE size;
	BYTE type;
	BYTE saleslotindex;
};
struct cs_packet_login
{
	BYTE size;
	BYTE type;
	WCHAR Userid[MAX_STR_SIZE];
};
struct cs_packet_logout
{
	BYTE size;
	BYTE type;
};
struct cs_packet_move {
	BYTE size;
	BYTE type;
	BYTE dir;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};
struct sc_packet_loginok
{
	BYTE size;
	BYTE type;
	WORD id;
	SHORT x;
	SHORT y;
	WORD hp;
	BYTE level;
	DWORD exp;
	DWORD money;
};
struct sc_packet_loginfail
{
	BYTE size;
	BYTE type;
};
struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	SHORT x;
	SHORT y;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	BYTE objecttype;
	WORD id;
	SHORT x;
	SHORT y;


};
struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};
struct sc_packet_itemCount
{
	BYTE size;
	BYTE type;
	BYTE count[16];
};
struct sc_packet_itemslotid
{
	BYTE size;
	BYTE type;
	BYTE index[16];
};
struct sc_packet_itembuyok
{
	BYTE size;
	BYTE type;
	BYTE buyitmeID;
	BYTE SlotNumber;
	DWORD money;
};
struct sc_packet_itembuyFail
{
	BYTE size;
	BYTE type;
	BYTE buyitmeID;
};
struct sc_packet_itemsaleok
{
	BYTE size;
	BYTE type;
	BYTE saleitemID;
	BYTE saleslot;
	DWORD money;
};
struct sc_packet_hpok
{
	BYTE size;
	BYTE type;
	BYTE Slot;
};
struct sc_packet_EM
{
	BYTE size;
	BYTE type;
	BYTE em[3];
};
struct sc_packet_EM_OK
{
	BYTE size;
	BYTE type;
	BYTE applyslot;
	BYTE check; //0이면 장착성공 ,1이면 해제 성공
};
struct sc_packet_EM_FALI
{
	BYTE size;
	BYTE type;
	BYTE applysolt;
};
struct sc_packet_ATTACK_OK
{
	BYTE size;
	BYTE type;
	WORD moneyID;
	WORD hp;

};
struct sc_packet_stat_change
{
	BYTE size;
	BYTE type;
	BYTE level;
	DWORD exp;
	DWORD money;
};
struct sc_packet_monster_attack
{
	BYTE size;
	BYTE type;
	WORD monsterid; //공격한 몬스터 아이디
};
struct sc_packet_hero_autohp
{
	BYTE size;
	BYTE type;
	WORD hp;
};
#pragma pack (pop)