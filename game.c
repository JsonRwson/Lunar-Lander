#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include "game.h"
#include "levels.h"
#include <time.h>

// collisionMap[y][x] holds the terrain data for collision detection
static char collisionMap[30][130];

// Initializes the game windows and sets up the game environment
void initGame(WINDOW **gameWin, WINDOW **infoWin)
{
    // Clear collisionMap to all spaces
    for (int y = 0; y < 30; y++)
    {
        for (int x = 0; x < 130; x++)
        {
            collisionMap[y][x] = ' ';
        }
    }

    // Initialize ncurses screen
    initscr();
    start_color();  // Enable color support
    noecho();       // Disable echoing of typed characters
    cbreak();       // Disable line buffering

    // Set up color pairs for different game elements
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Normal text
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);  // Lander
    init_pair(3, COLOR_RED, COLOR_BLACK);     // Thrust/fire
    init_pair(4, COLOR_GREEN, COLOR_BLACK);   // Landing pads

    // Create separate windows for the game and information panels
    *gameWin = newwin(30, 130, 0, 0);
    *infoWin = newwin(16, 30, 10, 135);
    refresh();

    // Add borders to the windows
    wborder(*gameWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wborder(*infoWin, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(*gameWin);
    wrefresh(*infoWin);

    // Add titles to the windows
    wattron(*gameWin, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(*gameWin, 0, 1, "Lunar Lander");
    wattroff(*gameWin, COLOR_PAIR(1) | A_BOLD);

    wattron(*infoWin, COLOR_PAIR(1) | A_BOLD);
    mvwprintw(*infoWin, 0, 1, "Info");
    wattroff(*infoWin, COLOR_PAIR(1) | A_BOLD);

    // Display controls and stats in the info window
    mvwprintw(*infoWin, 2, 2, "------Controls------");
    mvwprintw(*infoWin, 3, 2, "SPACE - thrust");
    mvwprintw(*infoWin, 4, 2, "A - rotate left");
    mvwprintw(*infoWin, 5, 2, "D - rotate right");
    mvwprintw(*infoWin, 6, 2, "Q - quit game");

    mvwprintw(*infoWin, 8, 2, "------Stats------");
    mvwprintw(*infoWin, 9, 2, "X speed: ");
    mvwprintw(*infoWin, 10, 2, "Y speed: ");
    mvwprintw(*infoWin, 11, 2, "facing: ");
    mvwprintw(*infoWin, 12, 2, "thrust: ");
    mvwprintw(*infoWin, 13, 2, "fuel: ");
    mvwprintw(*infoWin, 14, 2, "score: ");
    wrefresh(*gameWin);
    wrefresh(*infoWin);

    // Seed the random number generator for level selection
    srand(time(NULL));
    int randlevel = rand() % 2;  // Randomly select a level
    char **level;
    int levelHeight;
    int levelyoffset = 0;

    // Assign the selected level and its properties
    switch (randlevel)
    {
        case 0:
        {
            level = level1;
            levelHeight = lvl1y;
            levelyoffset = 12;
            break;
        }
        case 1:
        {
            level = level2;
            levelHeight = lvl2y;
            levelyoffset = 6;
            break;
        }
    }

    // Render the selected level into the game window
    for (int y = 0; y < levelHeight; y++)
    {
        if (y + levelyoffset >= 30)  // Skip if out of bounds
        {
            continue;
        }

        for (int x = 0; level[y][x] != '\0' && x < 128; x++)
        {
            int wy = y + levelyoffset;  // Window y-coordinate
            int wx = x + 1;            // Window x-coordinate
            char terrainChar = level[y][x];

            // Store the terrain in collisionMap
            collisionMap[wy][wx] = terrainChar;

            // Draw the terrain with appropriate colors
            if (terrainChar == '-')
            {
                wattron(*gameWin, COLOR_PAIR(1));
                mvwaddch(*gameWin, wy, wx, '-');
                wattroff(*gameWin, COLOR_PAIR(1));
            }
            else if (terrainChar >= '1' && terrainChar <= '9')
            {
                wattron(*gameWin, COLOR_PAIR(4) | A_BOLD);
                mvwaddch(*gameWin, wy, wx, '-');  // Draw a green pad
                wattroff(*gameWin, COLOR_PAIR(4) | A_BOLD);
            }
            else
            {
                mvwaddch(*gameWin, wy, wx, terrainChar);
            }
        }
    }

    wrefresh(*gameWin);
}

// Updates the game state, including physics, collision detection, and rendering
void updateGame(WINDOW *gameWin, WINDOW *infoWin, int *xpos, int *ypos, int *xspeed, int *yspeed,
    int *fuel, int *score, int *direction, int *thrust, bool *gameOver, int *frame_counter)
{
    // Apply gravity every 3 frames
    if (*frame_counter % 3 == 0)
    {
        (*yspeed)++;
    }
    (*frame_counter)++;

    // Clear the previous lander position
    clearLander(gameWin, *xpos, *ypos, *direction);

    // Predict the next position of the lander
    int nextX = *xpos + *xspeed;
    int nextY = *ypos + *yspeed;

    // Check if the lander is out of bounds
    if (nextY < 0 || nextY >= 30 || nextX < 0 || nextX >= 130)
    {
        wattron(gameWin, COLOR_PAIR(3) | A_BOLD);
        mvwprintw(gameWin, *ypos - 3, *xpos - 3, "OUT OF BOUNDS!");
        mvwprintw(gameWin, 0, 55, "GAME OVER - OUT OF BOUNDS!");
        wattroff(gameWin, COLOR_PAIR(3) | A_BOLD);
        *gameOver = true;
        wrefresh(gameWin);
        return;
    }

    // Check for collision at the predicted position
    char coll = ' ';
    bool collisionDetected = false;

    for (int dx = -1; dx <= 1; dx++)
    {
        int checkY = nextY + 1;  // Check directly below the lander
        int checkX = nextX + dx;
        if (checkY >= 0 && checkY < 30 && checkX >= 0 && checkX < 130)
        {
            char cell = collisionMap[checkY][checkX];
            if (cell != ' ' && cell != 32)  // Detect non-empty terrain
            {
                coll = cell;
                collisionDetected = true;
                break;
            }
        }
    }

    if (collisionDetected)
    {
        // Adjust the lander's position to align with the ground
        *ypos = nextY;
        *xpos = nextX;

        // Determine the type of terrain the lander collided with
        bool landedOnFlatPad = (coll == '-');
        bool landedOnBonusPad = (coll >= '1' && coll <= '9');
        bool safeLanding = *yspeed <= 2 && *xspeed <= 2 && *xspeed >= -2;

        if (landedOnFlatPad || landedOnBonusPad)
        {
            if (safeLanding)
            {
                wattron(gameWin, COLOR_PAIR(4) | A_BOLD);
                if (landedOnBonusPad)
                {
                    *score *= coll - '0';  // Apply bonus multiplier
                    mvwprintw(gameWin, *ypos - 3, *xpos - 3, "Landed! Bonus x%c!", coll);
                }
                else
                {
                    *score += 10;  // Add score for safe landing
                    mvwprintw(gameWin, *ypos - 3, *xpos - 3, "Landed safely!");
                }
                wattroff(gameWin, COLOR_PAIR(4) | A_BOLD);
            }
            else
            {
                wattron(gameWin, COLOR_PAIR(3) | A_BOLD);
                mvwprintw(gameWin, *ypos - 3, *xpos - 3, "CRASHED: Too Fast!");
                mvwprintw(gameWin, 0, 55, "CRASHED GAME OVER!");
                wattroff(gameWin, COLOR_PAIR(3) | A_BOLD);
            }
        }
        else
        {
            wattron(gameWin, COLOR_PAIR(3) | A_BOLD);
            mvwprintw(gameWin, *ypos - 3, *xpos - 3, "CRASHED: Bad Terrain!");
            mvwprintw(gameWin, 0, 55, "CRASHED GAME OVER!");
            wattroff(gameWin, COLOR_PAIR(3) | A_BOLD);
        }

        *gameOver = true;
        wrefresh(gameWin);
        return;
    }

    // Apply thrust if there's fuel
    if (*fuel > 0 && *thrust > 0)
    {
        switch (*direction)
        {
            case -1:
            {
                (*xspeed) -= *thrust;
                break;
            }
            case 0:
            {
                (*yspeed) -= *thrust;
                break;
            }
            case 1:
            {
                (*xspeed) += *thrust;
                break;
            }
        }
        *thrust = 0;  // Reset thrust after applying
    }

    // Clamp speeds to prevent excessive velocity
    if (*xspeed > 3)
    {
        *xspeed = 3;
    }
    if (*xspeed < -3)
    {
        *xspeed = -3;
    }
    if (*yspeed > 5)
    {
        *yspeed = 5;
    }

    // Update the lander's position
    *xpos = nextX;
    *ypos = nextY;

    // Wrap the x-coordinate if it goes out of bounds
    if (*xpos < 1)
    {
        *xpos = 128;
    }
    if (*xpos > 128)
    {
        *xpos = 1;
    }

    // Clamp the y-coordinate to prevent it from going off-screen
    if (*ypos < 1)
    {
        *ypos = 1;
        *yspeed = 0;
    }
}
