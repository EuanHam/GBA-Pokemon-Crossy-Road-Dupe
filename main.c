#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"

#include "images/pokemonCenter.h"
#include "images/masterball.h"
#include "images/pokeball.h"
#include "images/ultraball.h"
#include "images/title.h"
#include "images/loss.h"
#include "images/win.h"

enum gba_state {
  START,
  PLAY,
  WIN,
  LOSE,
};

struct entity *plr, *mb, *ub, *pb, *pc;
struct state cs, ps;
int plrWidth = 10;
int plrHeight = 10;

int enemyWidth = 15;
int enemyHeight = 15;
int timerCap = 60;

int collision(struct entity e1, struct entity e2) {
    return (
        e1.col < e2.col + e2.width && e1.col + e1.width > e2.col
        && e1.row < e2.row + e2.height && e1.row + e1.height > e2.row
    );
}

void checkPlayerInputs(u32 currentButtons) {
    if (KEY_DOWN(BUTTON_UP, currentButtons)) {
        if (plr->row > 0) {
            plr->row -= 1;
        }
    }
    if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
        if (plr->col > 0) {
            plr->col -= 1;
        }
    }
    if (KEY_DOWN(BUTTON_DOWN, currentButtons)) {
        if (plr->row < HEIGHT - plrHeight) {
            plr->row += 1;
        }
    }
    if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
        if (plr->col < WIDTH - plrWidth) {
            plr->col += 1;
        }
    }
}

void initializePlay(void) {
    plr = &cs.plr;
    plr->row = HEIGHT / 2;
    plr->col = 10;
    plr->width = plrWidth;
    plr->height = plrHeight;

    mb = &cs.mb;
    mb->row = HEIGHT / 2;
    mb->col = 160;
    mb->width = enemyWidth;
    mb->height = enemyHeight;

    ub = &cs.ub;
    ub->row = HEIGHT / 2 + 20;
    ub->col = 100;
    ub->width = enemyWidth;
    ub->height = enemyHeight;

    pb = &cs.pb;
    pb->row = HEIGHT / 2 - 20;
    pb->col = 45;
    pb->width = enemyWidth;
    pb->height = enemyHeight;

    pc = &cs.pc;
    pc->row = HEIGHT / 2;
    pc->col = 225;
    pc->width = 15;
    pc->height = 15;

    cs.timer = 20;
}

int main(void) {
  REG_DISPCNT = MODE3 | BG2_ENABLE;

  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  enum gba_state state = START;

  while (1) {
    currentButtons = BUTTONS;
    ps = cs;
    waitForVBlank();
    switch (state) {
      case START:
        initializePlay();
        drawFullScreenImageDMA(title);
        {
        char buffer[51];
        drawString(150, 5, buffer, BLACK);
        }

        if (KEY_JUST_PRESSED(BUTTON_START, currentButtons, previousButtons))) {
            state = PLAY;
            fillScreenDMA(BLUE);
        }
        break;
      case PLAY:
        drawRectDMA(ps.plr.row, ps.plr.col, ps.plr.width, ps.plr.height, BLUE);
        drawRectDMA(ps.mb.row, ps.mb.col, ps.mb.width, ps.mb.height, BLUE);
        drawRectDMA(ps.pb.row, ps.pb.col, ps.pb.width, ps.pb.height, BLUE);
        drawRectDMA(ps.ub.row, ps.ub.col, ps.ub.width, ps.ub.height, BLUE);
        
        checkPlayerInputs(currentButtons);

        if (mb->row <= 0 - enemyHeight) {
            mb->row = HEIGHT;
        } else {
            mb->row -= 2;
        }

        if (ub->row <= 0 - enemyHeight) {
            ub->row = HEIGHT;
        } else {
            ub->row -= 3;
        }

        if (pb->row <= 0 - enemyHeight) {
            pb->row = HEIGHT;
        } else {
            pb->row -= 1;
        }

        drawRectDMA(plr->row, plr->col, plr->width, plr->height, YELLOW);

        drawImageDMA(mb->row, mb->col, mb->width, mb->height, masterball);
        drawImageDMA(ub->row, ub->col, ub->width, ub->height, ultraball);
        drawImageDMA(pb->row, pb->col, pb->width, pb->height, pokeball);
        drawImageDMA(pc->row, pc->col, pc->width, pc->height, pokemonCenter);

        timerCap--;
        if (timerCap <= 0) {
            timerCap = 60;
            cs.timer--;
            if (cs.timer <= 0) {
                state = LOSE;
                drawFullScreenImageDMA(loss);
                break;
            }
        }

        {
        char buffer[51];
        snprintf(buffer, sizeof(buffer), "Timer: %d", ps.timer);
        drawString(150, 5, buffer, BLUE);
        }
        {
        char buffer[51];
        snprintf(buffer, sizeof(buffer), "Timer: %d", cs.timer);
        drawString(150, 5, buffer, BLACK);
        }

        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons))) {
            state = START;
        }

        if (collision(*mb, *plr) || collision(*ub, *plr) || collision(*pb, *plr)) {
            state = LOSE;
            drawFullScreenImageDMA(loss);
        }
        if (collision(*pc, *plr)) {
            state = WIN;
            drawFullScreenImageDMA(win);
        }

        break;
      case WIN:
        {
        char buffer[51];
        drawString(150, 5, buffer, BLUE);
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons))) {
            state = START;
        }
        break;
      case LOSE:
        {
        char buffer[51];
        drawString(150, 5, buffer, BLUE);
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons))) {
            state = START;
        }
        break;
    }

    previousButtons = currentButtons;
  }

  return 0;
}
