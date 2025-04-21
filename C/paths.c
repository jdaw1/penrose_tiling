// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// paths.c, in PenroseC

#include "penrose.h"

// Negative return means non-existent. Re direction: true ==> next; false ==> prev.
RhombId NextInPath_RhId(const Rhombus * const rhombii,  const Rhombus * const rhThisP, long int const pathLength,  bool const direction)
{
	int8_t  nghbrNum;
	RhombId rhId_Next;
	const Rhombus * rhNextP;

	if( rhThisP->physique != Fat )
		return(-1);

	for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum++ )
	{
		rhId_Next = rhThisP->neighbours[nghbrNum].rhId;
		rhNextP = &(rhombii[rhId_Next]);
		if( rhNextP->physique != Fat )
			continue;

		if( direction )
		{
			if( rhNextP->withinPathNum == (rhThisP->withinPathNum              + 1) % pathLength )
				return(rhId_Next);
		}
		else
		{
			if( rhNextP->withinPathNum == (rhThisP->withinPathNum + pathLength - 1) % pathLength )
				return(rhId_Next);
		}
	}  // for( nghbrNum ...)

	return(-1);
}  // NextInPath_RhId()



// For Closed path, start rhombus effectively arbitrary. For neatness should choose consistently. Allow for machine-precision wobbbles.
// Start at one of the five or ten rhombii closest to centre. Of those, prefer in first quadrant. Of those, prefer greatest y.
// Negative means don't want; positive is new wanted distance.
// This caused feedback to Apple: https://feedbackassistant.apple.com/feedback/15322282
inline static double rhWithinPathMoreSpecial(
	bool            const wantClosest,
	const Path    * const pathP,
	const Rhombus * const rhNewP,
	const Rhombus * const rhOldP,
	const double distance2old,
	const double distance2Epsilon,
	const double distanceEpsilon
)
{
	const double dist2New = pow(rhNewP->centre.x - pathP->centre.x,  2) + pow(rhNewP->centre.y - pathP->centre.y,  2) ;

	if(rhNewP == rhOldP)
		return dist2New;  // This special used to generate distance with initial setting.

	if(wantClosest)
	{
		if( dist2New >= distance2old + distance2Epsilon )  return -1;        // Definitely further away
		if( dist2New <  distance2old - distance2Epsilon )  return dist2New;  // Definitely closer. Failing this, distance a tie to within machine precision.
	}
	else
	{
		if( dist2New <= distance2old - distance2Epsilon )  return -1;        // Definitely closer
		if( dist2New >  distance2old + distance2Epsilon )  return dist2New;  // Definitely further. Failing this, distance a tie to within machine precision.
	}
	if( rhNewP->centre.x < pathP->centre.x - distanceEpsilon  ||  rhNewP->centre.y < pathP->centre.y - distanceEpsilon )  return -1;        // New not 1st quadrant
	if( rhOldP->centre.x < pathP->centre.x - distanceEpsilon  ||  rhOldP->centre.y < pathP->centre.y - distanceEpsilon )  return dist2New;  // New is 1st quadrant, old not
	return( rhNewP->centre.y > rhOldP->centre.y ? dist2New : -1 );  // Both 1st quadrant, choose more northerly
}  // rhWithinPathMoreSpecial()



int8_t pathClosedTypeNum(bool pathClosed, long int pathLength, bool pointy)
{
	// int8_t will accomodate answer for paths of any relevant length.
	if( pathClosed )
	{
		if( 5 == pathLength )
			return( pointy ? 2 : 1 );
		else
			return round(log2(0.6 * pathLength));  // 15-->9-->3.1699-->3;  25-->15-->3.90689-->4;  55-->33-->5.04439-->5;  105-->63-->5.977-->6;  215-->129-->7.0112-->7;  425-->255-->7.994-->8;  855-->513-->9.0028-->9
	}  // Closed
	else
		return -1;  // Open
}  // pathClosedTypeNum()


