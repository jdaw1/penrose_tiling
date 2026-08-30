// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// smalls.c, in PenroseC

#include "penrose.h"
#include <string.h>

/* Most of these functions 'should' be inline, but C99 does not allow an elegant single definition of inline. So not. :-( */

extern double max_2(double const d0, double const d1)  {return( d0 >= d1 ? d0 : d1 );}  // max_2()
extern double min_2(double const d0, double const d1)  {return( d0 <= d1 ? d0 : d1 );}  // min_2()

extern double max_4(double const d0, double const d1, double const d2, double const d3)  {return max_2( d0 >= d1 ? d0 : d1,  d2 >= d3 ? d2 : d3 );}  // max_4()
extern double min_4(double const d0, double const d1, double const d2, double const d3)  {return min_2( d0 <= d1 ? d0 : d1,  d2 <= d3 ? d2 : d3 );}  // min_4()


extern double avg_2(double const d0, double const d1)
{
	return (d0 == d1)  ?  d0  :  (d0 + d1)/2;  // Precision fussiness
}  // avg_2()


extern double median_3(double const d0, double const d1, double const d2)  // https://stackoverflow.com/a/1582406
{
	return (d0 <= d1)
		? ((d1 <= d2) ? d1 : ((d0 < d2) ? d2 : d0))
		: ((d0 <= d2) ? d0 : ((d1 < d2) ? d2 : d1));
}  // median_3()


int lessThan(double const * const dP0, double const * const dP1) {return ( (*dP0) < (*dP1)  ?  +1  :  (*dP0) > (*dP1)  ?  -1  :  0);}
extern double median_4(double const d0, double const d1, double const d2, double const d3)
{
	double d[4];
	d[0] = d0;
	d[1] = d1;
	d[2] = d2;
	d[3] = d3;
	qsort( d, 4, sizeof(double), (int(*)(const void * const,const void * const)) &lessThan );
	if(d[1] == d[2])
		return d[1];  // Precision fussiness
	else
		return (d[1] + d[2]) / 2;
}  // median_4()




extern bool points_diff_2(double const edgeLength, XY const xy0, XY const xy1)
{
	const double threshold = edgeLength / 16;
	return(
		fabs(xy0.x - xy1.x) > threshold  ||
		fabs(xy0.y - xy1.y) > threshold
	);  // return()
}  // points_diff_2()


extern bool points_same_2(double const edgeLength, XY const xy0, XY const xy1)
{
	const double threshold = edgeLength / 256;
	return(
		fabs(xy0.x - xy1.x) <= threshold  &&
		fabs(xy0.y - xy1.y) <= threshold
	);  // return()
}  // points_same_2()


extern bool points_diff_3(double const edgeLength, XY const xy0, XY const xy1, XY const xy2)
{
	const double threshold = edgeLength / 16;
	return(
		fabs(xy0.x - xy1.x) > threshold  ||
		fabs(xy0.y - xy1.y) > threshold  ||
		fabs(xy1.x - xy2.x) > threshold  ||
		fabs(xy1.y - xy2.y) > threshold  ||
		fabs(xy2.x - xy0.x) > threshold  ||
		fabs(xy2.y - xy0.y) > threshold
	);  // return()
}  // points_diff_3()


extern bool points_same_3(double const edgeLength, XY const xy0, XY const xy1, XY const xy2)
{
	const double threshold = edgeLength / 16;
	return(
		fabs(xy0.x - xy1.x) <= threshold  &&
		fabs(xy0.y - xy1.y) <= threshold  &&
		fabs(xy1.x - xy2.x) <= threshold  &&
		fabs(xy1.y - xy2.y) <= threshold  &&
		fabs(xy2.x - xy0.x) <= threshold  &&
		fabs(xy2.y - xy0.y) <= threshold
	);  // return()
}  // points_same_3()


extern bool points_diff_4(double const edgeLength, XY const xy0, XY const xy1, XY const xy2, XY const xy3)
{
	const double threshold = edgeLength / 16;
	return(
		fabs(xy0.x - xy1.x) > threshold  ||
		fabs(xy0.y - xy1.y) > threshold  ||
		fabs(xy1.x - xy2.x) > threshold  ||
		fabs(xy1.y - xy2.y) > threshold  ||
		fabs(xy2.x - xy3.x) > threshold  ||
		fabs(xy2.y - xy3.y) > threshold  ||
		fabs(xy3.x - xy0.x) > threshold  ||
		fabs(xy3.y - xy0.y) > threshold  ||
		fabs(xy0.x - xy2.x) > threshold  ||
		fabs(xy0.y - xy2.y) > threshold  ||
		fabs(xy1.x - xy3.x) > threshold  ||
		fabs(xy1.y - xy3.y) > threshold
	);  // return()
}  // points_diff_4()


