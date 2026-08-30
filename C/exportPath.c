// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportPath.c, in PenroseC

#include "penrose.h"

// If exportFormat==TSV && NULL==rhP, then output header row not data.
void path_export(
	FILE              * const fp,
	ExportFormat        const exportFormat,
	Tiling const      * const tlngP,
	Path const        * const pathP,
	int8_t              const indentDepth,
	bool                const notLast,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	long int Paths_MaxPerLine = 48;  // PS only
	PathId   pathId;
	long int NumOutputablePaths, pathNumOutput;  // PS only
	bool multiLine;  // PS only

	switch(exportFormat)
	{
	case PS_rhomb:
	case PS_arcs:
	case SVG_rhomb:
	case SVG_arcs:
		break;  // Not applicable here.


	case PS_data:
		if( NULL == pathP )
			break ;  // from PS  (inside path_export, NULL == pathP)
		if( ! pathP->wantedPostScript )
			break ;  // from PS  (inside path_export, NULL == pathP)
		(*numCharsThisFileP) += fprintf(fp,
			"dup  %li  << /PathId %li"
			"  /WantedPS %s  /Closed %s  /Length %li",
			pathP->pathId,  pathP->pathId,
			pathP->wantedPostScript ? "true" : "false",  pathP->pathClosed?"true":"false",  pathP->pathLength
		);  // fprintf()
		if( 5 == pathP->pathLength  &&  pathP->pathClosed )
			(*numCharsThisFileP) += fprintf(fp, "  /Pointy %s", pathP->pointy?"true":"false");
		(*numCharsThisFileP) += fprintf(fp,  "  /PathStatId %li",  pathP->pathStatId);
		if( pathP->pathClosed )
			(*numCharsThisFileP) += fprintf(fp, "  /VeryClosed %s", pathP->pathVeryClosed?"true":"false");
		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++;
		sprintf(scratchString,
			"/MinX %.9lf  /MaxX %.9lf  /MinY %.9lf  /MaxY %.9lf\n",
			tlngP->rhombi[ pathP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ pathP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMax_rhId ].yMax / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s",  scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);
		if( pathP->pathClosed )
		{
			// Closed
			(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/RhPathStart %li  /RhPathCentreFurthest %li\n",
				pathP->pathClosed && pathP->rhId_PathCentreClosest  >= 0  ? tlngP->wantedPostScriptRhombNum[ pathP->rhId_PathCentreClosest ]  : -1,
				pathP->pathClosed && pathP->rhId_PathCentreFurthest >= 0  ? tlngP->wantedPostScriptRhombNum[ pathP->rhId_PathCentreFurthest ] : -1
			);  // fprintf()
			(*numLinesThisFileP) ++;
			sprintf(scratchString,
				"/Orient %.9lf  /CentreX %.9lf  /CentreY %.9lf  /RadiusMin %.9lf  /RadiusMax %.9lf\n",
				pathP->orientationDegrees,
				pathP->centre.x  / tlngP->edgeLength,
				pathP->centre.y  / tlngP->edgeLength,
				pathP->radiusMin / tlngP->edgeLength,
				pathP->radiusMax / tlngP->edgeLength
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "%s",  scratchString);
			(*numLinesThisFileP) += newlinesInString(scratchString);
			(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/InsideThis_NumFats %li  /InsideThis_NumThins %li  /InsideDeep_NumFats %li  /InsideDeep_NumThins %li\n",
				pathP->insideThis_NumFats,  pathP->insideThis_NumThins,  pathP->insideDeep_NumFats,  pathP->insideDeep_NumThins
			);  // fprintf()
			(*numLinesThisFileP) ++;

			if( pathP->pathId_ShortestOuter >= 0 )
			{
				(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
				(*numCharsThisFileP) += fprintf(fp,
					"/PathId_ShortestOuter %li\n",
					tlngP->wantedPostScriptPathNum[ pathP->pathId_ShortestOuter ]
				);  // fprintf()
				(*numLinesThisFileP) ++;
			}

			if( pathP->pathId_LongestInner  >= 0 )
			{
				Paths_MaxPerLine = 48;
				NumOutputablePaths = 0;
				for( pathId = 1 + pathP->pathId  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ; pathId++ )
					if(
						tlngP->path[pathId].pathId_ShortestOuter == pathP->pathId  &&
						tlngP->path[pathId].wantedPostScript
					)
						NumOutputablePaths++;
				if( NumOutputablePaths > 0 )
				{
					multiLine = ( NumOutputablePaths > Paths_MaxPerLine / 4 );  // Lines already long, hence / 4.
					if(multiLine)
					{
						(*numCharsThisFileP) += fprintf(fp, "\n");
						(*numLinesThisFileP) ++;
						(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
					}
					else
						(*numCharsThisFileP) += fprintf(fp, "  ");
					(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
					(*numCharsThisFileP) += fprintf(fp, "/PathsImmediatelyInside [");
					pathNumOutput = 0;
					for( pathId = 1 + pathP->pathId  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ; pathId++ )
					{
						if(
							tlngP->path[pathId].pathId_ShortestOuter == pathP->pathId  &&
							tlngP->path[pathId].wantedPostScript
						)
						{
							if(multiLine  &&  pathNumOutput > 0  &&  0 == pathNumOutput % Paths_MaxPerLine)
							{
								(*numCharsThisFileP) += fprintf(fp,"\n");
								(*numLinesThisFileP) ++;
								(*numCharsThisFileP) += fIndent(fp, 4 + indentDepth);
								(*numCharsThisFileP) += fprintf(fp, "%li", tlngP->wantedPostScriptPathNum[ pathId ] );
							}
							else
							{
								(*numCharsThisFileP) += fprintf(fp, " %li", tlngP->wantedPostScriptPathNum[ pathId ] );
							}  // if(multiLine ...)

							pathNumOutput ++ ;
						}  // immediately inside, and wanted
					}  // pathId
					if(multiLine)
					{
						(*numCharsThisFileP) += fprintf(fp, " ]\n");
						(*numLinesThisFileP) ++;
						(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
					}
					else
						(*numCharsThisFileP) += fprintf(fp, " ] ");
				}
				else
				{
					(*numCharsThisFileP) += fprintf(fp, " ");
				}  // NumOutputablePaths > 0
			}  // ...pathId_LongestInner >= 0
		}  // Closed
		else
		{
			// Open
			(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "/RhOpenPathEnd %li", pathP->rhId_openPathEnd);
		}  // Open

		(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, ">>  put");
		if( pathP->pathId == 0 )
			(*numCharsThisFileP) += fprintf(fp, "  %% 'InsideThis_NumFats'===Length");
		break;  // from PS  (inside path_export)


	case JSON:
		if( NULL == pathP )
			break ;  // from PS  (inside path_export, NULL == pathP)
		(*numCharsThisFileP) += fprintf(fp,
			"{ \"PathId\":%li,  \"Closed\":%s,  \"Length\":%li",
			pathP->pathId,  pathP->pathClosed?"true":"false",  pathP->pathLength
		);  // fprintf()
		if( pathP->pathClosed  &&  5 == pathP->pathLength )
			(*numCharsThisFileP) += fprintf(fp, ",  \"Pointy\":%s", pathP->pointy?"true":"false");
		(*numCharsThisFileP) += fprintf(fp,  ",  \"PathStatId\":%li",  pathP->pathStatId);
		if( pathP->pathClosed)
			(*numCharsThisFileP) += fprintf(fp,  ",  \"VeryClosed\":%s",  pathP->pathVeryClosed?"true":"false");
		(*numCharsThisFileP) += fprintf(fp,  ",  \"WantedPostScript\":%s",  pathP->wantedPostScript?"true":"false");
		
		sprintf(scratchString,
			",  \"MinX\":%.9lf,  \"MaxX\":%.9lf,  \"MinY\":%.9lf,  \"MaxY\":%.9lf",
			tlngP->rhombi[ pathP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ pathP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMax_rhId ].yMax / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		if( pathP->pathClosed)
		{
			if( pathP->pathLength > 5 )  // Closed paths of length 5 contain no thins
				(*numCharsThisFileP) += fprintf(fp,
					",  \"RhPathStart\":%li,  \"rhId_PathCentreFurthest\":%li",
					pathP->pathClosed && pathP->rhId_PathCentreClosest  >= 0  ? pathP->rhId_PathCentreClosest  : -1,  // Conditionality should be redundant
					pathP->pathClosed && pathP->rhId_PathCentreFurthest >= 0  ? pathP->rhId_PathCentreFurthest : -1
				);  // fprintf()
			if( pathP->insideDeep_NumThins > 0 )  // condition should be equivalent to  pathClosed && pathLength >= 15, or to pathP->rhId_ThinWithin_First <= pathP->rhId_ThinWithin_Last
				(*numCharsThisFileP) += fprintf(fp,
					",  \"rhId_ThinWithin_First\":%li,  \"rhId_ThinWithin_Last\":%li",
					pathP->rhId_ThinWithin_First, pathP->rhId_ThinWithin_Last
				);  // fprintf()

			if( pathP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp,  ",  \"PathId_ShortestOuter\":%li",  pathP->pathId_ShortestOuter);

			sprintf(scratchString,
				",  \"Orient\":%.9lf,  \"CentreX\":%.9lf,  \"CentreY\":%.9lf,  \"RadiusMin\":%.9lf,  \"RadiusMax\":%.9lf",
				pathP->orientationDegrees,
				pathP->centre.x  / tlngP->edgeLength,
				pathP->centre.y  / tlngP->edgeLength,
				pathP->radiusMin / tlngP->edgeLength,
				pathP->radiusMax / tlngP->edgeLength
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

			(*numCharsThisFileP) += fprintf(fp,
				",  \"InsideThis_NumFats\":%li,  \"InsideThis_NumThins\":%li,  \"InsideDeep_NumFats\":%li,  \"InsideDeep_NumThins\":%li",
				pathP->insideThis_NumFats, pathP->insideThis_NumThins, pathP->insideDeep_NumFats, pathP->insideDeep_NumThins
			);  // fprintf()
		}  // Closed
		else
		{
			// Open
			(*numCharsThisFileP) += fprintf(fp, ",  \"RhOpenPathEnd\":%li", pathP->rhId_openPathEnd);
		}  // Open
			
		(*numCharsThisFileP) += fprintf(fp, "}%s",  notLast?",":"");

		break;  // from JSON  (inside path_export)


	case TSV:
		if( NULL == pathP )
		{
			// No data, just headers. Header code here to be near to the data-outputting code.
			// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
			(*numCharsThisFileP) += fprintf(fp, "\n"
				"Pth_%02" PRIi8 ".TilingId"  "\tPth_%02" PRIi8 ".PathId"  "\tPth_%02" PRIi8 ".Closed"  "\tPth_%02" PRIi8 ".Length"  "\tPth_%02" PRIi8 ".Pointy"
				"\tPth_%02" PRIi8 ".PathStatId"  "\tPth_%02" PRIi8 ".VeryClosed"  "\tPth_%02" PRIi8 ".WantedPS"
				"\tPth_%02" PRIi8 ".MinX"  "\tPth_%02" PRIi8 ".MaxX"  "\tPth_%02" PRIi8 ".MinY"  "\tPth_%02" PRIi8 ".MaxY"
				"\tPth_%02" PRIi8 ".RhOpenPathEnd"  // Only if open; beyond here only if closed
				"\tPth_%02" PRIi8 ".RhPathStart"  "\tPth_%02" PRIi8 ".RhPathCentreFurthest"  "\tPth_%02" PRIi8 ".RhThinWithin_First"   "\tPth_%02" PRIi8 ".RhThinWithin_Last"  "\tPth_%02" PRIi8 ".PathId_ShortestOuter"
				"\tPth_%02" PRIi8 ".Orient"  "\tPth_%02" PRIi8 ".CentreX"  "\tPth_%02" PRIi8 ".CentreY"  "\tPth_%02" PRIi8 ".RadiusMin"  "\tPth_%02" PRIi8 ".RadiusMax"
				"\tPth_%02" PRIi8 ".InsideThis_NumFats"  "\tPth_%02" PRIi8 ".InsideThis_NumThins"  "\tPth_%02" PRIi8 ".InsideDeep_NumFats"  "\tPth_%02" PRIi8 ".InsideDeep_NumThins",
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,  // TilingId ...
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,  // PathStatId
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,  // MinX
				tlngP->tilingId,  // RhOpenPathEnd
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,  // RhPathStart ...
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,  // Orient ...
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId  // InsideThis_NumFats
			);  // fprintf()
			(*numLinesThisFileP) ++;
			break ;  // from TSV  (inside path_export, NULL == pathP)
		}  // if( NULL == pathP )

		(*numCharsThisFileP) += fprintf(fp, "%" PRIi8  "\t%li"  "\t%s"  "\t%li"  "\t%s"  "\t%li"  "\t%s"  "\t%s",
			tlngP->tilingId,  pathP->pathId,  pathP->pathClosed?"TRUE":"FALSE",  pathP->pathLength,
			( (5 == pathP->pathLength  &&  pathP->pathClosed) ? (pathP->pointy?"TRUE":"FALSE") : "#N/A" ),  // #N/A as Excel awkward about CountIfs() blanks.
			pathP->pathStatId,  pathP->pathClosed && pathP->pathVeryClosed ? "TRUE" : "FALSE",  pathP->wantedPostScript ? "TRUE" : "FALSE"
		);  // fprintf()

		sprintf(scratchString,
			"\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf",
			tlngP->rhombi[ pathP->xMin_rhId ].xMin     / tlngP->edgeLength,
			tlngP->rhombi[ pathP->xMax_rhId ].xMax     / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMin_rhId ].yMin     / tlngP->edgeLength,
			tlngP->rhombi[ pathP->yMax_rhId ].yMax     / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		if( pathP->pathClosed )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"\t#N/A"  "\t%li"  "\t%li",  // First is rhId_openPathEnd
				pathP->rhId_PathCentreClosest,  pathP->rhId_PathCentreFurthest
			);  // fprintf()

			if( pathP->insideDeep_NumThins > 0 )  // condition should be equivalent to pathP->rhId_ThinWithin_First <= pathP->rhId_ThinWithin_Last, or to pathP->pathLength >= 15
				(*numCharsThisFileP) += fprintf(fp,  "\t%li\t%li",  pathP->rhId_ThinWithin_First,  pathP->rhId_ThinWithin_Last);
			else
				(*numCharsThisFileP) += fprintf(fp,  "\t#N/A\t#N/A");

			if( pathP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp, "\t%li", pathP->pathId_ShortestOuter) ;
			else
				(*numCharsThisFileP) += fprintf(fp, "\t#N/A") ;

			// ######
			sprintf(scratchString,
				"\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf",
				pathP->orientationDegrees,
				pathP->centre.x  / tlngP->edgeLength,
				pathP->centre.y  / tlngP->edgeLength,
				pathP->radiusMin / tlngP->edgeLength,
				pathP->radiusMax / tlngP->edgeLength
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

			(*numCharsThisFileP) += fprintf(fp,
				"\t%li\t%li\t%li\t%li",
				pathP->insideThis_NumFats,
				pathP->insideThis_NumThins,
				pathP->insideDeep_NumFats,
				pathP->insideDeep_NumThins
			);  // fprintf()
		}
		else  // Open
		{
			(*numCharsThisFileP) += fprintf(fp,  "\t%li\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A",  pathP->rhId_openPathEnd);
		}

		break;  // from TSV  (inside path_export, pathP has data)

	}  // switch(exportFormat)
}  // path_export
