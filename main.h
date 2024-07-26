#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

struct entity {
	int row;
	int col;
	int height;
	int width;

};

struct state {
	struct entity plr;
	struct entity mb;
	struct entity pb;
	struct entity pc;
	struct entity pokeball;
	struct entity ub;
	int timer;
};

int collision(struct entity e1, struct entity e2);
void initializePlay(void);

#endif
