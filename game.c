#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "levels.h"
#include <time.h>

void initGame(WINDOW **gameWin, WINDOW **infoWin)
{
    // initialise screen
    initscr();
    noecho();
    cbreak();

    // seperate window for main game and information screen
    *gameWin = newwin(30, 130, 0, 0);
    *infoWin = newwin(16, 30, 10.5, 135);
    refresh();

    // add borders to windows and refresh
    wborder(*gameWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(*infoWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(*gameWin);
    wrefresh(*infoWin);

    // add titles to windows, set up information lables on the info window
    mvwprintw(*gameWin, 0, 1, "Lunar Lander");
    mvwprintw(*infoWin, 0, 1, "Info");
    mvwprintw(*infoWin, 2, 2, "------Controls------");
    mvwprintw(*infoWin, 3, 2, "SPACE - thrust");
    mvwprintw(*infoWin, 4, 2, "A - rotate left");
    mvwprintw(*infoWin, 5, 2, "D - rotate right");
    mvwprintw(*infoWin, 7, 2, "------Stats------");
    mvwprintw(*infoWin, 8, 2, "X speed: ");
    mvwprintw(*infoWin, 9, 2, "Y speed: ");
    mvwprintw(*infoWin, 10, 2, "facing: ");
    mvwprintw(*infoWin, 11, 2, "thrust: ");
    mvwprintw(*infoWin, 12, 2, "fuel: ");
    mvwprintw(*infoWin, 13, 2, "score: ");

    wrefresh(*gameWin);
    wrefresh(*infoWin);

    // different random seed at each execution
    // randomly pick level by using an int to decide which array is pointed to by level pointer
    srand(time(NULL)); 
    int randlevel = rand() % 2;
    char** level;
    int levelyoffset = 0;

    switch (randlevel)
    {
        case 0:
            level = level1;
            levelyoffset = 12;
            break;
        
        case 1:
            level = level2;
            levelyoffset = 6;
            break;
    }

    // print the chosen level sequentially to game window
    for(int y = 0; y < lvl1y; y++)
    {
        mvwprintw(*gameWin, y+levelyoffset, 1, level[y]);
    }
}

void updateGame(WINDOW *gameWin, WINDOW *infoWin, int *xpos, int *ypos, int *xspeed, int *yspeed, int *fuel, int *score, int *direction, int *thrust, bool *gameOver)
{
    int coll = mvwinch(gameWin, *ypos + 1, *xpos) & A_CHARTEXT; // every frame detect the character under the player for collision

    if(coll != ' ' && coll != 32) // if the character is not space or empty we hit something
    { 
        if(coll != '-' ) // if we haven't hit regular land spot
        {
            if(coll == '2' || coll == '5' || coll == '7' || coll == '9') // check for score multipliers
            {
                *score *= coll - '0';
                mvwprintw(gameWin, *ypos - 1, *xpos + 3, "Landed! Bonus Points!");
            }
            else // otherwise we hit rough terrain and crashed
            {
                mvwprintw(gameWin, *ypos - 1, *xpos + 3, "CRASHED GAME OVER!");
                mvwprintw(gameWin, 0, 55, "CRASHED GAME OVER!");
            }
        }
        else // otherwise we hit a regular land spot '---'
        {
            mvwprintw(gameWin, *ypos - 1, *xpos + 1, "Landed!");
            *score += 10;
        }

        *gameOver = true;
    }

    if(*fuel > 0)
    {
        switch(*direction) // if we still have fuel, apply thrust
        {
            case -1:
                *xspeed -= *thrust;
                break;
            case 0:
                *yspeed -= *thrust;
                break;
            case 1:
                *xspeed += *thrust;
                break;
        }
    }

    *xpos += *xspeed;
    *ypos += *yspeed;

    // Reduce thrust after applying it
    *thrust = 0;
}
