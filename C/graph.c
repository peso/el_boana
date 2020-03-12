/** @file graph.c
  Fake graph.c - simulating Watcom C v6.0 using SDL 
*/

#include "graph.h"

#include <SDL.h>




/* * * * * * * * * * * * * * * * * * * 
            Global variables
 * * * * * * * * * * * * * * * * * * */

/** Index of current foreground colour. For _VRES16COLOR mode, range is 0..15 */
short _WC_cur_fg_color_inx;

/** Current position used for graphics */
struct xycoord _WC_cur_pos;



/* * * * * * * * * * * * * * * * * * * 
        Function implementation
 * * * * * * * * * * * * * * * * * * */

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
#if 0
struct rccoord _settextposition( short row, short col ) {
	  return 0;
}
#endif

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
    return old_pixval;
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
    old_pos = _WC_cur_pos;
    _WC_cur_pos.xcoord = x;
    _WC_cur_pos.ycoord = y;
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
    old_pos = _WC_cur_pos;
    _WC_cur_pos.xcoord = x;
    _WC_cur_pos.ycoord = y;
    
    /* TODO draw line between old_pos and _WC_cur_pos
       using _WC_cur_fg_color_inx */

    return 1;
}
