
#include "sysdept.h"

#define			MAX_DEPTH		32
#define			TRUE			1
#define			FALSE			0
#define			BLACK			1
#define			WHITE			-1
#define			INFINITY		32000
#define			COMPUTER		0
#define			HUMAN			1
#define			HASH_SIZE		16384

/* the following structure defines hash table entries */

struct hash_entry {
    uint_8	move_square : 6;
    uint_8	move_type   : 5;
    uint_8	move_length : 1;	/* 1 means 3, 0 means 2 */
    uint_8	hash_lock   : 4;	/* pads length to 16 bits */
};

/* hash tables for recognizing repeated positions */

struct hash_entry black_hash[HASH_SIZE];	
struct hash_entry white_hash[HASH_SIZE];	

/*
 *  Note that in multidimensional arrays to follow, secondary indices are
 *  always rounded up to the nearest power of 2.  This wastes memory, but
 *  improves the efficiency of code generated.
 */

/* board positions already encountered (indexed by hash lock) */

int_16 move_made[16][128];

/* keep track of boards searched statistic */

uint_32 boards_searched;

/* number of moves made in game */

uint_16 move_number = 0;

/* player indicators (HUMAN/COMPUTER) */

uint_8 black_player;
uint_8 white_player;

/* timing variables */

uint_32 black_time;
uint_32 white_time;
uint_32 black_game_time;
uint_32 white_game_time;

/* current search depth */

uint_8 search_depth;

/* most recent stored move at each level */

uint_8 move_square[MAX_DEPTH];
uint_8 move_type[MAX_DEPTH];
uint_8 move_length[MAX_DEPTH];

/* hash keys at each level */

uint_16 hash_key_1[MAX_DEPTH];
uint_8 hash_key_2[MAX_DEPTH];

/*
 *  Rather than storing a separate board for each depth, we maintain
 *  only a copy of the current board position which is updated moving
 *  up and down the tree by referring to the array move_played.
 */

/* structure containing move information */

struct move_data {
    uint_8	row;
    uint_8	col;
    int_8	old;
    int_8	new;
};

/* information regarding moves made at each level */

struct move_data move_played[MAX_DEPTH][8];

/* actual board stored at each depth */

