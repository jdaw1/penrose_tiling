// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportPathStats.c, in PenroseC

#include "penrose.h"

// If exportFormat==TSV && NULL==rhP, then output header row not data.
void pathStat_export(
	FILE * const fp,
	ExportFormat const exportFormat,
	const Tiling * const tlngP,
	PathStats const * const pathStatP,
	int const indentDepth,
	bool const notLast,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	PathId   pathId;
	long int Paths_MaxPerLine, NumOutputablePaths, pathNumOutput;
	bool multiLine;

	switch(exportFormat)
	{
	case PS_rhomb:
	case PS_arcs:
	case SVG_rhomb:
	case SVG_arcs:
		break;  // Not applicable here.

	case PS_data:
			if( NULL == pathStatP )
				break ;  // from PS  (inside pathStat_export, NULL == pathStatP)
			// Count NumOutputablePaths
			for( NumOutputablePaths = pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
			{
				if(
					tlngP->path[pathId].wantedPostScript  &&
					tlngP->path[pathId].pathLength == pathStatP->pathLength  &&
					tlngP->path[pathId].pathClosed == pathStatP->pathClosed  &&
					(
						5 != pathStatP->pathLength  ||
						(! pathStatP->pathClosed)  ||
						tlngP->path[pathId].pointy == pathStatP->pointy
					)
				)
					NumOutputablePaths ++ ;
			}

			Paths_MaxPerLine = 48;
			if( NumOutputablePaths > 0 )
			{
				(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
				(*numCharsThisFileP) += fprintf(fp, "<< /PathClosed %s  /PathLength %li",  pathStatP->pathClosed ? "true" : "false",  pathStatP->pathLength);
				if( 5 == pathStatP->pathLength  &&  pathStatP->pathClosed )
					(*numCharsThisFileP) += fprintf(fp, "  /Pointy %s",  pathStatP->pointy ? "true" : "false");

				if( pathStatP -> pathClosed )
				{
					(*numCharsThisFileP) += fprintf(fp, "  /NumPaths %li  /MaxNumThisFats %li  /MaxNumDeepFats %li  /MaxNumThisThins %li  /MaxNumDeepThins %li",
						pathStatP->numPaths,
						pathStatP->insideThis_MaxNumFats,
						pathStatP->insideDeep_MaxNumFats,
						pathStatP->insideThis_MaxNumThins,
						pathStatP->insideDeep_MaxNumThins
					);

					sprintf(scratchString,
						"  /RadiusMinMin_EdgeLengths %.9f  /RadiusMaxMax_EdgeLengths %.9f  /WidthMax_EdgeLengths %.9f  /HeightMax_EdgeLengths %.9f",
						pathStatP->radiusMinMin / tlngP->edgeLength,
						pathStatP->radiusMaxMax / tlngP->edgeLength,
						pathStatP->widthMax     / tlngP->edgeLength,
						pathStatP->heightMax    / tlngP->edgeLength
					);
					stringClean(scratchString);
					(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
				}  // Closed


				multiLine = ( NumOutputablePaths > Paths_MaxPerLine / 2 );  // Divide by two as already much on line.

				(*numCharsThisFileP) += fprintf(fp, "  /Paths_WantedPostScript [");
				for( pathNumOutput = pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
				{
					if(
						tlngP->path[pathId].wantedPostScript  &&
						tlngP->path[pathId].pathLength == pathStatP->pathLength  &&
						tlngP->path[pathId].pathClosed == pathStatP->pathClosed  &&
						(
							5 != pathStatP->pathLength  ||
							(! pathStatP->pathClosed)  ||
							tlngP->path[pathId].pointy == pathStatP->pointy
						)
					)
					{
						if(multiLine  &&  0 == pathNumOutput % Paths_MaxPerLine)
						{
							(*numCharsThisFileP) += fprintf(fp,"\n");
							(*numLinesThisFileP) ++;
							(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
							(*numCharsThisFileP) += fprintf(fp, "%li", tlngP->wantedPostScriptPathNum[ pathId ]);
						}
						else
						{
							(*numCharsThisFileP) += fprintf(fp, " %li", pathId);
						}

						pathNumOutput ++ ;
					}
				}  // pathId
				if(multiLine)
				{
					(*numCharsThisFileP) += fprintf(fp,"\n");
					(*numLinesThisFileP) ++;
					(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
					(*numCharsThisFileP) += fprintf(fp, "] >>  %% PathClosed=%s, PathLength=%li",  pathStatP->pathClosed ? "true" : "false",  pathStatP->pathLength);
					if( 5 == pathStatP->pathLength  &&  pathStatP->pathClosed )
						(*numCharsThisFileP) += fprintf(fp, ", Pointy=%s",  pathStatP->pointy ? "true" : "false");
					(*numCharsThisFileP) += fprintf(fp, ": NumPaths=%li\n", pathStatP->numPaths);
					(*numLinesThisFileP) ++;
				}
				else
				{
					(*numCharsThisFileP) += fprintf(fp, " ] >>\n");
					(*numLinesThisFileP) ++;
				}  // multiLine
			}  // NumOutputablePaths > 0

			break;  // from PS in pathStat_export


		case JSON:
			if( NULL == pathStatP )
				break ;  // from PS  (inside pathStat_export, NULL == pathStatP)
			Paths_MaxPerLine = 32;
			multiLine = ( pathStatP->numPaths > Paths_MaxPerLine / 2 );  // Divide by two as already much on line.
			(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "{ \"PathClosed\":%s, \"PathLength\":%li",  pathStatP->pathClosed ? "true" : "false",  pathStatP->pathLength);
			if( 5 == pathStatP->pathLength  &&  pathStatP->pathClosed )
				(*numCharsThisFileP) += fprintf(fp, ", \"Pointy\":%s",  pathStatP->pointy ? "true" : "false");
			(*numCharsThisFileP) += fprintf(fp, ", \"NumPaths\":%li, \"MaxNumThisFats\":%li, \"MaxNumDeepFats\":%li, \"MaxNumThisThins\":%li, \"MaxNumDeepThins\":%li",
				pathStatP->numPaths,
				pathStatP->insideThis_MaxNumFats,
				pathStatP->insideDeep_MaxNumFats,
				pathStatP->insideThis_MaxNumThins,
				pathStatP->insideDeep_MaxNumThins
			);

			sprintf(scratchString,
				",  \"WidthMax_EdgeLengths\":%.9f,  \"HeightMax_EdgeLengths\":%.9f",
				pathStatP->widthMax     / tlngP->edgeLength,
				pathStatP->heightMax    / tlngP->edgeLength
			);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
			if( pathStatP->pathClosed)
			{
				sprintf(scratchString,
					",  \"RadiusMinMin_EdgeLengths\":%.9f,  \"RadiusMaxMax_EdgeLengths\":%.9f",
					pathStatP->radiusMinMin / tlngP->edgeLength,
					pathStatP->radiusMaxMax / tlngP->edgeLength
				);
				stringClean(scratchString);
				(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
			}  // Closed

			Paths_MaxPerLine = 32;  // JSON
			(*numCharsThisFileP) += fprintf(fp, ",  \"PathIds\":[");
			for( pathNumOutput = pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
			{
				if(
					pathStatP->pathLength == tlngP->path[pathId].pathLength  &&
					pathStatP->pathClosed == tlngP->path[pathId].pathClosed  &&
					(
						pathStatP->pathLength != 5  ||
						!(pathStatP->pathClosed)  ||
						pathStatP->pointy == tlngP->path[pathId].pointy
					)
				)
				{
					if(multiLine)
					{
						if(0 == pathNumOutput % Paths_MaxPerLine)
						{
							if(0 == pathNumOutput)
								(*numCharsThisFileP) += fprintf(fp,"\n");
							else
								(*numCharsThisFileP) += fprintf(fp,",\n");
							(*numLinesThisFileP) ++;
							(*numCharsThisFileP) += fIndent(fp, 3 + indentDepth);
							(*numCharsThisFileP) += fprintf(fp, "%li", pathId);
						}
						else
							(*numCharsThisFileP) += fprintf(fp, ", %li", pathId);
					}
					else
					{
						if(0 == pathNumOutput)
							(*numCharsThisFileP) += fprintf(fp, " %li", pathId);
						else
							(*numCharsThisFileP) += fprintf(fp, ", %li", pathId);
					}

					pathNumOutput ++;
				}
			}  // for( pathId ... )

			if(multiLine)
			{
				(*numCharsThisFileP) += fprintf(fp,"\n");
				(*numLinesThisFileP) ++;
				(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
			}
			else
			{
				(*numCharsThisFileP) += fprintf(fp, " ");
			}  // multiLine
			(*numCharsThisFileP) += fprintf(fp, "] }%s\n",  notLast ? "," : "");
			(*numLinesThisFileP) ++;

			break;  // from JSON in pathStat_export


		case TSV:
			if( NULL == pathStatP )
			{
				// No data, just headers. Header code here to be near to the data-outputting code.
				// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
				(*numCharsThisFileP) += fprintf(fp,
					"PthStat_%02" PRIi8 ".TilingId"  "\tPthStat_%02" PRIi8 ".PathStatId"  "\tPthStat_%02" PRIi8 ".PathClosed"  "\tPthStat_%02" PRIi8 ".PathLength"  "\tPthStat_%02" PRIi8 ".Pointy"
					"\tPthStat_%02" PRIi8 ".NumPaths"  "\tPthStat_%02" PRIi8 ".RadiusMinMin_EdgeLengths"  "\tPthStat_%02" PRIi8 ".RadiusMaxMax_EdgeLengths"
					"\tPthStat_%02" PRIi8 ".WidthMax_EdgeLengths"  "\tPthStat_%02" PRIi8 ".HeightMax_EdgeLengths"
					"\tPthStat_%02" PRIi8 ".MaxNumThisFats"  "\tPthStat_%02" PRIi8 ".MaxNumThisThins"  "\tPthStat_%02" PRIi8 ".MaxNumDeepFats"  "\tPthStat_%02" PRIi8 ".MaxNumDeepThins"
					"\tPthStat_%02" PRIi8 ".MaxNumThisFats_Num"  "\tPthStat_%02" PRIi8 ".MaxNumThisThins_Num"  "\tPthStat_%02" PRIi8 ".MaxNumDeepFats_Num"  "\tPthStat_%02" PRIi8 ".MaxNumDeepThins_Num"
					"\tPthStat_%02" PRIi8 ".InteriorsConsistent",
					tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
					tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId
				);
			}  // if( NULL == pathStatP )
			else
			{
				(*numCharsThisFileP) += fprintf(fp, "%" PRIi8  "\t%li"  "\t%s"  "\t%li"  "\t%s"  "\t%li",
					tlngP->tilingId,
					pathStatP->pathStatId,
					pathStatP->pathClosed ? "TRUE" : "FALSE",
					pathStatP->pathLength,
					( pathStatP->pathClosed  &&  5==pathStatP->pathLength ) ? (pathStatP->pointy?"TRUE":"FALSE") : "#N/A",  // #N/A as Excel awkward about CountIfs() blanks.
					pathStatP->numPaths
				);

				if( pathStatP->pathClosed)
				{
					sprintf(scratchString,
						"\t%.12f\t%.12f\t%.12f\t%.12f",
						pathStatP->radiusMinMin / tlngP->edgeLength,
						pathStatP->radiusMaxMax / tlngP->edgeLength,
						pathStatP->widthMax     / tlngP->edgeLength,
						pathStatP->heightMax    / tlngP->edgeLength
					);
					stringClean(scratchString);
					(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

					(*numCharsThisFileP) += fprintf(fp, "\t%li"  "\t%li"  "\t%li"  "\t%li"  "\t%li"  "\t%li"  "\t%li"  "\t%li"  "\t%s",
						pathStatP->insideThis_MaxNumFats,
						pathStatP->insideThis_MaxNumThins,
						pathStatP->insideDeep_MaxNumFats,
						pathStatP->insideDeep_MaxNumThins,
						pathStatP->insideThis_MaxNumFats_Num,
						pathStatP->insideThis_MaxNumThins_Num,
						pathStatP->insideDeep_MaxNumFats_Num,
						pathStatP->insideDeep_MaxNumThins_Num,
						(  pathStatP->numPaths == pathStatP->insideThis_MaxNumFats_Num
						&& pathStatP->numPaths == pathStatP->insideThis_MaxNumThins_Num
						&& pathStatP->numPaths == pathStatP->insideDeep_MaxNumFats_Num
						&& pathStatP->numPaths == pathStatP->insideDeep_MaxNumThins_Num ) ? "TRUE" : "FALSE"
					);
				}
				else  // re open paths
					(*numCharsThisFileP) += fprintf(fp,
						"\t#N/A"  "\t#N/A"  "\t#N/A"  "\t#N/A"  // radius..., widthMax, heightMax
						"\t#N/A"  "\t#N/A"  "\t#N/A"  "\t#N/A"  // inside..._MaxNum...
						"\t#N/A"  "\t#N/A"  "\t#N/A"  "\t#N/A"  // inside..._MaxNum..._Num
						"\tTRUE"  // InteriorsConsistent
					);
			}  // pathStatP is or isn't NULL

			break;  // from TSV in pathStat_export

		}  // switch(exportFormat)

}  // pathStat_export(...)
