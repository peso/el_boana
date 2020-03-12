/* Interface to SDL simulated Watcom graphics */


/* ---- Constants ---- */

#define _GWINDOW 1
#define _GCLEARSCREEN 1
#define _VRES16COLOR 3
#define _DEFAULTMODE 4
#define _GFILLINTERIOR 5
#define _GBORDER 6


/* ---- Types ---- */


/* ---- Functions ---- */

#define _settextposition /* HIDE */
#define _clearscreen /* HIDE */
#define _setvideomode /* HIDE */
#define _getbkcolor() 0
short _getcolor();
#define _setbkcolor /* HIDE */
short _setcolor( short pixval );
#define _setbkcolor /* HIDE */
#define _moveto /* HIDE */
#define _lineto /* HIDE */
#define _floodfill /* HIDE */
#define _ellipse /* HIDE */
