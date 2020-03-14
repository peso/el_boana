#include <graph.h>
#include <stdio.h>

#include <conio.h>
#include <mouse.h>

#include "sysdept.h"
#include "data.h"
#include "vga.h"


int_8 get_square( uint_8 *row, uint_8 *col )
{
    int_8	x, y;
    int_8	bx, by;
    int_16	button;
    int_32	x_pix, y_pix;
    int_16	curr_x, curr_y;
    int_32	dist;
    int_32	min_dist;
    

    y_pix = 35 * (*row) + 74;
    x_pix = 20 * ( 2 * (*col) + (*row) ) - 100;

    SetMouse( x_pix, y_pix );
    HideCursor();

    highlight( *col, *row, 9 );

    GetMouse( &curr_x, &curr_y, &button );

    while( button == 0 || *row == 0 ) {
	min_dist = 1000000000;
	bx = 0;
	by = 0;
	for( y=0; y<=10; y++ ) {
	    for( x=0; x<=10; x++ ) {
		if( square_id[y][x] != -1 ) {
		    y_pix = 35 * y + 74;
		    x_pix = 20 * ( 2*x + y ) - 100;
		    if( x_pix < 0 ) {
		        x_pix = 0;
		    }
		    dist = ( curr_x - x_pix ) * ( curr_x - x_pix ) + 
		           ( curr_y - y_pix ) * ( curr_y - y_pix );
		    if( dist < min_dist ) {
		        min_dist = dist;
		        bx = x;
		        by = y;
		    }
		}
	    }
	}

	if( bx != *col || by != *row ) {
	    highlight( *col, *row, 0 );
	    *row = by;
	    *col = bx;
	    highlight( *col, *row, 9 );
	}
	GetMouse( &curr_x, &curr_y, &button );
        y_pix = 35 * (*row) + 74;
        x_pix = 20 * ( 2 * (*col) + (*row) ) - 100;
    }

    highlight( *col, *row, 0 );

    if( button & 2 ) {		/* right button pressed -- escape */
	do {
	    GetMouse( &curr_x, &curr_y, &button );
	} while( button != 0 );
	return( -1 );
    } else {
	do {
	    GetMouse( &curr_x, &curr_y, &button );
	} while( button != 0 );
	return( square_id[*row][*col] );
    }
}


#if 0
/* keyboard version */
int_8 get_square( uint_8 *row, uint_8 *col )
{
    char	key_pressed;
    int_8	dy, dx;

    highlight( *col, *row, 9 );
    key_pressed = getch();
    while( key_pressed != 13 && key_pressed != 27 ) {
	dy = 0;
	dx = 0;
	if( key_pressed == 55 ) {
	    dy = -1;
	    dx = 0;
	} else if( key_pressed == 57 ) {
	    dy = -1;
	    dx = 1;
	} else if( key_pressed == 54 ) {
	    dy = 0;
	    dx = 1;
	} else if( key_pressed == 51 ) {
	    dy = 1;
	    dx = 0;
	} else if( key_pressed == 49 ) {
	    dy = 1;
	    dx = -1;
	} else if( key_pressed == 52 ) {
	    dy = 0;
	    dx = -1;
	}
	if( square_id[*row+dy][*col+dx] >= 0 ) {
	    highlight( *col, *row, 0 );
	    *row += dy;
	    *col += dx;
	    highlight( *col, *row, 9 );
	}
	key_pressed = getch();
    }
    highlight( *col, *row, 0 );

    if( key_pressed == 27 ) {
	return( -1 );
    } else {
	return( square_id[*row][*col] );
    }
}
#endif


#if 0
/* text version of put_pieces() */
void put_pieces()
{
    uint_8	row, col;
    uint_16	seconds, minutes;

    for( row=1; row<10; row++ ) {
	for( col=1; col<10; col++ ) {
	    if( square_id[row][col] >= 0 && 
			board[row][col] != last_square[row][col] ) {
		last_square[row][col] = board[row][col];
		_settextposition( 8+row, 5+2*col+row );
		if( board[row][col] == BLACK ) {
		    printf( "#" );
		} else if( board[row][col] == WHITE ) {
		    printf( "O" );
		} else {
		    printf( "." );
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
#endif
