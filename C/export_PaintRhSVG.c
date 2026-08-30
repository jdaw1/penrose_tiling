// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintRhSVG.c, in PenroseC

#include "penrose.h"

void tiling_export_PaintRhombiSVG(
	FILE* const fp,
	Tiling const      * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	char colourStr[64], gStr[256];
	int8_t nghbrNum,  fatCount;
	bool isWhite, thinGood, firstThisContiguous, anyThisPathStatId;
	RhombId    rhId, rhId_This, rhId_Next;
	PathId     pathId, pathInnerId;
	PathStatId pathStatId;
	const Rhombus   *rhP, *rhThisP, *rhNextP;
	const Path      *pathP, *pathOuterP, *pathInnerP;
	const PathStats *pathStatP;
	const Neighbour *nghbrP;
	const int numDecimalPlaces = 4;

	const double displayWidth = svg_displayWidth(tlngP);  // in pixels
	const double strokeWidth  = svg_strokeWidth(tlngP);   // in TileSpace in which edge length is 1.

	// Some things here in original space; some in TileSpace in which edge length is 1. actual_xMin etc in original space, as used for not showing things.
	const double actual_xMin  =  max_2( svg_toPaint_xMin(tlngP) * tlngP->edgeLength,  tlngP->rhombi[ tlngP->xMin_rhId ].xMin - tlngP->edgeLength * strokeWidth/2 );
	const double actual_xMax  =  min_2( svg_toPaint_xMax(tlngP) * tlngP->edgeLength,  tlngP->rhombi[ tlngP->xMax_rhId ].xMax + tlngP->edgeLength * strokeWidth/2 );
	const double actual_yMin  =  max_2( svg_toPaint_yMin(tlngP) * tlngP->edgeLength,  tlngP->rhombi[ tlngP->yMin_rhId ].yMin - tlngP->edgeLength * strokeWidth/2 );
	const double actual_yMax  =  min_2( svg_toPaint_yMax(tlngP) * tlngP->edgeLength,  tlngP->rhombi[ tlngP->yMax_rhId ].yMax + tlngP->edgeLength * strokeWidth/2 );

	
	// Output preamble

	sprintf(scratchString,
		"<svg width='%.4lf' height='%.4lf' viewBox='%.9lf %.9lf %.9lf %.9lf'",
		displayWidth,
		displayWidth * (actual_yMax - actual_yMin) / (actual_xMax - actual_xMin),
		actual_xMin / tlngP->edgeLength,
		- actual_yMax / tlngP->edgeLength,  // This applied before reflection, so needs -yMax.
		(actual_xMax - actual_xMin) / tlngP->edgeLength,
		(actual_yMax - actual_yMin) / tlngP->edgeLength
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,
		"%s preserveAspectRatio='xMidYMid meet' id='Penrose_Rhombi_%02" PRIi8 "' xmlns='http://www.w3.org/2000/svg'>\n",
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
	sprintf(scratchString,
		"<!-- EdgeLength = 1; stroke-width defined to be EdgeLength / %.4lf. If changing stroke-width, need to alter viewBox"
		", for which it might help to know that, without line, and assuming inclusion of all the C-generated rhombi"
		", values would be %.4lf %.4lf %.4lf %.4lf"  "."  " -->\n",  // all in TileSpace in which edge length is 1.
		tlngP->edgeLength / strokeWidth,
		(tlngP->rhombi[ tlngP->xMin_rhId ].xMin) / tlngP->edgeLength,
		(tlngP->rhombi[ tlngP->yMin_rhId ].yMin) / tlngP->edgeLength,
		(tlngP->rhombi[ tlngP->xMax_rhId ].xMax  -  tlngP->rhombi[ tlngP->xMin_rhId ].xMin) / tlngP->edgeLength,
		(tlngP->rhombi[ tlngP->yMax_rhId ].yMax  -  tlngP->rhombi[ tlngP->yMin_rhId ].yMin) / tlngP->edgeLength
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "<!-- Browser fail for large SVG? Bug report at https://issues.chromium.org/issues/390969197 -->\n");
	(*numLinesThisFileP) ++ ;

	(*numCharsThisFileP) += fprintf(fp,
		"<g transform='scale(1,-1)'> <!-- PostScript and Excel positive y goes up the page, SVG it goes down. This reflection makes SVG behave as the others. -->\n"
	);  // fprintf()
	(*numLinesThisFileP) ++ ;

	if( exportQ(boundingPath, SVG_rhomb, tlngP, *numLinesThisFileP) )
	{
		sprintf(scratchString,
			"<!-- This path traces the outside of the rhombi, intended to be fill'able as a background colour. -->\n"
			"<!-- As all rhombi are filled, usually redundant. Indeed, usually omitted from SVG file. -->\n"
			"<!-- If stroke'd wide, then slightly enlarge the viewBox. -->\n"
			"<!-- Enlargement amount depends on whether vector-effect='non-scaling-stroke', on stroke-width, and slightly on stroke-linejoin. -->\n"
			"<path fill='#FFF'  stroke='none' vector-effect='non-scaling-stroke' stroke-width='2px' stroke-linejoin='round'  d='\n"
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);
		tiling_export_PaintBoundary(
			fp,
			1 / tlngP->edgeLength,
			SVG_rhomb,
			tlngP,
			0,
			numLinesThisFileP,
			numCharsThisFileP
		);  // tiling_export_PaintBoundary()
		(*numCharsThisFileP) += fprintf(fp, "'/> <!-- Outside of tiling of rhombi. -->\n\n");
		(*numLinesThisFileP) += 2;
	}  // exportQ(boundingPath ...)

	/*
	// Debugging assist: text some data.
	(*numCharsThisFileP) += fprintf(fp,
		"<text transform='scale(1,-1)' x='%.4lf' y='%.4lf' font-size='%.4lf' text-anchor='start' alignment-baseline='middle'>%.2hi</text>\n",
		actual_xMin,
		-0.875*actual_yMax -0.125*actual_yMin,
		1.75,
		tlngP->tilingId
	);  // fprintf()
	(*numLinesThisFileP) ++ ;
	sprintf(scratchString, "%02d%02d%02d", timeData->tm_hour,  timeData->tm_min,  (int)(timeData->tm_sec) );
	(*numCharsThisFileP) += fprintf(fp,
		"<text transform='scale(1,-1)' x='%.4lf' y='%.4lf' font-size='%.4lf' text-anchor='end' alignment-baseline='middle'>%s</text>\n",
		actual_xMax,
		-0.875*actual_yMax -0.125*actual_yMin,
		0.6666666,
		scratchString
	);  // fprintf()
	(*numLinesThisFileP) ++ ;
	*/


	// Output defs

	(*numCharsThisFileP) += fprintf(fp, "\n<defs>\n");
	(*numLinesThisFileP) += 2 ;

	// Output defs: rhombi

	sprintf(scratchString,
		"d='M 0 0   L %.14lf %.14lf   %.14lf 0   %.14lf %.14lf  Z'",
		-Cos72,  Cos18,  -GoldenRatioReciprocal,  -Cos72,  -Cos18
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,
		"\t<g id='t'> <!-- thin -->\n"
			"\t\t<path %s/> <!-- thin -->\n"
			"\t\t<!-- <circle fill='#000' cx='-0.15' cy='0' r='%.4lf'/> -->  <!-- north-corner circle, de-comment to show -->\n"
		"\t</g> <!-- thin -->\n",
		scratchString,  strokeWidth
	);  // fprintf()
	(*numLinesThisFileP) += 4;

	sprintf(scratchString, "d='M 0 0   L %.14lf %.14lf   %.14lf 0   %.14lf %.14lf  Z'",  -Cos36,  Sin36,  -GoldenRatio,  -Cos36,  -Sin36);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,
		"\t<g id='f'>\n"
			"\t\t<path %s/> <!-- fat (closed paths) -->\n"
			"\t\t<!-- <circle fill='#3CC' stroke='#C00' stroke-width='%.4lf' paint-order='stroke fill' cx='-0.3' cy='0' r='0.075'/> -->  <!-- north-corner circle, de-comment to show -->\n"
		"\t</g> <!-- fat in closed path -->\n",
		scratchString,  strokeWidth
	);  // fprintf()
	(*numLinesThisFileP) += 4;
	(*numCharsThisFileP) += fprintf(fp,
		"\t<g id='o'>\n"
			"\t\t<path %s/> <!-- fat in open path, for editability separated from fat-in-closed -->\n"
			"\t\t<!-- <circle fill='#000' stroke='#FFF' stroke-width='%.4lf' paint-order='stroke fill' cx='-0.3' cy='0' r='0.075'/> -->  <!-- north-corner circle, de-comment to show -->\n"
			"\t\t<!-- <circle stroke='#000' fill='#6F6' opacity='1' cx='%0.9lf' r='%0.4lf'/> --> <!-- To locate holes, and some other debugging: de-comment to show. -->\n"
		"\t</g> <!-- fat in open path -->\n",
		scratchString,  strokeWidth,  -Cos36,  1.0/6
	);  // fprintf()
	(*numLinesThisFileP) += 5;


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
			);  // fprintf()
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
						rhThisP  =  &(tlngP->rhombi[ rhId_This ]) ;
						for( nghbrNum = 0  ;  nghbrNum < rhThisP->numNeighbours  ;  nghbrNum ++ )
						{
							nghbrP = &(rhThisP->neighbours[nghbrNum]);
							if( nghbrP->touchesE  &&  ! nghbrP->touchesN )
							{
								// Seeking fats rather than thins is robust to being at edge and thins missing.
								rhId_This = rhId_Next = nghbrP->rhId ;
								rhNextP  =  &(tlngP->rhombi[ rhId_Next ]) ;
								(*numCharsThisFileP) += fprintf(fp,
									"\t\t\t<use href='#t' %s/>\n",
									svgTransform(scratchString,  numDecimalPlaces,
										(avg_2(rhThisP->east.x, rhNextP->west.x) - pathOuterP->centre.x) / tlngP->edgeLength,
										(avg_2(rhThisP->east.y, rhNextP->west.y) - pathOuterP->centre.y) / tlngP->edgeLength,
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
						rhP = &(tlngP->rhombi[rhId]);
						if( Thin == rhP->physique  &&   pathStatP->examplePathId == rhP->pathId_ShortestOuter )
						{
							thinGood = true;
							for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum ++ )
							{
								nghbrP = &(rhP->neighbours[nghbrNum]);
								if( Fat == nghbrP->physique  &&  nghbrP->touchesN )
								{
									pathP = &(tlngP->path[ tlngP->rhombi[ nghbrP->rhId ].pathId ]);
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
									svgTransform(scratchString,  numDecimalPlaces,
										(rhP->north.x - pathOuterP->centre.x) / tlngP->edgeLength,
										(rhP->north.y - pathOuterP->centre.y) / tlngP->edgeLength,
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
						svgTransform(scratchString,  numDecimalPlaces,
							(pathInnerP->centre.x - pathOuterP->centre.x) / tlngP->edgeLength,
							(pathInnerP->centre.y - pathOuterP->centre.y) / tlngP->edgeLength,
							pathInnerP->orientationDegrees  -  tlngP->path[ tlngP->pathStat[ pathInnerP->pathStatId ].examplePathId ].orientationDegrees,
							72
						)  // svgTransform()
					);  // fprintf()
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
				rhThisP  =  &(tlngP->rhombi[ rhId_This ]) ;
				rhId_Next = NextInPath_RhId(tlngP->rhombi, rhThisP, pathOuterP->pathLength, true);

				(*numCharsThisFileP) += fprintf(fp,
					"\t\t\t<use href='#f' %s/>\n",
					svgTransform(scratchString,  numDecimalPlaces,
						(rhThisP->north.x - pathOuterP->centre.x) / tlngP->edgeLength,
						(rhThisP->north.y - pathOuterP->centre.y) / tlngP->edgeLength,
						rhThisP->angleDegrees,
						360
					)  // svgTransform()
				);  // fprintf()
				(*numLinesThisFileP) ++ ;

				if( rhId_Next < 0 )
					break;  // Open path, impossible here
				if( tlngP->rhombi[rhId_Next].withinPathNum == 0 )
					break;  // Closed path, back to start

				rhId_This = rhId_Next ;
			}  // while(true)
			(*numCharsThisFileP) += fprintf(fp,  "\t\t</g>  <!-- fill='%s' -->\n", colourStr);
			(*numLinesThisFileP) ++ ;

			// Output white circles inside round closed 5 paths. If not wanted, easy to delete this one line from the SVG.

			if( 5 == pathOuterP->pathLength  &&  pathOuterP->pathClosed  &&  ! pathOuterP->pointy )
			{
				sprintf(scratchString,
					"\t\t<circle r='%.4lf' fill='#FFF' opacity='1'/>  <!-- circle inside round (so closed) 5 paths. If not wanted, delete or comment this one line. -->\n",
					1.0 / 4
				);  // sprintf()
				stringClean(scratchString);
				(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
				(*numLinesThisFileP) ++ ;
			}  // 5r

			(*numCharsThisFileP) += fprintf(fp,
				"\t</g>  <!-- id='c%li%s' -->\n",
				pathStatP->pathLength,  (5 == pathStatP->pathLength && pathStatP->pathClosed) ? (pathStatP->pointy ? "p" : "r") : ""
			);  // fprintf()
			(*numLinesThisFileP) ++ ;
		}  // pathClosed
	}  // for( pathStatId ... )

	(*numCharsThisFileP) += fprintf(fp, "</defs>\n\n");
	(*numLinesThisFileP) += 2 ;


	// Formatting applicable to all rhombi (except those explicitly reformatted)

	sprintf(scratchString,
		"<g stroke-width='%.4lf' stroke='#000' paint-order='fill stroke' stroke-linejoin='round' stroke-opacity='1'>  <!-- all rhombi -->\n",
		strokeWidth
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	// Output those thins not otherwise inside a path

	(*numCharsThisFileP) += fprintf(fp, "\n<g fill='#CCC' opacity='1'>  <!-- thins not otherwise inside a path. -->\n");
	(*numLinesThisFileP) +=2 ;
	for( rhId = tlngP->numFats + tlngP->numThins - 1  ;  rhId >= 0  ;  rhId -- )
	{
		rhP = &(tlngP->rhombi[rhId]);
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
					pathP = &(tlngP->path[ tlngP->rhombi[ nghbrP->rhId ].pathId ]);
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
					svgTransform(scratchString,  numDecimalPlaces,
						rhP->north.x / tlngP->edgeLength,
						rhP->north.y / tlngP->edgeLength,
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
		anyThisPathStatId = false;
		pathStatP = &(tlngP->pathStat[pathStatId]);
		if( ! pathStatP->pathClosed )  // Open
		{
			for( pathId = tlngP->numPathsClosed  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
			{
				pathP = &(tlngP->path[pathId]);
				if( pathP->pathStatId == pathStatId )
				{
					firstThisContiguous = true;
					rhId_This = pathP->rhId_openPathEnd;
					while(true)
					{
						rhThisP  =  &(tlngP->rhombi[ rhId_This ]) ;

						if( rhThisP->xMax >= actual_xMin
						&&  rhThisP->yMax >= actual_yMin
						&&  rhThisP->xMin <= actual_xMax
						&&  rhThisP->yMin <= actual_yMax )
						{
							if( ! anyThisPathStatId )
							{
								anyThisPathStatId = true;
								exportColourSVG(gStr,  colourStr,  &isWhite,  Fat,  pathStatP->pathClosed,  pathStatP->pathLength,  pathStatP->pointy);
								(*numCharsThisFileP) += fprintf(fp, "<g %s\n", gStr);  // This is why outer pathStatId loop.
								(*numLinesThisFileP) ++ ;

								if( ! isWhite )
								{
									(*numCharsThisFileP) += fprintf(fp,
										"<!-- <animate attributeName='fill' attributeType='XML' values='%s;#FFF' keyTimes='0;0.8' dur='10s' begin='0s' calcMode='discrete' repeatCount='indefinite'/> -->\n",
										colourStr
									);  // fprintf()
									(*numLinesThisFileP) ++ ;
								}  // if( ! isWhite )
							}  // if( ! anyThisPathStatId )

							svgTransform(scratchString,  numDecimalPlaces,
								rhThisP->north.x / tlngP->edgeLength,
								rhThisP->north.y / tlngP->edgeLength,
								rhThisP->angleDegrees,
								360
							);  // svgTransform()
							(*numCharsThisFileP) += fprintf(fp, "\t<use href='#o' %s/>", scratchString);
							if( firstThisContiguous )
							{
								(*numCharsThisFileP) += fprintf(fp, " <!-- %li,%li,%li -->\n", pathId, rhId_This, tlngP->rhombi[rhId_This].withinPathNum);
								firstThisContiguous = false;
							} else {
								(*numCharsThisFileP) += fprintf(fp, "\n");
							}
							(*numLinesThisFileP) ++ ;
						}  // in wanted box
						else
						{
							firstThisContiguous = true;
						}  // in outside wanted box

						if( tlngP->rhombi[rhId_This].withinPathNum == 0 )
							break;  // have worked down; this open path done.

						rhId_Next = NextInPath_RhId(tlngP->rhombi, rhThisP, pathP->pathLength, false);
						if( rhId_Next < 0 )
							break;  // Should never happen, as should be caught by previous test about This...withinPathNum == 0.

						rhId_This = rhId_Next ;
					}  // while(true)
				}  // pathP->pathStatId == pathStatId  which implies open
			}  // for( pathId ... )

			if( anyThisPathStatId )
			{
				(*numCharsThisFileP) += fprintf(fp, "</g>  <!-- Fats, open, PathLength=%li. Comments, at only start of contiguous part of a path, are: pathId,rhID,withinPathNum -->\n\n", pathStatP->pathLength);
				(*numLinesThisFileP) += 2;
			}  // anyThisPathStatId
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
		&&  tlngP->rhombi[ pathP->xMax_rhId ].xMax  >=  actual_xMin
		&&  tlngP->rhombi[ pathP->yMax_rhId ].yMax  >=  actual_yMin
		&&  tlngP->rhombi[ pathP->xMin_rhId ].xMin  <=  actual_xMax
		&&  tlngP->rhombi[ pathP->yMin_rhId ].yMin  <=  actual_yMax )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"\t<use href='#c%li%s'%s/> <!-- %li -->\n",
				pathP->pathLength,
				(5 == pathP->pathLength && pathP->pathClosed) ? (pathP->pointy ? "p" : "r") : "",
				svgTransform(scratchString,  numDecimalPlaces,
					pathP->centre.x / tlngP->edgeLength,
					pathP->centre.y / tlngP->edgeLength,
					pathP->orientationDegrees  -  tlngP->path[ tlngP->pathStat[ pathP->pathStatId ].examplePathId ].orientationDegrees,
					72
				),  // svgTransform()
				pathId
			);  // fprintf()
			(*numLinesThisFileP) ++ ;
		}  // if( ... pathP->pathId_ShortestOuter < 0 )
	}  // for( pathId ... )

	sprintf(scratchString, "%0.9lf", tlngP->radiusShortOpen / tlngP->edgeLength);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp,
		"\n<!-- To show circle within which paths the few open paths are long, decomment next four lines. -->\n"
		"<!-- <g opacity='50%%' fill='none'>\n"
			"\t<circle stroke='#000' stroke-width='0.9' r='%s'/>\n"
			"\t<circle stroke='#FFF' stroke-width='0.3' r='%s'/>\n"
		"</g> -->\n\n",
		scratchString, scratchString
	);
	(*numLinesThisFileP) += 7 ;

	(*numCharsThisFileP) += fprintf(fp, "</g>  <!-- all rhombi, stroke -->\n\n");
	(*numLinesThisFileP) += 2 ;

	tiling_export_Gridlines(fp,  tlngP,  SVG_rhomb,  numLinesThisFileP,  numCharsThisFileP);  // Code always present; whether or not active controlled by showGridlines()

	(*numCharsThisFileP) += fprintf(fp,
		"\n"
		"</g>  <!-- End of the global reflection -->\n"
		"</svg>\n"
	);  // fprintf()
	(*numLinesThisFileP) += 3 ;

	fflush(fp);
}  // tiling_export_PaintRhombiSVG
