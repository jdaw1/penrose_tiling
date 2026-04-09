// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// windingNumber.c, in PenroseC

#include "penrose.h"

/*
	Note to self. This is the slow code.
	Most of the total execution time is used populating insideness.
*/

// Functions isLeft() and point_winding_number(), and the copyright and disclaimer paragraph, from
// https://web.archive.org/web/20130126163405/http://geomalgorithms.com:80/a03-_inclusion.html
// // Copyright 2000 softSurfer, 2012 Dan Sunday
// // This code may be freely used and modified for any purpose providing that this copyright notice is included with it.
// // SoftSurfer makes no warranty for this code, and cannot be held liable for any real or imagined damage resulting from its use.
// // Users of this code must verify correctness for their application.

static inline double isLeft(double const innerX,  double const innerY,  XY const xy0,  XY const xy1)
{
	return (xy1.x - xy0.x) * (innerY - xy0.y)
	     - (xy1.y - xy0.y) * (innerX - xy0.x);
}  // isLeft()


// 0 means point outside pathP_Outer; positive means inside, path going clockwise; negative means inside, path going widdershins.
int point_winding_number(register double const x,  register double const y,  const Path * const pathP_Outer,  const Tiling * const tlngP)
{
	register RhombId rhId_This;
	RhombId rhId_Next, rhXXX_WithinPath ;
	Rhombus *rhThisP;
	int wn = 0 ;  // Winding number. Inside <==> wn!=0

	if(x >= pathP_Outer->xMax
	|| y >= pathP_Outer->yMax
	|| x <= pathP_Outer->xMin
	|| y <= pathP_Outer->yMin)
		return 0;

	rhId_This = pathP_Outer->rhId_PathCentreClosest ;
	for( rhXXX_WithinPath = 0  ;  rhXXX_WithinPath < pathP_Outer->pathLength ;  rhXXX_WithinPath++ )  // This technique will include the final edge, from length-1 to 0.
	{
		rhThisP  =  &(tlngP->rhombi[ rhId_This ]) ;
		rhId_Next = NextInPath_RhId(tlngP->rhombi, rhThisP, pathP_Outer->pathLength, true) ;
		if( rhId_Next < 0 )
			break;

		if( tlngP->rhombi[rhId_This].centre.y <= y )
		{
			// start below or on
			if( tlngP->rhombi[rhId_Next].centre.y >  y )  // end above, so upward crossing
				if( isLeft(x, y, tlngP->rhombi[rhId_This].centre, tlngP->rhombi[rhId_Next].centre) > 0 )  // point left of edge
					--wn ;
		}
		else
		{
			// start above
			if( tlngP->rhombi[rhId_Next].centre.y <= y )  // end below or on, so downward crossing
				if( isLeft(x, y, tlngP->rhombi[rhId_This].centre, tlngP->rhombi[rhId_Next].centre) < 0 )  // point right of edge
					++wn;
		}

		rhId_This = rhId_Next ;
	}  // for( rhXXX_WithinPath ...)

	return( wn ) ;
}  // point_winding_number()



// 0 means rhP outside pathP_Outer; positive means inside, path going clockwise; negative means inside, path going widdershins.
int rhombus_winding_number(const Rhombus * const rhP,  const Path * const pathP_Outer,  const Tiling * const tlngP)
{
	if( pathP_Outer->pathLength < 15  ||  !pathP_Outer->pathClosed )
		return 0;

	const double paddingThreshold = 0.9999 * tlngP->edgeLength;  // Mathematically exactly 1, but minus epsilon for machine-precision failures.
	if(rhP->xMax > pathP_Outer->xMax - paddingThreshold
	|| rhP->yMax > pathP_Outer->yMax - paddingThreshold
	|| rhP->xMin < pathP_Outer->xMin + paddingThreshold
	|| rhP->yMin < pathP_Outer->yMin + paddingThreshold)
		return 0;

	return point_winding_number(
		rhP->centre.x + (tlngP->edgeLength / 32),  // Small offset eliminates most equalities
		rhP->centre.y + (tlngP->edgeLength / 32),
		pathP_Outer,
		tlngP
	);
}  // rhombus_winding_number()



// 0 means pathP_Inner outside pathP_Outer; positive means inside, path going clockwise; negative means inside, path going widdershins.
int path_winding_number(
	const Path * const pathP_Inner,
	const Path * const pathP_Outer,
	const Tiling * const tlngP
)
{
	// Easy tests to dispose of easy cases.

	// These conditions might be embedded in calling code. Fast, so repetition is low-cost.
	if( pathP_Outer->pathLength < 3 * pathP_Inner->pathLength  // Boundary case is 15,5.
	||  ( ! pathP_Outer->pathClosed )
	||  ( ! pathP_Inner->pathClosed )
	||  (
		pathP_Inner->pathLength ==  5  &&
		pathP_Outer->pathLength <= 55  &&  // passing this line ==> outer is 15, 25, or 55
		(pathP_Outer->pathLength == 25) != pathP_Inner->pointy
	)  )
		return 0;  // Outside

	const double paddingThreshold = 0.99 * tlngP->edgeLength;  // Mathematically exactly 1, but minus epsilon for machine-precision failures.
	if( pathP_Inner->xMax > pathP_Outer->xMax - paddingThreshold
	||  pathP_Inner->yMax > pathP_Outer->yMax - paddingThreshold
	||  pathP_Inner->xMin < pathP_Outer->xMin + paddingThreshold
	||  pathP_Inner->yMin < pathP_Outer->yMin + paddingThreshold)
		return 0;  // Outside

	// Hence determination of insideness is non-trivial.
	return point_winding_number(
		pathP_Inner->centre.x + (tlngP->edgeLength / 32),  // Small offset eliminates most equalities
		pathP_Inner->centre.y + (tlngP->edgeLength / 32),
		pathP_Outer,
		tlngP
	);
}  // path_winding_number()