int_8 board[16][16] =	{ {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
             	          {  0,  0,  0,  0,  0, -1, -1, -1, -1, -1,  0 },
	                  {  0,  0,  0,  0, -1, -1, -1, -1, -1, -1,  0 },
	                  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
	                  {  0,  0,  1,  0,  0,  0,  0,  0,  0, -1,  0 },
	                  {  0,  1,  1,  0,  0,  0,  0,  0, -1, -1,  0 },
	                  {  0,  1,  1,  0,  0,  0,  0, -1, -1,  0,  0 },
	                  {  0,  1,  1,  0,  0,  0, -1, -1,  0,  0,  0 },
	                  {  0,  1,  1,  0,  0, -1, -1,  0,  0,  0,  0 },
	                  {  0,  1,  1,  0, -1, -1,  0,  0,  0,  0,  0 },
	                  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };

int_8 last_square[16][16];

/* lookup for square index of given row/col */

int_8 square_id[16][16] = { { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
                            { -1, -1, -1, -1, -1,  0,  1,  2,  3,  4, -1 }, 
                            { -1, -1, -1, -1,  5,  6,  7,  8,  9, 10, -1 }, 
                            { -1, -1, -1, 11, 12, 13, 14, 15, 16, 17, -1 }, 
                            { -1, -1, 18, 19, 20, 21, 22, 23, 24, 25, -1 }, 
                            { -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, -1 }, 
                            { -1, 35, 36, 37, 38, 39, 40, 41, 42, -1, -1 }, 
                            { -1, 43, 44, 45, 46, 47, 48, 49, -1, -1, -1 }, 
                            { -1, 50, 51, 52, 53, 54, 55, -1, -1, -1, -1 }, 
                            { -1, 56, 57, 58, 59, 60, -1, -1, -1, -1, -1 },
                            { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 } };

/* lookups for row/col of a given square index */

uint_8 row_sq[61] = {             1, 1, 1, 1, 1,
                               2, 2, 2, 2, 2, 2,
                            3, 3, 3, 3, 3, 3, 3,
                         4, 4, 4, 4, 4, 4, 4, 4,
                      5, 5, 5, 5, 5, 5, 5, 5, 5,
                      6, 6, 6, 6, 6, 6, 6, 6,
                      7, 7, 7, 7, 7, 7, 7,
                      8, 8, 8, 8, 8, 8,
                      9, 9, 9, 9, 9              };

uint_8 col_sq[61] = {             5, 6, 7, 8, 9,
                               4, 5, 6, 7, 8, 9,
                            3, 4, 5, 6, 7, 8, 9,
                         2, 3, 4, 5, 6, 7, 8, 9,
                      1, 2, 3, 4, 5, 6, 7, 8, 9,
                      1, 2, 3, 4, 5, 6, 7, 8,
                      1, 2, 3, 4, 5, 6, 7,
                      1, 2, 3, 4, 5, 6,
                      1, 2, 3, 4, 5              };

/*
 *  There are 18 different possible move types from a given base square:
 *
 *    - LINE/PUSH moves are possible in each of 6 directions:
 *
 *				      0     1
 *				      |   /
 *				      | /
 *				5 ----+---- 2
 *				    / |
 *		      		  /   |
 *	     		 	4     3
 *
 *    - BROADSIDE/ROLL moves are possible in two ways for each of the 6
 *	  possible initial directions:
 *
 *			      *     7	      8     *
 *			      *   /	      |   *
 *	    17		      * /	      | *	           10
 *	    |		6 ----*               *---- 9 	          /
 *	    |						        /
 *    *******                                                 *******
 *	  /						      |
 *	/	       15 ----*		      *---- 12	      |
 *    16		    * |		    / *		      11
 *			  *   |		  /   *
 *			*     14	13    *
 */

/*
 *  For each LINE/PUSH move direction, we create an array which stores
 *  the maximum group length which can be moved in this way so as to 
 *  remain on the legal portion of the board.  In testing moves which
 *  capture a piece, a group length of one more than this number is
 *  legal; otherwise, this is the maximum group length possible for a
 *  legal move.  Testing for BROADSIDE/ROLL moves then requires simply
 *  checking a possible LINE/MOVE in the rolling direction from both the
 *  base square and the head square.
 */

uint_8 max_length[6][64] = { {              0, 0, 0, 0, 0,	/* dir 0 */
                                         0, 1, 1, 1, 1, 1,
                                      0, 1, 2, 2, 2, 2, 2,
                                   0, 1, 2, 3, 3, 3, 3, 3,
                                0, 1, 2, 3, 4, 4, 4, 4, 4,
                                1, 2, 3, 4, 5, 5, 5, 5, 
                                2, 3, 4, 5, 6, 6, 6,  
                                3, 4, 5, 6, 7, 7,  
                                4, 5, 6, 7, 8              },

                             {              0, 0, 0, 0, 0,	/* dir 1 */
                                         1, 1, 1, 1, 1, 0,
                                      2, 2, 2, 2, 2, 1, 0,
                                   3, 3, 3, 3, 3, 2, 1, 0,
                                4, 4, 4, 4, 4, 3, 2, 1, 0,
                                5, 5, 5, 5, 4, 3, 2, 1, 
                                6, 6, 6, 5, 4, 3, 2,  
                                7, 7, 6, 5, 4, 3,  
                                8, 7, 6, 5, 4              },

                             {              4, 3, 2, 1, 0,	/* dir 2 */
                                         5, 4, 3, 2, 1, 0,
                                      6, 5, 4, 3, 2, 1, 0,
                                   7, 6, 5, 4, 3, 2, 1, 0,
                                8, 7, 6, 5, 4, 3, 2, 1, 0,
                                7, 6, 5, 4, 3, 2, 1, 0, 
                                6, 5, 4, 3, 2, 1, 0,  
                                5, 4, 3, 2, 1, 0,  
                                4, 3, 2, 1, 0              },

                             {              8, 7, 6, 5, 4,	/* dir 3 */
                                         7, 7, 6, 5, 4, 3,
                                      6, 6, 6, 5, 4, 3, 2,
                                   5, 5, 5, 5, 4, 3, 2, 1,
                                4, 4, 4, 4, 4, 3, 2, 1, 0,
                                3, 3, 3, 3, 3, 2, 1, 0, 
                                2, 2, 2, 2, 2, 1, 0,  
                                1, 1, 1, 1, 1, 0,  
                                0, 0, 0, 0, 0              },

                             {              4, 5, 6, 7, 8,	/* dir 4 */
                                         3, 4, 5, 6, 7, 7,
                                      2, 3, 4, 5, 6, 6, 6,
                                   1, 2, 3, 4, 5, 5, 5, 5,
                                0, 1, 2, 3, 4, 4, 4, 4, 4,
                                0, 1, 2, 3, 3, 3, 3, 3, 
                                0, 1, 2, 2, 2, 2, 2,
                                0, 1, 1, 1, 1, 1,  
                                0, 0, 0, 0, 0              },

                             {              0, 1, 2, 3, 4,	/* dir 5 */
                                         0, 1, 2, 3, 4, 5,
                                      0, 1, 2, 3, 4, 5, 6,
                                   0, 1, 2, 3, 4, 5, 6, 7,
                                0, 1, 2, 3, 4, 5, 6, 7, 8,
                                0, 1, 2, 3, 4, 5, 6, 7, 
                                0, 1, 2, 3, 4, 5, 6,  
                                0, 1, 2, 3, 4, 5,  
                                0, 1, 2, 3, 4              } };

/* This gives differentials dx and dy for given direction */

int_8 direction[2][8] = { { -1, -1,  0,  1,  1,  0 },
			  {  0,  1,  1,  0, -1, -1 } };

/*
 *  The following two tables are used for generating hash keys associated
 *  with each position.  The first will be used to generate the index 
 *  associated with the position in the hash table, and the second generates
 *  a hash lock which is tested to see if the position stored could 
 *  correspond to the position we're looking at.  Of course, it is still
 *  by no means a certainty that the positions were the same, so the move
 *  must still be tested for legality before it's searched.
 */

uint_16 rand_mask_1[16][16] = 
{ { 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0 },
  { 0,     0,     0,     0,     0, 15245,   322, 21843, 20112,   905,     0 },
  { 0,     0,     0,     0, 28558, 29615, 17340, 20293, 30362,  7115,     0 },
  { 0,     0,     0, 24488,  6849, 25190, 10663, 20052,  8701, 16114,     0 },
  { 0,     0, 14659, 31680, 24825, 22590,  9887,  5100,  5045, 14922,     0 },
  { 0,  3515,   728, 13873, 12566, 19095, 29828,  1133, 18594, 31027,     0 },
  { 0, 21744, 31337, 19694, 30095, 20508, 21541, 18938, 23467,     0,     0 },
  { 0, 21000,  3489,  3014,  1927,  1716, 25309,  7762,     0,     0,     0 },
  { 0,  5411, 23072, 20441, 19870, 24703, 30796,     0,     0,     0,     0 },
  { 0,  4245,  9642,  1435, 19768,  8465,     0,     0,     0,     0,     0 },
  { 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0 } };

uint_8 rand_mask_2[16][16] = 
{ { 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0 },
  { 0,     0,     0,     0,     0,   244,    29,   146,    99,    96,     0 },
  { 0,     0,     0,     0,    25,   222,   191,   140,   213,   234,     0 },
  { 0,     0,     0,   219,   120,    81,   182,   183,    36,   141,     0 },
  { 0,     0,    66,    83,   144,   137,   142,   175,   188,    69,     0 },
  { 0,   154,   203,   168,   193,   102,   167,    84,   253,   242,     0 },
  { 0,    67,   192,   249,    62,   159,   236,   181,    74,     0,     0 },
  { 0,   187,   216,    49,    22,   151,   132,   109,     0,     0,     0 },
  { 0,   162,    51,   240,   105,   238,   143,     0,     0,     0,     0 },
  { 0,    28,    37,   250,   171,     8,     0,     0,     0,     0,     0 } ,
  { 0,     0,     0,     0,     0,     0,     0,     0,     0,     0,     0 } };

/* 
 *  The evaluator needs to know how many black and white discs there are,
 *  along with the current static board value.  For efficiency, these are
 *  maintained as we traverse the tree rather than being computed at the
 *  terminal node to be evaluated.
 */

uint_8 black_discs[MAX_DEPTH] = { 14 };
uint_8 white_discs[MAX_DEPTH] = { 14 };
int_16 static_value[MAX_DEPTH] = { 0 };

/* this array stores the weighted square values */

int_16 weight[16][16] = { {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
			  {  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  0 },
			  {  0,  0,  0,  0,  1,  2,  2,  2,  2,  1,  0 },
			  {  0,  0,  0,  1,  2,  3,  3,  3,  2,  1,  0 },
			  {  0,  0,  1,  2,  3,  4,  4,  3,  2,  1,  0 },
			  {  0,  1,  2,  3,  4,  5,  4,  3,  2,  1,  0 },
			  {  0,  1,  2,  3,  4,  4,  3,  2,  1,  0,  0 },
			  {  0,  1,  2,  3,  3,  3,  2,  1,  0,  0,  0 },
			  {  0,  1,  2,  2,  2,  2,  1,  0,  0,  0,  0 },
			  {  0,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0 },
			  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 } };

/*  
 *  Search ordering is critical for speed.  The array order[][] lists the
 *  available move types at each square in approximate order of value.  For
 *  example, moves towards the center are stronger than moves away from
 *  the center of the board.  It is initialized in init.c.  After testing
 *  the hash table and killer moves, subsequent moves are taken in the
 *  order given here.
 */

uint_8 order[61][32];

/*
 *  El Boana's evaluator uses table-lookups for each of the 21 diagonal
 *  lines.  Line indices need to be maintained up and down the tree as
 *  moves are generated.  The array line[][] stores the index for each
 *  diagonal type at each depth.  Arrays col_mult[][], row_mult[][],
 *  and multiplier[] are constants showing how line indices change as
 *  a square on the board changes.
 */

uint_16 line[MAX_DEPTH][32];

uint_16 multiplier[10] =
      {     1,     3,     9,    27,    81,   243,   729,  2187,  6561, 19683 };

uint_16 col_mult[16][16] = 
      {	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    1,    3,    9,   27,   81,    0 },
	{    0,    0,    0,    0,    1,    3,    9,   27,   81,  243,    0 },
	{    0,    0,    0,    1,    3,    9,   27,   81,  243,  729,    0 },
	{    0,    0,    1,    3,    9,   27,   81,  243,  729, 2187,    0 },
	{    0,    1,    3,    9,   27,   81,  243,  729, 2187, 6561,    0 },
	{    0,    1,    3,    9,   27,   81,  243,  729, 2187,    0,    0 },
	{    0,    1,    3,    9,   27,   81,  243,  729,    0,    0,    0 },
	{    0,    1,    3,    9,   27,   81,  243,    0,    0,    0,    0 },
	{    0,    1,    3,    9,   27,   81,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };

uint_16 row_mult[16][16] = 
      {	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    1,    1,    1,    1,    1,    0 },
	{    0,    0,    0,    0,    1,    3,    3,    3,    3,    3,    0 },
	{    0,    0,    0,    1,    3,    9,    9,    9,    9,    9,    0 },
	{    0,    0,    1,    3,    9,   27,   27,   27,   27,   27,    0 },
	{    0,    1,    3,    9,   27,   81,   81,   81,   81,   81,    0 },
	{    0,    3,    9,   27,   81,  243,  243,  243,  243,    0,    0 },
	{    0,    9,   27,   81,  243,  729,  729,  729,    0,    0,    0 },
	{    0,   27,   81,  243,  729, 2187, 2187,    0,    0,    0,    0 },
	{    0,   81,  243,  729, 2187, 6561,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };

uint_16 diag_mult[16][16] = 
      {	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    1,    1,    1,    1,    1,    0 },
	{    0,    0,    0,    0,    3,    3,    3,    3,    3,    1,    0 },
	{    0,    0,    0,    9,    9,    9,    9,    9,    3,    1,    0 },
	{    0,    0,   27,   27,   27,   27,   27,    9,    3,    1,    0 },
	{    0,   81,   81,   81,   81,   81,   27,    9,    3,    1,    0 },
	{    0,  243,  243,  243,  243,   81,   27,    9,    3,    0,    0 },
	{    0,  729,  729,  729,  243,   81,   27,    9,    0,    0,    0 },
	{    0, 2187, 2187,  729,  243,   81,   27,    0,    0,    0,    0 },
	{    0, 6561, 2187,  729,  243,   81,    0,    0,    0,    0,    0 },
	{    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0 } };

/* these tables store evaluations for all possible line configurations */

int_8 value_5[243];
int_8 value_6[729];
int_8 value_7[2187];
int_8 value_8[6561];
int_8 value_9[19683];

/* these tables keep track of captures for all possible line configurations */

uint_8 capt_5[243];
uint_8 capt_6[729];
uint_8 capt_7[2187];
uint_8 capt_8[6561];
uint_8 capt_9[19683];

/* captured discs increase in weight by the ratio in stable_factor */

int_16 stable_factor[400];

