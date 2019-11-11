
#include <string.h>

#include "sysdept.h"
#include "data.h"
#include "move.h"
#include "eval.h"

/*
 *  We remember the most recent best moves for black and white.  These
 *  moves are tested after the move suggested by the hash tables.
 */

/* killer moves for last ply of search */

uint_8 black_killer_square=0;
uint_8 black_killer_type=0;
uint_8 black_killer_length=3;
uint_8 white_killer_square=0;
uint_8 white_killer_type=0;
uint_8 white_killer_length=3;

/* killer moves for rest of tree */

uint_8 bk_square[MAX_DEPTH];
uint_8 bk_type[MAX_DEPTH];
uint_8 bk_length[MAX_DEPTH];
uint_8 wk_square[MAX_DEPTH];
uint_8 wk_type[MAX_DEPTH];
uint_8 wk_length[MAX_DEPTH];

/*
 *  The following two routines do searching for all but the last
 *  ply of the tree.  All move types are searched, and moves are
 *  tested against previous board positions to prevent duplication
 *  of board positions.
 */

int_16 black_value( uint_8 depth, int_16 alpha, int_16 beta )
{
    int_16	best_yet, temp;
    uint_8	square;
    uint_8	index, type;
    uint_8	moves=0;
    uint_8	h_square=61, h_type, h_length;

    /* check for end of game first */

    if(  black_discs[depth] < 9 || move_number + depth > 398 ) {
	if( black_discs[depth] > white_discs[depth] ) {
	    return( INFINITY );
	} else if( black_discs[depth] < white_discs[depth] ) {
	    return( -INFINITY );
	} else {
	    return( 0 );
	}
    }

    /* Are we on the last ply of search?  If so, use faster routines */

    if( depth == search_depth-1 ) {
	return( last_black_value( depth, alpha, beta ) );
    }

    best_yet = alpha-1;

    hash_key_1[depth] %= HASH_SIZE;
    hash_key_2[depth] &= 15;

    /* try hash move first, if it matches hash lock and is legal */

    if( black_hash[hash_key_1[depth]].hash_lock == hash_key_2[depth] ) {
        h_square = black_hash[hash_key_1[depth]].move_square;
	h_type = black_hash[hash_key_1[depth]].move_type;
	h_length = black_hash[hash_key_1[depth]].move_length + 2;
	if( board[row_sq[h_square]][col_sq[h_square]] == BLACK ) {
	    if( ( *move_routine[h_type] ) ( depth+1, h_square, h_length )
								 == TRUE ) {
		moves++;
		temp = white_value( depth+1, alpha, beta );
		backup_move( depth+1 );
		if( temp > best_yet ) {
		    move_square[depth] = h_square;
		    move_type[depth] = h_type;
		    move_length[depth] = h_length;
		    best_yet = temp;
		}
	    }
	}
    }

    if( best_yet >= beta ) {
	goto black_cutoff;
    }

    /* try killer move next, if it's legal and different */

    if( board[row_sq[bk_square[depth]]][col_sq[bk_square[depth]]] == BLACK &&
      ( bk_square[depth] != h_square || bk_type[depth] != h_type 
				     || bk_length[depth] != h_length ) ) {
	if( ( *move_routine[bk_type[depth]] ) 
	    ( depth+1, bk_square[depth], bk_length[depth] ) == TRUE ) {
	    moves++;
	    temp = white_value( depth+1, best_yet, beta );
	    backup_move( depth+1 );
	    if( temp > best_yet ) {
		move_square[depth] = bk_square[depth];
		move_type[depth] = bk_type[depth];
		move_length[depth] = bk_length[depth];
		best_yet = temp;
	    }
	}
    }

    for( square=0; square<61 && beta > best_yet; square++ ) {
	if( board[row_sq[square]][col_sq[square]] == BLACK ) {
	    for( index=0; index<18 && beta > best_yet; index++ ) {
		type = order[square][index];
		if( ( h_square != square || h_type != type || h_length != 3 ) 
		 && ( bk_square[depth] != square || bk_type[depth] != type 
						 || bk_length[depth] != 3 ) 
		 && ( *move_routine[type] ) ( depth+1, square, 3 ) == TRUE ) {
		    moves++;
		    temp = white_value( depth+1, best_yet, beta );
		    backup_move( depth+1 );
		    if( temp > best_yet ) {
			move_square[depth] = square;
			move_type[depth] = type;
			move_length[depth] = 3;
			best_yet = temp;
		    }
		}
		if( type >= 6 ) {
		    if(( h_square != square || h_type != type || h_length != 2 )
		    && ( bk_square[depth] != square || bk_type[depth] != type 
						    || bk_length[depth] != 2 ) 
		    && ( *move_routine[type] ) (depth+1, square, 2 ) == TRUE ) {
		        moves++;
		        temp = white_value( depth+1, best_yet, beta );
			backup_move( depth+1 );
		        if( temp > best_yet ) {
			    move_square[depth] = square;
			    move_type[depth] = type;
			    move_length[depth] = 2;
			    best_yet = temp;
		        }
		    }
	        }
	    }
	}
    }

    if( moves == 0 ) {
	return( white_value( depth+1, alpha, beta ) );
    }

    black_hash[hash_key_1[depth]].move_square = move_square[depth];
    black_hash[hash_key_1[depth]].move_type = move_type[depth];
    black_hash[hash_key_1[depth]].move_length = move_length[depth]-2;
    black_hash[hash_key_1[depth]].hash_lock = hash_key_2[depth];

black_cutoff:

    bk_square[depth] = move_square[depth];
    bk_type[depth] = move_type[depth];
    bk_length[depth] = move_length[depth];

    return( best_yet );
}

