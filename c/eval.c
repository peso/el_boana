
#include "sysdept.h"
#include "data.h"

extern int_16 count_value( uint_8 depth );
extern uint_8 count_capture( uint_8 depth );

int_16 disc_worth[15] = { -999, -999, -999, -999, -999, 
			  -999, -781, -511, -331, -211,
			  -130,  -76,  -40,  -16,    0  };

int_16 board_value( uint_8 depth, int_8 side )
{
    int_16	captured, captured1, captured2;
    uint_8	black_capture, white_capture;
    int_16	static_val;
    uint_8	w_discs, b_discs;

    boards_searched++;
    w_discs = white_discs[depth];
    b_discs = black_discs[depth];

    captured = count_capture( depth );
    black_capture = captured >> 4;
    white_capture = captured & 0x0f;

    if( black_capture > 0 && side == BLACK ) {
	w_discs--;
    } else if( white_capture > 0 && side == WHITE ) {
	b_discs--;
    }

    if( b_discs < 9 || w_discs < 9 || move_number + depth > 397 ) {
	if( b_discs > w_discs ) {
	    return( INFINITY );
	} else if( b_discs < w_discs ) {
	    return( -INFINITY );
	} else {
	    return( 0 );
	}
    }

    static_val = static_value[depth];
    if( move_number < 20 ) {
	static_val <<= 2;
    } else if( move_number < 80 ) {
	static_val <<= 1;
    }
    captured1 = ( ( disc_worth[b_discs] - disc_worth[w_discs] ) *
					stable_factor[move_number] ) >> 4;
    b_discs = black_discs[depth];
    w_discs = white_discs[depth];
    captured2 = ( ( disc_worth[b_discs] - disc_worth[w_discs] ) *
					stable_factor[move_number] ) >> 4;
    captured = captured1 + captured2;

    return( count_value( depth )
          + 6*static_val
	  + captured );
}
