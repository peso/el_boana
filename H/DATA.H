#define			MAX_DEPTH		32
#define			TRUE			1
#define			FALSE			0
#define			BLACK			1
#define			WHITE			-1
#define			INFINITY		32000
#define			COMPUTER		0
#define			HUMAN			1
#define			HASH_SIZE		16384

struct hash_entry {
    uint_8	move_square : 6;
    uint_8	move_type   : 5;
    uint_8	move_length : 1;
    uint_8	hash_lock   : 4;
};

struct move_data {
    uint_8	row;
    uint_8	col;
    int_8	old;
    int_8	new;
};

struct eval_features {
    uint_8	bpm : 2;
    uint_8	wpm : 2;
    uint_8	bpp : 4;
    uint_8	wpp : 4;
    uint_8	bcm : 1;
    uint_8	wcm : 1;
    uint_8	bwd : 2;
    uint_8	wwd : 2;
    uint_8	bds : 4;
    uint_8	wds : 4;
    uint_8	bcq : 3;
    uint_8	wcq : 3;
};

extern struct hash_entry black_hash[];
extern struct hash_entry white_hash[];
extern int_16 move_made[16][128];
extern uint_32 boards_searched;
extern uint_16 move_number;
extern uint_8 black_player;
extern uint_8 white_player;
extern uint_32 black_time;
extern uint_32 white_time;
extern uint_32 black_game_time;
extern uint_32 white_game_time;
extern uint_8 search_depth;
extern uint_8 move_square[];
extern uint_8 move_type[];
extern uint_8 move_length[];
extern uint_16 hash_key_1[];
extern uint_8 hash_key_2[];
extern struct move_data move_played[MAX_DEPTH][8];
extern int_8 board[16][16];
extern int_8 last_square[16][16];
extern int_8 square_id[16][16];
extern uint_8 row_sq[];
extern uint_8 col_sq[];
extern uint_8 max_length[6][64];
extern int_8 direction[2][8];
extern uint_16 rand_mask_1[16][16];
extern uint_8 rand_mask_2[16][16];
extern uint_8 black_discs[];
extern uint_8 white_discs[];
extern int_16 static_value[];
extern int_16 weight[16][16];
extern uint_8 order[61][32];
extern uint_16 line[MAX_DEPTH][32];
extern uint_16 multiplier[];
extern uint_16 col_mult[16][16];
extern uint_16 row_mult[16][16];
extern uint_16 diag_mult[16][16];
extern int_8 value_5[];
extern int_8 value_6[];
extern int_8 value_7[];
extern int_8 value_8[];
extern int_8 value_9[];
extern uint_8 capt_5[];
extern uint_8 capt_6[];
extern uint_8 capt_7[];
extern uint_8 capt_8[];
extern uint_8 capt_9[];
extern int_16 stable_factor[];
