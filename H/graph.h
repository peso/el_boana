/* Interface to SDL simulated Watcom graphics */


/* ---- Constants ---- */

#define _GWINDOW 1
#define _GCLEARSCREEN 1
#define _VRES16COLOR 3
#define _DEFAULTMODE 4
#define _GFILLINTERIOR 5
#define _GBORDER 6


/* ---- Types ---- */

struct xycoord {
    short xcoord;
    short ycoord;
};

struct rccoord {
    short row;
    short col;
};


/* ---- Functions ---- */

void ui_init();
void ui_done();
void ui_printf( const char* format, ...);
int  ui_getch();
void ui_scan_ul( unsigned long* var );

struct rccoord _settextposition( short row, short col );
#define _clearscreen /* HIDE */
#define _setvideomode /* HIDE */
long _getbkcolor();
short _getcolor();
long _setbkcolor( long color );
short _setcolor( short pixval );
struct xycoord _moveto( short x, short y );
short _lineto( short x, short y );
#define _floodfill /* HIDE */
#define _ellipse /* HIDE */
