#pragma once
#define SAFE_DELETE(p) {if(p){delete (p); (p) = 0;}}
#define SAFE_DELETE_ARRAY(p) {if(p){delete[] (p); (p) = 0;}}

enum COLOR {
	BLACK = 0, BLUE, GREEN, JADE, RED, PURPLE, YELLOW, WHITE, GRAY,
	LIGHT_BLUE, LIGHT_GREEN, LIGHT_JADE, LIGHT_RED, LIGHT_PURPLE, LIGHT_YELLOW, LIGHT_WHITE
};

