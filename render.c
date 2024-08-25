#include <ncurses.h>
#include "render.h"

void renderGame(WINDOW *gameWin, WINDOW *infoWin, int xpos, int ypos, int xspeed, int yspeed, int fuel, int score, int direction, int thrust)
{
    mvwaddch(gameWin, ypos, xpos, '0' | A_BOLD);
    mvwaddch(gameWin, ypos - 1, xpos, '^' | A_BOLD);

    switch (direction)
    {
        case -1:
            mvwaddch(gameWin, ypos, xpos-1, '<' | A_BOLD); // arrow to show the facing direction of the lander
            mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)-1, ' '); // remove trailing character from last frame 
            break;

        case 0:
            mvwaddch(gameWin, ypos-1, xpos, '^' | A_BOLD);
            mvwaddch(gameWin, (ypos-yspeed)-1, xpos-xspeed, ' ');
            break;

        case 1:
            mvwaddch(gameWin, ypos, xpos+1, '>' | A_BOLD);
            if(xspeed != 1) // if the speed is 1 we dont want to remove the right facing char, it will replace the lander
            {
                mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)+1, ' ');
            }
            break;
    }

    mvwprintw(infoWin, 8, 2, "X speed: %d", xspeed);
    mvwprintw(infoWin, 9, 2, "Y speed: %d", -yspeed);
    mvwprintw(infoWin, 10, 2, "Facing: %d", direction);
    mvwprintw(infoWin, 11, 2, "Thrust: %d", thrust);
    mvwprintw(infoWin, 12, 2, "Fuel: %d", fuel);
    mvwprintw(infoWin, 13, 2, "Score: %d", score);

    wrefresh(gameWin);
    wrefresh(infoWin);
}