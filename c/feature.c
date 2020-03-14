
#include "sysdept.h"
#include "data.h"

int_16 count_value( uint_8 depth )
{
    return( (int_16) value_5[line[depth][0]] +
                     value_6[line[depth][1]] +
                     value_7[line[depth][2]] +
                     value_8[line[depth][3]] +
                     value_9[line[depth][4]] +
                     value_8[line[depth][5]] +
                     value_7[line[depth][6]] +
                     value_6[line[depth][7]] +
                     value_5[line[depth][8]] +
                     value_5[line[depth][9]] +
                     value_6[line[depth][10]] +
                     value_7[line[depth][11]] +
                     value_8[line[depth][12]] +
                     value_9[line[depth][13]] +
                     value_8[line[depth][14]] +
                     value_7[line[depth][15]] +
                     value_6[line[depth][16]] +
                     value_5[line[depth][17]] +
                     value_5[line[depth][18]] +
                     value_6[line[depth][19]] +
                     value_7[line[depth][20]] +
                     value_8[line[depth][21]] +
                     value_9[line[depth][22]] +
                     value_8[line[depth][23]] +
                     value_7[line[depth][24]] +
                     value_6[line[depth][25]] +
                     value_5[line[depth][26]] );
}

uint_8 count_capture( uint_8 depth )
{
    return( capt_5[line[depth][0]] +
            capt_6[line[depth][1]] +
            capt_7[line[depth][2]] +
            capt_8[line[depth][3]] +
            capt_9[line[depth][4]] +
            capt_8[line[depth][5]] +
            capt_7[line[depth][6]] +
            capt_6[line[depth][7]] +
            capt_5[line[depth][8]] +
            capt_5[line[depth][9]] +
            capt_6[line[depth][10]] +
            capt_7[line[depth][11]] +
            capt_8[line[depth][12]] +
            capt_9[line[depth][13]] +
            capt_8[line[depth][14]] +
            capt_7[line[depth][15]] +
            capt_6[line[depth][16]] +
            capt_5[line[depth][17]] +
            capt_5[line[depth][18]] +
            capt_6[line[depth][19]] +
            capt_7[line[depth][20]] +
            capt_8[line[depth][21]] +
            capt_9[line[depth][22]] +
            capt_8[line[depth][23]] +
            capt_7[line[depth][24]] +
            capt_6[line[depth][25]] +
            capt_5[line[depth][26]] );
}
