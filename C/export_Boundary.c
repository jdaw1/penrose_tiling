// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_Boundary.c, in PenroseC

#include "penrose.h"

#define traipseCountMax ( (int8_t)8 )

typedef enum  // Corner
{
	North = 0,
	East  = 1,
	South = 2,
	West  = 3
} Corner;

typedef struct {RhombId rhId; Corner corner;} RhombCorner;

void assign_exterior_booleans(
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


RhombCorner next_RhombCorner(
	const Tiling * const tlngP,
	RhombCorner const rcStrt
)
{
	bool exteriorNE, exteriorNW, exteriorSE, exteriorSW;
	Rhombus const *rhStrtP, *rhThisP, *rhNextP;
	RhombId rhPrevId, rhThisId;
	RhombCorner rcNext;
	XY xyStart;

	rhStrtP = &(tlngP->rhombi[rcStrt.rhId]);
	assign_exterior_booleans(rhStrtP, &exteriorNE, &exteriorNW, &exteriorSE, &exteriorSW);

	// Going widdershins = anti-clockwise.
	// Test whether on same rhombus.
	switch( rcStrt.corner )
	{
		case East:   {if(exteriorNE) return (RhombCorner){.rhId=rcStrt.rhId, .corner=North};     break;}
		case North:  {if(exteriorNW) return (RhombCorner){.rhId=rcStrt.rhId, .corner=West };     break;}
		case West:   {if(exteriorSW) return (RhombCorner){.rhId=rcStrt.rhId, .corner=South};     break;}
		case South:  {if(exteriorSE) return (RhombCorner){.rhId=rcStrt.rhId, .corner=East };     break;}
	}  // switch( rcStrt.corner )

	switch( rcStrt.corner )
	{
		case East:   {xyStart = rhStrtP->east ;   break;}
		case North:  {xyStart = rhStrtP->north;   break;}
		case West:   {xyStart = rhStrtP->west ;   break;}
		case South:  {xyStart = rhStrtP->south;   break;}
	}  // switch( rcThis.corner )

	// If here then need different rhombus. Traipse around xyStart, seeking external edge on this vertex.
	int8_t  nghbrNum,  traipseCount;
	bool foundNext;
	rhPrevId = rcStrt.rhId;
	rhThisId = rcStrt.rhId;
	rcNext = rcStrt;  // Only to prevent a compiler warning about not yet initialised.
	for( traipseCount = 0  ;  traipseCount < traipseCountMax  ;  traipseCount++ )  // Counter to prevent infinite loop around an interior vertex.
	{
		rhThisP = &(tlngP->rhombi[rhThisId]);
		foundNext = false;
		for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum ++ )
		{
			rcNext.rhId = rhThisP->neighbours[nghbrNum].rhId;
			if( rcNext.rhId != rhPrevId )
			{
				rhNextP = &(tlngP->rhombi[rcNext.rhId]);
				assign_exterior_booleans(rhNextP, &exteriorNE, &exteriorNW, &exteriorSE, &exteriorSW);

				// For next statements, very relevant that anti-clockwise.
				// If neighbour matches corner position, and clockwise edge exterior, done: hurray.
				// If neighbour matches corner position, and clockwise edge not exterior, advance to that rhombus.
				if( points_same_2(tlngP->edgeLength,  rhNextP->south,  xyStart) )  {rcNext.corner = East ;  foundNext = exteriorSE;  break;}
				if( points_same_2(tlngP->edgeLength,  rhNextP->east ,  xyStart) )  {rcNext.corner = North;  foundNext = exteriorNE;  break;}
				if( points_same_2(tlngP->edgeLength,  rhNextP->north,  xyStart) )  {rcNext.corner = West ;  foundNext = exteriorNW;  break;}
				if( points_same_2(tlngP->edgeLength,  rhNextP->west ,  xyStart) )  {rcNext.corner = South;  foundNext = exteriorSW;  break;}
			}  // NExt != Prev
		}  // for( nghbrNum ... )

		if( foundNext )
			return rcNext;
		
		// There must have been a matching corner. So edge not exterior. So next loop around point.
		rhPrevId = rhThisId;
		rhThisId = rcNext.rhId;
	}  // for( traipseCount ... )
	return rcNext;
}  // next_RhombCorner

