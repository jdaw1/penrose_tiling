// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// insideness.c, in PenroseC

#include "penrose.h"

typedef struct  // PathIdRange
{
	long int pathLength;
	int8_t   pathClosedTypeNum;
	PathId pathId_Start;
	PathId pathId_Final;
	double radiusMax;
} PathIdRange;


static inline PathId PathByY(
	Path const * const paths,
	PathId const pathIdLow,
	PathId const pathIdHigh,
	double const yCentre,
	bool const above
)
{
	// above means that want high-y if equal (i.e., also high Id).
	PathId pathIdguess;
	PathId pathIdGuess_Low  = pathIdLow;
	PathId pathIdGuess_High = pathIdHigh;

	while(pathIdGuess_High - pathIdGuess_Low > 1)
	{
		pathIdguess = (pathIdGuess_High + pathIdGuess_Low) / 2;
		if(      paths[pathIdguess].centre.y > yCentre )
			pathIdGuess_High = pathIdguess - 1;
		else if( paths[pathIdguess].centre.y < yCentre )
			pathIdGuess_Low  = pathIdguess + 1;
		else
		{
			// 'guess' == y
			if( above )
				pathIdGuess_Low  = pathIdguess;
			else
				pathIdGuess_High = pathIdguess;
		}  // 'guess' == y
	}  // while()

	return( above ? pathIdGuess_High : pathIdGuess_Low );
}  // PathByY()