extern bool points_same_4(double const edgeLength, XY const xy0, XY const xy1, XY const xy2, XY const xy3)
{
	const double threshold = edgeLength / 16;
	return(
		fabs(xy0.x - xy1.x) <= threshold  &&
		fabs(xy0.y - xy1.y) <= threshold  &&
		fabs(xy1.x - xy2.x) <= threshold  &&
		fabs(xy1.y - xy2.y) <= threshold  &&
		fabs(xy2.x - xy3.x) <= threshold  &&
		fabs(xy2.y - xy3.y) <= threshold  &&
		fabs(xy3.x - xy0.x) <= threshold  &&
		fabs(xy3.y - xy0.y) <= threshold  &&
		fabs(xy0.x - xy2.x) <= threshold  &&
		fabs(xy0.y - xy2.y) <= threshold  &&
		fabs(xy1.x - xy3.x) <= threshold  &&
		fabs(xy1.y - xy3.y) <= threshold
	);  // return()
}  // points_same_4()


extern long int newlinesInString(char const * const str)
{
	long int charNum, numLines = 0;
	for( charNum = 0  ;  str[charNum] != 0  ;  charNum ++ )
		if( '\n' == str[charNum] )
			numLines ++;
	return numLines;
}  // newlinesInString()




char * fileExtension_from_ExportFormat(char * const str, ExportFormat const ef)
{
	switch( ef )
	{
	case PS_rhomb:
	case PS_arcs:
	case PS_data:
		strcpy(str, "ps");
		return str;

	case TSV:
		strcpy(str, "tsv");
		return str;

	case SVG_rhomb:
	case SVG_arcs:
		strcpy(str, "svg");
		return str;

	case JSON:
		strcpy(str, "json");
		return str;

	default:
		strcpy(str, "???");
		return str;
	}  // switch
}  // fileExtension_from_exportFormat()



bool collinear(XY const xy0, XY const xy1, XY const xy2, const Tiling * const tlngP)
{
	const double twiceArea =
		xy0.x * (xy1.y - xy2.y) +
		xy1.x * (xy2.y - xy0.y) +
		xy2.x * (xy0.y - xy1.y) ;
	const double squareBase0 = (
		(xy1.x - xy2.x) * (xy1.x - xy2.x)  +
		(xy1.y - xy2.y) * (xy1.y - xy2.y)
	);  // squareBase0
	const double squareBase1 = (
		(xy0.x - xy2.x) * (xy0.x - xy2.x)  +
		(xy0.y - xy2.y) * (xy0.y - xy2.y)
	);  // squareBase1
	const double squareBase2 = (
		(xy0.x - xy1.x) * (xy0.x - xy1.x)  +
		(xy0.y - xy1.y) * (xy0.y - xy1.y)
	);  // squareBase2
	/*
		TwiceArea <= Base * (EdgeLength / 64)  <==>
		TwiceArea^2  <=  Base^2  *  EdgeLength^2 / 4096  <==>
		4096 * TwiceArea^2  <=  Base^2  *  EdgeLength^2
		Must be non-strict in case all points identical.
	*/
	return(
		4096 * twiceArea * twiceArea  // Threshold is one sixty-fourth of edgeLength
		<= max_2(max_2(squareBase0, squareBase1), squareBase2)
		* tlngP->edgeLength * tlngP->edgeLength
	);  // return()
}  // collinear()



FILE * file_open(const char *filename,  const char *mode,  const char *errorString)
{
	FILE *fp = fopen(filename, mode);
	if( fp == NULL )
	{
		printf(
			"\n\nError: fopen('%s', '%s') returned NULL; errorString being '%s'. Exiting.\n\n",
			filename, mode, errorString
		);  // printf()
		exit(EXIT_FAILURE);
	}
	else
		return fp;
}  // file_open()



char * filename_new_extension(const char * const filenameOld,  char * strNew,  const char * const extensionNew)
{
	size_t i;
	bool good;
	strcpy(strNew, filenameOld);

	for(i = 0;  strNew[i] != 0  ;  i++ )
		;

	for( ;  ;  i-- )
	{
		if( strNew[i] == '.' )
			{good = true;  break;}
		if( i < 0  ||  i > 16000)  // size_t usually unsigned
			{good = false;  break;}
	}

	if( good )
	{
		strcpy(strNew + i + 1,  extensionNew);
		return strNew ;
	}
	else
		return NULL ;
}  // filename_new_extension()



char * seed_type_name(char * const seedTypeName,  const SeedType seed)
{
	switch(seed)  // Possibilities: oneThin, oneFat, round5
	{
	case oneThin:
		sprintf(seedTypeName, "OneThin");
		break;

	case oneFat:
		sprintf(seedTypeName, "OneFat");
		break;

	case round5:
		sprintf(seedTypeName, "Round_5");
		break;

	default:
		printf("seed_type_name(): unknown seed type. Ending.\n");
		exit(EXIT_FAILURE);
	}  // switch(seed)

	return seedTypeName;
}  // seed_type_name()