XY corner_from_rhombus(Rhombus const * const rhP, Corner const crnr)
{
	switch(crnr)
	{
		case North:  return(rhP->north);
		case South:  return(rhP->south);
		case East :  return(rhP->east);
		case West :  return(rhP->west);
	}  // switch(c)
	fprintf(stderr, "!!! Error !!! Impossibility in corner_from_rhombus(), with crnr = %" PRIi8,  crnr);
	fflush(stderr);
	exit(EXIT_FAILURE);
}  // corner_from_rhombus()


void tiling_export_PaintBoundary_OneVertex(
	FILE * const fp,
	ExportFormat             const ef,
	TilingId                 const tlngId,
	double                   const scalingFactor,
	XY                       const point,
	long int                 const vertexNum,
	bool                     const lastPoint,
	int8_t                   const indentDepth,
	RhombCorner              const rc,
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
		sprintf(scratchString,  "%.9lf %.9lf",  point.x * scalingFactor,  point.y * scalingFactor);
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
		break;  // redundant

	case PS_arcs:
	case PS_rhomb:
	case PS_data:
		sprintf(scratchString,  "%.9lf %.9lf",  point.x * scalingFactor,  point.y * scalingFactor);
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, (PS_data == ef ? 1 : 0) + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"%s %s  %% %s%li%s%s\n",
			scratchString,
			vertexNum == 0 ? "moveto" : "lineto",
			vertexNum == 0 ? "rhId=" : "",
			rc.rhId,
			vertexNum == 0 ? ", corner=" : "",
			vertexNum == 0
				? (rc.corner == North ? "north" : rc.corner == South ? "south" : rc.corner == East ? "east" : "west")
				: (rc.corner == North ? "n"     : rc.corner == South ? "s"     : rc.corner == East ? "e"    : "w")
		);  // fprintf()
		(*numLinesThisFileP) ++;
		return;
		break;  // redundant

	case TSV:
		if( vertexNum == 0 )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"Bndry_%02"   PRIi8 ".TilingId"
				"\tBndry_%02" PRIi8 ".VertexNum"
				"\tBndry_%02" PRIi8 ".RhId"
				"\tBndry_%02" PRIi8 ".Corner"
				"\tBndry_%02" PRIi8 ".X"
				"\tBndry_%02" PRIi8 ".Y\n",
				tlngId, tlngId, tlngId, tlngId, tlngId, tlngId
			);  // fprintf()
			(*numLinesThisFileP) ++;
		}  // firstPoint
		sprintf(scratchString,  "%.12lf\t%.12lf",  point.x * scalingFactor,  point.y * scalingFactor);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp,
			"%" PRIi8 "\t%li\t%li\t%s\t%s\n",
			tlngId,
			vertexNum,
			rc.rhId,
			rc.corner == North ? "North" : rc.corner == South ? "South" : rc.corner == East ? "East" : "West",
			scratchString
		);  // fprintf()
		(*numLinesThisFileP) ++;
		return;

	case JSON:
		sprintf(scratchString,  "\"X\":%.12lf, \"Y\":%.12lf", point.x * scalingFactor,  point.y * scalingFactor);
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"{\"RhId\":%li, \"Corner\":\"%s\", %s}%s\n",
			rc.rhId,
			rc.corner == North ? "N" : rc.corner == South ? "S" : rc.corner == East ? "E" : "W",
			scratchString,
			lastPoint ? "" : ","
		);  // fprintf()
		(*numLinesThisFileP) ++;
		return;
	}  // switch(ef)

	fprintf(stderr, "tiling_export_PaintBoundary_OneVertex(): error with ef=%i, failure to catch all ExportFormat cases. Continuing.\n", ef);
}  // tiling_export_PaintBoundary_OneVertex()


