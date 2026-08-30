// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintArcsSVG.c, in PenroseC

#include "penrose.h"

void tiling_export_PaintArcsSVG(
	FILE* const fp,
	Tiling const      * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	RhombId rhId, rhIdStart;
	PathId pathId, pathId_goingOut;
	bool edgeN, edgeE, edgeStartE, foundNeighbour, *arcdEast, *arcdWest, oddToOutSide, firstArc;
	const Path    * pathP;
	const Rhombus * rhP;
	double x,  xMin,  xMax,  allMinX,  allMaxX,  boundaryAdjustment;
	double y,  yMin,  yMax,  allMinY,  allMaxY;
	int8_t nghbrNum, nnn, physiqueCounter;
	XY currentpoint;

	const double displayWidth = svg_displayWidth(tlngP);
	const double strokeWidth  = svg_strokeWidth( tlngP);

	allMinX = tlngP->rhombi[ tlngP->xMax_rhId ].xMax;
	allMaxX = tlngP->rhombi[ tlngP->xMin_rhId ].xMin;
	allMinY = tlngP->rhombi[ tlngP->yMax_rhId ].yMax;
	allMaxY = tlngP->rhombi[ tlngP->yMin_rhId ].yMin;

	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( rhP->numNeighbours < 4 )  // No need to test rhombi that are inside
		{
			x = (rhP->north.x + rhP->east.x) / 2;
			y = (rhP->north.y + rhP->east.y) / 2;
			if( allMaxX < x ) allMaxX = x;
			if( allMinX > x ) allMinX = x;
			if( allMaxY < y ) allMaxY = y;
			if( allMinY > y ) allMinY = y;

			x = (rhP->south.x + rhP->west.x) / 2;
			y = (rhP->south.y + rhP->west.y) / 2;
			if( allMaxX < x ) allMaxX = x;
			if( allMinX > x ) allMinX = x;
			if( allMaxY < y ) allMaxY = y;
			if( allMinY > y ) allMinY = y;

			x = (rhP->south.x + rhP->east.x) / 2;
			y = (rhP->south.y + rhP->east.y) / 2;
			if( allMaxX < x ) allMaxX = x;
			if( allMinX > x ) allMinX = x;
			if( allMaxY < y ) allMaxY = y;
			if( allMinY > y ) allMinY = y;

			x = (rhP->north.x + rhP->west.x) / 2;
			y = (rhP->north.y + rhP->west.y) / 2;
			if( allMaxX < x ) allMaxX = x;
			if( allMinX > x ) allMinX = x;
			if( allMaxY < y ) allMaxY = y;
			if( allMinY > y ) allMinY = y;
		}  // numNeighbours < 4
	}  // for( rhId ... )

	allMinX  -=  tlngP->edgeLength * strokeWidth / 2;
	allMinY  -=  tlngP->edgeLength * strokeWidth / 2;
	allMaxX  +=  tlngP->edgeLength * strokeWidth / 2;
	allMaxY  +=  tlngP->edgeLength * strokeWidth / 2;

	const double actual_xMin = max_2( svg_toPaint_xMin(tlngP) * tlngP->edgeLength,  allMinX );
	const double actual_yMin = max_2( svg_toPaint_yMin(tlngP) * tlngP->edgeLength,  allMinY );
	const double actual_xMax = min_2( svg_toPaint_xMax(tlngP) * tlngP->edgeLength,  allMaxX );
	const double actual_yMax = min_2( svg_toPaint_yMax(tlngP) * tlngP->edgeLength,  allMaxY );

	arcdEast = malloc( (tlngP->numFats + tlngP->numThins) * sizeof(bool) );  // Which arcs painted as part of closed path;
	arcdWest = malloc( (tlngP->numFats + tlngP->numThins) * sizeof(bool) );  // remainder to be stroked individually.
	if( NULL == arcdEast ) fprintf(stderr, "tiling_export_PaintArcsPS(): !!! NULL == arcdEast, with tilingId=%" PRIi8 ". Continuing, with slight misbehaviour. !!!\n", tlngP->tilingId);
	if( NULL == arcdWest ) fprintf(stderr, "tiling_export_PaintArcsPS(): !!! NULL == arcdWest, with tilingId=%" PRIi8 ". Continuing, with slight misbehaviour. !!!\n", tlngP->tilingId);
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
	{
		if( NULL != arcdEast ) arcdEast[rhId] = false;
		if( NULL != arcdWest ) arcdWest[rhId] = false;
	}  // for( rhId ... )


	// Output preamble

	sprintf(scratchString,
		"<svg width='%.4lf' height='%.4lf' viewBox='%.9lf %.9lf %.9lf %.9lf'",
		displayWidth,
		displayWidth * (actual_yMax - actual_yMin) / (actual_xMax - actual_xMin),
		( actual_xMin) / tlngP->edgeLength,
		(-actual_yMax) / tlngP->edgeLength,  // This applied before reflection, so needs -yMax.
		(actual_xMax - actual_xMin) / tlngP->edgeLength,
		(actual_yMax - actual_yMin) / tlngP->edgeLength
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,
		"%s preserveAspectRatio='xMidYMid meet' id='Penrose_Arcs_%02" PRIi8 "' xmlns='http://www.w3.org/2000/svg'>\n",
		scratchString,  tlngP->tilingId
	);  // fprintf()
	(*numLinesThisFileP) ++ ;

	(*numCharsThisFileP) += fprintf(fp,
		"<!-- TilingId = %" PRIi8 ";  NumFats = %li;  NumThins = %li;  ==> #tiles= %li;  NumPathsClosed = %li;  NumPathsOpen = %li;  BoundingPathNumVertices = %lli -->\n"
		"<!-- Licence = \"%s\"; URL = \"%s\"; Author = \"%s\""  "."  " -->\n",
		tlngP->tilingId,  tlngP->numFats,  tlngP->numThins,  tlngP->numFats + tlngP->numThins,  tlngP->numPathsClosed,  tlngP->numPathsOpen,  tlngP->boundingPathNumVertices,  
		TextLicence, TextURL, TextAuthor  // Do not stringClean() these.
	);  // fprintf()
	(*numLinesThisFileP) += 2 ;
	sprintf(scratchString, tlngP->edgeLength >= 0.1 ? "%.15lf" : "%.15E", tlngP->edgeLength);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,  "<!-- EdgeLength = %s -->\n",  scratchString);
	(*numLinesThisFileP) ++;

	sprintf(scratchString,
		"<!-- Browser fail for large SVG? Bug report at https://issues.chromium.org/issues/390969197 -->\n"
		"\n"
		"<g transform='scale(1,-1)'> <!-- PostScript and Excel positive y goes up the page, SVG it goes down. This reflection makes SVG behave as PostScript and Excel. -->\n"
		"\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	if( exportQ(boundingPath, SVG_arcs, tlngP, *numLinesThisFileP) )
	{
		sprintf(scratchString,
			"<!-- This path traces the outside of the rhombi, intended to be fill'able as a background colour to the arcs. -->\n"
			"<!-- The viewBox is almost tight to the arcs, but is a slight subset of the rhombi. -->\n"
			"<!-- If, as default, filling this path with a neutral background colour, e.g. white, it works as-is. -->\n"
			"<!-- But if manually editing so that path stroke'd, or fill'd dramatically, then slightly enlarge the viewBox. -->\n"
			"<!-- Enlargement amount depends on whether vector-effect='non-scaling-stroke', on stroke-width, and slightly on stroke-linejoin. -->\n"
			"<path fill='#FFF'  stroke='none' vector-effect='non-scaling-stroke' stroke-width='2px' stroke-linejoin='round'  d='\n"
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);
		tiling_export_PaintBoundary(
			fp,
			SVG_arcs,
			1.0 / tlngP->edgeLength,
			tlngP,
			0,
			numLinesThisFileP,
			numCharsThisFileP
		);  // tiling_export_PaintBoundary()
		(*numCharsThisFileP) += fprintf(fp, "'/> <!-- Outside of tiling of rhombi. -->\n\n");
		(*numLinesThisFileP) += 2;
	}  // exportQ(boundingPath ...)

	(*numCharsThisFileP) += fprintf(fp, "<g transform='scale(0.5)'> <!-- 1/2, so that all arc radii are precisely 1, lessening file size -->\n\n");
	(*numLinesThisFileP) += 2 ;

	sprintf(scratchString,
		"<style>\n"
			"\t.sf {stroke: #000;  stroke-width: 0.1;  paint-order: fill stroke markers;} <!-- sf = stroke's formatting -->\n"
			"\t.c5r_o  {fill: #FFF;}\n"
			"\t.c5p_o  {fill: #FFF;}\n"
			"\t.c15_o  {fill: #FFF;}\n"
			"\t.c25_o  {fill: #FFF;}\n"
			"\t.c55_o  {fill: #FFF;}\n"
			"\t.c105_o {fill: #FFF;}\n"
			"\t.c215_o {fill: #FFF;}\n"

			"\t.c5r_e  {fill: #FFF;}\n"
			"\t.c5p_e  {fill: #003D99;} <!-- Dark blue -->\n"
			"\t.c15_e  {fill: #B30000;} <!-- Dark red -->\n"
			"\t.c25_e  {fill: #0F0;} <!-- Green -->\n"
			"\t.c55_e  {fill: #FC0;} <!-- Yellow-gold -->\n"
			"\t.c105_e {fill: #4DFFFF;} <!-- Light cyan -->\n"
			"\t.c215_e {fill: #B3A1B3;} <!-- Grey-grey-red -->\n"

			"\t.cBig   {fill: #FFF;}\n"
		"</style>\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	const long int longestPathToBeColoured = svg_arcs_longestPathToBeColoured();

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
	{
		pathP = &(tlngP->path[pathId]);

		if( (! pathP->pathClosed)  ||  pathP->pathClosedTypeNum < 2 )  // open, or 5r. But 5 closed passes.
			break ;

		boundaryAdjustment  =  tlngP->edgeLength * ( 0 == pathP->pathClosedTypeNum % 2  ?  0.691  :  -0.49998 );  // 1.5 - cos(36), with a small machine-precision wobble.
		if( (! pathP->pathVeryClosed)  // Close to edge, such that arc might not be complete
		||  tlngP->rhombi[ pathP->xMin_rhId ].xMin  -  boundaryAdjustment > actual_xMax
		||  tlngP->rhombi[ pathP->xMax_rhId ].xMax  +  boundaryAdjustment < actual_xMin
		||  tlngP->rhombi[ pathP->yMin_rhId ].yMin  -  boundaryAdjustment > actual_yMax
		||  tlngP->rhombi[ pathP->yMax_rhId ].yMax  +  boundaryAdjustment < actual_yMin )
			continue;  // pathId loop

		if( 1 == pathP->pathClosedTypeNum % 2 )
		{
			rhIdStart = rhId = pathP->rhId_PathCentreClosest;
			rhP = &(tlngP->rhombi[rhIdStart]);
			edgeStartE = edgeE = (
				pow(rhP->east.x - pathP->centre.x, 2) + pow(rhP->east.y - pathP->centre.y, 2) <=
				pow(rhP->west.x - pathP->centre.x, 2) + pow(rhP->west.y - pathP->centre.y, 2)
			);  // edgeStartE, edgeE
		}  // 15, 55, 215, 855, 3415, 13655, 54615, 218455, 873815, ...
		else
		{
			rhIdStart = rhId = pathP->rhId_PathCentreFurthest;
			rhP = &(tlngP->rhombi[rhIdStart]);
			edgeStartE = edgeE = (
				pow(rhP->east.x - pathP->centre.x, 2) + pow(rhP->east.y - pathP->centre.y, 2) >=
				pow(rhP->west.x - pathP->centre.x, 2) + pow(rhP->west.y - pathP->centre.y, 2)
			);  // edgeStartE, edgeE
		}  // 5p, 25, 105, 425, 1705, 6825, 27305, 109225, 436905, ...
		edgeN = false ;  // Arcs broadly follow south side, so always start south.
		xMin = xMax = (edgeE ? rhP->east.x : rhP->west.x);
		yMin = yMax = (edgeE ? rhP->east.y : rhP->west.y);

		oddToOutSide = false;
		pathId_goingOut = pathId;
		do
		{
			pathId_goingOut = tlngP->path[pathId_goingOut].pathId_ShortestOuter;
			if( pathId_goingOut < 0  ||  tlngP->path[pathId_goingOut].pathLength > longestPathToBeColoured )
				break;
			oddToOutSide = (! oddToOutSide);  // Disliked " ^= true " and the other possibilities in https://stackoverflow.com/questions/610916/
		} while( true ) ;

		// Info to assist both debugging and subsequent end-user play.
		(*numCharsThisFileP) += fprintf(fp,
			"\n<!-- pathId=%li, length=%li%s",
			pathId,  pathP->pathLength,
			(5 == pathP->pathLength) ? (pathP->pointy ? "p" : "r") : ""
		);  // fprintf()
		(*numLinesThisFileP) ++ ;
		if( pathP->pathId_ShortestOuter >= 0 )
			(*numCharsThisFileP) += fprintf(fp, ", ShortestOuter=%li", pathP->pathId_ShortestOuter);
		if( pathP->pathId_LongestInner  >= 0 )
			(*numCharsThisFileP) += fprintf(fp, ", LongestInner=%li" , pathP->pathId_LongestInner );
		sprintf(scratchString,  ", centreX=%.9lf, centreY=%.9lf",  pathP->centre.x,  pathP->centre.y);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s -->\n", scratchString);
		(*numLinesThisFileP) ++ ;

		if( pathP->pathLength <= longestPathToBeColoured )
			(*numCharsThisFileP) +=
				fprintf(fp, "<path class='c%li%s_%s sf'",
					pathP->pathLength,
					pathP->pathLength == 5 ? (pathP->pointy ? "p" : "r") : "",
					oddToOutSide ? "o" : "e"
				);  // fprintf()
		else
			(*numCharsThisFileP) += fprintf(fp, "<path class='cBig sf'");

		sprintf(scratchString, " d='M %.9lf %.9lf\n",
			( (edgeN ? rhP->north.x : rhP->south.x)  +  (edgeE ? rhP->east.x : rhP->west.x) ) / tlngP->edgeLength,
			( (edgeN ? rhP->north.y : rhP->south.y)  +  (edgeE ? rhP->east.y : rhP->west.y) ) / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s",scratchString);
		(*numLinesThisFileP) ++ ;

		firstArc = true;
		do
		{
			if( edgeE )
			{
				if( xMin > rhP->east.x )  xMin = rhP->east.x;
				if( yMin > rhP->east.y )  yMin = rhP->east.y;
				if( xMax < rhP->east.x )  xMax = rhP->east.x;
				if( yMax < rhP->east.y )  yMax = rhP->east.y;
				if( NULL != arcdEast )  arcdEast[rhId] = true;
			}
			else
			{
				if( xMin > rhP->west.x )  xMin = rhP->west.x;
				if( yMin > rhP->west.y )  yMin = rhP->west.y;
				if( xMax < rhP->west.x )  xMax = rhP->west.x;
				if( yMax < rhP->west.y )  yMax = rhP->west.y;
				if( NULL != arcdWest ) arcdWest[rhId] = true;
			}

			// https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths#arcs
			// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
			sprintf(scratchString,
				"\t%s1 1 0 0 %i %.9lf %.9lf",
				firstArc ? "A " : "",
				edgeN == edgeE ? 1 : 0,
				( (edgeN ? rhP->south.x : rhP->north.x) + (edgeE ? rhP->east.x : rhP->west.x) ) / tlngP->edgeLength,
				( (edgeN ? rhP->south.y : rhP->north.y) + (edgeE ? rhP->east.y : rhP->west.y) ) / tlngP->edgeLength
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s\n",scratchString);
			(*numLinesThisFileP) ++;
			firstArc = false;

			foundNeighbour = false;
			for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum++ )  // VeryClosed  ==>  4==numNeighbours
			{
				if( edgeN != rhP->neighbours[nghbrNum].touchesN  // Because exit N|S is opposite to entry N|S
				&&  edgeE == rhP->neighbours[nghbrNum].touchesE )
				{
					foundNeighbour = true;
					nnn = rhP->neighbours[nghbrNum].nghbrsNghbrNum;
					rhId = rhP->neighbours[nghbrNum].rhId;
					rhP = &(tlngP->rhombi[rhId]);
					edgeN = rhP->neighbours[ nnn ].touchesN;
					edgeE = rhP->neighbours[ nnn ].touchesE;
					break;  // nghbrNum loop
				}  // correct nghbrNum, correct edge
			}  // for( nghbrNum ... )

			if( ! foundNeighbour )
			{
				fprintf(stderr,
					"tiling_export_PaintArcsPS(): !!! impossible failure to find neighbour, pathId=%li, rhId=%li, numNeighbours=%" PRIi8 " !!!\n",
					pathId, rhId, rhP->numNeighbours
				);  // fprintf()
				fflush(stderr);
				fflush(fp);
				fclose(fp);
				exit(EXIT_FAILURE);
			}

		}  // do
		while( rhIdStart != rhId  ||  edgeStartE != edgeE);

		(*numCharsThisFileP) += fprintf(fp,
			"Z'/> <!-- c%li%s_%s, pathId=%li -->\n",
			pathP->pathLength,
			pathP->pathLength == 5 ? (pathP->pointy ? "p" : "r") : "",
			oddToOutSide ? "o" : "e",
			pathId
		);  // fprintf()
		(*numLinesThisFileP) ++;

		fflush(fp);
	}  // for( pathId ... )

	for( physiqueCounter = 0  ;  physiqueCounter <= 1  ;  physiqueCounter++ )
	{
		currentpoint.x = currentpoint.y = DBL_MAX / 2;
		(*numCharsThisFileP) += fprintf(fp,
			"\n"
			"<!-- Arcs, not part of a closed loop, in %s. -->\n"
			"<path class='sf' fill='none' d='\n",
			0 == physiqueCounter ? "thins" : "fats"
		);  // fprintf()
		(*numLinesThisFileP) += 3 ;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			rhP = &(tlngP->rhombi[rhId]);

			if(  ( (Thin == rhP->physique) != (0 == physiqueCounter) )
			||  rhP->xMax < actual_xMin
			||  rhP->yMax < actual_yMin
			||  rhP->xMin > actual_xMax
			||  rhP->yMin > actual_yMax )
				continue;

			if( NULL == arcdEast  ||  (! arcdEast[rhId]) )
			{
				// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
				x = (rhP->east.x + rhP->south.x) / tlngP->edgeLength;
				y = (rhP->east.y + rhP->south.y) / tlngP->edgeLength;
				if( fabs(currentpoint.x - x) > 4E-10  ||  fabs(currentpoint.y - y)  > 4E-10 )
				{
					sprintf(scratchString, "M %.9lf %.9lf ", x, y);
					stringClean(scratchString);
					(*numCharsThisFileP) += fprintf(fp, "\t%s",scratchString);
				}
				else
					(*numCharsThisFileP) += fprintf(fp, "\t");
				currentpoint.x = (rhP->east.x + rhP->north.x) / tlngP->edgeLength;
				currentpoint.y = (rhP->east.y + rhP->north.y) / tlngP->edgeLength;
				sprintf(scratchString, "A 1 1 0 0 0 %.9lf %.9lf", currentpoint.x, currentpoint.y);
				stringClean(scratchString);
				(*numCharsThisFileP) += fprintf(fp, "%s\n",scratchString);
				(*numLinesThisFileP) ++;
			}  // ! arcdEast[]
			if( NULL == arcdWest  ||  (! arcdWest[rhId]) )
			{
				// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
				x = (rhP->west.x + rhP->north.x) / tlngP->edgeLength;
				y = (rhP->west.y + rhP->north.y) / tlngP->edgeLength;
				if( fabs(currentpoint.x - x) > 4E-10  ||  fabs(currentpoint.y - y)  > 4E-10 )
				{
					sprintf(scratchString, "M %.9lf %.9lf ", x, y);
					stringClean(scratchString);
					(*numCharsThisFileP) += fprintf(fp, "\t%s",scratchString);
				}
				else
					(*numCharsThisFileP) += fprintf(fp, "\t");
				currentpoint.x = (rhP->west.x + rhP->south.x) / tlngP->edgeLength;
				currentpoint.y = (rhP->west.y + rhP->south.y) / tlngP->edgeLength;
				sprintf(scratchString, "A 1 1 0 0 0 %.9lf %.9lf", currentpoint.x, currentpoint.y);
				stringClean(scratchString);
				(*numCharsThisFileP) += fprintf(fp, "%s\n",scratchString);
				(*numLinesThisFileP) ++;
			}  // ! arcdEast[]
		}  // for( rhId ... )
		(*numCharsThisFileP) += fprintf(fp, 
			"'/> <!-- End of stroking of loose ends not in a closed path that are in %s. -->\n",
			0 == physiqueCounter ? "thins" : "fats"
		);  // fprintf()
		(*numLinesThisFileP) ++;
	}  // for( physiqueCounter ... )

	(*numCharsThisFileP) += fprintf(fp, "\n</g> <!-- scale(0.5) -->\n");
	(*numLinesThisFileP) += 2 ;

	tiling_export_Gridlines(fp,  tlngP,  SVG_arcs,  numLinesThisFileP,  numCharsThisFileP);  // Code always present; whether or not active controlled by showGridlines()

	(*numCharsThisFileP) += fprintf(fp,
		"</g> <!-- scale(1,-1) -->\n"
		"</svg>\n"
	);  // fprintf()
	(*numLinesThisFileP) += 2;

	if( NULL != arcdEast ) {free(arcdEast);  arcdEast = NULL;}
	if( NULL != arcdWest ) {free(arcdWest);  arcdWest = NULL;}
}  // tiling_export_PaintArcsSVG()
