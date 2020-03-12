
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graph.h>

#include "sysdept.h"
#include "data.h"
#include "init.h"
#include "move.h"
#include "search.h"
#include "vga.h"
#include "intrface.h"


/* Keep C99 happy by declaring functions before they are used. */
void human_move( int_8 );
void computer_move( int_8 );
void print_move( uint_8 depth, uint_8 square, uint_8 type, uint_8 length );


uint_8	c_row=5, c_col=5;


int main(int argc, char* argv[])
{
    ui_init();
    bool game_over;

    game_over = FALSE;
    move_number = 0;
    black_discs[0] = 14;
    white_discs[0] = 14;
    static_value[0] = 0;
    initialize ();

    while( game_over == FALSE ) {
	if( black_player == HUMAN ) {
	    human_move( BLACK );
        } else {
	    computer_move( BLACK );
	}
	if( black_discs[0] < 9 || white_discs[0] < 9 || move_number > 398 ) {
	    game_over = TRUE;
	}
	move_number++;
	if( game_over == FALSE ) {
	    if( white_player == HUMAN ) {
	        human_move( WHITE );
            } else {
	        computer_move( WHITE );
	    }
	    if( black_discs[0] < 9 || white_discs[0] < 9 || move_number > 398 ){
                game_over = TRUE;
            }
            move_number++;
	}
    }
    _settextposition( 28, 1 );
    getch();
    vga_finish();
    ui_done();
}

void computer_move( int_8 side )
{
    uint_32	start_time;
    uint_8	i;
    int_16	b_val;
    uint_32	time_alloc, time_used;
    bool	same_move;
    uint_8	last_move_square;
    uint_8	last_move_type;
    uint_8	last_move_length;

    start_time = get_time_used();
    if( side == BLACK ) {
	if( black_game_time > black_time + 100 * (400-move_number) / 2 ) {
	    time_alloc = ( black_game_time - black_time - 
				100 * ( 400-move_number ) / 2 ) / 
					(400 - move_number) * 3;
	} else {
	    time_alloc = 3*100/4;
	}
	if( time_alloc > ( black_game_time - black_time ) / 8 ) {
	    time_alloc = ( black_game_time - black_time ) / 8;
	}
    } else {
	if( white_game_time > white_time + 100 * (400-move_number) / 2 ) {
	    time_alloc = ( white_game_time - white_time -
				100 * ( 400-move_number ) / 2 ) / 
					(400 - move_number) * 3;
	} else {
	    time_alloc = 3*100/4;
	}
	if( time_alloc > ( white_game_time - white_time ) / 8 ) {
	    time_alloc = ( white_game_time - white_time ) / 8;
	}
    }
    if( time_alloc < 3*100/4 || time_alloc > (long) 300*100 ) {
	time_alloc = 3*100/4;	/* allow at least 3/4 second */
    }
    boards_searched = 0;
    search_depth = 0;
    last_move_square = 0;
    last_move_type = 0;
    last_move_length = 0;
    do {
	search_depth++;
        if( side == BLACK ) {
	    b_val = black_value( 0, -INFINITY, INFINITY );
        } else {
	    b_val = - white_value( 0, -INFINITY, INFINITY );
        }

	_settextposition( 16, 51 );
	ui_printf( "    Time : %3.1f [%3.1f]    \n", 
	    (float) (get_time_used()-start_time)/100, (float) time_alloc/100 );
	_settextposition( 17, 51 );
	ui_printf( "   Depth : %d   \n", search_depth );
	_settextposition( 18, 51 );
	ui_printf( "Searched : %lu    \n", boards_searched );
	_settextposition( 19, 51 );
	if( get_time_used() > start_time ) {
	    ui_printf( "   Speed : %lu n/s    \n", 
			boards_searched*100/(get_time_used()-start_time) );
	} else {
	    ui_printf( "   Speed : 0 n/s      \n" );
	}
	_settextposition( 20, 51 );
	ui_printf( "   Value : %d   \n", b_val );
	_settextposition( 21, 51 );
	ui_printf( "    Best : " );
	print_move( 0, move_square[0], move_type[0], move_length[0] );
	ui_printf( "     \n");

	if( move_square[0] == last_move_square &&
		move_type[0] == last_move_type &&
		move_length[0] == last_move_length ) {
	    same_move = TRUE;
	} else {
	    same_move = FALSE;
	    last_move_square = move_square[0];
	    last_move_type = move_type[0];
	    last_move_length = move_length[0];
	}
	time_used = get_time_used() - start_time;
    } while( ( b_val > -32000 && b_val < 32000 && search_depth < 10 ) && 
		( time_used < time_alloc / 6 ||
		( same_move == FALSE && time_used < time_alloc / 3 ) ) );
    if( side == BLACK ) {
	black_time += get_time_used() - start_time;
	_settextposition( 7, 59 );
    } else {
	white_time += get_time_used() - start_time;
	_settextposition( 11, 59 );
    }
    ui_printf( "%3d. ", (move_number+2)/2 );
    print_move( 0, move_square[0], move_type[0], move_length[0] );
      
    i = black_discs[0] + white_discs[0];
 
    ( *move_routine[move_type[0]] ) ( 0, move_square[0], move_length[0] );

    if( i != black_discs[0] + white_discs[0] ) {	/* disc captured */
	for( i=0; i<16; i++ ) {			/* so we can reset the list */
	    move_made[i][0] = -1;		/* of illegal positions! */
	}
    }

    /* update list of positions already encountered */

    hash_key_2[0] &= 15;
    hash_key_1[0] %= HASH_SIZE;

    for( i=0; move_made[hash_key_2[0]][i] >= 0; i++ ) {}

    move_made[hash_key_2[0]][i] = hash_key_1[0];
    move_made[hash_key_2[0]][i+1] = -1;

    put_pieces();
}

