#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "game.h"
#include "render.h"
#include "input.h"
#include "levels.h"
#include <unistd.h>

#define FRAME_RATE 4
#define FRAME_DURATION (1000000 / FRAME_RATE) // Frame duration in microseconds

int main()
{
    int ch; // Variable to store user input for restarting or quitting

    do
    {
        // Game variables
        WINDOW *gameWin; // Game window
        WINDOW *infoWin; // Info window
        int xpos = 20, ypos = 5; // Initial position of the lander
        int xspeed = 0, yspeed = 0; // Initial speed of the lander
        int fuel = 1000, score = 100, direction = 0, thrust = 0; // Game stats
        int frame_counter = 0; // Frame counter for timing
        bool gameOver = false; // Game over flag

        // Initialize the game environment
        initGame(&gameWin, &infoWin);

        // Configure ncurses settings
        nodelay(stdscr, TRUE); // Non-blocking input
        keypad(stdscr, TRUE); // Enable special keys
        curs_set(0); // Hide the cursor

        // Track the last frame time
        struct timespec lastTime;
        clock_gettime(CLOCK_MONOTONIC, &lastTime);

        // Main game loop
        while (!gameOver)
        {
            usleep(FRAME_DURATION); // Wait for the next frame

            // Handle user input
            handleInput(&direction, &thrust, &fuel);

            // Update the game state
            updateGame(gameWin, infoWin, &xpos, &ypos, &xspeed, &yspeed,
                       &fuel, &score, &direction, &thrust, &gameOver, &frame_counter);

            // Render the game state
            renderGame(gameWin, infoWin, xpos, ypos, xspeed, yspeed,
                       fuel, score, direction, thrust);
        }

        // Display the game over screen
        timeout(-1); // Blocking input for game over screen
        mvwprintw(gameWin, 15, 50, "Game Over! Press 'r' to restart or 'q' to quit.");
        wrefresh(gameWin);

        // Wait for user input to restart or quit
        do
        {
            ch = getch();
        } while (ch != 'q' && ch != 'r');

        // Clean up windows and reset the screen
        delwin(gameWin);
        delwin(infoWin);
        clear();
        refresh();
    } while (ch == 'r'); // Restart the game if 'r' is pressed

    // End ncurses mode
    endwin();
    return 0;
}