int_16 white_value( uint_8 depth, int_16 alpha, int_16 beta )
{
    int_16	best_yet, temp;
    uint_8	square;
    uint_8	index, type;
    uint_8	moves=0;
    uint_8	h_square=61, h_type, h_length;

    /* check for end of game first */

    if( white_discs[depth] < 9 || move_number + depth > 398 ) {
	if( black_discs[depth] > white_discs[depth] ) {
	    return( INFINITY );
	} else if( black_discs[depth] < white_discs[depth] ) {
	    return( -INFINITY );
	} else {
	    return( 0 );
	}
    }

    /* Are we on the last ply of search?  If so, use faster routines */

    if( depth == search_depth-1 ) {
	return( last_white_value( depth, alpha, beta ) );
    }

    best_yet = beta+1;

    hash_key_1[depth] %= HASH_SIZE;
    hash_key_2[depth] &= 15;

    /* try hash move first, if it matches hash lock and is legal */

    if( white_hash[hash_key_1[depth]].hash_lock == hash_key_2[depth] ) {
        h_square = white_hash[hash_key_1[depth]].move_square;
	h_type = white_hash[hash_key_1[depth]].move_type;
	h_length = white_hash[hash_key_1[depth]].move_length + 2;
	if( board[row_sq[h_square]][col_sq[h_square]] == WHITE ) {
	    if( ( *move_routine[h_type] ) ( depth+1, h_square, h_length )
								 == TRUE ) {
		moves++;
		temp = black_value( depth+1, alpha, beta );
		backup_move( depth+1 );
		if( temp < best_yet ) {
		    move_square[depth] = h_square;
		    move_type[depth] = h_type;
		    move_length[depth] = h_length;
		    best_yet = temp;
		}
	    }
	}
    }

    if( best_yet <= alpha ) {
	goto white_cutoff;
    }

    /* try killer move next, if it's legal and different */

    if( board[row_sq[wk_square[depth]]][col_sq[wk_square[depth]]] == WHITE &&
      ( wk_square[depth] != h_square || wk_type[depth] != h_type 
				     || wk_length[depth] != h_length ) ) {
	if( ( *move_routine[wk_type[depth]] ) 
	    ( depth+1, wk_square[depth], wk_length[depth] ) == TRUE ) {
	    moves++;
	    temp = black_value( depth+1, alpha, best_yet );
	    backup_move( depth+1 );
	    if( temp < best_yet ) {
		move_square[depth] = wk_square[depth];
		move_type[depth] = wk_type[depth];
		move_length[depth] = wk_length[depth];
		best_yet = temp;
	    }
	}
    }

    for( square=0; square<61 && alpha < best_yet; square++ ) {
	if( board[row_sq[square]][col_sq[square]] == WHITE ) {
	    for( index=0; index<18 && alpha < best_yet; index++ ) {
		type = order[square][index];
		if( ( h_square != square || h_type != type || h_length != 3 )
		 && ( wk_square[depth] != square || wk_type[depth] != type 
						 || wk_length[depth] != 3 ) 
		 && ( *move_routine[type] ) ( depth+1, square, 3 ) == TRUE ) {
		    moves++;
		    temp = black_value( depth+1, alpha, best_yet );
		    backup_move( depth+1 );
		    if( temp < best_yet ) {
			move_square[depth] = square;
			move_type[depth] = type;
			move_length[depth] = 3;
			best_yet = temp;
		    }
		}
		if( type >= 6 ) {
		    if(( h_square != square || h_type != type || h_length != 2 )
		    && ( wk_square[depth] != square || wk_type[depth] != type 
						    || wk_length[depth] != 2 ) 
		    && ( *move_routine[type] ) (depth+1, square, 2 ) == TRUE ) {
		        moves++;
		        temp = black_value( depth+1, alpha, best_yet );
			backup_move( depth+1 );
		        if( temp < best_yet ) {
			    move_square[depth] = square;
			    move_type[depth] = type;
			    move_length[depth] = 2;
			    best_yet = temp;
		        }
		    }
		}
	    }
	}
    }

    if( moves == 0 ) {
	return( black_value( depth+1, alpha, beta ) );
    }

    white_hash[hash_key_1[depth]].move_square = move_square[depth];
    white_hash[hash_key_1[depth]].move_type = move_type[depth];
    white_hash[hash_key_1[depth]].move_length = move_length[depth]-2;
    white_hash[hash_key_1[depth]].hash_lock = hash_key_2[depth];

white_cutoff:

    wk_square[depth] = move_square[depth];
    wk_type[depth] = move_type[depth];
    wk_length[depth] = move_length[depth];

    return( best_yet );
}

