/** @file mouse.c
  Fake mouse.c - simulating Watcom C v6.0 using SDL 
*/

#include "mouse.h"

#include <SDL_mouse.h>

void SetMouse( int_32 x, int_32 y )
{
    /* Not impleented */
}
/*
  1  left button
  2  right button
*/
void GetMouse( int_16* x , int_16* y, int_16* b )
{
    Uint32 button = SDL_GetMouseState(x,y);
    *b = 0 
    | button & SDL_BUTTON_LMASK ? MOUSE_LB : 0
    | button & SDL_BUTTON_RMASK ? MOUSE_RB : 0
    ;
}
