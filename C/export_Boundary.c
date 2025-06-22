// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, June 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintRhSVG.c, in PenroseC

#include "penrose.h"

static inline void tiling_export_PaintBoundary_OneVertex(
	FILE * const fp,
	ExportFormat             const ef,
	TilingId                 const tlngId,
	XY                       const point,
	long int                 const vertexNum,
	bool                     const lastPoint,
	int8_t                   const indentDepth,
	RhombId                  const rhId,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	if( NULL == fp)  // I.e., if counting vertices without outputting.
		return ;

	extern char scratchString[];

	switch(ef)
	{
	case SVG_arcs:
	case SVG_rhomb:
		sprintf(scratchString,  "%.9lf %.9lf",  point.x,  point.y);
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s%s\n",  vertexNum == 0 ? "M " : vertexNum == 1 ? "L " : "",  scratchString);
		(*numLinesThisFileP) ++;
		if( lastPoint )
		{
			(*numCharsThisFileP) += fIndent(fp, indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "Z");
		}
		return;

	case PS_arcs:
	case PS_rhomb:
	case PS_data:
		sprintf(scratchString,  "%.9lf %.9lf",  point.x,  point.y);
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, (PS_data == ef ? 1 : 0) + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s %seto%s\n", scratchString,  vertexNum == 0 ? "mov" : "lin",  lastPoint ? " closepath" : "");
		(*numLinesThisFileP) ++;
		return;

	case TSV:
		if( vertexNum == 0 )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"Bndry_%02"   PRIi8 ".TilingId"
				"\tBndry_%02" PRIi8 ".VertexNum"
				"\tBndry_%02" PRIi8 ".RhId"
				"\tBndry_%02" PRIi8 ".X"
				"\tBndry_%02" PRIi8 ".Y\n",
				tlngId, tlngId, tlngId, tlngId, tlngId
			);
			(*numLinesThisFileP) ++;
		}  // firstPoint
		sprintf(scratchString,  "%.12lf\t%.12lf",  point.x,  point.y);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%" PRIi8 "\t%li\t%li\t%s\n",  tlngId,  vertexNum,  rhId,  scratchString);
		(*numLinesThisFileP) ++;
		return;

	case JSON:
		sprintf(scratchString,  "\"X\":%.12lf, \"Y\":%.12lf", point.x,  point.y);
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "{\"RhId\":%li, %s}%s\n",  rhId,  scratchString,  lastPoint ? "" : ",");
		(*numLinesThisFileP) ++;
		return;
	}  // switch(ef)

	fprintf(stderr, "tiling_export_PaintBoundary_OneVertex(): error with ef=%i, failure to catch all ExportFormat cases. Continuing.\n", ef);
}  // tiling_export_PaintBoundary_OneVertex()


static inline void assign_exterior_booleans(
	const Rhombus * const rhP,
	bool * const exteriorNE_P,
	bool * const exteriorNW_P,
	bool * const exteriorSE_P,
	bool * const exteriorSW_P
)
{
	int8_t  nghbrNum;

	(*exteriorNE_P) = (*exteriorNW_P) = (*exteriorSE_P) = (*exteriorSW_P) = true;
	for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum ++ )
	{
		if( rhP->neighbours[nghbrNum].touchesN)
		{
			if( rhP->neighbours[nghbrNum].touchesE)
				(*exteriorNE_P) = false;
			else
				(*exteriorNW_P) = false;
		}  // touches north
		else
		{
			if( rhP->neighbours[nghbrNum].touchesE)
				(*exteriorSE_P) = false;
			else
				(*exteriorSW_P) = false;
		}  // touches south
	}  // for( nghbrNum ... )
}  // assign_exterior_booleans()


