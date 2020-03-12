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


