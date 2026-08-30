// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// radii_populate.c, in PenroseC

#include "penrose.h"

void radii_populate(Tiling * const tlngP)
{
	RhombId rhId;
	Rhombus const * rhP;
	double rSqd, rSqdMax,  rSqdShortOpen;
	long int openPathLengthThreshold = -1;
	PathStatId   pathStatId;
	PathStats  * pathStatP;
	Path       * pathP;

	// Need length longest open path
	for( pathStatId = 0  ;  pathStatId < tlngP->pathStats_NumMax  ;  pathStatId++ )
	{
		pathStatP = &(tlngP->pathStat[ pathStatId ]);
		if( (! pathStatP->pathClosed)  &&  openPathLengthThreshold < pathStatP->pathLength )  // Open, longer
			openPathLengthThreshold = pathStatP->pathLength;  // Longest open path
	}  // for( pathStatId ... )

	rSqdMax = 0;
	rSqdShortOpen = DBL_MAX;

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( Fat == rhP->physique )
		{
			pathP = &(tlngP->path[ rhP->pathId ]);
			if( (! pathP->pathClosed)  &&  pathP->pathLength < openPathLengthThreshold )  // Open, shorter than longest
			{
				rSqd = min_4(
					rhP->north.x * rhP->north.x  +  rhP->north.y * rhP->north.y,
					rhP->south.x * rhP->south.x  +  rhP->south.y * rhP->south.y,
					rhP->east.x  * rhP->east.x   +  rhP->east.y  * rhP->east.y ,
					rhP->west.x  * rhP->west.x   +  rhP->west.y  * rhP->west.y
				 );  // min_4()
				if( rSqdShortOpen > rSqd )
					rSqdShortOpen = rSqd;
			}  // Inside of short open path
		}  // Fat
		rSqd = max_4(
			rhP->north.x * rhP->north.x  +  rhP->north.y * rhP->north.y,
			rhP->south.x * rhP->south.x  +  rhP->south.y * rhP->south.y,
			rhP->east.x  * rhP->east.x   +  rhP->east.y  * rhP->east.y ,
			rhP->west.x  * rhP->west.x   +  rhP->west.y  * rhP->west.y
		 );  // max_4()
		if( rSqdMax < rSqd )
			rSqdMax = rSqd;
	}  // for( rhId ... )

	tlngP->radiusMax       = sqrt( rSqdMax );
	tlngP->radiusShortOpen = (rSqdShortOpen <= rSqdMax  ?  sqrt(rSqdShortOpen)  :  tlngP->radiusMax);

	printf(
		"radii_populate(): tilingId=%" PRIi8 ";  radiusMax=%0.6lf;  openPathLengthThreshold=%li;  radiusShortOpen=%0.6lf;  'area'~=%0.1lf;  propn all tiles~=%0.6lf\n",
		tlngP->tilingId,   tlngP->radiusMax / tlngP->edgeLength,  openPathLengthThreshold,
		tlngP->radiusShortOpen / tlngP->edgeLength,  Pi * pow(tlngP->radiusShortOpen / tlngP->edgeLength, 2),
		Pi * pow(tlngP->radiusShortOpen / tlngP->edgeLength, 2) / (Cos18 * tlngP->numFats  +  Cos36 * tlngP->numThins)
	);  // printf()

}  // radii_populate()
