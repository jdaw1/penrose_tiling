// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// thins_0T4F_1T3F_count.c, in PenroseC

#include "penrose.h"

void thins_0T4F_1T3F_count(Tiling * const tlngP)
{
	register RhombId rhId;
	register Rhombus const * rhP;
	int numThinNeighbours;

	tlngP->numThins_0T4F = 0;
	tlngP->numThins_1T3F = 0;
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( Thin == rhP->physique  &&  4 == rhP->numNeighbours )
		{
			numThinNeighbours = 0;
			if(Thin == rhP->neighbours[0].physique) {numThinNeighbours++;}
			if(Thin == rhP->neighbours[1].physique) {numThinNeighbours++;}
			if(Thin == rhP->neighbours[2].physique) {numThinNeighbours++;}
			if(Thin == rhP->neighbours[3].physique) {numThinNeighbours++;}

			if( 0 == numThinNeighbours )
				(tlngP->numThins_0T4F) ++;
			else if( 1 == numThinNeighbours )
				(tlngP->numThins_1T3F) ++;
			else
				fprintf(stderr,
					"\n\n\nError, thins_0T4F_1T3F_count(): tilingId=%" PRIi8
					", thin with rhId=%li, impossibly has %i thin neighbours. Continuing.\n\n\n",
					tlngP->tilingId, rhId, numThinNeighbours
				);  // fprintf()
		}  // Thin  &&  4 neighbours
	}  // for( rhId ... )
	return;
}  // thins_0T4F_1T3F_count()
