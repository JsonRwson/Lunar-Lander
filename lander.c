#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "levels.h"

int main()
{
    WINDOW *gameWin;
    WINDOW *infoWin;
    int startx, starty, width, height, lvlx, lvly, x;
    int yspeed, xspeed, xpos, ypos;
    bool gameOver = false;

    initscr();
    noecho();
    cbreak();

    startx = 0;
    starty = 0;

    width = 130;
    height = 30;

    xpos = 10;
    ypos = 10.5;
    xspeed = 1;
    yspeed = 1;

    int fuel = 100;

    gameWin = newwin(height, width, starty, startx);
    infoWin = newwin(15, 50, 10.5, 135);

    refresh();

    wborder(gameWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(infoWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(gameWin);
    wrefresh(infoWin);

    mvwprintw(gameWin, 0, 1, "Lunar Lander");
    mvwprintw(gameWin, 10.5, 136, "Info");
    mvwprintw(gameWin, ypos, xpos, "*");
    wrefresh(gameWin);
    wrefresh(infoWin);

    for(int x = 0; x < lvl1y; x++)
    {
        mvwprintw(gameWin, x+12, 1, level1[x]);
    }
    
    while(!gameOver)
    {
        usleep(400000);
        timeout(50);
        int inp = getch();

        switch(inp)
        {
            case 'w':
                yspeed -=1;
                break;
            case 'a':
                xspeed -=1;
                break;
            case 'd':
                xspeed ++;
                break;
        }

        xpos += xspeed;
        ypos += yspeed;
        
        int coll = mvwinch(gameWin, ypos-yspeed, xpos & A_CHARTEXT);
        if(coll != ' ' || coll != 32)
        {
            mvwprintw(gameWin, ypos, xpos, "GAME OVER!");
            gameOver = true;
        }
        else
        {
            mvwprintw(gameWin, ypos-yspeed, xpos-xspeed, " ");
            mvwprintw(gameWin, ypos, xpos, "*");
        }
        wrefresh(gameWin);
    }

    timeout(-1);
    getch();
    endwin();

    return 0;
}