// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// pathStats.c, in PenroseC

#include "penrose.h"

void tiling_count_path_lengths(Tiling * const tlngP);

void pathStats_populate(Tiling * const tlngP)
{
	PathId          pathId;
	PathStatId      pathStatId;
	PathStats * pathStatP;
	bool            pathTypeFound;
	Path          * pathP;

	tlngP->numPathStats = 0;

	tlngP->pathStats_NumMax = 1  +  2 * ( tlngP->numFats > 0  ?  (long int)ceil(pow(1 + tlngP->numFats, 0.6))  :  0 );
	tlngP->pathStat = malloc( tlngP->pathStats_NumMax  *  sizeof(PathStats) );
	if(NULL == tlngP->pathStat)
	{
		fprintf(stderr, "pathStats_populate: malloc() failed with tilingId=%" PRIi8 ", pathStats_NumMax=%li\n", tlngP->tilingId, tlngP->pathStats_NumMax);
		fflush(stderr);
		exit( EXIT_FAILURE );
	}  // NULL == tlngP->pathStat

	for( pathStatId = 0  ;  pathStatId < tlngP->pathStats_NumMax  ;  pathStatId++ )
	{
		pathStatP = &(tlngP->pathStat[ pathStatId ]);
		pathStatP->pathStatId = pathStatId;
		pathStatP->pathClosed = false;
		pathStatP->pathLength = -1;
		pathStatP->pointy = false;
		pathStatP->pathClosedTypeNum = -2;  // -1 is open, -2 undefined.
		pathStatP->numPaths = 0;
		pathStatP->examplePathId = -1;  // I.e., invalid.
		pathStatP->insideThis_MaxNumFats = -1;
		pathStatP->insideDeep_MaxNumFats = -1;
		pathStatP->insideThis_MaxNumThins = -1;
		pathStatP->insideDeep_MaxNumThins = -1;
		pathStatP->insideThis_MaxNumFats_Num = -1;
		pathStatP->insideDeep_MaxNumFats_Num = -1;
		pathStatP->insideThis_MaxNumThins_Num = -1;
		pathStatP->insideDeep_MaxNumThins_Num = -1;
		pathStatP->radiusMinMin = DBL_MAX;
		pathStatP->radiusMaxMax = 0;
		pathStatP->widthMax  = 0;
		pathStatP->heightMax = 0;
	}  // for( pathStatId ... )

	if( tlngP->path == NULL  ||  tlngP->numPathsClosed + tlngP->numPathsOpen == 0 )
		return;

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
	{
		pathP = &(tlngP->path[pathId]);
		pathTypeFound = false;
		for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
			if( pathP->pathClosed == tlngP->pathStat[pathStatId].pathClosed
			&&  pathP->pathLength == tlngP->pathStat[pathStatId].pathLength
			&&  ( pathP->pathLength != 5  ||  !(pathP->pathClosed)  ||  pathP->pointy == tlngP->pathStat[pathStatId].pointy )  )
			{
				pathTypeFound = true;
				break;
			}  // Matching

		if( pathTypeFound )
		{
			pathStatP = &(tlngP->pathStat[ pathStatId ]);
			pathStatP->pathStatId = pathStatId;
			pathP->pathStatId = pathStatId;
			pathStatP->numPaths ++ ;

			if( pathStatP->insideThis_MaxNumFats  < pathP->insideThis_NumFats  )
			{
				pathStatP->insideThis_MaxNumFats  = pathP->insideThis_NumFats ;
				pathStatP->insideThis_MaxNumFats_Num   = 1;
			}
			else if( pathStatP->insideThis_MaxNumFats == pathP->insideThis_NumFats  )
				(pathStatP->insideThis_MaxNumFats_Num) ++;


			if( pathStatP->insideDeep_MaxNumFats  < pathP->insideDeep_NumFats  )
			{
				pathStatP->insideDeep_MaxNumFats = pathP->insideDeep_NumFats ;
				pathStatP->insideDeep_MaxNumFats_Num = 1;
			}
			else if( pathStatP->insideDeep_MaxNumFats == pathP->insideDeep_NumFats  )
				(pathStatP->insideDeep_MaxNumFats_Num) ++;

			
			if( pathStatP->insideThis_MaxNumThins < pathP->insideThis_NumThins )
			{
				pathStatP->insideThis_MaxNumThins = pathP->insideThis_NumThins;
				pathStatP->insideThis_MaxNumThins_Num = 1;
			}
			else if( pathStatP->insideThis_MaxNumThins == pathP->insideThis_NumThins )
				(pathStatP->insideThis_MaxNumThins_Num) ++;


			if( pathStatP->insideDeep_MaxNumThins < pathP->insideDeep_NumThins )
			{
				pathStatP->insideDeep_MaxNumThins = pathP->insideDeep_NumThins;
				pathStatP->insideDeep_MaxNumThins_Num = 1;
			}
			else if( pathStatP->insideDeep_MaxNumThins == pathP->insideDeep_NumThins )
				(pathStatP->insideDeep_MaxNumThins_Num) ++;

			if(pathStatP->radiusMinMin > pathP->radiusMin)
				pathStatP->radiusMinMin = pathP->radiusMin;
			if(pathStatP->radiusMaxMax < pathP->radiusMax)
				pathStatP->radiusMaxMax = pathP->radiusMax;

			if(pathStatP->widthMax  < pathP->xMax - pathP->xMin)
				pathStatP->widthMax  = pathP->xMax - pathP->xMin;
			if(pathStatP->heightMax < pathP->yMax - pathP->yMin)
				pathStatP->heightMax = pathP->yMax - pathP->yMin;

			pathStatP->examplePathId = pathId ;

			if(pow(pathP->centre.x, 2) + pow(pathP->centre.y, 2) <
				pow(tlngP->path[ pathStatP->examplePathId ].centre.x, 2) +
				pow(tlngP->path[ pathStatP->examplePathId ].centre.y, 2)
			)
				pathStatP->examplePathId = pathId ;  // Choosing path closest to (0,0), for needless precision optimality.
		}  // pathTypeFound
		else
		{
			// New PathStat
			pathStatId = tlngP->numPathStats;
			if( pathStatId >= tlngP->pathStats_NumMax )
			{
				fprintf(stderr, "pathStats_populate(): !!! pathStatId >= pathStats_NumMax=%li; tilingId=%" PRIi8 "\n", tlngP->pathStats_NumMax, tlngP->tilingId);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}  // pathStatId >= tlngP->pathStats_NumMax

			pathStatP = &(tlngP->pathStat[pathStatId]);

			pathStatP->pathStatId = pathStatId;
			pathP->pathStatId = pathStatId;
			pathStatP->pathClosed = pathP->pathClosed;
			pathStatP->pathLength = pathP->pathLength;
			pathStatP->pointy = (pathP->pathLength == 5  &&  pathP->pathClosed)  ?  pathP->pointy  :  false ;
			pathStatP->pathClosedTypeNum = pathClosedTypeNum(pathStatP->pathClosed,  pathStatP->pathLength,  pathStatP->pointy) ;
			pathStatP->numPaths = 1 ;
			pathStatP->examplePathId = pathId ;
			pathStatP->insideThis_MaxNumFats  = pathP->insideThis_NumFats ;
			pathStatP->insideDeep_MaxNumFats  = pathP->insideDeep_NumFats ;
			pathStatP->insideThis_MaxNumThins = pathP->insideThis_NumThins;
			pathStatP->insideDeep_MaxNumThins = pathP->insideDeep_NumThins;
			pathStatP->insideThis_MaxNumFats_Num   = 1;
			pathStatP->insideDeep_MaxNumFats_Num   = 1;
			pathStatP->insideThis_MaxNumThins_Num  = 1;
			pathStatP->insideDeep_MaxNumThins_Num  = 1;
			pathStatP->radiusMinMin = pathP->radiusMin;
			pathStatP->radiusMaxMax = pathP->radiusMax;
			pathStatP->widthMax     = pathP->xMax - pathP->xMin;
			pathStatP->heightMax    = pathP->yMax - pathP->yMin;

			(tlngP->numPathStats) ++ ;
		}  // pathTypeFound
	}  // for( pathId ... )

	pathStats_sort(tlngP);

}  // pathStats_populate()
