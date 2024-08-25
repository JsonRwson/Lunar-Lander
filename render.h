#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>

void renderGame(WINDOW *gameWin, WINDOW *infoWin, int xpos, int ypos, int xspeed, int yspeed, int fuel, int score, int direction, int thrust);

#endif
