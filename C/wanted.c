// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// wanted.c, in PenroseC

#include "penrose.h"

/*
	PostScript in an early-nineteen-eighties printer-control language. It has a
	maximum array length of 65535, (per PLRM3, Appendix B, p739, table B.1, row 3),
	so each tiling's PostScript data can have at most that many rhombi. So for the
	ExportFormat PS_data there is a need to restrict output to those rhombi most wanted.
 
	wantedPostScriptCentre and tlngP->wantedPostScriptHalfWidth are in the unit-edgeLength used for output.
*/

double const rhAreaAvg = 0.89680224666742;  // (sqrt(5) + sqrt(10 - sqrt(5)*2) - 1) / 4

long int rhombi_count_wanted(Tiling * const tlngP,  double const halfWidth,  bool const assignWantedness)
{
	RhombId rhId;
	Rhombus *rhP;
	long int numRhombiWanted = 0;

	// wantedPostScriptCentre and halfWidth are in the units assuming unit edgeLength.
	// Multiply by actual edge to work in original space.
	double const xMin  =  tlngP->edgeLength * (tlngP->wantedPostScriptCentre.x  -  halfWidth);
	double const xMax  =  tlngP->edgeLength * (tlngP->wantedPostScriptCentre.x  +  halfWidth);
	double const yMin  =  tlngP->edgeLength * (tlngP->wantedPostScriptCentre.y  -  halfWidth * tlngP->wantedPostScriptAspect);
	double const yMax  =  tlngP->edgeLength * (tlngP->wantedPostScriptCentre.y  +  halfWidth * tlngP->wantedPostScriptAspect);

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if(
		   rhP->xMin < xMax  &&  rhP->xMax > xMin  &&
		   rhP->yMin < yMax  &&  rhP->yMax > yMin
		)
		{
			numRhombiWanted ++;
			if( assignWantedness )
			{
				rhP->wantedPostScript = true;
				if( Fat == rhP->physique  &&  rhP->pathId >= 0 )  // second condition should be redundant
					tlngP->path[rhP->pathId].wantedPostScript = true;
			} // if( assignWantedness )
		}  // in bounds
	}  // for( rhId ... )

	return numRhombiWanted;
}  // rhombi_count_wanted()


