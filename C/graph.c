/** @file graph.c
  Fake graph.c - simulating Watcom C v6.0 using SDL 
*/

#include "graph.h"

#include <stdarg.h>
#include <SDL.h>




/* * * * * * * * * * * * * * * * * * * 
            Global variables
 * * * * * * * * * * * * * * * * * * */

/** Global window used for all output */
SDL_Window *ui_window;
SDL_Renderer *ui_renderer;
int ui_font_width;
int ui_font_height;
struct ui_rgb {
    short r, g, b;
};
struct ui_rgb ui_rgb_inx[16];

/** Index of current foreground colour. For _VRES16COLOR mode, range is 0..15 */
short _WC_cur_fg_color_inx;

/** Index of current background colour. For _VRES16COLOR mode, range is 0..15 */
short _WC_cur_bg_color_inx;

/** Current position used for graphics */
struct xycoord _WC_gr_pos;

/** Current position used for text */
struct rccoord _WC_text_pos;



/* * * * * * * * * * * * * * * * * * * 
        Function implementation
 * * * * * * * * * * * * * * * * * * */

/** Sets up SDL. You must call this once before any code in main */
void ui_init()
{
    SDL_Init(SDL_INIT_VIDEO);

    ui_window = SDL_CreateWindow(
        "ElBoana",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        0
    );

    ui_renderer = SDL_CreateRenderer(ui_window, -1, SDL_RENDERER_SOFTWARE);

    for (int i=0; i<16; i++) {
        int v = i&8 ? 255 : 127;
        ui_rgb_inx[i].r = i&4 ? v : 0;
        ui_rgb_inx[i].g = i&2 ? v : 0;
        ui_rgb_inx[i].b = i&1 ? v : 0;
    }
    _WC_cur_fg_color_inx = 15;
    _WC_cur_bg_color_inx = 0;

    SDL_SetRenderDrawColor(ui_renderer, 10, 20, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ui_renderer);
    SDL_RenderPresent(ui_renderer);

    ui_font_width = 8;
    ui_font_height = 16;
}
/** Shut down SDL. You must call this once after all code in main */
void ui_done()
{
    SDL_DestroyWindow(ui_window);
    SDL_Quit();
}

void ui_set_sdl_color(int inx)
{
    struct ui_rgb col = ui_rgb_inx[inx & 15];
    SDL_SetRenderDrawColor(ui_renderer, col.r, col.g, col.b, SDL_ALPHA_OPAQUE);
}

/** Print on graphics window */
void ui_printf( const char* format, ...)
{
    char str[2000];
    va_list ap;
    va_start(ap, format);
    SDL_vsnprintf(str, sizeof(str), format, ap);
    va_end(ap);

    ui_set_sdl_color(15);
    SDL_Rect rect;
    rect.x = (_WC_text_pos.col - 1) * ui_font_width;
    rect.y = (_WC_text_pos.row - 1) * ui_font_height;
    rect.w = ui_font_width - 1;
    rect.h = ui_font_height - 1;

    for (char* s=str; *s != 0; s++) {
        SDL_RenderFillRect(ui_renderer, &rect);
        rect.x += rect.w + 1;
        _WC_text_pos.col ++;
    }
    SDL_RenderPresent(ui_renderer);
}

/** #include <conio.h>
Description:
    The getch function obtains the next available keystroke from the console.
    Nothing is echoed on the screen (the function getche will echo the
    keystroke, if possible). When no keystroke is available, the function waits
    until a key is depressed.

    The kbhit function can be used to determine if a keystroke is available.

Returns:
    A value of EOF is returned when an error is detected; otherwise the getch
    function returns the value of the keystroke (or character).

    When the keystroke represents an extended function key (for example, a
    function key, a cursor-movement key or the ALT key with a letter or a
    digit), zero is returned and the next call to getch returns a value for the
    extended function.
*/
int ui_getch()
{
    /* TODO wait for 1 keypress */
    SDL_Delay(100);
    static int k = 54;
    k = (k-32) % 90 + 33;
    return k;
}

