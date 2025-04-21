// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// propertyVerifying.c, in PenroseC

#include "penrose.h"

void verifyHypothesisedProperties(Tiling const * const tlngP)
{
/*
	bool hypothesis;
	RhombId rhId_outer, rhId_inner;
	long long int numTests;
	Rhombus *rhOuterP, *rhInnerP;
	const double threshold = tlngP->edgeLength / 16;

	rhId_inner = -1;  // removes incorrect compiler warning about being possibly undefined.
*/


	
/*
	// Is it true that thin south touches only souths?
	// Slow, so suppressed.

	hypothesis = true;
	numTests = 0;
	for( rhId_outer = 0  ;  rhId_outer < tlngP->numFats + tlngP->numThins  ;  rhId_outer ++ )
	{
		rhOuterP = tlngP->rhombii + rhId_outer;
		if( Thin == rhOuterP->physique )
		{
			for( rhId_inner = 0  ;  rhId_inner < tlngP->numFats + tlngP->numThins  ;  rhId_inner ++ )
			{
				rhInnerP = tlngP->rhombii + rhId_inner;
				numTests ++;
				if(
					(fabs(rhOuterP->south.x - rhInnerP->north.x) < threshold  &&  fabs(rhOuterP->south.y - rhInnerP->north.y) < threshold) ||
					(fabs(rhOuterP->south.x - rhInnerP->east.x ) < threshold  &&  fabs(rhOuterP->south.y - rhInnerP->east.y ) < threshold) ||
					(fabs(rhOuterP->south.x - rhInnerP->west.x ) < threshold  &&  fabs(rhOuterP->south.y - rhInnerP->west.y ) < threshold)
				)
				{
					hypothesis = false;
					goto hypothesis_thin_south;
				}  // this south == a non-south
			}  // for( rhId_inner ... )
		}  // Outer is thin
	}  // for( rhId_outer ... )
hypothesis_thin_south:
	if( hypothesis )
		printf("verifyHypothesisedProperties: tilingId=%" PRIi8 ", numTests=%lli: holds that south of thins shares a vertex only with other souths.\n", tlngP->tilingId, numTests);
	else
		printf(
			"!!! verifyHypothesisedProperties: tilingId=%" PRIi8 ", numTests=%lli: hypothesis fails, as it there is a south of thin shares a vertex with a non-souths, with rhId_outer=%li, rhId_inner=%li.\n",
			tlngP->tilingId, numTests, rhId_outer, rhId_inner
		);
*/



/*
	// Is it true that fat north never touches fat south?
	// Slow, so suppressed.

hypothesis = true;
	numTests = 0;
	for( rhId_outer = 0  ;  rhId_outer < tlngP->numFats + tlngP->numThins  ;  rhId_outer ++ )
	{
		rhOuterP = tlngP->rhombii + rhId_outer;
		if( Fat == rhOuterP->physique )
		{
			for( rhId_inner = 0  ;  rhId_inner < rhId_outer  ;  rhId_inner ++ )
			{
				rhInnerP = tlngP->rhombii + rhId_inner;
				if( Fat == rhInnerP->physique )
				{
					numTests ++;
					if(
						(fabs(rhOuterP->south.x - rhInnerP->north.x) < threshold  &&  fabs(rhOuterP->south.y - rhInnerP->north.y) < threshold) ||
						(fabs(rhOuterP->north.x - rhInnerP->south.x) < threshold  &&  fabs(rhOuterP->north.y - rhInnerP->south.y) < threshold)
						)
					{
						hypothesis = false;
						goto hypothesis_fat_north_south;
					}  // fat south == fat north
				}  // Inner is fat
			}  // for( rhId_inner ... )
		}  // Outer is fat
	}  // for( rhId_outer ... )
hypothesis_fat_north_south:
	if( hypothesis )
		printf("verifyHypothesisedProperties: tilingId=%" PRIi8 ", numTests=%lli: holds that fat north never shares a vertex with fat south.\n", tlngP->tilingId, numTests);
	else
		printf(
			"!!! verifyHypothesisedProperties: tilingId=%" PRIi8 ", numTests=%lli: hypothesis fails, as it there is a fat north sharing a vertex with a fat south, with rhId_outer=%li, rhId_inner=%li.\n",
			tlngP->tilingId, numTests, rhId_outer, rhId_inner
		);
*/




}  // verifyHypothesisedProperties()
