
#include <time.h>

#include "sysdept.h"

uint_32 get_time_used()
{
    return( (uint_32) clock() );
}
