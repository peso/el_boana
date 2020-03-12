
#include <graph.h>
#include <stdio.h>

#include "sysdept.h"
#include "data.h"


uint_16  old_bk, old_col;	/* remember colours to restore when finished */

#define USE_WATCOM_GRAPH_H
#ifndef USE_WATCOM_GRAPH_H
#include <SDL.h>
/* Resoution as Watcom _VRES16COLOR */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
SDL_Window* window = 0; /* Window to render to */
SDL_Surface* surf = 0; /* Surface inside window */
#endif


void vga_init()
{
#ifdef USE_WATCOM_GRAPH_H
    _setvideomode( _VRES16COLOR );
    old_bk = _getbkcolor();
    old_col = _getcolor();
    _setbkcolor( 0 );	/* BLACK */
#else /* USER_SDL */
    printf( "FANCY WATCOM GRAPHICS SIMULATOR that uses sdl\n" );
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize. SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }
#endif
}


void vga_finish()
{
#ifdef USE_WATCOM_GRAPH_H
    _setcolor( old_col );
    _setbkcolor( old_bk );
    _setvideomode( _DEFAULTMODE );
#else /* USER_SDL */
#endif
}


void highlight( uint_8 x, uint_8 y, uint_8 colour )
{
    uint_16	bx, by;

    /* calculate coordinates of hexagon's top corner */

    bx = 20*(2*x+y) - 100;
    by = 35*y + 44;

    _setcolor( colour );

    /* trace hexagon perimeter */

    _moveto( bx, by );
    _lineto( bx+20, by+12 );
    _lineto( bx+20, by+35 );
    _lineto( bx, by+47 );
    _lineto( bx-20, by+35 );
    _lineto( bx-20, by+12 );
    _lineto( bx, by );
}

void fill_hex( uint_8 x, uint_8 y, uint_8 colour )
{
    uint_16	bx, by;

    /* calculate coordinates of hexagon's top corner */

    bx = 20*(2*x+y) - 100;
    by = 35*y + 44;

    _setcolor( colour );

    /* trace hexagon perimeter */

    int xr;
    for (int dy=0; dy<12; dy++) {
        xr = 20*dy/12;
        _moveto(bx-xr, by+dy); _lineto(bx+xr,by+dy);
    }
    xr = 20;
    for (int dy=12; dy<35; dy++) {
        _moveto(bx-xr, by+dy); _lineto(bx+xr,by+dy);
    }
    for (int dy=35; dy<47; dy++) {
        xr = 20*(47-dy)/12;
        _moveto(bx-xr, by+dy); _lineto(bx+xr,by+dy);
    }
}


/* 
 *  The playing board in abalone consists of 61 hexagons, each of which
 *  may contain a black or white marble.  This routine draws one such
 *  hexagon at a position where its top point is at (bx,by).
 */

void hex( uint_8 x, uint_8 y )
{
    uint_16	bx, by;

    /* calculate coordinates of hexagon's top corner */

    bx = 20*(2*x+y) - 100;
    by = 35*y + 44;

 #if 0
    highlight( x, y, 3 );		/* draw the hexagon */
 
    _floodfill( bx, by+1, -1 );		/* fill it in */
#else
    fill_hex( x, y, 3);
#endif

    highlight( x, y, 0 );		/* redraw the perimeter in black */
}


void draw_board()
{
    uint_8   x, y;

    /* draw hexagons, row by row */

    for( y = 1; y <= 9; y++ ) {
	for( x = 1; x <= 9; x++ ) {
	    if( square_id[y][x] != -1 ) {
		hex( x, y );
	    }
	}
    }
}


void put_pieces()
{
    uint_8	row, col;
    uint_16	seconds, minutes;

    for( row=1; row<10; row++ ) {
	for( col=1; col<10; col++ ) {
	    if( square_id[row][col] >= 0 && 
			board[row][col] != last_square[row][col] ) {
		last_square[row][col] = board[row][col];
		if( board[row][col] == BLACK ) {
		    _setcolor( 0 );
		    _ellipse( _GFILLINTERIOR, 20*(2*col+row)-116,35*row+51,
					      20*(2*col+row)- 84,35*row+83 );
		    _setcolor( 7 );
		    _ellipse( _GBORDER, 20*(2*col+row)-116,35*row+51,
				        20*(2*col+row)- 84,35*row+83 );
		} else if( board[row][col] == WHITE ) {
		    _setcolor( 15 );
		    _ellipse( _GFILLINTERIOR, 20*(2*col+row)-116,35*row+51,
					      20*(2*col+row)- 84,35*row+83 );
		    _setcolor( 0 );
		    _ellipse( _GBORDER, 20*(2*col+row)-116,35*row+51,
				        20*(2*col+row)- 84,35*row+83 );
		} else {
		    _setcolor( 3 );
		    _ellipse( _GFILLINTERIOR, 20*(2*col+row)-116,35*row+51,
					      20*(2*col+row)- 84,35*row+83 );
		}
	    }
	}
    }

    _settextposition( 9, 63 );
    printf( "%2d", black_discs[0] );
    _settextposition( 13, 63 );
    printf( "%2d", white_discs[0] );

    _settextposition( 8, 50 );
    seconds = black_time / 100;
    minutes = seconds / 60;
    seconds = seconds % 60;
    printf( "%3d:%2.2d", minutes, seconds );

    _settextposition( 12, 50 );
    seconds = white_time / 100;
    minutes = seconds / 60;
    seconds = seconds % 60;
    printf( "%3d:%2.2d\n", minutes, seconds );
}