/*
 *  The following two routines do searching for the last ply of the
 *  search tree.  Only push type moves are searched, and positions
 *  are not tested for duplication.
 */

int_16 last_black_value( uint_8 depth, int_16 alpha, int_16 beta )
{
    int_16	best_yet, temp;
    uint_8	square;
    uint_8	index, type;
    uint_8	moves=0;
    uint_8	h_square=61, h_type, h_length;

    best_yet = alpha;

    hash_key_1[depth] %= HASH_SIZE;
    hash_key_2[depth] &= 15;

    /* try hash move first, if it matches hash lock and is legal */

    if( black_hash[hash_key_1[depth]].hash_lock == hash_key_2[depth] ) {
        h_square = black_hash[hash_key_1[depth]].move_square;
	h_type = black_hash[hash_key_1[depth]].move_type;
	h_length = black_hash[hash_key_1[depth]].move_length + 2;
	if( board[row_sq[h_square]][col_sq[h_square]] == BLACK ) {
	    if( ( *move_routine[h_type] ) ( depth+1, h_square, h_length )
								 == TRUE ) {
		moves++;
		temp = board_value( depth+1, WHITE );
		backup_move( depth+1 );
		if( temp > best_yet ) {
		    move_square[depth] = h_square;
		    move_type[depth] = h_type;
		    move_length[depth] = h_length;
		    best_yet = temp;
		}
	    }
	}
    }

    if( best_yet >= beta ) {
	goto last_black_cutoff;
    }

    /* try killer move next, if it's legal and different */

    if( board[row_sq[black_killer_square]][col_sq[black_killer_square]] 
								== BLACK &&
      ( black_killer_square != h_square || black_killer_type != h_type ) ) {
	if( ( *move_routine[black_killer_type] ) 
	    ( depth+1, black_killer_square, black_killer_length ) == TRUE ) {
	    moves++;
	    temp = board_value( depth+1, WHITE );
	    backup_move( depth+1 );
	    if( temp > best_yet ) {
		move_square[depth] = black_killer_square;
		move_type[depth] = black_killer_type;
		move_length[depth] = black_killer_length;
		best_yet = temp;
	    }
	}
    }

    for( square=0; square<61 && beta > best_yet; square++ ) {
	if( board[row_sq[square]][col_sq[square]] == BLACK ) {
	    for( index=0; index<6 && beta > best_yet; index++ ) {
		type = order[square][index];
		if( ( h_square != square || h_type != type ) 
		 && ( black_killer_square!=square || black_killer_type!=type )
		 && ( *move_routine[type] ) ( depth+1, square, 3 ) == TRUE ) {
		    moves++;
		    temp = board_value( depth+1, WHITE );
		    backup_move( depth+1 );
		    if( temp > best_yet ) {
			move_square[depth] = square;
			move_type[depth] = type;
			move_length[depth] = 3;
			best_yet = temp;
		    }
		}
	    }
	}
    }

    if( moves == 0 ) {
	return( board_value( depth+1, WHITE ) );
    }
    
    black_hash[hash_key_1[depth]].move_square = move_square[depth];
    black_hash[hash_key_1[depth]].move_type = move_type[depth];
    black_hash[hash_key_1[depth]].move_length = move_length[depth]-2;
    black_hash[hash_key_1[depth]].hash_lock = hash_key_2[depth];

last_black_cutoff:

    black_killer_square = move_square[depth];
    black_killer_type = move_type[depth];
    black_killer_length = move_length[depth];

    return( best_yet );
}

