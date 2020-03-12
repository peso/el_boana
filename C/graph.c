/** @file graph.c
  Fake graph.c - simulating Watcom C v6.0 using SDL 
*/

#include <SDL.h>


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
	return 0;
}
