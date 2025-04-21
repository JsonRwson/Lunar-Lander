#include <ncurses.h>
#include "render.h"

// Clears the lander's previous position from the game window
void clearLander(WINDOW *gameWin, int xpos, int ypos, int direction)
{
    // Clear the main lander body
    mvwaddch(gameWin, ypos, xpos, ' ');

    // Clear directional indicators
    mvwaddch(gameWin, ypos - 1, xpos, ' ');  // Up thruster
    mvwaddch(gameWin, ypos, xpos - 1, ' ');  // Left thruster
    mvwaddch(gameWin, ypos, xpos + 1, ' ');  // Right thruster

    // Clear possible thrust flame
    mvwaddch(gameWin, ypos + 1, xpos, ' ');
}

// Renders the game state, including the lander, stats, and thrust animation
void renderGame(WINDOW *gameWin, WINDOW *infoWin, int xpos, int ypos, int xspeed, int yspeed, int fuel, int score, int direction, int thrust)
{
    // Draw the ship with colors
    wattron(gameWin, COLOR_PAIR(2) | A_BOLD);
    mvwaddch(gameWin, ypos, xpos, 'O');  // Main body of lander

    // Draw direction indicators with colors
    switch (direction)
    {
        case -1:  // Left facing
        {
            mvwaddch(gameWin, ypos, xpos - 1, '<');
            break;
        }
        case 0:   // Up facing
        {
            mvwaddch(gameWin, ypos - 1, xpos, '^');
            break;
        }
        case 1:   // Right facing
        {
            mvwaddch(gameWin, ypos, xpos + 1, '>');
            break;
        }
    }
    wattroff(gameWin, COLOR_PAIR(2) | A_BOLD);

    // Show thrust animation if thrusting
    if (thrust > 0 && fuel > 0)
    {
        wattron(gameWin, COLOR_PAIR(3) | A_BOLD);
        switch (direction)
        {
            case -1:  // Left thrust, flame appears to the right
            {
                mvwaddch(gameWin, ypos, xpos + 1, '<');
                break;
            }
            case 0:   // Up thrust, flame appears below
            {
                mvwaddch(gameWin, ypos + 1, xpos, 'v');
                break;
            }
            case 1:   // Right thrust, flame appears to the left
            {
                mvwaddch(gameWin, ypos, xpos - 1, '>');
                break;
            }
        }
        wattroff(gameWin, COLOR_PAIR(3) | A_BOLD);
    }

    // Update info panel with color-coded values
    mvwprintw(infoWin, 9, 11, "%d    ", xspeed);       // Display horizontal speed
    mvwprintw(infoWin, 10, 11, "%d    ", -yspeed);    // Display vertical speed (inverted)

    // Display direction as text
    switch (direction)
    {
        case -1:
        {
            mvwprintw(infoWin, 11, 11, "LEFT     ");
            break;
        }
        case 0:
        {
            mvwprintw(infoWin, 11, 11, "UP       ");
            break;
        }
        case 1:
        {
            mvwprintw(infoWin, 11, 11, "RIGHT    ");
            break;
        }
    }

    // Display thrust value
    mvwprintw(infoWin, 12, 11, "%d     ", thrust);

    // Show fuel warning in red when low
    if (fuel < 20)
    {
        wattron(infoWin, COLOR_PAIR(3) | A_BOLD);
    }
    mvwprintw(infoWin, 13, 11, "%d     ", fuel);  // Display remaining fuel
    if (fuel < 20)
    {
        wattroff(infoWin, COLOR_PAIR(3) | A_BOLD);
    }

    // Display the current score
    mvwprintw(infoWin, 14, 11, "%d     ", score);

    // Refresh windows to display updates
    wrefresh(gameWin);
    wrefresh(infoWin);
}