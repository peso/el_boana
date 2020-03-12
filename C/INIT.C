
#include <stdio.h>
#include <graph.h>

#include "sysdept.h"
#include "ab.h"
#include "data.h"
#include "tables.h"
#include "vga.h"

void initialize()
{
    char	input;
    uint_8	row, col;
    uint_8	square, type;
    int_8	strength[18];
    uint_16	i;
    uint_8	hold;

    search_depth = 1;
    black_player = COMPUTER;
    white_player = COMPUTER;

    for( row=1; row<10; row++ ) {
        for( col=1; col<10; col++ ) {
	    last_square[row][col] = -2;
        }
    }

    _clearscreen( _GWINDOW );
    _settextposition( 9, 25 );
    ui_printf( "EL BOANA : An Abalone Program" );
    _settextposition( 11, 30 );
    ui_printf( "by Colin Springer" );

    _settextposition( 13, 26 );
    ui_printf( "How much time do I get?      " );
    do {
	_settextposition( 13, 50 );
	scanf( "%d", &black_game_time );
    } while( black_game_time < 1 || black_game_time > 1024 );
    black_game_time--;		/* reserve a minute, just in case ... */
    black_game_time *= 60*100;
    white_game_time = black_game_time;
    black_time = 0;
    white_time = 0;

    _settextposition( 14, 26 );
    ui_printf( "Shall I go first? (Y/N)      " );
    do {
	_settextposition( 14, 50 );
	scanf( "%c", &input );
    } while( input != 'y' && input != 'n' && input != 'c' );

    if( input == 'n' ) {
	black_player = HUMAN;
	white_player = COMPUTER;
    } else if( input == 'y' ) {
	black_player = COMPUTER;
	white_player = HUMAN;
    } else {
	black_player = COMPUTER;
	white_player = COMPUTER;
    }

    /* set up initial hash key values */

    hash_key_1[0] = 0;
    hash_key_2[0] = 0;

    for( row=1; row<10; row++ ) {
	for( col=1; col<10; col++ ) {
	    hash_key_1[0] += board[row][col] * rand_mask_1[row][col];
	    hash_key_2[0] += board[row][col] * rand_mask_2[row][col];
	}
    }

    for( i=0; i<16; i++ ) {
	move_made[i][0] = -1;
    }

    /* initialize static ordering */

    for( square=0; square<61; square++ ) {

	/* assign each move a relative merit */

	for( type=0; type<18; type++ ) {
	    order[square][type] = type;
	    if( type < 6 ) {
		strength[type] = max_length[type][square] + 100;
	    } else {
		strength[type] = max_length[(type-6)/2][square];
		if( type % 2 == 0 ) {
		    strength[type] += max_length[((type-6)/2+5) % 6][square];
		} else {
		    strength[type] += max_length[((type-6)/2+1) % 6][square];
		}
	    }
	}

	/* sort moves in descending order of merit */

	for( type=0; type<17; type++ ) {
	    for( i=type+1; i<18; i++ ) {
		if( strength[order[square][type]] < 
				strength[order[square][i]] ) {
		    hold = order[square][type];
		    order[square][type] = order[square][i];
		    order[square][i] = hold;
		}
	    }
	}
    }

    /* initialize line indices */

    for( type=0; type<27; type++ ) {
	line[0][type] = 0;
    }

    for( row=1; row<10; row++ ) {
	for( col=1; col<10; col++ ) {
	    if( square_id[row][col] >= 0 ) {
		if( board[row][col] == BLACK ) {
		    line[0][row-1] += 2*col_mult[row][col];
		    line[0][col+8] += 2*row_mult[row][col];
		    line[0][row+col+12] += 2*diag_mult[row][col];
		} else if( board[row][col] == 0 ) {
		    line[0][row-1] += 1*col_mult[row][col];
		    line[0][col+8] += 1*row_mult[row][col];
		    line[0][row+col+12] += 1*diag_mult[row][col];
		}
	    }
	}
    }

    /* initialize captured disc weights */

    for( i=0; i<400; i++ ) {
	stable_factor[i] = 6400/(410-i);
    }

    /* now initialize the evaluation tables */

    init_tables();

    vga_init();
    _clearscreen( _GCLEARSCREEN );
    draw_board();

    _settextposition( 2, 16 );
    ui_printf ("EL BOANA : An Abalone Program");
    _settextposition( 2, 60 );
    ui_printf( "version 1.3" );

    _settextposition (8, 61);
    if( black_player == HUMAN ) {
	ui_printf( "Opponent (#)" );
    } else {
	ui_printf( "El Boana (#)" );
    }
    _settextposition (12, 61);
    if( white_player == HUMAN ) {
	ui_printf( "Opponent (O)" );
	hash_key_1[0] = -hash_key_1[0];
	hash_key_2[0] = -hash_key_2[0];
    } else {
	ui_printf( "El Boana (O)" );
    }

    put_pieces ();
}
