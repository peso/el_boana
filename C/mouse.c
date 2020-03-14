/** @file mouse.c
  Fake mouse.c - simulating Watcom C v6.0 using SDL 
*/

#include "mouse.h"

#include <SDL.h>

void SetMouse( int_32 x, int_32 y )
{
    /* Not impleented */
}
/* Blocking method! waits for next event for up to 1000 ms
  1  left button
  2  right button
*/
void GetMouse( int_16* x , int_16* y, int_16* b )
{
    SDL_Event event;
    SDL_WaitEventTimeout(&event, 1000);
    
    if (event.type == SDL_QUIT) exit(1);

    Uint32 button = SDL_GetMouseState(x,y);
    *b = 0 
    | button & SDL_BUTTON_LMASK ? MOUSE_LB : 0
    | button & SDL_BUTTON_RMASK ? MOUSE_RB : 0
    ;
}
