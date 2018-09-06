#include "AStar.h"
#include "MapData.h"
#include "ClientManager.h"
void AStar::FindAStar(int id, int other,int* outx,int* outy)
{

	ClientManager& cm = ClientManager::getInstance();
	unordered_set<int> SectorSet;
	int sx = cm.clients[id].Data.x;
	int sy = cm.clients[id].Data.y;
	int endx = cm.clients[other].Data.x;
	int endy = cm.clients[other].Data.y;
	if (id == other)
	{
		endx = cm.clients[id].RespawnX;
		endy = cm.clients[id].RespawnY;

	}
	cm.getSctor(sx, sy, SectorSet);

	NODE*			deletTemp[400] = { NULL, };
	int index = 0;
	MapData& p = MapData::getInstance();
	int count = 0;
	vector<NODE> close;
	multiset<NODE, cmp> open;

	close.reserve(100);
	open.insert(NODE(sx, sy, 0,0));
	while (!open.empty())
	{
		if (count >= 10)
			break;
		NODE Temp[4];
		NODE top = *open.begin();
		open.erase(open.begin());
		if (top.x == endx && top.y == endy)
		{
			break;
		}
		close.push_back(top);

		if (top.x - 1 >= 0)
		{
			if (!p.Map[top.x - 1][top.y] && !cm.isColl(id,top.x-1,top.y,SectorSet,other))
				Temp[0] = FindNode(sx, sy, endx, endy, top.x - 1, top.y, top.g);

		}
		if (top.y - 1 >= 0)
		{
			if (!p.Map[top.x][top.y - 1] && !cm.isColl(id, top.x, top.y-1, SectorSet, other))
				Temp[1] = FindNode(sx, sy, endx, endy, top.x, top.y - 1, top.g);
		}
		if (top.x + 1 < BOARD_WIDTH)
		{
			if (!p.Map[top.x+1][top.y] && !cm.isColl(id , top.x + 1, top.y,SectorSet, other))
				Temp[2] = FindNode(sx, sy, endx, endy, top.x + 1, top.y, top.g);
		}
		if (top.y + 1 < BOARD_HEIGHT)
		{
			if (!p.Map[top.x][top.y + 1] && !cm.isColl(id, top.x, top.y + 1, SectorSet, other))
				Temp[3] = FindNode(sx, sy, endx, endy, top.x, top.y + 1, top.g);
		}
		for (int i = 0; i < 4; ++i)
		{
			if (Temp[i].g != -999)
			{
				NODE* dd = new NODE(top.x, top.y, top.g, top.h);
				Temp[i].p = dd;
				dd->p = top.p;
				deletTemp[index++] = dd;
				if (Temp[i].x == endx && Temp[i].y == endy)
					Temp[i].f += -200;
				auto t = find_if(close.begin(), close.end(), [&](const NODE& c1)
				{
					return Temp[i].x == c1.x && Temp[i].y == c1.y;
				});
				auto t1 = find_if(open.begin(), open.end(), [&](const NODE& c1)
				{
					return Temp[i].x == c1.x && Temp[i].y == c1.y;
				});

				if (t != close.end())
				{
					if (t->f  > Temp[i].f)
					{
						close.erase(t);
						open.insert(Temp[i]);
						continue;
					}
				}
				if (t1 != open.end())
				{
					if (t1->f > Temp[i].f)
					{
						open.erase(t1);
						open.insert(Temp[i]);
						continue;
					}
				}
				if (t == close.end() && t1 == open.end())
				{
					open.insert(Temp[i]);
				}
			}
		}
		count++;
	}
	if (close.size() <= 1)
	{
		*outx = endx;
		*outy = endy;
	}
	else
	{
		NODE* pc = &close[close.size() - 1];
		for (; pc->p != NULL; pc = pc->p)
		{
			
				if (pc->p->x == sx && pc->p->y == sy)
					break;
		}
		*outx = pc->x;
		*outy = pc->y;
	}
	for (int i = 0; i < index; ++i)
	{
		if (deletTemp[i] != NULL)
			delete deletTemp[i];
		deletTemp[i] = NULL;
	}
}
NODE AStar::FindNode(int sx, int sy, int endx, int endy, int x, int y,int g)
{
	int h = ((endx - x) * (endx - x)) + ((endy - y) * (endy - y));
	int g1 = g +1;

	return NODE(x, y, g1, h);
}