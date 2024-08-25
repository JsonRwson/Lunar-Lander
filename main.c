#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "game.h"
#include <unistd.h>
#include "render.h"
#include "input.h"
#include "levels.h"

#define FRAME_RATE 10
#define FRAME_DURATION (1000000 / FRAME_RATE) // in microseconds

int main()
{
    // game vairables, 2 windows, x and y positions etc
    WINDOW *gameWin;
    WINDOW *infoWin;

    int xpos = 1, ypos = 1, xspeed = 0.5, yspeed = 1;
    int fuel = 100, score = 0, direction = 0, thrust = 0;
    bool gameOver = false;

    initGame(&gameWin, &infoWin);
    nodelay(stdscr, TRUE); // Make getch non-blocking

    struct timespec lastTime;
    clock_gettime(CLOCK_MONOTONIC, &lastTime);

    while(!gameOver)
    {
        usleep(FRAME_DURATION);
        handleInput(&direction, &thrust, &fuel);
        updateGame(gameWin, infoWin, &xpos, &ypos, &xspeed, &yspeed, &fuel, &score, &direction, &thrust, &gameOver);
        renderGame(gameWin, infoWin, xpos, ypos, xspeed, yspeed, fuel, score, direction, thrust);
    }

    timeout(-1);
    getch();

    endwin();
    return 0;
}