void human_move( int_8 side )
{
    uint_32	start_time;
    int_8	tail_sq, head_sq, new_tail;
    int_8	dy, dx;
    int_8	oy, ox;
    uint_8	y, x;
    uint_8	index;
    uint_8	d1, d2;
    uint_8	move_type;
    uint_8	move_length;

    start_time = get_time_used();

get_move:

    _settextposition( 28, 10 );
    ui_printf( "Please select tail marble position\n" );

    do {
        tail_sq = get_square( &c_row, &c_col );
        if( tail_sq == -1 ) {
	    goto get_move;
       }
    } while( board[row_sq[tail_sq]][col_sq[tail_sq]] != side );

    _settextposition( 28, 10 );
    ui_printf( "Please select head marble position\n" );

get_head_sq:

    head_sq = get_square( &c_row, &c_col );
    if( head_sq == -1 ) {
	goto get_move;
    }

    /* test for possible legality */

    y = row_sq[tail_sq];
    x = col_sq[tail_sq];

    if( y < c_row ) {
	dy = 1;
    } else if( y > c_row ) {
	dy = -1;
    } else {
	dy = 0;
    }
    if( x < c_col ) {
	dx = 1;
    } else if( x > c_col ) {
	dx = -1;
    } else {
	dx = 0;
    }

    if( ( c_row != y      || c_col != x ) &&
	( c_row != y+dy   || c_col != x+dx ) &&
	( c_row != y+2*dy || c_col != x+2*dx ) ) {
	goto get_head_sq;	/* group not formed legally */
    }

    if( c_row == y+2*dy && c_col == x+2*dx ) {
	move_length = 3;
    } else {
	move_length = 2;
    }

    if( c_row == y+dy && c_col == x+dx && board[c_row][c_col] == 0 ) {
	ox = dx;
	oy = dy;
	head_sq = tail_sq;
	goto move_ok;
    }

    if( board[c_row][c_col] != side ||
	board[y+dy][x+dx] != side ) {
	goto get_head_sq;
    }

    _settextposition( 28, 10 );
    ui_printf( "Please select new tail marble position\n" );

get_new_tail:

    new_tail = get_square( &c_row, &c_col );
    if( new_tail == -1 ) {
	goto get_move;
    }

    /* test for possible legality */

    if( y < c_row ) {
	oy = 1;
    } else if( y > c_row ) {
	oy = -1;
    } else {
	oy = 0;
    }
    if( x < c_col ) {
	ox = 1;
    } else if( x > c_col ) {
	ox = -1;
    } else {
	ox = 0;
    }

    if( c_row != y+oy || c_col != x+ox ) {
	goto get_move;		/* group not formed legally */
    }

    if( head_sq == tail_sq ) {
	dx = ox;
	dy = oy;
    }

move_ok:

    if( ox == dx && oy == dy &&
	board[row_sq[head_sq]+dy][col_sq[head_sq]+dx] == side ) {
	goto get_move;
    }

    d1 = 99;
    d2 = 99;

    for( index=0; index<6; index++ ) {
	if( dy == direction[0][index] && dx == direction[1][index] ) {
	    d1 = index;
	}
	if( oy == direction[0][index] && ox == direction[1][index] ) {
	    d2 = index;
	}
    }

    if( d1 == 99 || d2 == 99 ) {
	goto get_move;
    }

    if( d1 == d2 ) {
	move_type = d1;
    } else if( d1 == d2+1 || d1 == d2-5 ) {
	move_type = 6+2*d1;
    } else if( d1 == d2-1 || d1 == d2+5 ) {
	move_type = 6+2*d1+1;
    }

    index = black_discs[0] + white_discs[0];

    if( ( *move_routine[move_type] ) ( 0, tail_sq, move_length ) == FALSE ) {
	goto get_move;
    }

    /* move successful! */

    if( index != black_discs[0] + white_discs[0] ) {	/* disc captured */
	for( index=0; index<16; index++ ) {	/* so we can reset the list */
	    move_made[index][0] = -1;		/* of illegal positions! */
	}
    }

    /* update list of positions already encountered */

    hash_key_2[0] &= 15;
    hash_key_1[0] %= HASH_SIZE;

    for( index=0; move_made[hash_key_2[0]][index] >= 0; index++ ) {}

    move_made[hash_key_2[0]][index] = hash_key_1[0];
    move_made[hash_key_2[0]][index+1] = -1;

    if( side == BLACK ) {
	black_time += get_time_used() - start_time;
	_settextposition( 7, 59 );
	ui_printf( "%3d. ", (move_number+2)/2 );
	print_move( 0, tail_sq, move_type, move_length );
    } else {
	white_time += get_time_used() - start_time;
	_settextposition( 11, 59 );
	ui_printf( "%3d. ", (move_number+2)/2 );
	print_move( 0, tail_sq, move_type, move_length );
    }

    put_pieces();

    _settextposition( 28, 10 );
    ui_printf( "                                      \n" );
}

