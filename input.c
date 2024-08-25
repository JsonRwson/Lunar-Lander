#include <ncurses.h>
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
                (*fuel) -= 10;
            }
            break;
            
        case 'a':
            if
            (*direction > -1)
            {
                (*direction)--;
                (*fuel) -= 10;
            }
            break;

        case 'd':
            if(*direction < 1)
            {
                (*direction)++;
                (*fuel) -= 10;
            }
            break;

        default:
            *thrust = 0; // Reset thrust if 'w' is not pressed
            break;
    }
}