
#include <time.h>

#include "sysdept.h"

/* Return time in sec/100 units */
uint_32 get_time_used()
{
    return( (uint_32) clock()/10 );
}
