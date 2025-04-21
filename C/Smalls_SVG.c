// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// smalls_SVG.c, in PenroseC

#include "penrose.h"

#define almostZero 5E-10  // consistent with %0.9lf

char * svgTransform(char * const str,  double const x,  double const y,  double const angDeg, short int angMod )
{
	double angDegNew = angDeg;
	int8_t i;
	extern char scratchString[];

	for( i = 0  ;  i < 5  ; i++ )  // Finitely many in case a DBL_MAX has somehow crept in. Shouldn't happen, but...
		if( angDegNew < 0 )
			angDegNew += angMod;
		else
			break;

	for( i = 0  ;  i < 5  ; i++ )
		if( angDegNew + almostZero >= angMod )
			angDegNew -= angMod;
		else
			break;


	// Six cases: {Ang != 0,  Ang == 0}  *  {Y != 0,  Y == 0 but X != 0,  both == 0}
	if( fabs(y) <= almostZero )
	{
		if( fabs(angDegNew) <= almostZero )
		{
			if( fabs(x) <= almostZero )
				str[0] = 0;
			else
				sprintf(str, " transform='translate(%0.9lf)'", x);
		}  // angDegNew == 0
		else
		{
			if( fabs(x) <= almostZero )
				sprintf(str, " transform='rotate(%0.9lf)'", angDegNew);
			else
				sprintf(str, " transform='translate(%0.9lf) rotate(%0.9lf)'", x, angDegNew);
		}  // angDegNew != 0
	}  // y == 0
	else
	{
		if( fabs(angDegNew) <= almostZero )
			sprintf(str, " transform='translate(%0.9lf,%0.9lf)'", x, y);
		else
			sprintf(str, " transform='translate(%0.9lf,%0.9lf) rotate(%0.9lf)'", x, y, angDegNew);
	}  // y != 0

	stringClean(scratchString);
	return str;
}  // svgTransform()



// A is 'long' string; B is 'short' string.
void exportColourSVG(char * const strA,  char * const strB,  bool * const isWhiteP,  const Physique ph,  const bool pathClosed,  const long int pathLength,  const bool pointy)
{
	strA[0] = 0;  // default
	strB[0] = 0;  // default
	*isWhiteP = false;  // default

	if( Thin == ph )
	{
		sprintf(strB, "#CCC");
		sprintf(strA, "fill='%s' opacity='1'>  <!-- Thins: light grey -->", strB);
		return ;
	}

	if( pathClosed )  // Fat and closed
	{
		if( pathLength == 5  &&  pointy )
		{
			sprintf(strB, "#9F9");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed PathLength=5 pointy: light green -->", strB);
			return ;
		}  // 5 pointy

		if( pathLength == 5 ) // so round
		{
			*isWhiteP = true;
			sprintf(strB, "#FFF");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed PathLength=5 round: white -->", strB);
			return ;
		}  // 5 round

		if( pathLength == 15 )
		{
			sprintf(strB, "#004C00");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=15: very dark green -->", strB);
			return ;
		}  // 15 closed

		if( pathLength == 25 )
		{
			sprintf(strB, "#009");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=25: dark blue -->", strB);
			return ;
		}  // 25 closed

		if( pathLength == 55 )
		{
			sprintf(strB, "#00F");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=55: blue -->", strB);
			return ;
		}  // 55 closed

		if( pathLength == 105 )
		{
			sprintf(strB, "#F6F");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=105: pink -->", strB);
			return ;
		}  // 105 closed

		if( pathLength == 215 )
		{
			sprintf(strB, "#600000");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=215: maroon -->", strB);
			return ;
		}  // 215 closed

		if( pathLength == 425 )
		{
			sprintf(strB, "#090");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=425: dark green -->", strB);
			return ;
		}  // 425 closed

		if( pathLength == 855 )
		{
			sprintf(strB, "#09F");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=855: light blue -->", strB);
			return ;
		}  // 855 closed

		if( pathLength == 1705 )
		{
			sprintf(strB, "#F60");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength=1705: orange -->", strB);
			return ;
		}  // 1705 closed

		if( pathLength >= 3415 )
		{
			sprintf(strB, "#191919");
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, closed, PathLength>=3415: very dark grey -->", strB);
			return ;
		}  // >=3415 closed
	}  // Closed
	else
	{
		if( pathLength >= 84 )
		{
			sprintf(strB, "#%2X0000", 128  +  ( 73 * ((int)floor(log2(pathLength) + 0.5)) ) % 128 );  // The +0.5 distinguishes the likes of 128 and 253.
			sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, open, PathLength=%li: shades of red -->", strB, pathLength);
			return ;
		}  // Open and long

		if( pathLength == 3  ||  pathLength == 4 )
		{
			sprintf(strB, "#333");
			sprintf(strA, "fill='%s' opacity='1' stroke='#000'>  <!-- Fats, open, PathLength=%li: dark grey, to give whole tiling appearance of a serrated edge -->", strB, pathLength);
			return ;
		}  // Open, 3 or 4, to give whole tiling appearance ofa serrated edge.

		*isWhiteP = true;
		sprintf(strB, "#FFF");
		sprintf(strA, "fill='%s' opacity='1'>  <!-- Fats, open PathLength=%li: white -->", strB, pathLength);
		return ;  // Open, neither long nor 3 nor 4, boringly white.

	} // Open
}  // exportColourSVG()


