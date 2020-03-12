/* Fake mouse.h -- Watcom 6.0 1993 */
/*
#include <sysdept.h>
void SetMouse( int_32, int_32 );
void GetMouse( int_16*, int_16*, int_16* );
void HideCursor();
*/
#define SetMouse(x,y) /*HIDE*/
#define GetMouse(x,y,b) /*HIDE*/
#define HideCursor() /*HIDE*/