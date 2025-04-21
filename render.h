#ifndef RENDER_H
#define RENDER_H

#include <ncurses.h>

void clearLander(WINDOW *gameWin, int xpos, int ypos, int direction);
void renderGame(WINDOW *gameWin, WINDOW *infoWin, int xpos, int ypos, int xspeed, int yspeed, int fuel, int score, int direction, int thrust);

#endif // RENDER_H

// input.h - Add missing header declarations
#ifndef INPUT_H
#define INPUT_H

void handleInput(int *direction, int *thrust, int *fuel);

#endif // INPUT_H