double wanted_halfWidth(Tiling * const tlngP)
{
	if( tlngP->numFats + tlngP->numThins <= PostScriptArrayLengthMax )
	{
		return max_4(
			+(tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength  -  tlngP->wantedPostScriptCentre.x),
			-(tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength  -  tlngP->wantedPostScriptCentre.x),
			+(tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength  -  tlngP->wantedPostScriptCentre.y) / tlngP->wantedPostScriptAspect,
			-(tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength  -  tlngP->wantedPostScriptCentre.y) / tlngP->wantedPostScriptAspect
		);  // return max_4()
	}  // Everything fits into PostScript array: hurray!

	// Know that numFats + numThins > PostScriptArrayLengthMax
	long int countSmall = PostScriptArrayLengthMax + 1,  countLarge = -1,  countGuess;  // Assignments prevent compiler mis-complaining about possibly not assigned.
	double halfWidthLarge, halfWidthSmall, halfWidthGuess, fractionBetween, halfWidthSmall_Min;
	int8_t loopCount;

	halfWidthLarge = -1;
	halfWidthSmall_Min = halfWidthSmall  =  sqrt(PostScriptArrayLengthMax * rhAreaAvg / tlngP->wantedPostScriptAspect);
	for( loopCount = 0  ;  loopCount < 64  ;  loopCount++ )
	{
		countSmall = rhombi_count_wanted(tlngP,  halfWidthSmall,  false);
		if( countSmall == PostScriptArrayLengthMax )
			return halfWidthSmall;
		else if(countSmall < PostScriptArrayLengthMax)
			break;
		else
		{
			// countSmall > PostScriptArrayLengthMax
			halfWidthLarge = halfWidthSmall;
			countLarge = countSmall;
			halfWidthSmall_Min = halfWidthSmall = 0.9 * halfWidthSmall;
		}
	}  // halfWidthSmall

	if( countLarge < 0  ||  halfWidthLarge < 0 )  // either condition ought to suffice.
	{
		halfWidthLarge = halfWidthSmall / 0.9;
		for( loopCount = 0  ;  loopCount < 64  ;  loopCount++ )
		{
			countLarge = rhombi_count_wanted(tlngP,  halfWidthLarge,  false);
			if( countLarge == PostScriptArrayLengthMax )
				return halfWidthLarge;
			else if(countLarge > PostScriptArrayLengthMax)
				break;
			else  // countLarge < PostScriptArrayLengthMax
				halfWidthLarge = 1.1 * halfWidthLarge;
		}  // halfWidthLarge
	}  // countLarge, halfWidthLarge not yet defined

	for( loopCount = 0  ;  loopCount < 64  ;  loopCount++ )
	{
		fractionBetween = ( sqrt(PostScriptArrayLengthMax) - sqrt(countSmall) ) / ( sqrt(countLarge) - sqrt(countSmall) );

		/* printf(
			"wanted_halfWidth(): tilingId=%" PRIi8 ", loopCount=%" PRIi8 ", countSmall=%li, countLarge=%li, fractionBetween=%.9lf, halfWidthLarge=%.9lf, halfWidthSmall=%.9lf, halfWidthSmall_Min=%.9lf.\n",
			tlngP->tilingId, loopCount, countSmall, countLarge, fractionBetween, halfWidthLarge, halfWidthSmall, halfWidthSmall_Min
		);  fflush(stdout); */

		if( fractionBetween < 0.0625 )
			fractionBetween = 0.0625;
		else if( fractionBetween > 0.9375 )
			fractionBetween = 0.9375;

		halfWidthGuess = halfWidthLarge * fractionBetween  +  (1 - fractionBetween) * halfWidthSmall;
		countGuess = rhombi_count_wanted(tlngP,  halfWidthGuess,  false);
		if( countGuess < PostScriptArrayLengthMax )
		{
			halfWidthSmall = halfWidthGuess;
			if( countGuess > countSmall)
				halfWidthSmall_Min = halfWidthSmall;
			countSmall = countGuess;
		}
		else if( countGuess > PostScriptArrayLengthMax )
		{
			halfWidthLarge = halfWidthGuess;
			countLarge = countGuess;
		}
		else
			return halfWidthGuess;
	}  // for( loopCount ... )

	if( (double)countSmall < 0.99 * PostScriptArrayLengthMax )  // 65535 * 0.01 = 655.35
		fprintf(stderr,
			"wanted_halfWidth(): possible error, tilingId=%" PRIi8 ", failure to converge with countSmall=%li, countLarge=%li, halfWidthSmall=%.9lf, halfWidthLarge=%.9lf.\n",
			tlngP->tilingId, countSmall, countLarge, halfWidthSmall, halfWidthLarge
		);  fflush(stderr);

	if( halfWidthSmall_Min == halfWidthSmall )
		return halfWidthSmall;
	else
		return( (halfWidthSmall_Min + halfWidthSmall) / 2 );
}  // wanted_halfWidth()


void wanted_populate(Tiling * const tlngP)
{
	RhombId rhId;
	PathId pathId;

	tlngP->wantedPostScriptHalfWidth  =  wanted_halfWidth(tlngP);

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
		tlngP->rhombi[rhId].wantedPostScript = false;
	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
		tlngP->path[pathId].wantedPostScript = false;

	tlngP->wantedPostScriptNumRhombi = rhombi_count_wanted(tlngP,  tlngP->wantedPostScriptHalfWidth,  true);

	tlngP->wantedPostScriptNumPaths = 0;
	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
		if( tlngP->path[pathId].wantedPostScript )
			(tlngP->wantedPostScriptNumPaths) ++;
}  // wanted_populate()