void paths_populate(Tiling * const tlngP)
{
	register RhombId  rhId_This;
	RhombId           rhId_PathStart, rhId_Prev, rhId_Next, rhId_PathStart_Better;
	PathId            pathId, pathRhPathRhombNumClosest, rhPathNum;
	Rhombus           *rhThisP;
	Path              *pathThisP;
	int8_t            nghbrNum;
	bool              noNewNeighbours, isClockwise, orientationFound;
	double            thisPathSumX, thisPathSumY, dist2Temp, dist2Closest, dist2Furthest;
	double const      distance2Epsilon = pow(tlngP->edgeLength / 16, 2);  // Machine-precision allowance for Pythagorean square of distance
	double const      distanceEpsilon = tlngP->edgeLength / 256;          // Can be in 1st quadrant with x or y slightly negative, by machine precision

	tlngP->numPathsClosed = 0;
	tlngP->numPathsOpen = 0;

	if( tlngP->numFats <= 0 )
	{
		// tlngP->numFats == 0, so no data to hold.
		tlngP->path_NumMax = 0;
		tlngP->path = NULL ;
		printf("paths_populate: tilingId=%" PRIi8 ", no fats so no paths.\n", tlngP->tilingId);
		return ;
	}

	// Simple numerical tests suggest that the following is a reasonable over-estimate.
	// But if the rectangle being populated very very long, and about one tile wide, then paths ~= fats, so this would be too small.
	// But in such an awkward case, paths would not be useful, so incompleteness wouldn't matter.
	tlngP->path_NumMax = (long int)( 64 + (double)(tlngP->numFats) / 8.0 );  // Slightly too many: denominator should be nearer 9.47.
	if( tlngP->path_NumMax > tlngP->numFats )
		tlngP->path_NumMax = tlngP->numFats ;

	tlngP->path = malloc( tlngP->path_NumMax  *  sizeof(Path) );
	if( NULL == tlngP->path )
	{
		fprintf(stderr, "paths_populate(): !!! NULL == tlngP->path !!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // if( NULL == tlngP->path )

	for( rhId_PathStart = 0;  rhId_PathStart < tlngP->numFats + tlngP->numThins ; rhId_PathStart++ )
		tlngP->rhombii[rhId_PathStart].pathId = -1 ;

	for( rhId_PathStart = 0;  rhId_PathStart < tlngP->numFats + tlngP->numThins ; rhId_PathStart++ )
	{
		if( Fat != tlngP->rhombii[rhId_PathStart].physique
		||  tlngP->rhombii[rhId_PathStart].pathId >= 0 )  // rhId_PathStart already has path assigned
			continue;

		if( tlngP->numPathsClosed + tlngP->numPathsOpen >= tlngP->path_NumMax )
		{
			fprintf(stderr, "!!! Error in paths_populate(): path_NumMax = %li is too small.\n", tlngP->path_NumMax);
			fflush(stderr);
			exit(EXIT_FAILURE);
		}
		pathThisP = &(tlngP->path[ tlngP->numPathsClosed + tlngP->numPathsOpen ]);
		pathThisP->pathId = tlngP->numPathsClosed + tlngP->numPathsOpen;
		pathThisP->pathId_ShortestOuter = -1;
		pathThisP->pathId_LongestInner  = -1;
		pathThisP->insideThis_NumFats  = 0 ;  // Sum of lengths of paths immediately enclosed.
		pathThisP->insideDeep_NumFats  = 0 ;  // Sum of lengths of paths enclosed at any depth.
		pathThisP->insideThis_NumThins = 0 ;  // Num thins in this path, but not in a sub-path.
		pathThisP->insideDeep_NumThins = 0 ;  // Num thins in this path, at any depth.
		pathThisP->orientationDegrees  = DBL_MAX ;
		pathThisP->rhId_ThinWithin_First = tlngP->numFats + tlngP->numThins - 1;
		pathThisP->rhId_ThinWithin_Last  = 0;

		// Path either open or closed. Let's follow path until re-reach start, or reach dead end, revealing whether closed and if not, a sensible beginning place.

		tlngP->rhombii[rhId_PathStart].pathId = pathThisP->pathId;

		rhId_Prev = -1;
		rhId_This = rhId_PathStart;
		rhId_Next = rhId_PathStart;
		while(true)
		{
			rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;

			noNewNeighbours = true ;
			for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum++ )
			{
				rhId_Next = rhThisP->neighbours[nghbrNum].rhId ;
				if( rhId_Next >= 0  &&  rhId_Next != rhId_Prev  &&  Fat == tlngP->rhombii[rhId_Next].physique )
				{
					if(rhId_Next == rhId_PathStart)
						{pathThisP->pathClosed = true;  rhId_PathStart_Better = rhId_PathStart;  goto know_rhIdPathStart_pathClosed;}  // goto used as a multi-loop break.
					else
						{noNewNeighbours = false;  break;}
				}
			}  // for nghbrNum
			if( noNewNeighbours )
				{pathThisP->pathClosed = false;  rhId_PathStart_Better = rhId_This;  goto know_rhIdPathStart_pathClosed;}  // goto used for consistency with previous.

			rhId_Prev = rhId_This ;
			rhId_This = rhId_Next ;
		}  // while(true)

know_rhIdPathStart_pathClosed:


		pathThisP->rhId_PathCentreClosest = rhId_PathStart_Better;
		pathThisP->pathVeryClosed = pathThisP->pathClosed;

		// Starting at rhPathBegin, re-follow path, counting length, and populating rhombus field withinPathNum.
		pathThisP->pathLength = 1;

		rhId_Prev = -1;
		rhId_This = pathThisP->rhId_PathCentreClosest;
		rhThisP = &(tlngP->rhombii[ rhId_This ]) ;
		thisPathSumX = 0;
		thisPathSumY = 0;
		pathThisP->xMax = rhThisP->xMax;  // This start value because -DBL_MAX is ugly.
		pathThisP->yMax = rhThisP->yMax;
		pathThisP->xMin = rhThisP->xMin;
		pathThisP->yMin = rhThisP->yMin;
		pathThisP->pathVeryClosed = pathThisP->pathClosed;
		pathThisP->rhId_openPathEnd = -1;
		while(true)
		{
			rhThisP = &(tlngP->rhombii[ rhId_This ]) ;
			rhThisP->pathId = pathThisP->pathId ;
			rhThisP->withinPathNum = pathThisP->pathLength - 1;  // Because this done here, can't in this loop use NextInPath_RhId()

			thisPathSumX += rhThisP->centre.x ;
			thisPathSumY += rhThisP->centre.y ;
			if(pathThisP->xMax < rhThisP->xMax)  pathThisP->xMax = rhThisP->xMax;
			if(pathThisP->yMax < rhThisP->yMax)  pathThisP->yMax = rhThisP->yMax;
			if(pathThisP->xMin > rhThisP->xMin)  pathThisP->xMin = rhThisP->xMin;
			if(pathThisP->yMin > rhThisP->yMin)  pathThisP->yMin = rhThisP->yMin;

			if( pathThisP->pathVeryClosed  &&  pathThisP->pathClosedTypeNum % 2 == 0 )
			{
				if( rhThisP->numNeighbours < 4 )
					pathThisP->pathVeryClosed = false;
				else
				{
					for( nghbrNum = 0  ;  nghbrNum < 4  ;  nghbrNum++ )  // rhThisP->numNeighbours must be 4
						if( tlngP->rhombii[ rhThisP->neighbours[nghbrNum].rhId ].numNeighbours < 4 )
							{pathThisP->pathVeryClosed = false;  break;}
				}  // rhThisP->numNeighbours == 4
			}  // pathVeryClosed

			noNewNeighbours = true ;
			for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum++ )
			{
				rhId_Next = rhThisP->neighbours[nghbrNum].rhId ;
				if( Fat == rhThisP->neighbours[nghbrNum].physique  &&  rhId_Next != rhId_Prev )
				{
					if(rhId_Next == rhId_PathStart_Better)
						goto know_pathLength;  // goto used as a multi-loop break.
					else
					{
						noNewNeighbours = false;
						(pathThisP->pathLength) ++;
						break;
					}
				}  // Fat, etc
			}  // for nghbrNum
			pathThisP->rhId_openPathEnd = rhId_This;
			if( noNewNeighbours )
				goto know_pathLength;  // goto used for consistency with previous.

			rhId_Prev = rhId_This ;
			rhId_This = rhId_Next ;
		}  // while(true)

know_pathLength:

		if( pathThisP->pathVeryClosed )
			tlngP->anyPathsVeryClosed = true;

		pathThisP->centre.x = thisPathSumX / pathThisP->pathLength ;
		pathThisP->centre.y = thisPathSumY / pathThisP->pathLength ;

		if( pathThisP->pathClosed )
		{
			// Starting at rhId_PathStart_Better, re-follow path, and calculating which closest to centre
			rhId_This = rhId_PathStart_Better;
			pathRhPathRhombNumClosest = tlngP->rhombii[ rhId_This ].withinPathNum;
			pathThisP->rhId_PathCentreClosest  = rhId_This;
			pathThisP->rhId_PathCentreFurthest = rhId_This;
			dist2Closest = rhWithinPathMoreSpecial(
				true,  // Want closest corner
				pathThisP,
				&(tlngP->rhombii[ rhId_This ]),
				&(tlngP->rhombii[ rhId_This ]),
				0,  // Irrelevant
				distance2Epsilon, distanceEpsilon  // Irrelevant
			);
			dist2Furthest = rhWithinPathMoreSpecial(
				false,  // Want furthest corner
				pathThisP,
				&(tlngP->rhombii[ rhId_This ]),
				&(tlngP->rhombii[ rhId_This ]),
				0,  // Irrelevant
				distance2Epsilon, distanceEpsilon  // Irrelevant
			);

			while(true)
			{
				rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;
				rhId_Next = NextInPath_RhId(tlngP->rhombii, rhThisP, pathThisP->pathLength, true);

				dist2Temp = rhWithinPathMoreSpecial(
					true,  // Want closest
					pathThisP,
					&(tlngP->rhombii[ rhId_This ]),
					&(tlngP->rhombii[ pathThisP->rhId_PathCentreClosest ]),
					dist2Closest,
					distance2Epsilon, distanceEpsilon
				);
				if( dist2Temp >= 0 )
				{
					pathRhPathRhombNumClosest = rhThisP->withinPathNum ;
					pathThisP->rhId_PathCentreClosest = rhId_This ;
					dist2Closest = dist2Temp;
				}  // dist2Temp > 0

				dist2Temp = rhWithinPathMoreSpecial(
					false,  // Want furthest
					pathThisP,
					&(tlngP->rhombii[ rhId_This ]),
					&(tlngP->rhombii[ pathThisP->rhId_PathCentreFurthest ]),
					dist2Furthest,
					distance2Epsilon, distanceEpsilon
				);
				if( dist2Temp >= 0 )
				{
					pathThisP->rhId_PathCentreFurthest = rhId_This ;
					dist2Furthest = dist2Temp;
				}  // dist2Temp > 0

				if( rhId_Next < 0 )
					break;  // Open path, impossible here
				if( tlngP->rhombii[rhId_Next].withinPathNum == 0 )
					break;  // Closed path, back to start

				rhId_This = rhId_Next ;
			}  // while(true)

			pathThisP->radiusMin = sqrt(dist2Closest);
			pathThisP->radiusMax = sqrt(dist2Furthest);

			// Starting at rhPathBegin, reset pathRhombNum such that the closest to the centre is 0
			isClockwise = ( point_winding_number(
				pathThisP->centre.x + tlngP->edgeLength / 64,
				pathThisP->centre.y + tlngP->edgeLength / 64,
				pathThisP,
				tlngP
			) > 0 );

			// Known: centre, rhId_PathStart, isClockwise

			// Rotate such that pathRhPathRhombNumClosest has pathRhombNum of 0.
			// And, such that path goes clockwise, if necessary reflect.
			long int * const pathListed = malloc( pathThisP->pathLength * sizeof(rhId_This) );
			if( pathListed != NULL )
			{
				pathListed[0] = pathThisP->rhId_PathCentreClosest;
				for( rhPathNum = 1  ;  rhPathNum < pathThisP->pathLength ;  rhPathNum++ )
					pathListed[rhPathNum] = NextInPath_RhId(tlngP->rhombii, tlngP->rhombii + pathListed[rhPathNum - 1], pathThisP->pathLength, true);

				for( rhPathNum = 0  ;  rhPathNum < pathThisP->pathLength ;  rhPathNum++ )
				{
					tlngP->rhombii[pathListed[rhPathNum]].withinPathNum = ( pathThisP->pathLength
						+ (tlngP->rhombii[pathListed[rhPathNum]].withinPathNum - pathRhPathRhombNumClosest) * (isClockwise ? +1 : -1)
					) % pathThisP->pathLength;
				}  // for( rhPathNum ... )
				free(pathListed);
			}  // if( pathListed != NULL )

			rhId_This = pathThisP->rhId_PathCentreClosest;
			while(true)
			{
				rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;
				rhId_Next = NextInPath_RhId(tlngP->rhombii, rhThisP, pathThisP->pathLength, true);

				rhThisP->closerPathCentreN = (
					( pow(pathThisP->centre.x  -  rhThisP->north.x, 2)  +  pow(pathThisP->centre.y  -  rhThisP->north.y, 2) ) <
					( pow(pathThisP->centre.x  -  rhThisP->south.x, 2)  +  pow(pathThisP->centre.y  -  rhThisP->south.y, 2) )
				);  // closerPathCentreN
				rhThisP->closerPathCentreE = (
					( pow(pathThisP->centre.x  -  rhThisP->east.x , 2)  +  pow(pathThisP->centre.y  -  rhThisP->east.y , 2) ) <
					( pow(pathThisP->centre.x  -  rhThisP->west.x , 2)  +  pow(pathThisP->centre.y  -  rhThisP->west.y , 2) )
				);  // closerPathCentreE

				if( rhId_Next < 0 )
					break;  // Open path, impossible here
				if( tlngP->rhombii[rhId_Next].withinPathNum == 0 )
					break;  // Closed path, back to start

				rhId_This = rhId_Next ;
			}  // while(true)

			// set orientationDegrees
			do  // break'able construct
			{
				rhThisP = &(tlngP->rhombii[ pathThisP->rhId_PathCentreClosest ]);
				orientationFound = false;

				if( collinear(rhThisP->north,  rhThisP->south,  pathThisP->centre,  tlngP) )
				{
					pathThisP->orientationDegrees = atan2(
						rhThisP->north.y - rhThisP->south.y,
						rhThisP->north.x - rhThisP->south.x
					) * DegreesPerRadian ;
					orientationFound = true;
					break;
				}  // north-south

				if( collinear(rhThisP->north,  rhThisP->east,  pathThisP->centre,  tlngP) )
				{
					pathThisP->orientationDegrees = atan2(
						rhThisP->north.y - rhThisP->east.y,
						rhThisP->north.x - rhThisP->east.x
					) * DegreesPerRadian ;
					orientationFound = true;
					break;
				}  // north-east

				if( collinear(rhThisP->north,  rhThisP->west,  pathThisP->centre,  tlngP) )
				{
					pathThisP->orientationDegrees = atan2(
						rhThisP->north.y - rhThisP->west.y,
						rhThisP->north.x - rhThisP->west.x
					) * DegreesPerRadian ;
					orientationFound = true;
					break;
				}  // north-west

				if( collinear(rhThisP->south,  rhThisP->east,  pathThisP->centre,  tlngP) )
				{
					pathThisP->orientationDegrees = atan2(
						rhThisP->south.y - rhThisP->east.y,
						rhThisP->south.x - rhThisP->east.x
					) * DegreesPerRadian ;
					orientationFound = true;
					break;
				}  // south-east

				if( collinear(rhThisP->south,  rhThisP->west,  pathThisP->centre,  tlngP) )
				{
					pathThisP->orientationDegrees = atan2(
						rhThisP->south.y - rhThisP->west.y,
						rhThisP->south.x - rhThisP->west.x
					) * DegreesPerRadian ;
					orientationFound = true;
					break;
				}  // south-west

			} while(false);  // break'able construct

			if( orientationFound )
			{
				if( fabs(pathThisP->orientationDegrees - round(pathThisP->orientationDegrees)) < 1E-11 )
					pathThisP->orientationDegrees = round(pathThisP->orientationDegrees) ;
				while( pathThisP->orientationDegrees <   0 ) pathThisP->orientationDegrees += 72;
				while( pathThisP->orientationDegrees >= 72 ) pathThisP->orientationDegrees -= 72;
			}  // if( orientationFound )

		}  // if pathClosed

		if( pathThisP->pathClosed )
			(tlngP->numPathsClosed) ++ ;
		else
			(tlngP->numPathsOpen) ++ ;

	}  // for( rhId_PathStart ... )

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
	{
		pathThisP = &(tlngP->path[ pathId ]);
		rhThisP = &(tlngP->rhombii[ pathThisP->rhId_PathCentreClosest ]);
		pathThisP->pointy = ( 5 == pathThisP->pathLength  &&  pathThisP->pathClosed  &&  points_same_2(tlngP, rhThisP->north, pathThisP->centre) ) ;

		pathThisP->pathClosedTypeNum = pathClosedTypeNum(pathThisP->pathClosed,  pathThisP->pathLength,  pathThisP->pointy);
	}  // rhId_This

	paths_sort(tlngP, &pathGt_ByClosedEtc);

}  // paths_populate()
