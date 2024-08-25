#ifndef GAME_H
#define GAME_H

#include <ncurses.h>
#include <stdbool.h>

void initGame(WINDOW **gameWin, WINDOW **infoWin);
void updateGame(WINDOW *gameWin, WINDOW *infoWin, int *xpos, int *ypos, int *xspeed, int *yspeed, int *fuel, int *score, int *direction, int *thrust, bool *gameOver);
bool isValidLandingSpot(char ch);

#endif