void insideness_populate(Tiling * const tlngP)
{
	RhombId rhId;
	PathId  pathThisId, pathOuterId;
	Path    *pathThisP;
	PathIdRange *pathIdRange;
	long int const pathIdRangeNum_NumMax = 32;  // Enough for max path length of 7bn, containing 18.6tr tiles, enough to cover the whole UK.
	PathId   pathIdRange_Id, pathLoopStart, pathLoopEnd;
	long int pathIdRangeNum_Num, pathLength_Shortest;
	int8_t   innerPCTN, diffsPCTN, nghbrNum, nghbrNum_best;
	Neighbour *nghbrP;

	if( tlngP->numPathsClosed == 0 )
		return ;

	pathIdRange = malloc( pathIdRangeNum_NumMax  *  sizeof(PathIdRange) );
	if( NULL == pathIdRange )
	{
		fprintf(stderr, "insideness_populate(): !!! NULL == pathIdRange !!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // if( NULL == pathIdRange )

	// Closed paths earlier.  Within which, longer paths earlier.  Then low-Y paths earlier.  Then low-X paths earlier.
	paths_sort(tlngP, &pathGt_ByClosedEtc);

	for( pathIdRange_Id = 0  ;  pathIdRange_Id < pathIdRangeNum_NumMax  ;  pathIdRange_Id ++ )
	{
		pathIdRange[pathIdRange_Id].pathLength = 0;
		pathIdRange[pathIdRange_Id].pathId_Start = -1;
		pathIdRange[pathIdRange_Id].pathId_Final = -1;
		pathIdRange[pathIdRange_Id].radiusMax = 0;
	}  // for( pathIdRange_Id ... )

	for( pathThisId = 0  ;  pathThisId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathThisId ++ )
	{
		tlngP->path[pathThisId].pathId_ShortestOuter = -1;
		tlngP->path[pathThisId].pathId_LongestInner  = -1;
		tlngP->path[pathThisId].insideThis_NumFats   =  0;
		tlngP->path[pathThisId].insideDeep_NumFats   =  0;
		tlngP->path[pathThisId].insideThis_NumThins  =  0;
		tlngP->path[pathThisId].insideDeep_NumThins  =  0;
	}  // for( pathThisId ... )

	for( rhId = 0;  rhId < tlngP->numFats + tlngP->numThins;  rhId++ )
		tlngP->rhombi[rhId].pathId_ShortestOuter = -1;

	pathIdRangeNum_Num = 0;
	for( pathThisId = 0  ;  pathThisId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathThisId++ )
	{
		if( tlngP->path[pathThisId].pathLength < 15  ||  ! tlngP->path[pathThisId].pathClosed )
			break;
		if( pathIdRangeNum_Num > 0  &&  pathIdRange[pathIdRangeNum_Num - 1].pathLength == tlngP->path[pathThisId].pathLength )
		{
			pathIdRange[pathIdRangeNum_Num - 1].pathId_Final = pathThisId;
			if( pathIdRange[pathIdRangeNum_Num - 1].radiusMax < tlngP->path[pathThisId].radiusMax )
				pathIdRange[pathIdRangeNum_Num - 1].radiusMax = tlngP->path[pathThisId].radiusMax;
		}  // length same as previous
		else
		{
			// the first, or different length to previous
			if( pathIdRangeNum_Num >= pathIdRangeNum_NumMax )
			{
				fprintf(stderr,
					"insideness_populate(): pathIdRangeNum_Num=%li >= pathIdRangeNum_NumMax=%li\n",
					pathIdRangeNum_Num, pathIdRangeNum_NumMax
				);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}
			pathIdRange[pathIdRangeNum_Num].pathLength = tlngP->path[pathThisId].pathLength;
			pathIdRange[pathIdRangeNum_Num].radiusMax = tlngP->path[pathThisId].radiusMax;
			pathIdRange[pathIdRangeNum_Num].pathClosedTypeNum = pathClosedTypeNum(
				tlngP->path[pathThisId].pathClosed,
				tlngP->path[pathThisId].pathLength,
				tlngP->path[pathThisId].pointy
			);
			pathIdRange[pathIdRangeNum_Num].pathId_Start = pathThisId;
			pathIdRange[pathIdRangeNum_Num].pathId_Final = pathThisId;
			pathIdRangeNum_Num ++;
		}  // length different to previous
	}  // for( pathThisId ... )


	// Debugging
	/*
	for( pathIdRange_Id = 0  ;  pathIdRange_Id < pathIdRangeNum_Num  ;  pathIdRange_Id ++ )  // so increasinglength
		printf(
			"insideness_populate(), pathIdRange: tilingId=%" PRIi8 ", pathLength=%li, radiusMax=%0.9lf, pathId_Start=%li, pathId_Final=%li\n",
			tlngP->tilingId, pathIdRange[pathIdRange_Id].pathLength, pathIdRange[pathIdRange_Id].radiusMax,
			pathIdRange[pathIdRange_Id].pathId_Start, pathIdRange[pathIdRange_Id].pathId_Final
		);
	*/

	// Fats' insideness done at level of Path.
	for( pathThisId = 1  ;  pathThisId < tlngP->numPathsClosed  ;  pathThisId ++ )  // Start at 1 as longest path can't be inside
	{
		if( tlngP->path[pathThisId].pathClosed )
		{
			innerPCTN = pathClosedTypeNum(tlngP->path[pathThisId].pathClosed,  tlngP->path[pathThisId].pathLength,  tlngP->path[pathThisId].pointy);
			for( pathIdRange_Id = pathIdRangeNum_Num - 1  ;  pathIdRange_Id >= 0  ;  pathIdRange_Id -- )
			{
				diffsPCTN = pathIdRange[pathIdRange_Id].pathClosedTypeNum - innerPCTN;
				if( diffsPCTN >=2  &&  diffsPCTN != 3 )
				{
					pathLoopStart = PathByY(
						tlngP->path,
						pathIdRange[pathIdRange_Id].pathId_Start,
						pathIdRange[pathIdRange_Id].pathId_Final,
						tlngP->path[pathThisId].centre.y - pathIdRange[pathIdRange_Id].radiusMax - tlngP->edgeLength / 5,
						false  // bool const above (i.e., false ==> want smaller y)
					);
					pathLoopEnd = PathByY(
						tlngP->path,
						pathLoopStart,
						pathIdRange[pathIdRange_Id].pathId_Final,
						tlngP->path[pathThisId].centre.y + pathIdRange[pathIdRange_Id].radiusMax + tlngP->edgeLength / 5,
						true  // bool const above
					);

					for( pathOuterId = pathLoopStart  ;  pathOuterId <= pathLoopEnd  ;  pathOuterId++ )
					{
						if( 0 != path_winding_number( &(tlngP->path[pathThisId]),  &(tlngP->path[pathOuterId]),  tlngP ) )
						{
							tlngP->path[pathThisId].pathId_ShortestOuter = pathOuterId ;

							if( tlngP->path[pathOuterId].pathId_LongestInner < 0
							||  tlngP->path[pathThisId].pathLength > tlngP->path[ tlngP->path[pathOuterId].pathId_LongestInner ].pathLength )
								tlngP->path[pathOuterId].pathId_LongestInner = pathThisId ;

							goto this_path_done;
						}  // if( path_inside... )
					}  // for( pathOuterId ... )
				}  // if( diffsPCTN >=2  &&  diffsPCTN != 3 )
			}  // for( pathIdRange_Id ... )
		}  // pathClosed
		else
			break;
this_path_done: ;
	}  // for( pathThisId ... )

	free(pathIdRange);


	// Thins' insideness done at level of Rhombus.
	// But thin is easy. Check a fat neighbour.
	// If inside the path of that neighbour, use that.
	// If not, then has same pathId_ShortestOuter as that.
	// Best fat neighbour to check is one with shortest path, because rhombus_winding_number() will be fastest.
	for( rhId = 0;  rhId < tlngP->numFats + tlngP->numThins;  rhId++ )
	{
		if( Thin == tlngP->rhombi[rhId].physique )
		{
			tlngP->rhombi[rhId].pathId_ShortestOuter = -1;

			nghbrNum_best = -1;
			pathLength_Shortest = LONG_MAX;

			for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombi[rhId].numNeighbours  ;  nghbrNum ++ )
			{
				nghbrP = &(tlngP->rhombi[rhId].neighbours[nghbrNum]);
				if( Fat == nghbrP->physique )
				{
					pathThisId = tlngP->rhombi[ nghbrP->rhId ].pathId ;
					if( ! tlngP->path[pathThisId].pathClosed )
						break;  // for( nghbrNum ... )
					if( pathThisId >= 0 )  // Should be redundant
					{
						if( pathLength_Shortest > tlngP->path[pathThisId].pathLength  )
						{
							pathLength_Shortest = tlngP->path[pathThisId].pathLength;
							nghbrNum_best = nghbrNum;
						}  // pathLength_Shortest > ...
					}  // if( pathThisId >= 0 )
				}  // Fat neighbour
			}  // for( nghbrNum ... )

			if( nghbrNum_best >= 0 )
			{
				pathThisId = tlngP->rhombi[ tlngP->rhombi[rhId].neighbours[nghbrNum_best].rhId ].pathId ;
				if( pathThisId >= 0  &&  tlngP->path[pathThisId].pathClosed )
				{
					if( 
						5 == pathLength_Shortest  ||  // Must be outside a 5.
						0 == rhombus_winding_number(&(tlngP->rhombi[rhId]),  &(tlngP->path[pathThisId]),  tlngP)  // Outside
					)
						pathThisId = tlngP->path[pathThisId].pathId_ShortestOuter;  // Not inside pathThisId; instead inside whatever contains pathThisId.

					if( pathThisId >= 0 )  // Could be adjacent to a path that is outside everything. Redundant if previous test, outsideness, was false.
					{
						pathThisP = &(tlngP->path[pathThisId]);
						tlngP->rhombi[rhId].pathId_ShortestOuter = pathThisId;
						pathThisP->insideThis_NumThins ++;
						pathThisP->insideDeep_NumThins ++;

						if( pathThisP->rhId_ThinWithin_First > rhId )   {pathThisP->rhId_ThinWithin_First = rhId;}
						if( pathThisP->rhId_ThinWithin_Last  < rhId )   {pathThisP->rhId_ThinWithin_Last  = rhId;}
					}  // pathThisId >= 0
				}  // Closed
			}  // nghbrNum_best >= 0

		}  // Thin rhombus
	}  // for( rhId ... )

	// Populate ...Deep...
	for( pathThisId = tlngP->numPathsClosed - 1  ;  pathThisId >= 0  ;  pathThisId-- )
	{
		pathThisP = &(tlngP->path[pathThisId]);
		if( pathThisP->pathId_ShortestOuter >= 0  &&  pathThisP->pathClosed )
		{
			tlngP->path[ pathThisP->pathId_ShortestOuter ].insideThis_NumFats  +=  pathThisP->pathLength;
			tlngP->path[ pathThisP->pathId_ShortestOuter ].insideDeep_NumFats  += (pathThisP->pathLength + pathThisP->insideDeep_NumFats);
			tlngP->path[ pathThisP->pathId_ShortestOuter ].insideDeep_NumThins +=  pathThisP->insideDeep_NumThins;
		}  // ShortestOuter >= 0
	}  // for(pathThisId ...)

}  // insideness_populate()
