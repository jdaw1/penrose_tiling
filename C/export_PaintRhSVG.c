// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintRhSVG.c, in PenroseC

#include "penrose.h"

void tiling_export_PaintRhombiiSVG(
	FILE* const fp,
	Tiling const      * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	char colourStr[64], gStr[256];
	int8_t nghbrNum,  fatCount;
	bool isWhite, thinGood;
	RhombId    rhId, rhId_This, rhId_Next;
	PathId     pathId, pathInnerId;
	PathStatId pathStatId;
	const Rhombus   *rhP, *rhThisP, *rhNextP;
	const Path      *pathP, *pathOuterP, *pathInnerP;
	const PathStats *pathStatP;
	const Neighbour *nghbrP;

	const double displayWidth = svg_displayWidth(tlngP);
	const double strokeWidth  = svg_strokeWidth(tlngP);

	const double toPaint_xMin = svg_toPaint_xMin(tlngP);  // in controls.c
	const double toPaint_yMin = svg_toPaint_yMin(tlngP);
	const double toPaint_xMax = svg_toPaint_xMax(tlngP);
	const double toPaint_yMax = svg_toPaint_yMax(tlngP);

	const double actual_xMin  =  (toPaint_xMin >= tlngP->xMin - strokeWidth/2)  ?  toPaint_xMin  :  tlngP->xMin - strokeWidth/2;
	const double actual_yMin  =  (toPaint_yMin >= tlngP->yMin - strokeWidth/2)  ?  toPaint_yMin  :  tlngP->yMin - strokeWidth/2;
	const double actual_xMax  =  (toPaint_xMax <= tlngP->xMax + strokeWidth/2)  ?  toPaint_xMax  :  tlngP->xMax + strokeWidth/2;
	const double actual_yMax  =  (toPaint_yMax <= tlngP->yMax + strokeWidth/2)  ?  toPaint_yMax  :  tlngP->yMax + strokeWidth/2;

	// Output preamble

	sprintf(scratchString,
		"<svg width='%0.9lf' height='%0.9lf' viewBox='%0.9lf %0.9lf %0.9lf %0.9lf' preserveAspectRatio='xMidYMid meet' id='Penrose_Rhombii_%02" PRIi8 "' ",
		displayWidth,
		displayWidth * (actual_yMax - actual_yMin) / (actual_xMax - actual_xMin),
		actual_xMin,
		- actual_yMax,  // This applied before reflection, so needs -yMax.
		actual_xMax - actual_xMin,
		actual_yMax - actual_yMin,
		tlngP->tilingId
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numCharsThisFileP) += fprintf(fp, "xmlns='http://www.w3.org/2000/svg'>\n");
	(*numLinesThisFileP) ++ ;

	(*numCharsThisFileP) += fprintf(fp,
		"<!-- TilingId = %" PRIi8 ";  NumFats = %li;  NumThins = %li;  ==> #tiles= %li;  NumPathsClosed = %li;  NumPathsOpen = %li -->\n"
		"<!-- Licence = \"%s\"; URL = \"%s\"; Author = \"%s\""  "."  " -->\n",
		tlngP->tilingId,  tlngP->numFats,  tlngP->numThins,  tlngP->numFats + tlngP->numThins,  tlngP->numPathsClosed,  tlngP->numPathsOpen,
		TextLicence, TextURL, TextAuthor  // Do not stringClean() these.
	);
	(*numLinesThisFileP) += 2 ;
	sprintf(scratchString,
		"<!-- EdgeLength = %0.14lf; stroke-width defined to be EdgeLength / %0.9lf. If changing stroke-width, need to alter viewBox"
		", for which it might help to know that, without line, and assuming inclusion of all the C-generated rhombii"
		", values would be %0.9lf %0.9lf %0.9lf %0.9lf"  "."  " -->\n",
		tlngP->edgeLength, tlngP->edgeLength / strokeWidth,
		tlngP->xMin,  tlngP->yMin,  tlngP->xMax - tlngP->xMin,  tlngP->yMax - tlngP->yMin
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "<!-- Browser fail for large SVG? Bug report at https://issues.chromium.org/issues/390969197 -->\n");
	(*numLinesThisFileP) ++ ;

	(*numCharsThisFileP) += fprintf(fp,
		"<g transform='scale(1,-1)'> <!-- PostScript and Excel positive y goes up the page, SVG it goes down. This reflection makes SVG behave as the others. -->\n"
	);
	(*numLinesThisFileP) ++ ;


	/*
	// Debugging assist: text some data.
	(*numCharsThisFileP) += fprintf(fp,
		"<text transform='scale(1,-1)' x='%0.4lf' y='%0.4lf' font-size='%0.4lf' text-anchor='start' alignment-baseline='middle'>%0.2hi</text>\n",
		actual_xMin,
		-0.875*actual_yMax -0.125*actual_yMin,
		1.75,
		tlngP->tilingId
	);
	(*numLinesThisFileP) ++ ;
	sprintf(scratchString, "%02d%02d%02d", timeData->tm_hour,  timeData->tm_min,  (int)(timeData->tm_sec) );
	(*numCharsThisFileP) += fprintf(fp,
		"<text transform='scale(1,-1)' x='%0.4lf' y='%0.4lf' font-size='%0.4lf' text-anchor='end' alignment-baseline='middle'>%s</text>\n",
		actual_xMax,
		-0.875*actual_yMax -0.125*actual_yMin,
		0.6666666,
		scratchString
	);
	(*numLinesThisFileP) ++ ;
	*/


	// Output defs

	(*numCharsThisFileP) += fprintf(fp, "\n<defs>\n");
	(*numLinesThisFileP) += 2 ;

	// Output defs: rhombii

	sprintf(scratchString,
		"d='M 0 0   L %0.14lf %0.14lf   %0.14lf 0   %0.14lf %0.14lf  Z'",
		- tlngP->edgeLength * Cos72,  tlngP->edgeLength * Cos18,
		-2 * tlngP->edgeLength * Cos72,
		- tlngP->edgeLength * Cos72,  - tlngP->edgeLength * Cos18
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "\t<path id='t' %s/> <!-- thin -->\n", scratchString);
	(*numLinesThisFileP) ++;

	sprintf(scratchString,
		"d='M 0 0   L %0.14lf %0.14lf   %0.14lf 0   %0.14lf %0.14lf  Z'",
		- tlngP->edgeLength * Cos36,  tlngP->edgeLength * Sin36,
		-2 * tlngP->edgeLength * Cos36,
		- tlngP->edgeLength * Cos36,  - tlngP->edgeLength * Sin36
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "\t<path id='f' %s/> <!-- fat (closed paths) -->\n", scratchString);
	(*numLinesThisFileP) ++;
	(*numCharsThisFileP) += fprintf(fp, "\t<path id='o' %s/> <!-- fat (open paths, separate for editability) -->\n", scratchString);
	(*numLinesThisFileP) ++;
	(*numCharsThisFileP) += fprintf(fp,
		"\t<!-- To locate holes, and some other debugging, replace previous line with:"
		"  <g id='o'><path %s/><circle stroke='#000' fill='#6F6' opacity='1' ",
		scratchString
	);
	sprintf(scratchString, "cx='%0.14lf' r='%0.14lf'",  - tlngP->edgeLength * Cos36,  tlngP->edgeLength / 6);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s/></g>  -->\n", scratchString);
	(*numLinesThisFileP) ++;


	// Output defs: paths, which contain both thins and paths

	for( pathStatId = tlngP->numPathStats - 1  ;  pathStatId >= 0  ;  pathStatId -- )
	{
		pathStatP = &(tlngP->pathStat[pathStatId]);
		if( pathStatP->pathClosed )
		{
			pathOuterP = &(tlngP->path[ pathStatP->examplePathId ]);

			(*numCharsThisFileP) += fprintf(fp,
				"\t<g id='c%li%s'>\n",
				pathStatP->pathLength,
				(5 == pathStatP->pathLength && pathStatP->pathClosed) ? (pathStatP->pointy ? "p" : "r") : ""
			);
			(*numLinesThisFileP) ++ ;

			// Output defs: inner thins

			if( pathOuterP->pathClosed   &&  (pathOuterP->pathLength > 15  ||  (pathOuterP->pathLength == 5  &&  ! pathOuterP->pointy)) )
			{
				exportColourSVG(gStr,  colourStr,  &isWhite,  Thin,  false,  0,  false);
				(*numCharsThisFileP) += fprintf(fp,  "\t\t<g %s\n",  gStr);
				(*numLinesThisFileP) ++ ;

				// Include in the def the five thins touching the outside of a round closed 5.
				// This saves about one third of the separate outside-all-paths thins, a material lessening of file size.
				// But cannot do this for other paths, as there would be overlapping thins, causing painting imperfections if transparency were being used.
				if( 5 == pathOuterP->pathLength  &&  pathOuterP->pathClosed  &&  ! pathOuterP->pointy )
				{
					rhId_This = pathOuterP->rhId_PathCentreClosest;

					for( fatCount = 0  ;  fatCount < 5  ;  fatCount ++ )
					{
						rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;
						for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum ++ )
						{
							nghbrP = &(rhThisP->neighbours[nghbrNum]);
							if( nghbrP->touchesE  &&  ! nghbrP->touchesN )
							{
								// Seeking fats rather than thins is robust to being at edge and thins missing.
								rhId_This = rhId_Next = nghbrP->rhId ;
								rhNextP  =  &(tlngP->rhombii[ rhId_Next ]) ;
								(*numCharsThisFileP) += fprintf(fp,
									"\t\t\t<use href='#t' %s/>\n",
									svgTransform(scratchString,
										avg_2(rhThisP->east.x, rhNextP->west.x) - pathOuterP->centre.x,
										avg_2(rhThisP->east.y, rhNextP->west.y) - pathOuterP->centre.y,
										rhThisP->angleDegrees + 144,
										360
									)  // svgTransform()
								);  // fprintf()
								(*numLinesThisFileP) ++ ;
								break;  // nghbrNum loop
							}  // south-east
						}  // for( nghbrNum ... )
					}  // for( fatCount ... )
				}  // 5 Closed round

				if( pathOuterP->pathLength > 15 )
				{
					for( rhId = pathOuterP->rhId_ThinWithin_First  ;  rhId <= pathOuterP->rhId_ThinWithin_Last  ;  rhId ++ )
					{
						rhP = &(tlngP->rhombii[rhId]);
						if( Thin == rhP->physique  &&   pathStatP->examplePathId == rhP->pathId_ShortestOuter )
						{
							thinGood = true;
							for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum ++ )
							{
								nghbrP = &(rhP->neighbours[nghbrNum]);
								if( Fat == nghbrP->physique  &&  nghbrP->touchesN )
								{
									pathP = &(tlngP->path[ tlngP->rhombii[ nghbrP->rhId ].pathId ]);
									if( 5 == pathP->pathLength  &&  pathP->pathClosed  &&  ! pathP->pointy )
									{
										thinGood = false;
										break;  // nghbrNum loop
									}  // 5 closed round
								}  // Fat
							}  // for( nghbrNum ... )

							if( thinGood )
							{
								(*numCharsThisFileP) += fprintf(fp,
									"\t\t\t<use href='#t' %s/>\n",
									svgTransform(scratchString,
										rhP->north.x - pathOuterP->centre.x,
										rhP->north.y - pathOuterP->centre.y,
										rhP->angleDegrees,
										360
									)  // svgTransform()
								);  // fprintf()
								(*numLinesThisFileP) ++ ;
							}  // thinGood
						}  // if( ...examplePathId == ...pathId_ShortestOuter )
					}  // for( rhId ... )
				}  // pathLength > 15

				(*numCharsThisFileP) += fprintf(fp,  "\t\t</g>  <!-- thins, fill='%s' -->\n", colourStr);
				(*numLinesThisFileP) ++ ;
			}  // pathClosed  &&  ...


			// Output defs: inner paths

			for( pathInnerId = tlngP->numPathsClosed - 1  ;  pathInnerId > 0  ;  pathInnerId -- )
			{
				pathInnerP = &(tlngP->path[pathInnerId]);
				if( pathStatP->examplePathId == pathInnerP->pathId_ShortestOuter )
				{
					(*numCharsThisFileP) += fprintf(fp,
						"\t\t<use href='#c%li%s'%s/>\n",
						pathInnerP->pathLength,
						(5 == pathInnerP->pathLength && pathInnerP->pathClosed) ? (pathInnerP->pointy ? "p" : "r") : "",
						svgTransform(scratchString,
							pathInnerP->centre.x - pathOuterP->centre.x,
							pathInnerP->centre.y - pathOuterP->centre.y,
							pathInnerP->orientationDegrees  -  tlngP->path[ tlngP->pathStat[ pathInnerP->pathStatId ].examplePathId ].orientationDegrees,
							72
						)  // svgTransform()
					);
					(*numLinesThisFileP) ++ ;
				}  // inside
			}  // for( pathInnerId ... )

			// Output defs: outer path of fats

			exportColourSVG(gStr,  colourStr,  &isWhite,  Fat,  pathStatP->pathClosed,  pathStatP->pathLength,  pathStatP->pointy);
			(*numCharsThisFileP) += fprintf(fp,  "\t\t<g %s\n",  gStr);
			(*numLinesThisFileP) ++ ;

			rhId_This = pathOuterP->rhId_PathCentreClosest;
			while(true)
			{
				rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;
				rhId_Next = NextInPath_RhId(tlngP->rhombii, rhThisP, pathOuterP->pathLength, true);

				(*numCharsThisFileP) += fprintf(fp,
					"\t\t\t<use href='#f' %s/>\n",
					svgTransform(scratchString,
						rhThisP->north.x - pathOuterP->centre.x,
						rhThisP->north.y - pathOuterP->centre.y,
						rhThisP->angleDegrees,
						360
					)  // svgTransform()
				);
				(*numLinesThisFileP) ++ ;

				if( rhId_Next < 0 )
					break;  // Open path, impossible here
				if( tlngP->rhombii[rhId_Next].withinPathNum == 0 )
					break;  // Closed path, back to start

				rhId_This = rhId_Next ;
			}  // while(true)
			(*numCharsThisFileP) += fprintf(fp,  "\t\t</g>  <!-- fill='%s' -->\n", colourStr);
			(*numLinesThisFileP) ++ ;

			// Output white circles inside round closed 5 paths. If not wanted, easy to delete this one line from the SVG.

			if( 5 == pathOuterP->pathLength  &&  pathOuterP->pathClosed  &&  ! pathOuterP->pointy )
			{
				sprintf(scratchString,
					"\t\t<circle r='%0.9lf' fill='#FFF' opacity='1'/>  <!-- circle inside non-pointy closed 5 paths. If not wanted, delete this one line. -->\n",
					tlngP->edgeLength / 4
				);
				stringClean(scratchString);
				(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
				(*numLinesThisFileP) ++ ;
			}  // 5r

			(*numCharsThisFileP) += fprintf(fp,
				"\t</g>  <!-- id='c%li%s' -->\n",
				pathStatP->pathLength,  (5 == pathStatP->pathLength && pathStatP->pathClosed) ? (pathStatP->pointy ? "p" : "r") : ""
			);
			(*numLinesThisFileP) ++ ;
		}  // pathClosed
	}  // for( pathStatId ... )

	(*numCharsThisFileP) += fprintf(fp, "</defs>\n\n");
	(*numLinesThisFileP) += 2 ;


	// Formatting applicable to all rhombii (except those explicitly reformatted)

	sprintf(scratchString,
		"<g stroke-width='%0.9lf' stroke='#000' paint-order='fill stroke' stroke-linejoin='round' stroke-opacity='1'>  <!-- all rhombii -->\n",
		strokeWidth
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	// Output those thins not otherwise inside a path

	(*numCharsThisFileP) += fprintf(fp, "\n<g fill='#CCC' opacity='1'>  <!-- thins not otherwise inside a path. -->\n");
	(*numLinesThisFileP) +=2 ;
	for( rhId = tlngP->numFats + tlngP->numThins - 1  ;  rhId >= 0  ;  rhId -- )
	{
		rhP = &(tlngP->rhombii[rhId]);
		if( Thin == rhP->physique
		&&  rhP->pathId_ShortestOuter < 0
		&&  rhP->xMax >= actual_xMin
		&&  rhP->yMax >= actual_yMin
		&&  rhP->xMin <= actual_xMax
		&&  rhP->yMin <= actual_yMax )
		{
			thinGood = true;
			for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum ++ )
			{
				nghbrP = &(rhP->neighbours[nghbrNum]);
				if( nghbrP->touchesN  &&  Fat == nghbrP->physique )
				{
					pathP = &(tlngP->path[ tlngP->rhombii[ nghbrP->rhId ].pathId ]);
					if( 5 == pathP->pathLength  &&  pathP->pathClosed  &&  ! pathP->pointy )
					{
						thinGood = false;
						break;  // nghbrNum loop
					}  // 5 closed round
				}  // Fat
			}  // for( nghbrNum ... )

			if( thinGood )
			{
				(*numCharsThisFileP) += fprintf(fp,
					"\t<use href='#t' %s/>\n",
					svgTransform(scratchString,
						rhP->north.x,
						rhP->north.y,
						rhP->angleDegrees,
						360
					)  // svgTransform()
				);  // fprintf()
				(*numLinesThisFileP) ++ ;
			} // thinGood
		}  // thin  and  not in path  and  in box
	}  // for( rhId ... )
	(*numCharsThisFileP) += fprintf(fp, "</g>  <!-- thins not otherwise inside a path -->\n\n");
	(*numLinesThisFileP) += 2;
	fflush(fp);


	// Output open paths

	(*numCharsThisFileP) += fprintf(fp, "\n<!-- Paint open paths -->\n\n");
	(*numLinesThisFileP) += 3;

	for( pathStatId = tlngP->numPathStats - 1  ;  pathStatId >= 0  ;  pathStatId -- )
	{
		pathStatP = &(tlngP->pathStat[pathStatId]);
		if( ! pathStatP->pathClosed )
		{
			exportColourSVG(gStr,  colourStr,  &isWhite,  Fat,  pathStatP->pathClosed,  pathStatP->pathLength,  pathStatP->pointy);
			(*numCharsThisFileP) += fprintf(fp, "<g %s\n", gStr);
			(*numLinesThisFileP) ++ ;

			if( !isWhite )
			{
				(*numCharsThisFileP) += fprintf(fp,
					"<!-- <animate attributeName='fill' attributeType='XML' values='%s;#FFF' keyTimes='0;0.8' dur='10s' begin='0s' calcMode='discrete' repeatCount='indefinite'/> -->\n",
					colourStr
				);
				(*numLinesThisFileP) ++ ;
			}  // if( !isWhite )

			for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
			{
				pathP = &(tlngP->path[pathId]);
				if( pathP->pathStatId == pathStatId )
				{

					rhId_This = pathP->rhId_PathCentreClosest;
					while(true)
					{
						rhThisP  =  &(tlngP->rhombii[ rhId_This ]) ;
						rhId_Next = NextInPath_RhId(tlngP->rhombii, rhThisP, pathP->pathLength, true);

						if( rhThisP->xMax >= actual_xMin
						&&  rhThisP->yMax >= actual_yMin
						&&  rhThisP->xMin <= actual_xMax
						&&  rhThisP->yMin <= actual_yMax )
						{
							(*numCharsThisFileP) += fprintf(fp,
								"\t<use href='#o' %s/>",
								svgTransform(scratchString,
									rhThisP->north.x,
									rhThisP->north.y,
									rhThisP->angleDegrees,
									360
								)  // svgTransform()
							);
							if( rhId_This == pathP->rhId_PathCentreClosest )
								(*numCharsThisFileP) += fprintf(fp, " <!-- %li -->\n", pathId);
							else
								(*numCharsThisFileP) += fprintf(fp, "\n");
							(*numLinesThisFileP) ++ ;
						}  // in wanted box

						if( rhId_Next < 0 )
							break;  // Open path, impossible here
						if( tlngP->rhombii[rhId_Next].withinPathNum == 0 )
							break;  // Closed path, back to start

						rhId_This = rhId_Next ;
					}  // while(true)
				}  // pathP->pathStatId == pathStatId  which implies open
			}  // for( pathId ... )

			(*numCharsThisFileP) += fprintf(fp, "</g>  <!-- Fats, open, PathLength=%li -->\n\n", pathStatP->pathLength);
			(*numLinesThisFileP) += 2;
		}  // pathStatP is open
	}  // for( pathStatId ... )


	// Output closed paths that are not inside a larger path. Such paths contain their contents.

	(*numCharsThisFileP) += fprintf(fp, "\n<!-- Paint closed paths not otherwise inside a larger path. Comment is pathId. -->\n\n");
	(*numLinesThisFileP) += 3;

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
	{
		pathP = &(tlngP->path[pathId]);
		if( pathP->pathClosed
		&&  pathP->pathId_ShortestOuter < 0
		&&  pathP->xMax >= actual_xMin
		&&  pathP->yMax >= actual_yMin
		&&  pathP->xMin <= actual_xMax
		&&  pathP->yMin <= actual_yMax )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"\t<use href='#c%li%s'%s/> <!-- %li -->\n",
				pathP->pathLength,
				(5 == pathP->pathLength && pathP->pathClosed) ? (pathP->pointy ? "p" : "r") : "",
				svgTransform(scratchString,
					pathP->centre.x,
					pathP->centre.y,
					pathP->orientationDegrees  -  tlngP->path[ tlngP->pathStat[ pathP->pathStatId ].examplePathId ].orientationDegrees,
					72
				),  // svgTransform()
				pathId
			);
			(*numLinesThisFileP) ++ ;
		}  // if( ... pathP->pathId_ShortestOuter < 0 )
	}  // for( pathId ... )


	(*numCharsThisFileP) += fprintf(fp, "</g>  <!-- all rhombii, stroke -->\n\n");
	(*numLinesThisFileP) += 2 ;

	(*numCharsThisFileP) += fprintf(fp, "<!-- If gridlines wanted, uncomment these. -->\n");
	(*numLinesThisFileP) ++ ;
	short int gridVal, yLow, xLow, xSize, ySize;
	xLow = (short int)floor(tlngP->xMin);  xSize = (short int)ceil(tlngP->xMax - xLow);
	yLow = (short int)floor(tlngP->yMin);  ySize = (short int)ceil(tlngP->yMax - yLow);
	bool isFirst;
	(*numCharsThisFileP) += fprintf(fp,
		"<!-- Gridlines, start, commented out \n"
		"<path vector-effect='non-scaling-stroke' stroke-width='3px' stroke='#000' opacity='0.2' d='\n"
	);
	(*numLinesThisFileP) += 2 ;
	isFirst = true;
	for( gridVal = yLow  ;  gridVal <= tlngP->yMax  ;  gridVal++ )
	{
		(*numCharsThisFileP) += fprintf(fp, "%sM %hi %hi  h %hi",  isFirst ? "\t" : "   ",  xLow,  gridVal,  xSize);
		isFirst = false;
	}  // for( gridVal ... )
	(*numCharsThisFileP) += fprintf(fp, "\n");
	(*numLinesThisFileP) ++ ;
	isFirst = true;
	for( gridVal = (short int)floor(tlngP->xMin)  ;  gridVal <= tlngP->xMax  ;  gridVal++ )
	{
		(*numCharsThisFileP) += fprintf(fp, "%sM %hi %hi  v %hi",  isFirst ? "\t" : "   ",  gridVal,  yLow,  ySize);
		isFirst = false;
	}  // for( gridVal ... )
	(*numCharsThisFileP) += fprintf(fp, "\n'/>\n<g font-size='0.2' paint-order='stroke fill' stroke-width='0.04' stroke='#FFF' stroke-linejoin='round' fill='#000' opacity='1' text-anchor='middle'>\n");
	(*numLinesThisFileP) += 3 ;
	(*numCharsThisFileP) += fprintf(fp,
		"\t<g transform='translate(%hi,%hi)'><text text-anchor='start' alignment-baseline='baseline' transform='scale(1,-1)'>(%s%hi,%s%hi)</text></g>\n",
		(short int)ceil(tlngP->xMin),
		(short int)ceil(tlngP->yMin),
		ceil(tlngP->xMin) < 0 ? "&#8722;" : "+",
		(short int)fabs(ceil(tlngP->xMin)),
		ceil(tlngP->yMin) < 0 ? "&#8722;" : "+",
		(short int)fabs(ceil(tlngP->yMin))
	);
	(*numLinesThisFileP) ++ ;
	(*numCharsThisFileP) += fprintf(fp,
		"\t<g transform='translate(%hi,%hi)'><text text-anchor='end' alignment-baseline='hanging' transform='scale(1,-1)'>(%s%hi,%s%hi)</text></g>\n",
		(short int)floor(tlngP->xMax),
		(short int)floor(tlngP->yMax),
		floor(tlngP->xMax) < 0 ? "&#8722;" : "+",
		(short int)fabs(floor(tlngP->xMax)),
		floor(tlngP->yMax) < 0 ? "&#8722;" : "+",
		(short int)fabs(floor(tlngP->yMax))
	);
	(*numLinesThisFileP) ++ ;

	(*numCharsThisFileP) += fprintf(fp,
		"</g>\n"
		"Gridlines, end, commented out -->\n\n"
	);
	(*numLinesThisFileP) += 3 ;

	(*numCharsThisFileP) += fprintf(fp,
		"</g>  <!-- End of the global reflection -->\n"
		"</svg>\n"
	);
	(*numLinesThisFileP) += 2 ;

	fflush(fp);
}  // tiling_export_PaintRhombiiSVG
