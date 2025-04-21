// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// sortPaths.c, in PenroseC

#include "penrose.h"

int pathGt_ByClosedEtc(
	Path const * const pathP0,
	Path const * const pathP1
)
{
	if( pathP0->pathClosed != pathP1->pathClosed )  return (pathP0->pathClosed ? -1 : +1);  // Closed paths earlier
	if( pathP0->pathLength >  pathP1->pathLength )  return -1 ;  // Longer paths earlier
	if( pathP0->pathLength <  pathP1->pathLength )  return +1 ;  // Shorter paths later
	if( pathP0->pathClosed  &&  5 == pathP0->pathLength  &&  pathP0->pointy != pathP1->pointy )
		return (pathP0->pointy ? -1 : +1);
	if( pathP0->centre.y   <  pathP1->centre.y   )  return -1 ;  // Lower path Ys earlier.
	if( pathP0->centre.y   >  pathP1->centre.y   )  return +1 ;  // Higher path Ys later
	if( pathP0->centre.x   <  pathP1->centre.x   )  return -1 ;  // Left paths earlier
	if( pathP0->centre.x   >  pathP1->centre.x   )  return +1 ;  // Right paths later
	return 0;  // pathP0 == pathP1
}  // pathGt_ByClosedEtc()


void paths_sort(Tiling * const tlngP,  int orderedFn(const Path * const, const Path * const) )
{
	PathId     pathId, *pathIdsNew;
	PathStatId pathStatId;
	RhombId    rhId_This;
	int8_t     nghbrNum;

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
		tlngP->path[pathId].pathId = pathId;

	qsort( tlngP->path,  tlngP->numPathsClosed + tlngP->numPathsOpen,  sizeof (tlngP->path[0]),  (int(*)(const void * const,const void * const)) orderedFn );

	pathIdsNew = malloc( (tlngP->numPathsClosed + tlngP->numPathsOpen)  *  sizeof(PathId) );
	if( pathIdsNew == NULL )
	{
		fprintf(stderr,
			"Error in paths_sort: malloc(...) == NULL;  tlngP->numPathsClosed = %li;  tlngP->numPathsOpen = %li;  sizeof(PathId) = %li.",
			tlngP->numPathsClosed,  tlngP->numPathsOpen,  sizeof(PathId)
		);
		fflush(stderr);
		exit(EXIT_FAILURE) ;
	}  // pathIdsNew == NULL

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
		pathIdsNew[ tlngP->path[pathId].pathId ] = pathId;

	for( rhId_This = 0  ;  rhId_This < tlngP->numFats + tlngP->numThins  ;  rhId_This++ )
	{
		if( Fat == tlngP->rhombii[rhId_This].physique )
			tlngP->rhombii[rhId_This].pathId = pathIdsNew[ tlngP->rhombii[rhId_This].pathId ] ;
		else
		{
			if( tlngP->rhombii[rhId_This].pathId_ShortestOuter >= 0 )
				tlngP->rhombii[rhId_This].pathId_ShortestOuter = pathIdsNew[ tlngP->rhombii[rhId_This].pathId_ShortestOuter ];
		}  // Thin
		for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_This].numNeighbours  ;  nghbrNum++ )
			if( tlngP->rhombii[rhId_This].neighbours[nghbrNum].pathId >= 0 )
				tlngP->rhombii[rhId_This].neighbours[nghbrNum].pathId = pathIdsNew[ tlngP->rhombii[rhId_This].neighbours[nghbrNum].pathId ];
	}  // for( rhId_This ... )

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
	{
		if( tlngP->path[pathId].pathId_ShortestOuter >= 0 )  tlngP->path[pathId].pathId_ShortestOuter = pathIdsNew[ tlngP->path[pathId].pathId_ShortestOuter ] ;
		if( tlngP->path[pathId].pathId_LongestInner  >= 0 )  tlngP->path[pathId].pathId_LongestInner  = pathIdsNew[ tlngP->path[pathId].pathId_LongestInner  ] ;
		tlngP->path[pathId].pathId = pathId;
	}

	for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId ++ )
		tlngP->pathStat[pathStatId].examplePathId = pathIdsNew[ tlngP->pathStat[pathStatId].examplePathId ] ;

	free(pathIdsNew);
	pathIdsNew = NULL;
}  // paths_sort()