void ui_scan_ul( unsigned long *var )
{
    /* TODO print and accept keyboard input */
    SDL_Delay(2000);
    *var = 20;
}

/**
Description:
    The _settextposition function sets the current output position for text
    to be (row,col) where this position is in terms of characters, not
    pixels.

    The text position is relative to the current text window.  It defaults
    to the top left corner of the screen, (1,1), when a new video mode is
    selected, or when a new text window is set.  The position is updated as
    text is drawn with the  _outtext and  _outmem functions.

    Note that the output position for graphics output differs from that for
    text output.  The output position for graphics output can be set by use
    of the  _moveto function.

Returns:
    The _settextposition function returns, as an rccoord structure, the
    previous output position for text.
*/
struct rccoord _settextposition( short row, short col ) {
    struct rccoord old_pos = _WC_text_pos;
    _WC_text_pos.row = row;
    _WC_text_pos.col = col;
    return old_pos;
}

/**
Description:
    The getcolor function returns the pixel value for the current color. This
    is the color used for displaying graphics output. The default color value
    is one less than the maximum number of colors in the current video mode.

Returns:
    The getcolor function returns the pixel value for the current color.
*/
short _getcolor() {
    return _WC_cur_fg_color_inx;
}

/**
Description:
    The setcolor function sets the pixel value for the current color to be that
    indicated by the pixval argument. The current color is only used by the
    functions that produce graphics output; text output with outtext uses the
    current text color (see the settextcolor function). The default color value
    is one less than the maximum number of colors in the current video mode.

Returns:
    The setcolor function returns the previous value of the current color.
*/
short _setcolor( short pixval ) {
    short old_pixval = _WC_cur_fg_color_inx;
    _WC_cur_fg_color_inx = pixval;

    ui_set_sdl_color(_WC_cur_fg_color_inx);

    return old_pixval;
}

/**
Description:
    The getbkcolor function returns the current background color. In text
    modes, the background color controls the area behind each individual 
    character. In graphics modes, the background refers to the entire screen. 
    The default background color is 0.

Returns:
    The getbkcolor function returns the current background color. 
*/
long _getbkcolor() {
    return _WC_cur_bg_color_inx;
}

/**
Description:
    The setbkcolor function sets the current background color to be that of the
    color argument. In text modes, the background color controls the area 
    behind each individual character. In graphics modes, the background refers 
    to the entire screen. The default background color is 0.

    When the current video mode is a graphics mode, any pixels with a zero 
    pixel value will change to the color of the color argument. When the 
    current video mode is a text mode, nothing will immediately change; only 
    subsequent output is affected.

Returns:
    The setbkcolor function returns the previous background color.
*/
long _setbkcolor( long color )
{
    long old_color = _WC_cur_bg_color_inx;
    _WC_cur_bg_color_inx = color;
    return old_color;
}


/**
Description:
    The clearscreen function clears the indicated area and fills it with the
    background color. The area argument must be one of the following values:
_GCLEARSCREEN area is entire screen
_GVIEWPORT area is current viewport or clip region
_GWINDOW area is current text window
Returns:
    The clearscreen function does not return a value.
*/
void _clearscreen( short area )
{
    ui_set_sdl_color(_WC_cur_bg_color_inx);
    SDL_RenderClear(ui_renderer);
    SDL_RenderPresent(ui_renderer);
}

/**
Description:
    The moveto functions set the current output position for graphics. The
    moveto function uses the view coordinate system. The moveto w function uses
    the window coordinate system.

    The current output position is set to be the point at the coordinates
    (x,y). Nothing is drawn by the function. The lineto function uses the
    current output position as the starting point when a line is drawn.

    Note that the output position for graphics output differs from that for
    text output. The output position for text output can be set by use of the
    settextposition function.

Returns:
    The moveto functions return the previous value of the output position for
    graphics.
*/
struct xycoord _moveto( short x, short y ) {
    struct xycoord old_pos;
    old_pos = _WC_gr_pos;
    _WC_gr_pos.xcoord = x;
    _WC_gr_pos.ycoord = y;
    return old_pos;
}