void print_square( uint_8 square )
{
    uint_8	row, col;

    row = row_sq[square];
    col = col_sq[square];

    ui_printf( "%c", 'j'-row );
    if( row > 4 ) {
	ui_printf( "%c", '0'+col );
    } else {
	ui_printf( "%c", '0'+row+col-5 );
    }
}

void print_move( uint_8 depth, uint_8 square, uint_8 type, uint_8 length )
{
    int_8	dy, dx;
    int_8	oy, ox;
    uint_8	i;
    uint_8	row, col;
    int_8	piece;

    depth = depth;

    row = row_sq[square];
    col = col_sq[square];
    piece = board[row][col];
    print_square( square );
    
    if( type < 6 ) {
	dy = direction[0][type];
	dx = direction[1][type];
	for( i=1; board[row+i*dy][col+i*dx] == piece; i++ ) {}
	if( i > 1 ) {
	    print_square( square_id[row+(i-1)*dy][col+(i-1)*dx] );
	}
	ui_printf( "-" );
	print_square( square_id[row+dy][col+dx] );
	ui_printf( "  " );
    } else {
	dy = direction[0][(type-6)/2];
	dx = direction[1][(type-6)/2];
	if( type % 2 == 0 ) {
	    oy = direction[0][((type-6)/2+5) % 6];
	    ox = direction[1][((type-6)/2+5) % 6];
	} else {
	    oy = direction[0][((type-6)/2+1) % 6];
	    ox = direction[1][((type-6)/2+1) % 6];
	}
	print_square( square_id[row+(length-1)*dy][col+(length-1)*dx] );
	ui_printf( "-" );
	print_square( square_id[row+oy][col+ox] );
    }
}