static inline RhombId next_exterior_rhId_at_vertex_recursive(
	const Tiling * const tlngP,
	const XY xy,
	const RhombId rhThisId,
	const RhombId rhPrevId,
	const int8_t counter
)
{
	int8_t  nghbrNum;
	RhombId rhNextId;
	Rhombus const *rhNextP, *rhThisP;

	if( counter <= 0 )  // Counter to prevent perpetual loop of an interior vertex.
		return -1;

	rhThisP = &(tlngP->rhombi[rhThisId]);

	for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum ++ )
	{
		rhNextId = rhThisP->neighbours[nghbrNum].rhId;
		if( rhNextId == rhPrevId )
			continue;
		rhNextP = &(tlngP->rhombi[rhNextId]);
		if( points_same_2(tlngP->edgeLength,  xy,  rhNextP->north)
		||  points_same_2(tlngP->edgeLength,  xy,  rhNextP->south)
		||  points_same_2(tlngP->edgeLength,  xy,  rhNextP->east )
		||  points_same_2(tlngP->edgeLength,  xy,  rhNextP->west ) )
		{
			if( tlngP->rhombi[rhNextId].numNeighbours <= 3 )
				return( rhNextId );
			else
				return next_exterior_rhId_at_vertex_recursive(tlngP,  xy,  rhNextId,  rhThisId,  counter - 1) ;
		}  // points_same_2(...)
	}  // for( nghbrNum ... )

	return -1;
}  // next_exterior_rhId_at_vertex_recursive()

static inline RhombId next_exterior_rhId_at_vertex(
	const Tiling * const tlngP,
	const XY xy,
	const RhombId rhThisId,
	const RhombId rhPrevId
)
{
	// Ten is 360 / 36, but actually seven, from 3*72 + 4*36,  would have been performant.
	// Yes, this could have been written non-recursively, but with a max recursion depth of about 5, doesn't matter.
	return next_exterior_rhId_at_vertex_recursive(tlngP,  xy,  rhThisId,  rhPrevId,  10);
}  // next_exterior_rhId_at_vertex()


// Naughty overload: if not double-exterior, then set to {DLB_MAX, DLB_MAX}
static inline XY double_exterior_next(double const edgeLength,  const Rhombus * const rhThisP,  const XY xyThis,  const XY xyPrev)
{
	bool exteriorNE, exteriorNW, exteriorSE, exteriorSW;

	assign_exterior_booleans(rhThisP, &exteriorNE, &exteriorNW, &exteriorSE, &exteriorSW);

	if( (exteriorNE && exteriorNW && points_same_2(edgeLength, xyThis, rhThisP->north))
	||  (exteriorSE && exteriorSW && points_same_2(edgeLength, xyThis, rhThisP->south)) )
	{
		if( points_same_2(edgeLength, xyPrev, rhThisP->east ) )   return rhThisP->west ;
		if( points_same_2(edgeLength, xyPrev, rhThisP->west ) )   return rhThisP->east ;
		fprintf(stderr, "double_exterior_next(): impossible N||S error with rhId=%li.\n", rhThisP->rhId);
		fflush(stderr);
		return (XY){.x=DBL_MAX, .y=DBL_MAX};
	}  // this == north or south

	if( (exteriorNE && exteriorSE && points_same_2(edgeLength, xyThis, rhThisP->east ))
	||  (exteriorNW && exteriorSW && points_same_2(edgeLength, xyThis, rhThisP->west )) )
	{
		if( points_same_2(edgeLength, xyPrev, rhThisP->north) )   return rhThisP->south;
		if( points_same_2(edgeLength, xyPrev, rhThisP->south) )   return rhThisP->north;
		fprintf(stderr, "double_exterior_next(): impossible E||W error with rhId=%li.\n", rhThisP->rhId);
		fflush(stderr);
		return (XY){.x=DBL_MAX, .y=DBL_MAX};
	}  // this == east or west

	return (XY){.x=DBL_MAX, .y=DBL_MAX};
}  // double_exterior_next()