/**
Description:
    The lineto functions draw straight lines. The lineto function uses the view
    coordinate system. The lineto w function uses the window coordinate system.
    The line is drawn from the current position to the point at the coordinates
    (x,y). The point (x,y) becomes the new current position. The line is drawn
    with the current plotting action using the current line style and the
    current color.

Returns:
    The lineto functions return a non-zero value when the line was successfully
    drawn; otherwise, zero is returned.
*/
short _lineto( short x, short y ) {
    struct xycoord old_pos;
    old_pos = _WC_gr_pos;
    _WC_gr_pos.xcoord = x;
    _WC_gr_pos.ycoord = y;
    
    /* Draw line between old_pos and _WC_gr_pos
       using _WC_cur_fg_color_inx */
    ui_set_sdl_color(_WC_cur_fg_color_inx);
    SDL_RenderDrawLine(ui_renderer,
        old_pos.xcoord, old_pos.ycoord,
        x, y );
    SDL_RenderPresent(ui_renderer);

    return 1;
}

/**
Description:
    The ellipse functions draw ellipses. The ellipse function uses the view
    coordinate system. The ellipse w and ellipse wxy functions use the window
    coordinate system.
    
    The center of the ellipse is the center of the rectangle established by the
    points (x1,y1) and (x2,y2).

    The argument fill determines whether the ellipse is filled in or has only
    its outline drawn. The argument can have one of two values:
        _GFILLINTERIOR fill the interior by writing pixels with the current plot action using the current color and the current fill mask
        _GBORDER leave the interior unchanged; draw the outline of the figure with the current plot action using the current color and line style

    When the coordinates (x1,y1) and (x2,y2) establish a line or a point (this
    happens when one or more of the x-coordinates or y-coordinates are equal),
    nothing is drawn.

Returns:
    The ellipse functions return a non-zero value when the ellipse was
    successfully drawn; otherwise, zero is returned.
*/
short _ellipse( short fill, short x1, short y1, short x2, short y2 )
{
    /* do draw on x1,y1 - do NOT draw on x2,y2 */
    /* xx*xx + yy*yy = 1
    xx = x/xr, yy=y/yr
    xr = (x2-x1)/2
    line is 1 pix width, 0,0 is upper left
    xw = 3 -> 3 pix wide, inc line width -> circ is 2 pix
    center or circ is xw/2+.5
    */
    int xw, yw;
    xw = x2 - x1, yw = y2 - y1;
    if (xw == 0 || yw == 0) return 1;

    /* x/f*x/f + y*y = r*r
    f = x/y and r = yw/2 */
    double xr, yr;
    double xc, yc;
    xr = (xw-1)/2.0;
    yr = (yw-1)/2.0;
    xc = x1 + 0.5 + xr;
    yc = y1 + 0.5 + yr;

    if (fill == _GBORDER) {
        int count;
        count = 13;
        SDL_Point points[13];
        for (int i=0; i<count; i++) {
            double rad = 2*3.1415 * i / 12;
            points[i].x = (int)(xc + sin(rad) * xr);
            points[i].y = (int)(yc + cos(rad) * yr);
        }
        SDL_RenderDrawLines(ui_renderer, points, count);
    }
    else if (fill == _GFILLINTERIOR) {
        double dx = 0;    
        for (int y=0; y<yw; y++) {
            double dy = y+0.5 - yr;
            dx = xr * sqrt(1 - (dy/yr)*(dy/yr));
            SDL_RenderDrawLine(ui_renderer, xc-dx,y1+y, xc+dx,y1+y);
        }
    }
    else {

    }
    SDL_RenderPresent(ui_renderer);
    return 1;
}
