#include <ncurses.h>
#include <curses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "levels.h"

int main()
{
    // game vairables, 2 windows, x and y positions etc
    WINDOW *gameWin;
    WINDOW *infoWin;
    int startx, starty, width, height, lvlx, lvly, x;
    int yspeed, xspeed, xpos, ypos;
    bool gameOver = false;

    // initialise screen and prevent getch from pausing program
    initscr();
    noecho();
    cbreak();

    // initialise values for game attributes
    startx = 0;
    starty = 0;

    width = 130;
    height = 30;

    xpos = 10;
    ypos = 10;
    xspeed = 1;
    yspeed = 1;

    int fuel = 100;
    int score = 0;
    int direction = 0;
    int thrust = 0;

    // seperate window for main game and information screen
    gameWin = newwin(height, width, starty, startx);
    infoWin = newwin(16, 50, 10.5, 135);

    // refresh screen after initialising screen
    refresh();

    // add borders to windows and refresh
    wborder(gameWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(infoWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(gameWin);
    wrefresh(infoWin);

    // add titles to windows, set up information lables on the info window
    mvwprintw(gameWin, 0, 1, "Lunar Lander");
    mvwprintw(infoWin, 0, 1, "Info");
    mvwprintw(infoWin, 2, 2, "------Controls------");
    mvwprintw(infoWin, 3, 2, "W - thrust");
    mvwprintw(infoWin, 4, 2, "A - rotate left");
    mvwprintw(infoWin, 5, 2, "W - rotate right");
    mvwprintw(infoWin, 7, 2, "------Stats------");
    mvwprintw(infoWin, 8, 2, "X speed: ");
    mvwprintw(infoWin, 9, 2, "Y speed: ");
    mvwprintw(infoWin, 10, 2, "facing: ");
    mvwprintw(infoWin, 11, 2, "thrust: ");
    mvwprintw(infoWin, 12, 2, "fuel: ");
    mvwprintw(infoWin, 13, 2, "score: ");

    // spawn the player at starting position and refresh windows
    mvwaddch(gameWin, ypos, xpos, '*' | A_BOLD);
    mvwaddch(gameWin, ypos-1, xpos, '^' | A_BOLD);

    wrefresh(gameWin);
    wrefresh(infoWin);

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
        mvwprintw(gameWin, y+levelyoffset, 1, level[y]);
    }
    
    // main game loop with a frame rate of 0.2 seconds
    while(!gameOver)
    {
        usleep(200000); // delay buffer
        timeout(50); // prevent getch from pausing program execution

        int inp = getch(); // every frame record user input for controls

        int coll = mvwinch(gameWin, ypos+1, xpos & A_CHARTEXT); // every frame detect the character under the player for collision
        if(coll != ' ' || coll != 32) // if the character is not space or empty we hit something
        {
            if(coll != '-') // if we havent hit regular land spot
            {
                if(coll == '2' || coll == '5' || coll == '7' || coll == '9') // check for score multipliers
                {
                    score *= coll;
                    mvwprintw(gameWin, ypos-1, xpos+1, "Landed! Bonus Points!");
                    gameOver = true;
                }
                else // otherwise we hit rough terrain and crashed
                {
                    mvwprintw(gameWin, ypos-1, xpos+1, " CRASHED GAME OVER!");
                    mvwprintw(gameWin, 0, 55, "CRASHED GAME OVER!");
                    gameOver = true;
                }
            }
            else // otherwise we hit a regular land spot '---'
            {
                mvwprintw(gameWin, ypos-1, xpos+1, "Landed!");
                score += 10;
                gameOver = true;
            }
        }

        /* directions
           -1 - left
            0 - up
            1 - right
        */

        int tempdir = direction; // temp direction for removing trailing characters

        switch(inp) // switch case for user input, if a or d, rotate the lander, if w, add thrust
        { // rotation and thrust subtracts fuel
            case 'w':
                thrust +=1;
                fuel -= 10;
                break;

            case 'a':
                if(direction -1 >= -1)
                {
                    direction -=1;
                    fuel -= 10;
                }
                break;

            case 'd':
                if(direction +1 <= 1)
                {
                    direction +=1;
                    fuel -= 10;
                }
                break;
        }

        int flip = 0; // flip switch for flashing fuel empty sign
        if(!(fuel <= 0)) // if we still have fuel, apply thrust
        {
            switch (direction)
            {
                case -1:
                    mvwaddch(gameWin, ypos, xpos-1, '<' | A_BOLD); // arrow to show the facing direction of the lander
                    mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)-1, ' '); // remove trailing character from last frame 
                    xspeed -= thrust;
                    break;

                case 0:
                    mvwaddch(gameWin, ypos-1, xpos, '^' | A_BOLD);
                    mvwaddch(gameWin, (ypos-yspeed)-1, xpos-xspeed, ' ');
                    yspeed = yspeed + (-1*thrust);
                    break;

                case 1:
                    mvwaddch(gameWin, ypos, xpos+1, '>' | A_BOLD);
                    if(xspeed != 1) // if the speed is 1 we dont want to remove the right facing char, it will replace the lander
                    {
                        mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)+1, ' ');
                    }
                    xspeed += thrust;
                    break;
            }
        }
        else // fuel is empty, reduce thrust and tell the user
        {
            if(flip == 0)
            {
                mvwprintw(gameWin, 0, 55, "fuel empty!");
                flip = 1;
            }
            else if(flip == 1)
            {
                mvwprintw(gameWin, 0, 55, "[][][][][] ");
                flip = 0;
            }
            thrust = 0;
        }

        if(tempdir != direction) // using the last direction to remove trailing characters
        {
            switch (tempdir)
            {
                case -1:
                    mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)-1, ' ');
                    break;
                case 0:
                    mvwaddch(gameWin, (ypos-yspeed)-1, (xpos-xspeed)-1, ' ');
                    break;
                case 1:
                    if(xspeed != 1) // if the speed is 1 we dont want to remove the right facing char, it will replace the lander
                    {
                        mvwaddch(gameWin, ypos-yspeed, (xpos-xspeed)-1, ' ');
                    }
                    break;
            }
        }
        
        // update player position graphically
        mvwaddch(gameWin, ypos-yspeed, xpos-xspeed, ' ');
        mvwaddch(gameWin, ypos, xpos, '*' | A_BOLD);        

        // update player statistics
        mvwprintw(infoWin, 8, 2, "X speed: %d", xspeed);
        mvwprintw(infoWin, 9, 2, "Y speed: %d", -1*yspeed);
        mvwprintw(infoWin, 10, 2, "facing: %d", direction);
        mvwprintw(infoWin, 11, 2, "thrust: %d", thrust);
        mvwprintw(infoWin, 12, 2, "fuel: %d", fuel);
        mvwprintw(infoWin, 13, 2, "score: %d", score);
        
        // update actual positional values using speed
        xpos += xspeed;
        ypos += yspeed;

        // decide where to move player if there is any thrust
        switch (direction)
        {
            case -1:
                xspeed += thrust;
                break;
            case 0:
                yspeed += thrust;
                break;
            case 1:
                xspeed -= thrust;
                break;
        }

        // refresh windows every frame
        wrefresh(infoWin);
        wrefresh(gameWin);
    }

    // set getch to go back to pausing the program execution
    // let player press a key before the game exits
    timeout(-1);
    getch();
    endwin();

    return 0;
}