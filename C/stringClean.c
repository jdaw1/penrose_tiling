// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// stringClean.c, in PenroseC

#include "penrose.h"

void stringClean(char *str)
{
	long int posnDot, posnNumberEnd, posnNonZeroesEnd, offset, to, from, i;

	for( posnDot = 0  ;  str[posnDot] != 0  ;  posnDot++ )
		if( '.' == str[posnDot] )
		{
			for( posnNumberEnd = 1 + posnDot  ;  '0' <= str[posnNumberEnd]  &&  str[posnNumberEnd] <= '9'  ;  posnNumberEnd++ )
				;
			for( posnNonZeroesEnd = posnNumberEnd - 1  ;  '0' == str[posnNonZeroesEnd] || '.' == str[posnNonZeroesEnd]  ;  posnNonZeroesEnd-- )
				if( '.' == str[posnNonZeroesEnd] )
				{
					posnNonZeroesEnd = posnNonZeroesEnd - 1 ;
					break ;
				}
			offset = posnNumberEnd - posnNonZeroesEnd - 1 ;
			if( offset > 0 )
				for( to = posnNonZeroesEnd + 1  ;  ;  to++ )
				{
					from = to + offset ;
					/*
						0123456789        = char position
						3.145000          = string
						posnNumberEnd     = 8
						posnNonZeroesEnd  = 4
						offset            = 8 - 4 - 1 = 3
						first to          = 5
						first from        = 5 + 3 = 8
					*/
					str[to] = str[from] ;
					if( 0 == str[from] )
						break ;
				}  // to
			posnDot = posnNonZeroesEnd + 1 ;
		}  // '.'

	// Remove "-0"
	for( i = 0  ;  0 != str[i]  ;  i++ )
		if( i >= 2  &&  '-' == str[i-2]  &&  '0' == str[i-1]  &&  (str[i] < '0'  ||  str[i] > '9')  &&  '.' != str[i] )
		{
			offset = 1 ;
			for( to = i - 2  ;  ;  to++ )
			{
				from = to + offset ;
				str[to] = str[from] ;
				if( 0 == str[from] )
					break ;
			}  // to
		}
}  // stringClean