long long int tiling_export_PaintBoundary(
	FILE * const fp,
	ExportFormat        const ef,
	double              const scalingFactor,
	const Tiling      * const tlngP,
	int8_t              const indentDepth,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	if( tlngP->numFats + tlngP->numThins <= 0 )
		return 0;

	if(
		tlngP->xMax_rhId < 0  ||  tlngP->yMax_rhId < 0  ||
		tlngP->xMin_rhId < 0  ||  tlngP->yMin_rhId < 0
	)
	{
		fprintf(stderr, "!!! Error !!! tiling_export_PaintBoundary(): impossibly, extremal rhombii not defined.\n");
		exit(EXIT_FAILURE);
	}  // impossibly, extremal rhombii not defined

	// The upper upper limit, for a theoretical chain of rhombi connected at corners, is 4(f+t).
	// But if the number of rhombi were just over 2^29 = 536,870,912, then possible awkward overflow of a long int.
	// Hence the long long int type. Any possible slowness is irrelevant.
	long long int const noInfiniteLoopCountRh =  ( (long long int)(tlngP->numFats) + (long long int)(tlngP->numThins) ) * 4  +  1;
	long long int numVertices = 0;

	RhombCorner rcThis, rcPrev;
	RhombId     rhStartId;
	Corner      cnStart;
	Rhombus     *rhThisP;
	bool        finished = false;
	XY start;

	rhStartId = rcThis.rhId = tlngP->xMax_rhId;
	rhThisP = &(tlngP->rhombi[rcThis.rhId]);
	if( 4 == rhThisP->numNeighbours )
	{
		fprintf(stderr,
			"!!! Error !!! tiling_export_PaintBoundary(): impossible, xMax_rhId=%li has four neighbours, with tilingId=%" PRIi8 ".\n",
			tlngP->xMax_rhId,  tlngP->tilingId
		);  // fprintf()
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	// Start at corner with maximum x.
	cnStart = North;  start = rhThisP->north;
	if( rhThisP->south.x > start.x ) {cnStart = South;  start = rhThisP->south;}
	if( rhThisP->east.x  > start.x ) {cnStart = East ;  start = rhThisP->east; }
	if( rhThisP->west.x  > start.x ) {cnStart = West ;  start = rhThisP->west; }
	rcThis.corner = cnStart;

	for( numVertices = 0  ;  numVertices < noInfiniteLoopCountRh  ;   )
	{
		rcPrev = rcThis;
		rcThis = next_RhombCorner(tlngP, rcThis);
		rhThisP = &(tlngP->rhombi[rcThis.rhId]);

		if(points_same_2( tlngP->edgeLength,  start,  corner_from_rhombus(rhThisP, rcThis.corner) ) )
		{
			finished = true;
			if( ef == PS_data  &&  numVertices * 3 + 1 >= PostScriptArrayLengthMax - 50 )
			{
				printf(
					"!!! tiling_export_PaintBoundary(), warning with ef == PS_data, tilingId = %" PRIi8
					": at finish numVertices = %lli, which %s cause stack overflow or array-construction overflow.\n",
					tlngP->tilingId,   numVertices,   numVertices * 3 + 1 > PostScriptArrayLengthMax ? "will" : "might"
				);  // printf()
			}  // warning re length of executable array containing boundary path
			fflush(fp);
		}  // if points_same_2 ...

		// Have delayed the output to here so that can know whether finished.
		tiling_export_PaintBoundary_OneVertex(
			fp,
			ef,
			tlngP->tilingId,
			1 / tlngP->edgeLength,
			corner_from_rhombus(
				&(tlngP->rhombi[rcPrev.rhId]),
				rcPrev.corner
			),  // corner_from_rhombus()
			numVertices,
			finished,
			indentDepth,
			rcPrev,
			numLinesThisFileP,
			numCharsThisFileP
		);
		numVertices ++;
		if( finished )
			return numVertices;
	}  // for( numVertices ... )

	return -numVertices;  // Negative signals error
}  // tiling_export_PaintBoundary()
