// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// purgeDuplicates.c, in PenroseC

#include "penrose.h"

int rhombiGt_ByY(Rhombus const * const rhP0, Rhombus const * const rhP1)
{
	if(     rhP0->centre.y > rhP1->centre.y ) return +1 ;
	if(     rhP0->centre.y < rhP1->centre.y ) return -1 ;
	if(     rhP0->centre.x > rhP1->centre.x ) return +1 ;
	if(     rhP0->centre.x < rhP1->centre.x ) return -1 ;
	return 0;
}  // rhombiGt_ByY()


void rhombi_purgeDuplicates(Tiling * const tlngP)
{
	RhombId        rhId1, rhId2;
	long int       numUniques = 0;
	bool           isUnique;
	double const   thresholdSame = tlngP->edgeLength * 0.01 ;  // Arbitrary constant, smaller than 1, bigger than machine-precision epsilon
	long int const numRhombi = tlngP->numFats + tlngP->numThins;

	if( tlngP->numFats < 2  &&  tlngP->numThins < 2 )
		return;

	rhombi_sort(tlngP,  &rhombiGt_ByY,  false);

	printf(
		"rhombi_purgeDuplicates(): tilingId=%" PRIi8 ",  rhNumMax=%li,  before numRhombi=%li; ",
		tlngP->tilingId,
		tlngP->rhombi_NumMax,
		tlngP->numFats + tlngP->numThins
	);  // Erroneously, a fflush() here seems to generates extra newline.

	for (numUniques = rhId1 = 0  ;  rhId1 < numRhombi  ;  rhId1++)  // numRhombi, not Fats+Thins, so sure to reach end despite duplicates being pruned.
	{
		isUnique = true ;
		for( rhId2 = numUniques - 1  ;  rhId2 >= 0  ;  rhId2-- )
		{
			if(     ( tlngP->rhombi[rhId1].centre.y - tlngP->rhombi[rhId2].centre.y )  >  thresholdSame )  break;  // Because sorted by y, know no matches
			if( fabs( tlngP->rhombi[rhId1].centre.x - tlngP->rhombi[rhId2].centre.x )  >  thresholdSame )  continue;
			isUnique = false;  // know this is a match
			break;
		}  // for rhId2

		if( isUnique )
		{
			if( numUniques < rhId1 )
				tlngP->rhombi[numUniques] = tlngP->rhombi[rhId1] ;
			numUniques++;
		} else {
			if( Fat == tlngP->rhombi[rhId1].physique )
				(tlngP->numFats)--;
			else
				(tlngP->numThins)--;
		}
	}  // for rhId1

	printf("after numUniques=%li\n", numUniques);  fflush(stdout);

	// Renumber remaining rhombi
	for (rhId1 = 0  ;  rhId1 < tlngP->numFats + tlngP->numThins  ;  rhId1 ++)
		tlngP->rhombi[rhId1].rhId = rhId1 ;
}  // rhombi_purgeDuplicates()