long int tiling_export_PaintBoundary(
	FILE * const fp,
	ExportFormat        const ef,
	const Tiling      * const tlngP,
	int8_t              const indentDepth,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	const double edgeLength = tlngP->edgeLength;
	const double epsilon = edgeLength / 1024;  // Machine-precision wobble, or angle of 0.05595 degrees.

	RhombId       rhThisId,  rhNextId,  rhStartId = -1,  rhPrevId = -999;
	Rhombus       *rhThisP,  *rhNextP,  *rhStartP = NULL;
	XY            xyStart,  xyNext,  xyThis,  xyPrev;
	long long int noInfiniteLoopCountRh;
	bool          exteriorNE, exteriorNW, exteriorSE, exteriorSW;
	bool          thisSameN, thisSameS, thisSameE, thisSameW;
	bool          lastPoint = false;
	double        xThis, xMax = -DBL_MAX / 2;  // Halved so that epsilon can be super-safely subtracted.
	long int      numVertices = 0;

	if( tlngP->numFats + tlngP->numThins <= 0 )
		return numVertices;

	for( rhThisId = 0  ;  rhThisId < tlngP->numFats + tlngP->numThins  ;  rhThisId ++ )
	{
		rhThisP = &(tlngP->rhombi[rhThisId]);
		if( rhThisP->numNeighbours <= 3 )
		{
			xThis = max_4(rhThisP->north.x,  rhThisP->south.x,  rhThisP->east.x, rhThisP->west.x);
			// Important to choose a rhombus at an x or y extremity.
			// If holesFill() not call, or not much called, there can be missing internal rhombi,
			// in which case an 'outside' path might merely circumnavigate on of the there.
			if( xThis > xMax - epsilon  &&  (rhThisP->centre.y >= 0  ||  xThis > xMax + epsilon) )
			{
				xMax = xThis;
				rhStartId = rhThisId;
				rhStartP  = rhThisP ;
			}  // xThis > xMax, etc
		}  // numNeighbours <= 3
	}  // for( rhId ... )

	if( NULL == rhStartP  ||  0 > rhStartId )
	{
		fprintf(stderr,
			"tiling_export_PaintBoundayRh(): tilingId=%" PRIi8
			", on a finite plane, not a torus, should always be a rhombus with <=3 neighbours. Not here: weird.\n",
			tlngP->tilingId
		 );
		fflush(stderr);
		return -numVertices;  // Negative signals error
	}  // NULL == rhStartP

	assign_exterior_booleans(rhStartP, &exteriorNE, &exteriorNW, &exteriorSE, &exteriorSW);

	if( !(exteriorNE || exteriorNW || exteriorSE || exteriorSW) )
	{
		fprintf(stderr, "tiling_export_PaintBoundary(): impossibly, <=3 neighbours yet no exterior edge.\n");
		fflush(stderr);
		return -numVertices;  // Negative signals error
	}  // none exterior

	// Choosing clockwise, just to be consistent.
	// exteriorNE ==> E then N.
	// exteriorNW ==> N then W.
	// exteriorSW ==> W then S.
	// exteriorSE ==> S then E.
	xyStart = exteriorNE ? rhStartP->east  : exteriorNW ? rhStartP->north : exteriorSW ? rhStartP->west  : rhStartP->south ;
	xyThis  = exteriorNE ? rhStartP->north : exteriorNW ? rhStartP->west  : exteriorSW ? rhStartP->south : rhStartP->east  ;
	tiling_export_PaintBoundary_OneVertex(fp,  ef,  tlngP->tilingId,  xyStart,  numVertices++,  false,  indentDepth,  rhStartId,  numLinesThisFileP,  numCharsThisFileP);
	xyPrev  = xyStart;

	rhThisId = rhStartId;
	rhThisP  = rhStartP ;
	for( noInfiniteLoopCountRh = 1 + 4*(tlngP->numFats + tlngP->numThins)  ;  noInfiniteLoopCountRh >= 0 ;  noInfiniteLoopCountRh -- )
	{
		xyNext = double_exterior_next(tlngP->edgeLength,  rhThisP,  xyThis,  xyPrev);
		if( xyNext.x < DBL_MAX/2  &&  xyNext.y < DBL_MAX/2 )
		{
			lastPoint = points_same_2(edgeLength, xyStart, xyNext);
			tiling_export_PaintBoundary_OneVertex(fp,  ef,  tlngP->tilingId,  xyThis,  numVertices++,  lastPoint,  indentDepth,  rhThisId,  numLinesThisFileP,  numCharsThisFileP);
			xyPrev = xyThis;
			xyThis = xyNext;
		}  // next is on same rhombus
		else
		{
			rhNextId = next_exterior_rhId_at_vertex(tlngP,  xyThis,  rhThisId,  rhPrevId);
			if( rhNextId < 0 )
			{
				fprintf(stderr, "!!! tiling_export_PaintBoundary(): impossibly , no valid rhNextId.\n");
				fflush(stderr);
				fflush(fp);
				return -numVertices;  // Negative signals error
			}  // no rhNextId
			else
			{
				rhNextP = &(tlngP->rhombi[rhNextId]);
				assign_exterior_booleans(rhNextP, &exteriorNE, &exteriorNW, &exteriorSE, &exteriorSW);
				thisSameN = points_same_2(edgeLength, xyThis, rhNextP->north);
				thisSameS = points_same_2(edgeLength, xyThis, rhNextP->south);
				thisSameE = points_same_2(edgeLength, xyThis, rhNextP->east );
				thisSameW = points_same_2(edgeLength, xyThis, rhNextP->west );

				if     ( (thisSameE  &&  exteriorNE) || (thisSameW  &&  exteriorNW) )
					xyNext = rhNextP->north;
				else if( (thisSameE  &&  exteriorSE) || (thisSameW  &&  exteriorSW) )
					xyNext = rhNextP->south;
				else if( (thisSameN  &&  exteriorNE) || (thisSameS  &&  exteriorSE) )
					xyNext = rhNextP->east;
				else if( (thisSameN  &&  exteriorNW) || (thisSameS  &&  exteriorSW) )
					xyNext = rhNextP->west;
				else
				{
					fprintf(stderr, "!!! tiling_export_PaintBoundary(): impossibly , no valid point on rhNextId.\n");
					fflush(stderr);
					fflush(fp);
					return -numVertices;  // Negative signals error
				}

				lastPoint = points_same_2(edgeLength, xyStart, xyNext);
				tiling_export_PaintBoundary_OneVertex(fp,  ef,  tlngP->tilingId,  xyThis,  numVertices++,  lastPoint,  indentDepth,  rhThisId,  numLinesThisFileP,  numCharsThisFileP);

				xyPrev = xyThis;  rhPrevId = rhThisId;
				xyThis = xyNext;  rhThisId = rhNextId;  rhThisP = rhNextP;
			}  // valid rhNextId
		}  // next is on different rhombus

		if( lastPoint )
		{
			if( ef == PS_data  &&  numVertices * 3 + 1 >= 65535 - 50 )  // uses stack size as of relevant point in PostScript
			{
				printf(
					"!!! tiling_export_PaintBoundary(), warning with ef == PS_data, tilingId = %" PRIi8
					": at finish numVertices = %li, which %s cause stack overflow or array-construction overflow.\n",
					tlngP->tilingId,   numVertices,   numVertices * 3 + 1 >  65535 ? "will" : "might"
				);
			}  // warning re length of executable array containing boundary path
			fflush(fp);
			return numVertices;  // Good so positive return
		}  // lastPoint

	}  // for( noInfiniteLoopCount ... ). Should be redundant; preventing weird infinite loop.

	fflush(fp);
	fprintf(stderr, "!!! tiling_export_PaintBoundary(): strange failure to return naturally. Continuing.\n");
	fflush(stderr);
	return -numVertices;  // Negative signals error
}  // tiling_export_PaintBoundary()
