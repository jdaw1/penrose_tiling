// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// pathStats.c, in PenroseC

#include "penrose.h"


int PathStatsGt(
	PathStats const * const psP0,
	PathStats const * const psP1
)
{
	if(psP0->pathClosed != psP1->pathClosed )
		return( psP0->pathClosed ? -1 : +1 ) ;

	if(psP0->pathLength < psP1->pathLength )
		return +1 ;
	if(psP0->pathLength > psP1->pathLength )
		return -1 ;

	if(5 == psP0->pathLength  &&  psP0->pathClosed)
	{
		if( psP0->pointy  &&  !psP1->pointy )
			return -1 ;
		if( psP1->pointy  &&  !psP0->pointy )
			return +1 ;
	}

	return 0;
}  // PathStatsGt()



void pathStats_sort(Tiling * const tlngP)
{
	PathStatId pathStatId, *pathStatIdsNew;
	PathId     pathId;

	qsort( tlngP->pathStat,  tlngP->numPathStats,  sizeof (tlngP->pathStat[0]),  (int(*)(const void * const,const void * const)) &PathStatsGt );

	pathStatIdsNew = malloc( tlngP->numPathStats  *  sizeof(pathStatId) );
	if( pathStatIdsNew == NULL )
	{
		fprintf(stderr, "Error in pathStats_sort: malloc(...) == NULL;  tlngP->numPathStats = %li;  sizeof(PathStatId) = %li.", tlngP->numPathStats, sizeof(PathStatId) );
		fflush(stderr);
		exit(EXIT_FAILURE) ;
	}  // pathStatIdsNew == NULL
	for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
		pathStatIdsNew[ tlngP->pathStat[pathStatId].pathStatId ] = pathStatId;

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
		tlngP->path[pathId].pathStatId = pathStatIdsNew[ tlngP->path[pathId].pathStatId ];

	free(pathStatIdsNew);
	pathStatIdsNew = NULL;
}  // pathStats_sort()
