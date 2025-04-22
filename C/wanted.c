// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// wanted.c, in PenroseC

#include "penrose.h"

#define PostScriptArrayLengthMax 65535
/*
	PostScript has a maximum array length of 65535, (per PLRM3, Appendix B, p739, table B.1, row 3), so
	each tiling's PostScript data can have at most that many rhombi. So for the ExportFormat PS_data
	there is a need to restrict output to those rhombi most wanted.
*/

double const rhAreaAvg = 0.8968; // = (sqrt(5) + sqrt(10 - sqrt(5)*2) - 1) / 4  ~=  0.8968022466674

long int rhombi_count_wanted(Tiling * const tlngP,  double const halfWidth,  bool const assignWantedness)
{
	RhombId rhId;
	Rhombus *rhP;
	long int numRhombi = 0;

	double const xMin = tlngP->wantedPostScriptCentre.x  -  halfWidth;
	double const xMax = tlngP->wantedPostScriptCentre.x  +  halfWidth;
	double const yMin = tlngP->wantedPostScriptCentre.y  -  halfWidth * tlngP->wantedPostScriptAspect;
	double const yMax = tlngP->wantedPostScriptCentre.y  +  halfWidth * tlngP->wantedPostScriptAspect;

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if(
			rhP->xMax > xMin  &&
			rhP->yMax > yMin  &&
			rhP->xMin < xMax  &&
			rhP->yMin < yMax
		)
		{
			numRhombi++;
			if( assignWantedness )
			{
				rhP->wantedPostScript = true;
				if( Fat == rhP->physique  &&  rhP->pathId >= 0 )  // second condition should be redundant
					tlngP->path[ rhP->pathId ].wantedPostScript = true;
			} // if( assignWantedness )
		}  // in bounds
	}  // for( rhId ... )
	return numRhombi;
}  // rhombi_count_wanted()


double wanted_halfWidth(Tiling * const tlngP)
{
	double halfWidthLarge = max_4(
		(tlngP->xMax - tlngP->wantedPostScriptCentre.x),
		(tlngP->wantedPostScriptCentre.x - tlngP->xMin),
		(tlngP->yMax - tlngP->wantedPostScriptCentre.y) / tlngP->wantedPostScriptAspect,
		(tlngP->wantedPostScriptCentre.y - tlngP->yMin) / tlngP->wantedPostScriptAspect
	);  // wantedPostScriptHalfWidth = max_4(..);

	if( tlngP->numFats + tlngP->numThins <= PostScriptArrayLengthMax )
	{
		return halfWidthLarge;
	}  // Everything fits into PostScript: hurray!
	else
	{
		long int countSmall, countLarge, countGuess;
		double halfWidthSmall, halfWidthGuess, fractionBetween, halfWidthSmall_Min;
		int8_t loopCount;

		halfWidthSmall_Min = halfWidthSmall = tlngP->edgeLength * sqrt(PostScriptArrayLengthMax * rhAreaAvg /tlngP->wantedPostScriptAspect);
		countSmall = rhombi_count_wanted(tlngP,  halfWidthSmall,  false);
		if( countSmall == PostScriptArrayLengthMax )
			return halfWidthSmall;
		while(countSmall > PostScriptArrayLengthMax )
		{
			halfWidthSmall_Min = halfWidthSmall = 0.9 * halfWidthSmall;
			countSmall = rhombi_count_wanted(tlngP,  halfWidthSmall,  false);
		}

		countLarge = rhombi_count_wanted(tlngP,  halfWidthLarge,  false);
		if( countLarge == PostScriptArrayLengthMax )
			return halfWidthLarge;
		while(countLarge < PostScriptArrayLengthMax )
		{
			halfWidthLarge = 1.1 * halfWidthLarge;
			countLarge = rhombi_count_wanted(tlngP,  halfWidthLarge,  false);
		}

		for( loopCount = 0  ;  loopCount < 32  ;  loopCount++ )
		{
			fractionBetween = ( sqrt(PostScriptArrayLengthMax) - sqrt(countSmall) ) / ( sqrt(countLarge) - sqrt(countSmall) );

			/* printf(
				"wanted_halfWidth(): tilingId=%" PRIi8 ", loopCount=%" PRIi8 ", countSmall=%li, countLarge=%li, fractionBetween=%0.9lf, halfWidthLarge=%0.9lf, halfWidthSmall=%0.9lf, halfWidthSmall_Min=%0.9lf.\n",
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
				"wanted_halfWidth(): possible error, tilingId=%" PRIi8 ", failure to converge with countSmall=%li, countLarge=%li, halfWidthSmall=%0.9lf, halfWidthLarge=%0.9lf.\n",
				tlngP->tilingId, countSmall, countLarge, halfWidthSmall, halfWidthLarge
			);  fflush(stderr);

		if( halfWidthSmall_Min == halfWidthSmall )
			return halfWidthSmall;
		else
			return( (halfWidthSmall_Min + halfWidthSmall) / 2 );
	}  // numFats + numThins > PostScriptArrayLengthMax
}  // wanted_halfWidth()


void wanted_populate(Tiling * const tlngP)
{
	RhombId rhId;
	PathId pathId;

	tlngP->wantedPostScriptHalfWidth = wanted_halfWidth(tlngP);

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
		tlngP->rhombi[rhId].wantedPostScript = false;
	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
		tlngP->path[pathId].wantedPostScript = false;

	tlngP->wantedPostScriptNumberRhombi = rhombi_count_wanted(tlngP,  tlngP->wantedPostScriptHalfWidth,  true);

	tlngP->wantedPostScriptNumberPaths = 0;
	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
		if( tlngP->path[pathId].wantedPostScript )
			(tlngP->wantedPostScriptNumberPaths) ++;

}  // wanted_populate()
