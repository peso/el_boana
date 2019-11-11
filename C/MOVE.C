
#include <string.h>

#include "sysdept.h"
#include "data.h"

/*
 *  Rules state that repetition of a position is not allowed.  This routine
 *  makes sure the current position does not match any earlier position.
 */

bool check_hash( uint_8 depth )
{
    uint_8	index;

    if( depth == search_depth || depth == 0 ) {
	return( TRUE );
    }

    hash_key_2[depth] &= 15;
    hash_key_1[depth] %= HASH_SIZE;
    for( index=0; move_made[hash_key_2[depth]][index] >= 0 &&
	 move_made[hash_key_2[depth]][index] != hash_key_1[depth]; index++ ) {}

    if( move_made[hash_key_2[depth]][index] < 0 ) {
	return( TRUE );
    } else {
	backup_move( depth );
	return( FALSE );
    }
} 

/* change the state of squares on the board, along with hash updates */

void set_board( uint_8 depth )
{
    uint_8	index;
    uint_8	row, col;
    int_8	piece, old;

    if( depth > 0 ) {
	hash_key_1[depth] = hash_key_1[depth-1];
	hash_key_2[depth] = hash_key_2[depth-1];
	black_discs[depth] = black_discs[depth-1];
	white_discs[depth] = white_discs[depth-1];
	static_value[depth] = static_value[depth-1];
	memcpy( line[depth], line[depth-1], 27*sizeof( uint_16 ) );
    }

    for( index=0; move_played[depth][index].row > 0; index++ ) {

	row = move_played[depth][index].row;
	col = move_played[depth][index].col;
	piece = move_played[depth][index].new;
	old = move_played[depth][index].old;

        board[row][col] = piece;
        if( piece == 1 ) {
	    black_discs[depth]++;
	    if( old == -1 ) {
	        white_discs[depth]--;
	        hash_key_1[depth] += 2*rand_mask_1[row][col];
	        hash_key_2[depth] += 2*rand_mask_2[row][col];
	        static_value[depth] += 2*weight[row][col];
		line[depth][row-1] += 2*col_mult[row][col];
		line[depth][col+8] += 2*row_mult[row][col];
		line[depth][row+col+12] += 2*diag_mult[row][col];
	    } else {	/* old = 0 */
	        hash_key_1[depth] += rand_mask_1[row][col];
	        hash_key_2[depth] += rand_mask_2[row][col];
	        static_value[depth] += weight[row][col];
		line[depth][row-1] += col_mult[row][col];
		line[depth][col+8] += row_mult[row][col];
		line[depth][row+col+12] += diag_mult[row][col];
	    }
        } else if( piece == -1 ) {
	    white_discs[depth]++;
	    if( old == 1 ) {
	        black_discs[depth]--;
	        hash_key_1[depth] -= 2*rand_mask_1[row][col];
	        hash_key_2[depth] -= 2*rand_mask_2[row][col];
	        static_value[depth] -= 2*weight[row][col];
		line[depth][row-1] -= 2*col_mult[row][col];
		line[depth][col+8] -= 2*row_mult[row][col];
		line[depth][row+col+12] -= 2*diag_mult[row][col];
	    } else {	/* old = 0 */
	        hash_key_1[depth] -= rand_mask_1[row][col];
	        hash_key_2[depth] -= rand_mask_2[row][col];
	        static_value[depth] -= weight[row][col];
		line[depth][row-1] -= col_mult[row][col];
		line[depth][col+8] -= row_mult[row][col];
		line[depth][row+col+12] -= diag_mult[row][col];
	    }
        } else if( old == 1 ) {
	    black_discs[depth]--;
	    hash_key_1[depth] -= rand_mask_1[row][col];
	    hash_key_2[depth] -= rand_mask_2[row][col];
	    static_value[depth] -= weight[row][col];
	    line[depth][row-1] -= col_mult[row][col];
	    line[depth][col+8] -= row_mult[row][col];
	    line[depth][row+col+12] -= diag_mult[row][col];
        } else {	/* old = -1 */
	    white_discs[depth]--;
	    hash_key_1[depth] += rand_mask_1[row][col];
	    hash_key_2[depth] += rand_mask_2[row][col];
	    static_value[depth] += weight[row][col];
	    line[depth][row-1] += col_mult[row][col];
	    line[depth][col+8] += row_mult[row][col];
	    line[depth][row+col+12] += diag_mult[row][col];
        }
    }
}

/* undo board changes made */

void backup_move( uint_8 depth )
{
    uint_8	index;

    for( index=0; move_played[depth][index].row > 0; index++ ) {
        board[move_played[depth][index].row][move_played[depth][index].col] 
						= move_played[depth][index].old;
    }
}

/* test for rolling move in certain direction and offset */

bool try_roll( uint_8 depth, uint_8 tail, uint_8 length, uint_8 d1, uint_8 d2 )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	index;
    int_8	dy, dx;
    int_8	oy, ox;

    dy = direction[0][d1];
    dx = direction[1][d1];
    oy = direction[0][d2];
    ox = direction[1][d2];
    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];

    if( max_length[d1][tail] + 1 < length ) {
	return( FALSE );	/* no room for group */
    } else if( max_length[d2][tail] == 0 ) {
	return( FALSE );	/* no room to move group */
    } else if( max_length[d1][square_id[row+oy][col+ox]] + 1 < length ) {
	return( FALSE );	/* no room to move group */
    }

    for( index=0; index<length; index++ ) {
	if( board[row+oy+index*dy][col+ox+index*dx] != 0 ) {
	    return( FALSE );	/* space not empty */
	}
    }

    for( index=1; index<length; index++ ) {
	if( board[row+index*dy][col+index*dx] != piece ) {
	    return( FALSE );	/* group inconsistent */
	}
    }

    for( index=0; index<length; index++ ) {
	move_played[depth][2*index].row = row+index*dy;
	move_played[depth][2*index].col = col+index*dx;
	move_played[depth][2*index].new = 0;
	move_played[depth][2*index].old = piece;
	move_played[depth][2*index+1].row = row+oy+index*dy;
	move_played[depth][2*index+1].col = col+ox+index*dx;
	move_played[depth][2*index+1].new = piece;
	move_played[depth][2*index+1].old = 0;
	move_played[depth][2*index+2].row = 0;
    }
    set_board( depth );

    return( check_hash( depth ) );	/* group successfully rolled! */
}