int_16 last_white_value( uint_8 depth, int_16 alpha, int_16 beta )
{
    int_16	best_yet, temp;
    uint_8	square;
    uint_8	index, type;
    uint_8	moves=0;
    uint_8	h_square=61, h_type, h_length;

    best_yet = beta;

    hash_key_1[depth] %= HASH_SIZE;
    hash_key_2[depth] &= 15;

    /* try hash move first, if it matches hash lock and is legal */

    if( white_hash[hash_key_1[depth]].hash_lock == hash_key_2[depth] ) {
        h_square = white_hash[hash_key_1[depth]].move_square;
	h_type = white_hash[hash_key_1[depth]].move_type;
	h_length = white_hash[hash_key_1[depth]].move_length + 2;
	if( board[row_sq[h_square]][col_sq[h_square]] == WHITE ) {
	    if( ( *move_routine[h_type] ) ( depth+1, h_square, h_length )
								 == TRUE ) {
		moves++;
		temp = board_value( depth+1, BLACK );
		backup_move( depth+1 );
		if( temp < best_yet ) {
		    move_square[depth] = h_square;
		    move_type[depth] = h_type;
		    move_length[depth] = h_length;
		    best_yet = temp;
		}
	    }
	}
    }

    if( best_yet <= alpha ) {
	goto last_white_cutoff;
    }

    /* try killer move next, if it's legal */

    if( board[row_sq[white_killer_square]][col_sq[white_killer_square]] 
								== WHITE &&
      ( white_killer_square != h_square || white_killer_type != h_type ) ) {
	if( ( *move_routine[white_killer_type] ) 
	    ( depth+1, white_killer_square, white_killer_length ) == TRUE ) {
	    moves++;
	    temp = board_value( depth+1, BLACK );
	    backup_move( depth+1 );
	    if( temp < best_yet ) {
		move_square[depth] = white_killer_square;
		move_type[depth] = white_killer_type;
		move_length[depth] = white_killer_length;
		best_yet = temp;
	    }
	}
    }

    for( square=0; square<61 && alpha < best_yet; square++ ) {
	if( board[row_sq[square]][col_sq[square]] == WHITE ) {
	    for( index=0; index<6 && alpha < best_yet; index++ ) {
		type = order[square][index];
		if( ( h_square != square || h_type != type )
		 && ( white_killer_square!=square || white_killer_type!=type )
		 && ( *move_routine[type] ) ( depth+1, square, 3 ) == TRUE ) {
		    moves++;
		    temp = board_value( depth+1, BLACK );
		    backup_move( depth+1 );
		    if( temp < best_yet ) {
			move_square[depth] = square;
			move_type[depth] = type;
			move_length[depth] = 3;
			best_yet = temp;
		    }
		}
	    }
	}
    }

    if( moves == 0 ) {
	return( board_value( depth+1, BLACK ) );
    }

    white_hash[hash_key_1[depth]].move_square = move_square[depth];
    white_hash[hash_key_1[depth]].move_type = move_type[depth];
    white_hash[hash_key_1[depth]].move_length = move_length[depth]-2;
    white_hash[hash_key_1[depth]].hash_lock = hash_key_2[depth];

last_white_cutoff:

    white_killer_square = move_square[depth];
    white_killer_type = move_type[depth];
    white_killer_length = move_length[depth];

    return( best_yet );
}

