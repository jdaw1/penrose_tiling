// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// sortRhombi.c, in PenroseC

#include "penrose.h"

int rhombiGt_ByPath(
	Rhombus const * const rhP0,
	Rhombus const * const rhP1
)
{
	if( rhP0->physique != rhP1->physique )
		return ( Fat == rhP0->physique ? -1 : +1 );  // Fats first.

	switch(rhP0->physique)
	{
	case Fat:
		if( rhP0->pathId      > rhP1->pathId      )  return +1;  //  Earlier path ==> earlier rhombus
		if( rhP0->pathId      < rhP1->pathId      )  return -1;  //  Later path ==> later rhombus
		if( rhP0->withinPathNum > rhP1->withinPathNum ) return +1;  // If in same path, earlier in path ==> earlier in array
		if( rhP0->withinPathNum < rhP1->withinPathNum ) return -1;  // If in same path, later in path ==> later in array
		return 0;  // rhP0 == rhP1
		break;  // Entirely redundant.

	case Thin:
		if( rhP0->centre.y < rhP1->centre.y )  return +1 ;  // Higher thin rhombus Ys earlier
		if( rhP0->centre.y > rhP1->centre.y )  return -1 ;  // Lower thin rhombus Ys later
		if( rhP0->centre.x > rhP1->centre.x )  return +1 ;  // Left thin rhombus earlier
		if( rhP0->centre.x < rhP1->centre.x )  return -1 ;  // Right thin rhombus later
		return 0;  // rhP0 == rhP1
		break;  // Entirely redundant.
	}  // switch(rhP0->physique)
}  // rhombiGt_ByPath()


int neighbourGt(Neighbour const * const nP0,  Neighbour const * const nP1)
{
	if( nP0->physique != nP1->physique )
		return ( Fat == nP1->physique ? +1 : -1 ) ;

	switch( nP0->physique )
	{
	case Fat:  // ==> both Fat
		if(nP0->pathId == nP1->pathId)
		{
			if( nP1->withinPathNum == 0 )  return +1 ;
			if( nP0->withinPathNum == 0 )  return -1 ;
			return( nP0->withinPathNum < nP1->withinPathNum ? +1 : -1 );
		}
		else
		{
			// Two Fat neighbours of a Thin; cannot be two Fat neighbours of a Fat.
			return( nP0->pathId > nP1->pathId ? +1 : -1 );  // cannot be equal
		}
		break;  // Entirely redundant.

	case Thin:  // ==> both Thin
		if( nP0->rhId < nP1->rhId )  return +1 ;
		if( nP0->rhId > nP1->rhId )  return -1 ;
		return 0 ;
		break;  // Entirely redundant.
	}  // switch( rhP0->physique )
}  // neighbourGt()


void rhombi_sort(
	Tiling * const tlngP,
	int orderedFn(const Rhombus * const, const Rhombus * const),
	bool const alsoRenumber
)
{
	RhombId        rhId;
	PathId         pathId;
	RhombId        *rhombIdsNew;
	int8_t         nghbrNum;
	Rhombus        *rhP;
	Path           *pathP;
	long int const numRhombi = tlngP->numFats + tlngP->numThins;

	if( alsoRenumber )
		for( rhId = 0  ;  rhId < numRhombi  ;  rhId++ )
			tlngP->rhombi[rhId].rhId = rhId;

	qsort(
		tlngP->rhombi,
		numRhombi,
		sizeof(Rhombus),
		( int(*)(const void * const,const void * const) ) orderedFn
	);  // qsort()

	if( alsoRenumber )
	{
		rhombIdsNew = malloc( numRhombi  *  sizeof(RhombId) );
		if( rhombIdsNew == NULL )
		{
			fprintf(stderr, "Error in rhombi_sort: malloc(...) == NULL;  numRhombi = %li;  sizeof(RhombId) = %li.", numRhombi, sizeof(RhombId) );
			fflush(stderr);
			exit(EXIT_FAILURE) ;
		}  // rhombIdsNew == NULL

		for( rhId = 0  ;  rhId < numRhombi  ;  rhId++ )
			rhombIdsNew[rhId] = -1;

		for( rhId = 0  ;  rhId < numRhombi  ;  rhId++ )
			if( tlngP->rhombi[rhId].rhId >= 0 )
				rhombIdsNew[ tlngP->rhombi[rhId].rhId ] = rhId;

		for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
		{
			pathP = &(tlngP->path[pathId]);
			pathP->rhId_ThinWithin_First = numRhombi - 1;
			pathP->rhId_ThinWithin_Last  = 0;
			if( tlngP->path[pathId].pathClosed )
			{
				pathP->rhId_PathCentreClosest  = rhombIdsNew[ pathP->rhId_PathCentreClosest  ];
				pathP->rhId_PathCentreFurthest = rhombIdsNew[ pathP->rhId_PathCentreFurthest ];
				pathP->rhId_openPathEnd = -1;
			}  // Closed
			else  // Open
				pathP->rhId_openPathEnd = rhombIdsNew[ pathP->rhId_openPathEnd ];
		}  // for( pathId ... )

		for( rhId = 0  ;  rhId < numRhombi  ;  rhId++ )
		{
			rhP = &(tlngP->rhombi[rhId]);
			rhP->rhId = rhId ;
			for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum++ )
				rhP->neighbours[nghbrNum].rhId = rhombIdsNew[ rhP->neighbours[nghbrNum].rhId ] ;
			if( Thin == rhP->physique  &&  rhP->pathId_ShortestOuter >= 0 )
			{
				pathP = &(tlngP->path[ rhP->pathId_ShortestOuter ]);
				if( pathP->rhId_ThinWithin_First > rhId )  {pathP->rhId_ThinWithin_First = rhId;}
				if( pathP->rhId_ThinWithin_Last  < rhId )  {pathP->rhId_ThinWithin_Last  = rhId;}
			}  // Thin ...
		}  // for( rhId ... )

		free(rhombIdsNew);
		rhombIdsNew = NULL;

	}  // alsoRenumber
	else
	{
		// alsoRenumber == false
		for( rhId = 0  ;  rhId < numRhombi  ;  rhId++ )
			tlngP->rhombi[rhId].rhId = rhId;
	}

}  // rhombi_sort()
