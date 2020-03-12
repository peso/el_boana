/* Fake mouse.h -- Watcom 6.0 1993 */
#include <sysdept.h>
/*
void SetMouse( int_32, int_32 );
void GetMouse( int_16*, int_16*, int_16* );
void HideCursor();
*/
void SetMouse( int_32 x, int_32 y );
void GetMouse( int_16* x, int_16* y, int_16* b );
#define HideCursor() /*HIDE*/

#define MOUSE_LB 1
#define MOUSE_RB 2
