// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportPath.c, in PenroseC

#include "penrose.h"

// If exportFormat==TSV && NULL==rhP, then output header row not data.
void path_export(
	FILE              * const fp,
	ExportFormat        const exportFormat,
	Tiling const      * const tlngP,
	Path const        * const pathP,
	int                 const indentDepth,
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
		(*numCharsThisFileP) += fprintf(fp,
			"dup  %li  << /PathId %li"
			"  /WantedPS %s  /Closed %s  /VeryClosed %s  /Length %li  /PathStatId %li"
			"  /RhPathStart %li"
			"  /RhPathCentreFurthest %li"
			"  /RhOpenPathEnd %li ",
			tlngP->wantedPostScriptPathNum[ pathP->pathId ],
			pathP->pathId, pathP->wantedPostScript ? "true" : "false",
			pathP->pathClosed?"true":"false",  pathP->pathVeryClosed?"true":"false",  pathP->pathLength,  pathP->pathStatId,
			pathP->pathClosed ? tlngP->wantedPostScriptRhombNum[ pathP->rhId_PathCentreClosest ]  : -1,
			pathP->pathClosed ? tlngP->wantedPostScriptRhombNum[ pathP->rhId_PathCentreFurthest ] : -1,
			pathP->pathClosed ? -1 : tlngP->wantedPostScriptRhombNum[ pathP->rhId_openPathEnd ]
		);
		if( 5 == pathP->pathLength  &&  pathP->pathClosed )
			(*numCharsThisFileP) += fprintf(fp, " /Pointy %s ", pathP->pointy?"true":"false");
		sprintf(scratchString,
			" /CentreX %.9f  /CentreY %.9f  /MinX %.9f  /MaxX %.9f  /MinY %.9f  /MaxY %.9f ",
			pathP->centre.x,  pathP->centre.y,  pathP->xMin,  pathP->xMax,  pathP->yMin,  pathP->yMax
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s",  scratchString);
		if( pathP->pathClosed )
		{
			sprintf(scratchString, " /Orient %.9f  /RadiusMin %.9f  /RadiusMax %.9f ", pathP->orientationDegrees, pathP->radiusMin, pathP->radiusMax);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s",  scratchString);
			(*numCharsThisFileP) += fprintf(fp,
				" /InsideThis_NumFats %li  /InsideThis_NumThins %li  /InsideDeep_NumFats %li  /InsideDeep_NumThins %li",
				pathP->insideThis_NumFats, pathP->insideThis_NumThins, pathP->insideDeep_NumFats, pathP->insideDeep_NumThins
			);
			if( pathP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp,
					"  /PathId_ShortestOuter %li",
					tlngP->wantedPostScriptPathNum[ pathP->pathId_ShortestOuter ]
				);
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
		}  // pathClosed
			(*numCharsThisFileP) += fprintf(fp, ">>  put");
		if( pathP->pathId == 0 )
			(*numCharsThisFileP) += fprintf(fp, "  %% 'InsideThis_NumFats'===Length");
		break;  // from PS  (inside path_export)


	case JSON:
		if( NULL == pathP )
			break ;  // from PS  (inside path_export, NULL == pathP)
		(*numCharsThisFileP) += fprintf(fp,
			"{ \"PathId\":%li,  \"Closed\":%s,  \"VeryClosed\":%s,  \"Length\":%li,  "
			"\"PathStatId\":%li,  \"RhPathStart\":%li,  \"rhId_PathCentreFurthest\":%li",
			pathP->pathId, pathP->pathClosed?"true":"false", pathP->pathVeryClosed?"true":"false", pathP->pathLength,
			pathP->pathStatId,  pathP->rhId_PathCentreClosest,  pathP->rhId_PathCentreFurthest
		);
		if( ! pathP->pathClosed)
			(*numCharsThisFileP) += fprintf(fp, ",  \"rhId_OpenPathEnd\":%li", pathP->rhId_openPathEnd);
		if( 5 == pathP->pathLength  &&  pathP->pathClosed)
			(*numCharsThisFileP) += fprintf(fp, ",  \"Pointy\":%s", pathP->pointy?"true":"false");

		sprintf(scratchString,
			",  \"CentreX\":%.12f,  \"CentreY\":%.12f,  \"MinX\":%.12f,  \"MaxX\":%.12f,  \"MinY\":%.12f,  \"MaxY\":%.12f",
			pathP->centre.x, pathP->centre.y,  pathP->xMin,  pathP->xMax,  pathP->yMin,  pathP->yMax
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		if( pathP->pathClosed )
		{
			sprintf(scratchString, ",  \"Orient\":%.12f,  \"RadiusMin\":%.12f,  \"RadiusMax\":%.12f", pathP->orientationDegrees, pathP->radiusMin, pathP->radiusMax);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
			(*numCharsThisFileP) += fprintf(fp,
				",  \"InsideThis_NumFats\":%li,  \"InsideThis_NumThins\":%li,  \"InsideDeep_NumFats\":%li,  \"InsideDeep_NumThins\":%li",
				pathP->insideThis_NumFats, pathP->insideThis_NumThins, pathP->insideDeep_NumFats, pathP->insideDeep_NumThins
			);
			if( pathP->rhId_ThinWithin_First <= pathP->rhId_ThinWithin_Last )  // condition should be equivalent to  pathClosed && pathLength >= 15, or to insideDeep_NumThins > 0
				(*numCharsThisFileP) += fprintf(fp,  ",  \"rhId_ThinWithin_First\":%li,  \"rhId_ThinWithin_Last\":%li",  pathP->rhId_ThinWithin_First, pathP->rhId_ThinWithin_Last);
			if( pathP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp, ",  \"PathId_ShortestOuter\":%li", pathP->pathId_ShortestOuter);
		}  // pathClosed
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "}%s",  notLast?",":"");
		break;  // from JSON  (inside path_export)


	case TSV:
		if( NULL == pathP )
		{
			// No data, just headers. Header code here to be near to the data-outputting code.
			// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
			(*numCharsThisFileP) += fprintf(fp, "\n"
				"Pth_%02" PRIi8 ".TilingId"  "\tPth_%02" PRIi8 ".PathId"  "\tPth_%02" PRIi8 ".Closed"  "\tPth_%02" PRIi8 ".Length"  "\tPth_%02" PRIi8 ".Pointy"
				"\tPth_%02" PRIi8 ".PathStatId"
				"\tPth_%02" PRIi8 ".RadiusMin"  "\tPth_%02" PRIi8 ".RadiusMax"  "\tPth_%02" PRIi8 ".VeryClosed"  "\tPth_%02" PRIi8 ".PathId_ShortestOuter"
				"\tPth_%02" PRIi8 ".InsideThis_NumFats"  "\tPth_%02" PRIi8 ".InsideThis_NumThins"  "\tPth_%02" PRIi8 ".InsideDeep_NumFats"  "\tPth_%02" PRIi8 ".InsideDeep_NumThins"
				"\tPth_%02" PRIi8 ".CentreX"  "\tPth_%02" PRIi8 ".CentreY"
				"\tPth_%02" PRIi8 ".MinX"  "\tPth_%02" PRIi8 ".MaxX"  "\tPth_%02" PRIi8 ".MinY"  "\tPth_%02" PRIi8 ".MaxY"
				"\tPth_%02" PRIi8 ".Orient"  "\tPth_%02" PRIi8 ".WantedPS"
				"\tPth_%02" PRIi8 ".RhOpenPathEnd"  "\tPth_%02" PRIi8 ".RhPathStart"  "\tPth_%02" PRIi8 ".RhPathCentreFurthest"
				"\tPth_%02" PRIi8 ".RhThinWithin_First"   "\tPth_%02" PRIi8 ".RhThinWithin_Last",
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId
			);
			(*numLinesThisFileP) ++;
			break ;  // from PS  (inside path_export, NULL == pathP)
		}  // if( NULL == pathP )

		(*numCharsThisFileP) += fprintf(fp, "%" PRIi8  "\t%li"  "\t%s"  "\t%li"  "\t%s"  "\t%li",
			tlngP->tilingId,  pathP->pathId,  pathP->pathClosed?"TRUE":"FALSE",  pathP->pathLength,
			( (5 == pathP->pathLength  &&  pathP->pathClosed) ? (pathP->pointy?"TRUE":"FALSE") : "#N/A" ),  // #N/A as Excel awkward about CountIfs() blanks.
			pathP->pathStatId
		) ;

		if( pathP->pathClosed )
		{
			sprintf(scratchString, "\t%.12f"  "\t%.12f", pathP->radiusMin, pathP->radiusMax);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
			(*numCharsThisFileP) += fprintf(fp, "\t%s",  pathP->pathVeryClosed ? "TRUE" : "FALSE");
			if( pathP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp, "\t%li", pathP->pathId_ShortestOuter) ;
			else
				(*numCharsThisFileP) += fprintf(fp, "\t#N/A") ;

			(*numCharsThisFileP) += fprintf(fp,  "\t%li\t%li\t%li\t%li",  pathP->insideThis_NumFats,  pathP->insideThis_NumThins, pathP->insideDeep_NumFats, pathP->insideDeep_NumThins);
		}
		else
			(*numCharsThisFileP) += fprintf(fp, "\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A\t#N/A");

		sprintf(scratchString,
			"\t%.12f"  "\t%.12f"
			"\t%.12f"  "\t%.12f"  "\t%.12f"  "\t%.12f",
			pathP->centre.x,  pathP->centre.y,
			pathP->xMin,  pathP->xMax,  pathP->yMin,  pathP->yMax
		) ;
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		if( pathP->pathClosed )
		{
			sprintf(scratchString, "\t%.12f", pathP->orientationDegrees);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		}
		else
			(*numCharsThisFileP) += fprintf(fp, "\t#N/A");

		(*numCharsThisFileP) += fprintf(fp, "\t%s",  pathP->wantedPostScript ? "TRUE" : "FALSE");

		if( pathP->pathClosed )
		{
			(*numCharsThisFileP) += fprintf(fp,  "\t#N/A\t%li\t%li",  pathP->rhId_PathCentreClosest,  pathP->rhId_PathCentreFurthest);
			if( pathP->rhId_ThinWithin_First <= pathP->rhId_ThinWithin_Last )  // condition should be equivalent to  pathClosed && pathLength >= 15, or to insideDeep_NumThins > 0
				(*numCharsThisFileP) += fprintf(fp,  "\t%li\t%li", pathP->rhId_ThinWithin_First,  pathP->rhId_ThinWithin_Last);
			else
				(*numCharsThisFileP) += fprintf(fp,  "\t#N/A\t#N/A");
		}
		else
			(*numCharsThisFileP) += fprintf(fp,  "\t%li\t#N/A\t#N/A\t#N/A\t#N/A",  pathP->rhId_openPathEnd);

		break;  // from TSV  (inside path_export, pathP has data)

	}  // switch(exportFormat)
}  // path_export
