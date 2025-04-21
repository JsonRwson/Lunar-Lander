#include <ncurses.h>
#include <stdlib.h>
#include "input.h"

void handleInput(int *direction, int *thrust, int *fuel)
{
    int inp = getch();
    switch (inp)
    {
        case ' ':
            if(*fuel > 0)
            {
                *thrust = 1; // Apply thrust
                (*fuel) -= 5;  // Reduced fuel consumption
            }
            break;
           
        case 'a':
            if (*direction > -1)
            {
                (*direction)--;
                (*fuel) -= 2;  // Reduced fuel consumption for rotation
            }
            break;
            
        case 'd':
            if(*direction < 1)
            {
                (*direction)++;
                (*fuel) -= 2;  // Reduced fuel consumption for rotation
            }
            break;
            
        case 'q':  // Add quit option
        case 'Q':
            endwin();
            exit(0);
            break;
            
        default:
            *thrust = 0; // Reset thrust if space is not pressed
            break;
    }
}
