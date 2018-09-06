#pragma once
#include <set>
#include "MyInclude.h"

struct NODE
{
	int x;
	int y;
	int f;
	int g;
	int h;
	struct NODE* p;
	NODE()
	{
		x = -999;
		y = -999;
		f = -999;
		g = -999;
		h = -999;
		p = NULL;

	}
	NODE(int _x, int _y,int _g,int _h)
	{
		x = _x;
		y = _y;
		f = _g + _h;
		g = _g;
		h = _h;
		p = NULL;

	}
};
struct cmp {
	bool operator()(const NODE& t, const NODE& u) const {
		return t.f  < u.f;
	}
};
class AStar
{
private:

public:
	void FindAStar(int id, int ohter, int* outx, int* outy);
	NODE FindNode(int sx, int sy, int endx, int endy, int x, int y,int g);
};