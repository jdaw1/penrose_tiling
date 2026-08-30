// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// stringClean.c, in PenroseC

#include "penrose.h"

void bounding_box_tiling(Tiling * const tlngP)
{
	RhombId rhId;
	Rhombus * rhP;

	if( tlngP->numFats <= 0  &&  tlngP->numThins <= 0 )  // Hopefully impossible
	{
		tlngP->xMin_rhId = -1;
		tlngP->xMax_rhId = -1;
		tlngP->yMin_rhId = -1;
		tlngP->yMax_rhId = -1;
		return;
	}  // No rhombi

	tlngP->xMin_rhId = 0;  // Should be redundant, as always tiles with fewer than four neighbours.
	tlngP->xMax_rhId = 0;
	tlngP->yMin_rhId = 0;
	tlngP->yMax_rhId = 0;

	// Several rhombi can share the extremal point; want one that is actually outside, so fewer than four neighbours.
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		if( tlngP->rhombi[rhId].numNeighbours < 4 )
		{
			tlngP->xMin_rhId = rhId;
			tlngP->xMax_rhId = rhId;
			tlngP->yMin_rhId = rhId;
			tlngP->yMax_rhId = rhId;
			break;
		}  // numNeighbours < 4
	}  // for( rhId ... )

	if( tlngP->axisAligned )
	{
		// To lessen machine-precision inconsistencies, the standard is materially further, hence machinePrecisionConsistency.
		// Huge radius is 10^4 tile edges, ~=0.4bn tiles; double good to ~=14 sig. fig.; so precision ~= 10^-10 edges; here assuming only 10^-6.
		double const machinePrecisionConsistency  =  tlngP->edgeLength / 1048576;  // 2^20 = 1048576 ~= 10^6
		for( rhId++  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )  // Deliberate start at next
		{
			rhP = &(tlngP->rhombi[rhId]);
			if(rhP->numNeighbours < 4)
			{
				if(tlngP->rhombi[ tlngP->xMin_rhId ].xMin > rhP->xMin + machinePrecisionConsistency)  {tlngP->xMin_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->xMax_rhId ].xMax < rhP->xMax - machinePrecisionConsistency)  {tlngP->xMax_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->yMin_rhId ].yMin > rhP->yMin + machinePrecisionConsistency)  {tlngP->yMin_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->yMax_rhId ].yMax < rhP->yMax - machinePrecisionConsistency)  {tlngP->yMax_rhId = rhId;}
			}  // numNeighbours < 4
		}  // for( rhId ... )
	}  // axisAligned?
	else
	{
		// Tiling is at a jaunty angle; fewer calculations for speed and accuracy.
		for( rhId++  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )  // Deliberate start at next
		{
			rhP = &(tlngP->rhombi[rhId]);
			if(rhP->numNeighbours < 4)
			{
				if(tlngP->rhombi[ tlngP->xMin_rhId ].xMin > rhP->xMin                              )  {tlngP->xMin_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->xMax_rhId ].xMax < rhP->xMax                              )  {tlngP->xMax_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->yMin_rhId ].yMin > rhP->yMin                              )  {tlngP->yMin_rhId = rhId;}
				if(tlngP->rhombi[ tlngP->yMax_rhId ].yMax < rhP->yMax                              )  {tlngP->yMax_rhId = rhId;}
			}  // numNeighbours < 4
		}  // for( rhId ... )
	}  // axisAligned?
}  // bounding_box_tiling()