/* these are the entry points from the move generation routines */

bool try_move_0( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[0][tail];

    for( s1=1; board[row-s1][col] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row-s2][col] == -piece; s2++ ) {}

    if( board[row-s2][col] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row-s1;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row-s2;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row-s1;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row-s1;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_1( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[1][tail];

    for( s1=1; board[row-s1][col+s1] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row-s2][col+s2] == -piece; s2++ ) {}

    if( board[row-s2][col+s2] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row-s1;
	    move_played[depth][0].col = col+s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row-s2;
	    move_played[depth][0].col = col+s2;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row-s1;
	    move_played[depth][1].col = col+s1;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row-s1;
	    move_played[depth][0].col = col+s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_2( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[2][tail];

    for( s1=1; board[row][col+s1] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row][col+s2] == -piece; s2++ ) {}

    if( board[row][col+s2] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col+s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col+s2;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col+s1;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col+s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_3( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[3][tail];

    for( s1=1; board[row+s1][col] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row+s2][col] == -piece; s2++ ) {}

    if( board[row+s2][col] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row+s1;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row+s2;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row+s1;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row+s1;
	    move_played[depth][0].col = col;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_4( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[4][tail];

    for( s1=1; board[row+s1][col-s1] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row+s2][col-s2] == -piece; s2++ ) {}

    if( board[row+s2][col-s2] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row+s1;
	    move_played[depth][0].col = col-s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row+s2;
	    move_played[depth][0].col = col-s2;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row+s1;
	    move_played[depth][1].col = col-s1;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row+s1;
	    move_played[depth][0].col = col-s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_5( uint_8 depth, uint_8 tail, uint_8 length )
{
    uint_8	row, col;
    int_8	piece;
    uint_8	s1, s2;
    uint_8	room;

    length = length;		/* to prevent unreferenced symbol warning */

    row = row_sq[tail];
    col = col_sq[tail];
    piece = board[row][col];
    room = max_length[5][tail];

    for( s1=1; board[row][col-s1] == piece; s1++ ) {}

    if( s1 > 3 ) {
	return( FALSE );	/* can't move -- block too large */
    }

    for( s2=s1; board[row][col-s2] == -piece; s2++ ) {}

    if( board[row][col-s2] != 0 ) {
	return( FALSE );	/* can't push -- disc wedged */
    } else if( s2 >= (2*s1) ) {
	return( FALSE );	/* too many discs to push */
    } else if( s2 != room+1 ) {
	if( s2 == s1 ) {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col-s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs moved! */
	} else {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col-s2;
	    move_played[depth][0].new = -piece;
	    move_played[depth][0].old = 0;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col-s1;
	    move_played[depth][1].new = piece;
	    move_played[depth][1].old = -piece;
	    move_played[depth][2].row = row;
	    move_played[depth][2].col = col;
	    move_played[depth][2].new = 0;
	    move_played[depth][2].old = piece;
	    move_played[depth][3].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* discs pushed! */
	}
    } else {
	if( s2 == s1 ) {
	    return( FALSE );	/* can't move off edge */
	} else {
	    move_played[depth][0].row = row;
	    move_played[depth][0].col = col-s1;
	    move_played[depth][0].new = piece;
	    move_played[depth][0].old = -piece;
	    move_played[depth][1].row = row;
	    move_played[depth][1].col = col;
	    move_played[depth][1].new = 0;
	    move_played[depth][1].old = piece;
	    move_played[depth][2].row = 0;
	    set_board( depth );
	    return( check_hash( depth ) );	/* disc captured! */
	}
    }
}

bool try_move_6( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 0, 5 ) );
    }
}

bool try_move_7( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 0, 1 ) );
    }
}

bool try_move_8( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 1, 0 ) );
    }
}

bool try_move_9( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 1, 2 ) );
    }
}

bool try_move_10( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 2, 1 ) );
    }
}

bool try_move_11( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 2, 3 ) );
    }
}

bool try_move_12( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 3, 2 ) );
    }
}

bool try_move_13( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 3, 4 ) );
    }
}

bool try_move_14( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 4, 3 ) );
    }
}

bool try_move_15( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 4, 5 ) );
    }
}

bool try_move_16( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 5, 4 ) );
    }
}

bool try_move_17( uint_8 depth, uint_8 tail, uint_8 length )
{
    if( depth > search_depth-2 ) {
	return( FALSE );
    } else {
	return( try_roll( depth, tail, length, 5, 0 ) );
    }
}

/*
 *  The following array is used to access the above routines
 *  indirectly by move type.  
 */

bool (*move_routine[18]) () = { try_move_0,
				try_move_1,
				try_move_2,
				try_move_3,
				try_move_4,
				try_move_5,
				try_move_6,
				try_move_7,
				try_move_8,
				try_move_9,
				try_move_10,
				try_move_11,
				try_move_12,
				try_move_13,
				try_move_14,
				try_move_15,
				try_move_16,
				try_move_17 };

