
#include "sysdept.h"
#include "data.h"

/*
 *  El Boana's evaluation function uses only table-lookups for each of the
 *  27 possible lines.  These tables include a variety of different features,
 *  which are initialized in this routine.
 */

void init_tables()
{
    uint_8	bpm, wpm;
    uint_8	bpp, wpp;
    uint_8	bcm, wcm;
    uint_8	bwd, wwd;
    uint_8	bds, wds;
    uint_8	bcq, wcq;
    uint_8	length;
    uint_16	pos, test;
    int_8	i;
    int_8	disc[9];
    int_8	vulnerable;
    uint_8	c1, c2, empty;
    bool	bcontrol[9], wcontrol[9];
    uint_8	start, size;
    int_16	value;
    

    for( length=5; length < 10; length++ ) {
	for( pos=0; pos<multiplier[length]; pos++ ) { 

	/* initialize feature values */

	    bpm = 0;
	    wpm = 0;	/* push mobility */
	    bpp = 0;
	    wpp = 0;	/* potential push mobility */
	    bcm = 0;
	    wcm = 0;	/* capture mobility */
	    bwd = 0;
	    wwd = 0;	/* wedged discs */
	    bds = 0;
	    wds = 0;	/* disc strength */
	    bcq = 0;
	    wcq = 0;	/* connectedness quotient */

	/* determine disc arrangement */

	    test = pos;
	    for( i=0; i<length; i++ ) {
		if( test % 3 == 0 ) {
		    disc[i] = WHITE;
		} else if( test % 3 == 1 ) {
		    disc[i] = 0;
		} else {
		    disc[i] = BLACK;
		}
		test /= 3;
	    }

	/* compute push mobilty and capture mobility */

	    for( i=-1; i<length-3; i++ ) {
		if( ( i == -1 || disc[i] == 0 ) && disc[i+1] != 0 ) {
		    if( disc[i+2] == disc[i+3] && disc[i+2] == -disc[i+1] ) {
			if( disc[i+3] == BLACK ) {
			    bpm++;
			    if( i == -1 ) {
				bcm++;
			    }
			} else {
			    wpm++;
			    if( i == -1 ) {
				wcm++;
			    }
			}
		    } else if( i < length-5 && disc[i+1] == disc[i+2]
		        && disc[i+3] == -disc[i+2] && disc[i+4] == disc[i+3]
						   && disc[i+5] == disc[i+4] ) {
			if( disc[i+3] == BLACK ) {
			    bpm++;
			    if( i == -1 ) {
				bcm++;
			    }
			} else {
			    wpm++;
			    if( i == -1 ) {
				wcm++;
			    }
			}
		    }
		}
	    }

	    for( i=length; i>2; i-- ) {
		if( ( i == length || disc[i] == 0 ) && disc[i-1] != 0 ) {
		    if( disc[i-2] == disc[i-3] && disc[i-2] == -disc[i-1] ) {
			if( disc[i-3] == BLACK ) {
			    bpm++;
			    if( i == length ) {
				bcm++;
			    }
			} else {
			    wpm++;
			    if( i == length ) {
				wcm++;
			    }
			}
		    } else if( i > 4 && disc[i-1] == disc[i-2]
		        && disc[i-3] == -disc[i-2] && disc[i-4] == disc[i-3]
						   && disc[i-5] == disc[i-4] ) {
			if( disc[i-3] == BLACK ) {
			    bpm++;
			    if( i == length ) {
				bcm++;
			    }
			} else {
			    wpm++;
			    if( i == length ) {
				wcm++;
			    }
			}
		    }
		}
	    }

	/* potential push mobility */

	    for( i=0; i<length && disc[i] == 0; i++ ) {}

	    if( i < length ) {
		vulnerable = disc[i];
		c1 = 0;
		for( ; i<length && disc[i] != -vulnerable; i++ ) {
		    if( disc[i] == vulnerable ) {
			c1++;
		    }
		}
		if( i<length && c1 < 3 ) {
		    c2 = 0;
		    for( ; i<length && disc[i] != vulnerable; i++ ) {
			if( disc[i] == -vulnerable ) {
			    c2++;
			}
		    }
		    if( c2 > c1 ) {
			empty = 0;
			for( i=0; disc[i] != -vulnerable; i++ ) {
			    if( disc[i] == 0 ) {
				empty++;
			    }
			}
			if( empty == 0 ) {
			    if( vulnerable == BLACK ) {
				wpp += 2*c1 + 2;
			    } else {
				bpp += 2*c1 + 2;
			    }
			} else if( empty == 1 ) {
			    if( vulnerable == BLACK ) {
				wpp += c1 + 1;
			    } else {
				bpp += c1 + 1;
			    }
			} else {
			    if( vulnerable == BLACK ) {
				wpp += c1;
			    } else {
				bpp += c1;
			    }
			}
		    }
		}
	    }

	    for( i=length-1; i>=0 && disc[i] == 0; i-- ) {}

	    if( i >= 0 ) {
		vulnerable = disc[i];
		c1 = 0;
		for( ; i>=0 && disc[i] != -vulnerable; i-- ) {
		    if( disc[i] == vulnerable ) {
			c1++;
		    }
		}
		if( i>=0 && c1 < 3 ) {
		    c2 = 0;
		    for( ; i>=0 && disc[i] != vulnerable; i-- ) {
			if( disc[i] == -vulnerable ) {
			    c2++;
			}
		    }
		    if( c2 > c1 ) {
			empty = 0;
			for( i=length-1; disc[i] != -vulnerable; i-- ) {
			    if( disc[i] == 0 ) {
				empty++;
			    }
			}
			if( empty == 0 ) {
			    if( vulnerable == BLACK ) {
				wpp += 2*c1 + 2;
			    } else {
				bpp += 2*c1 + 2;
			    }
			} else if( empty == 1 ) {
			    if( vulnerable == BLACK ) {
				wpp += c1 + 1;
			    } else {
				bpp += c1 + 1;
			    }
			} else {
			    if( vulnerable == BLACK ) {
				wpp += c1;
			    } else {
				bpp += c1;
			    }
			}
		    }
		}
	    }

	/* disc strength */

	    for( i=0; i<length; i++ ) {
		bcontrol[i] = FALSE;
		wcontrol[i] = FALSE;
	    }

	    start = 0;
	    while( start < length ) {
		size=0;
		for( i=start; i<length && disc[i] != WHITE; i++ ) {
		    if( disc[i] == BLACK ) {
			size++;
		    }
		}
		for( i=start; i<length && disc[i] != WHITE; i++ ) {}
		if( size == 1 ) {
		    bds++;
		} else if( size == 2 ) {
		    bds += 3;
		} else {
		    bds += 2*size;
		}
		start = i;

		size=0;
		for( i=start; i<length && disc[i] != BLACK; i++ ) {
		    if( disc[i] == WHITE ) {
			size++;
		    }
		}
		for( i=start; i<length && disc[i] != BLACK; i++ ) {}
		if( size == 1 ) {
		    wds++;
		} else if( size == 2 ) {
		    wds += 3;
		} else {
		    wds += 2*size;
		}

		start = i;
	    }

	/* connectedness quotient & wedged discs */

	    for( c1=0; c1<length && disc[c1] != BLACK; c1++ ) {}
	    if( c1 < length ) {
	        for( c2=length-1; c2>=0 && disc[c2] != BLACK; c2-- ) {}
	        bcq = c2-c1+1;
	        for( i=c1+1; i<c2; i++ ) {
		    if( disc[i] == WHITE ) {
			wwd++;
		    }
		}
	    }
	    for( c1=0; c1<length && disc[c1] != WHITE; c1++ ) {}
	    if( c1 < length ) {
	        for( c2=length-1; c2>=0 && disc[c2] != WHITE; c2-- ) {}
	        wcq = c2-c1+1;
	        for( i=c1+1; i<c2; i++ ) {
		    if( disc[i] == BLACK ) {
			bwd++;
		    }
		}
	    }

	    value = 15*(bpm-wpm)
       	    	   +14*(bpp-wpp)
       	    	   +18*(bwd-wwd)
       	    	   +28*(bds-wds)
       	    	   - 5*(bcq-wcq);

	    if( length == 5 ) {
	        value_5[pos] = value/10;
		capt_5[pos] = 16*bcm + wcm;
	    } else if( length == 6 ) {
	        value_6[pos] = value/8;
		capt_6[pos] = 16*bcm + wcm;
	    } else if( length == 7 ) {
	        value_7[pos] = value*3/20;
		capt_7[pos] = 16*bcm + wcm;
	    } else if( length == 8 ) {
	        value_8[pos] = value*7/40;
		capt_8[pos] = 16*bcm + wcm;
	    } else if( length == 9 ) {
	        value_9[pos] = value/5;
		capt_9[pos] = 16*bcm + wcm;
	    }
	}
    }
}
