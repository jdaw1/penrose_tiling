// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportTiling.c, in PenroseC

#include "penrose.h"

void tiling_export(
	FILE* const fp,
	ExportFormat        const exportFormat,
	bool exportQ(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFileP),
	Tiling            * const tlngP,
	int8_t              const indentDepth,
	bool                const notLast,
	TilingId            const tilingId,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	RhombId rhId;
	PathId pathId;
	PathStatId pathStatId;
	long int rhNumOutput, pathsNumToBeOutput, PathStarts_MaxPerLine;
	extern char scratchString[];
	char tempStr[128];

	if( ! exportQ(anything, exportFormat, tlngP, *numLinesThisFileP) )
		return;

	switch(exportFormat)
	{
	case PS_rhomb:
	case PS_arcs:
	case SVG_rhomb:
	case SVG_arcs:
		break;  // Not applicable here.

	case PS_data:
		tlngP->wantedPostScriptRhombNum = malloc(sizeof(struct RhombId *) * (tlngP->numFats + tlngP->numThins) );
		if( tlngP->wantedPostScriptRhombNum == NULL )
		{
			fprintf(stderr,
				"Error in wanted_populate: ...wantedPostScriptRhombNum = malloc(...) returned NULL: "
				"tilingId = %i;  numFats+numThins = %li;  sizeof(RhombId *) = %lu.",
				tlngP->tilingId,  tlngP->numFats + tlngP->numThins,  sizeof(RhombId *)
			);  // fprintf()
			fflush(stderr);
			exit(EXIT_FAILURE) ;
		}  // tlngP->wantedPostScriptRhombNum == NULL
	   
		tlngP->wantedPostScriptPathNum = malloc( (tlngP->numPathsClosed + tlngP->numPathsOpen)  *  sizeof(struct RhombId *) );
		if( tlngP->wantedPostScriptPathNum == NULL )
		{
			fprintf(stderr,
				"Error in wanted_populate: ...wantedPostScriptPathNum = malloc(...) returned NULL: "
				"tilingId = %i;  numPaths = %li;  sizeof(RhombId *) = %lu.",
				tlngP->tilingId,  tlngP->numPathsClosed + tlngP->numPathsOpen,  sizeof(RhombId *)
			);  // fprintf()
			fflush(stderr);
			exit(EXIT_FAILURE) ;
		}  // tlngP->wantedPostScriptPathNum == NULL

		// Scope separation
		{
			long int rhombCounter = 0,  pathCounter = 0;

			for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
				if( tlngP->rhombi[rhId].wantedPostScript )
					tlngP->wantedPostScriptRhombNum[rhId] = (rhombCounter ++);
				else
					tlngP->wantedPostScriptRhombNum[rhId] = -1;

			for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
				if( tlngP->path[pathId].wantedPostScript )
					tlngP->wantedPostScriptPathNum[pathId] = (pathCounter ++);
				else
					tlngP->wantedPostScriptPathNum[pathId] = -1;
		}

		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"<<  %% + tilingId=%" PRIi8 ", in which numFats=%li, numThins=%li, numPathsClosed=%li, numPathsOpen=%li, numWantedRhombi=%li, NumWantedPaths=%li\n",
			tilingId,  tlngP->numFats,  tlngP->numThins, tlngP->numPathsClosed, tlngP->numPathsOpen,
			tlngP->wantedPostScriptNumRhombi, tlngP->wantedPostScriptNumPaths
		);  // fprintf()
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"/TilingId %" PRIi8 "  /DataAsOf (D:%04d%02d%02d%02d%02d%02d)  /SecondsToStartExportFromStartFirstTiling %0.6lf  /SecondsToStartExportFromStartThisTiling %0.6lf  /FileTimeString (%s)\n",
			tilingId,
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec),
			tlngP->SecondsToStartExportFromStartFirstTiling, tlngP->SecondsToStartExportFromStartThisTiling,
			tlngP->timeString
		);  // fprintf()
		(*numLinesThisFileP) ++ ;
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,  "/Licence (%s)  /URL (%s)  /Author (%s)\n",  TextLicence, TextURL, TextAuthor);
		(*numLinesThisFileP) ++ ;
			
		seed_type_name(tempStr,  tlngP->seedType);
		(*numCharsThisFileP) += fprintf(fp,
			"/NumFats %li   /NumThins %li   /BoundingPathNumVertices %lli  /EdgeLength 1  /AxisAligned %s  /SeedType /%s\n",
			tlngP->numFats,  tlngP->numThins,  tlngP->boundingPathNumVertices,  tlngP->axisAligned ? "true" : "false",  tempStr
		);  // fprintf()
		(*numLinesThisFileP) ++;

		sprintf(scratchString, "/MinX %.9lf   /MaxX %.9lf   /MinY %.9lf   /MaxY %.9lf  /RadiusMax %.9lf  /RadiusShortOpen %.9lf",
			tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength,
			tlngP->radiusMax                       / tlngP->edgeLength,
			tlngP->radiusShortOpen                 / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s\n", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString,
			"/wantedPostScriptCentreX %.9lf   /WantedPostScriptCentreY %.9lf   /WantedPostScriptAspect %.9lf   /WantedPostScriptHalfWidth %.9lf   /WantedPostScriptHalfHeight %.9lf",
			tlngP->wantedPostScriptCentre.x,
			tlngP->wantedPostScriptCentre.y,
			tlngP->wantedPostScriptAspect,
			tlngP->wantedPostScriptHalfWidth,
			tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s\n", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"/wantedPostScriptNumRhombi %li   /wantedPostScriptNumPaths %li  %% [ WhollyOutside RhombOutPathIn RhombPartlyIn RhombWhollyIn ]\n",
			tlngP->wantedPostScriptNumRhombi,  tlngP->wantedPostScriptNumPaths
		);  // fprintf()
		(*numLinesThisFileP) ++;

		if( exportQ(pathStats, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "/PathStats [  %% + /PathStats: tilingId=%" PRIi8 ", numPathStats=%li\n", tlngP->tilingId, tlngP->numPathStats);
			(*numLinesThisFileP) ++;
			PathStarts_MaxPerLine = 48;  // PS
			for(pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
			{
				pathStat_export(fp, exportFormat, tlngP, &(tlngP->pathStat[pathStatId]), indentDepth, (pathStatId < tlngP->numPathStats - 1), numLinesThisFileP, numCharsThisFileP);
			}  // pathStatId
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "]  %% - /PathStats: tilingId=%" PRIi8 ", numPathStats=%li\n", tlngP->tilingId, tlngP->numPathStats);
			(*numLinesThisFileP) ++;
		}  // if exportpathStats

		if(exportQ(paths, exportFormat, tlngP, *numLinesThisFileP) )
		{
			pathsNumToBeOutput = tlngP->numPathsClosed + tlngP->numPathsOpen < 65535 ? tlngP->numPathsClosed + tlngP->numPathsOpen : 65535 ;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/Paths %li array  %% + /Paths: tilingId=%" PRIi8 ", numPaths=%li, numPathsOutput=%li",
				pathsNumToBeOutput, tlngP->tilingId, tlngP->numPathsClosed + tlngP->numPathsOpen, pathsNumToBeOutput
			);  // fprintf()
			if( tlngP->numPathStats > 0  &&  tlngP->pathStat[0].pathClosed  &&  tlngP->pathStat[0].numPaths > 0 )
				(*numCharsThisFileP) += fprintf(fp,
					", the longest closed path being of length %li, of which there %s %li.\n",
					tlngP->pathStat[0].pathLength,  tlngP->pathStat[0].numPaths > 1 ? "are" : "is only",  tlngP->pathStat[0].numPaths
				);  // fprintf()
			else
				(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;

			for( pathId = 0  ;  pathId < pathsNumToBeOutput  ;  pathId++ )
			{
				if( tlngP->path[pathId].wantedPostScript )
				{
					(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
					path_export(fp, exportFormat, tlngP, &(tlngP->path[pathId]), indentDepth, (pathId < pathsNumToBeOutput - 1), numLinesThisFileP, numCharsThisFileP);
					(*numCharsThisFileP) += fprintf(fp, "\n");
					(*numLinesThisFileP) ++;
				}
			}  // pathId
			if( tlngP->numPathsClosed + tlngP->numPathsOpen > 0 )
			{
				(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
				(*numCharsThisFileP) += fprintf(fp,
					"%% - /Paths: tilingId=%" PRIi8 ", numPathsClosed=%li, numPathsOpen=%li, numPathsOutput=%li",
					tlngP->tilingId, tlngP->numPathsClosed, tlngP->numPathsOpen, pathsNumToBeOutput
				);  // fprintf()
				if( tlngP->numPathStats > 0  &&  tlngP->pathStat[0].pathClosed  &&  tlngP->pathStat[0].numPaths > 0 )
					(*numCharsThisFileP) += fprintf(fp,
						", the longest closed path being of length %li, of which there %s %li.\n",
						tlngP->pathStat[0].pathLength,  tlngP->pathStat[0].numPaths > 1 ? "are" : "is only",  tlngP->pathStat[0].numPaths
					);  // fprintf()
				else
					(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // if( numPaths > 0 )
		}  // if exportPaths


		if( exportQ(rhombi, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/Rhombi %li array  %% + /Rhombi: tilingId=%" PRIi8 ", numFats=%li, numThins=%li, wantedPostScriptNumRhombi=%li, wantedPostScriptNumPaths=%li\n",
				tlngP->wantedPostScriptNumRhombi,
				tlngP->tilingId, tlngP->numFats, tlngP->numThins,
				tlngP->wantedPostScriptNumRhombi,  tlngP->wantedPostScriptNumPaths
			);  // fprintf()
			(*numLinesThisFileP) ++;
			// PostScript arrays limited to 2^16 elements, so outputting only those wanted.
			for(rhNumOutput = rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++)
				if( tlngP->rhombi[rhId].wantedPostScript )
				{
					(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
					(*numCharsThisFileP) += fprintf(fp, "dup  %li  ", rhNumOutput++);
					rhombus_export(fp, exportFormat, tlngP, tlngP->rhombi + rhId, (rhId < tlngP->numFats + tlngP->numThins - 1), numCharsThisFileP );
					(*numCharsThisFileP) += fprintf(fp, "  put\n");
					(*numLinesThisFileP) ++;
				}
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"%% - /Rhombi: tilingId=%" PRIi8 ", numFats=%li, numThins=%li, numPathsClosed=%li, "
				"numPathsOpen=%li, wantedPostScriptNumRhombi=%li, wantedPostScriptNumPaths=%li\n",
				tilingId,  tlngP->numFats,  tlngP->numThins, tlngP->numPathsClosed, tlngP->numPathsOpen,
				tlngP->wantedPostScriptNumRhombi, tlngP->wantedPostScriptNumPaths
			);  // fprintf()
			(*numLinesThisFileP) ++;
		}  // if exportRhombi

		if( exportQ(boundingPath, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "/RhombiBoundingPath {\n");
			(*numLinesThisFileP) ++;
			tiling_export_PaintBoundary(
				fp,
				PS_data,
				1.0 / tlngP->edgeLength,
				tlngP,
				indentDepth,
				numLinesThisFileP,
				numCharsThisFileP
			);  // tiling_export_PaintBoundary()
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "}  %% /RhombiBoundingPath\n");
			(*numLinesThisFileP) ++;
		}  // if boundingPath

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "/Licence (%s)\n", TextLicence);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "/URL (%s)\n", TextURL);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "/Author (%s)\n", TextAuthor);
		(*numLinesThisFileP) += 3;

		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, ">>  %% - tilingId=%" PRIi8 "\n", tilingId);
		(*numLinesThisFileP) ++;
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"(After ) usertime usertimeStart sub 1000 div 12 string cvs Concatenate ( seconds execution, interpreted tilingId=%" PRIi8 ", containing %li rhombi) Concatenate = flush\n",
			tilingId, tlngP->wantedPostScriptNumRhombi
		);  // fprintf()
		(*numLinesThisFileP) ++;

		if( tlngP->wantedPostScriptRhombNum != NULL )
			{free(tlngP->wantedPostScriptRhombNum);  tlngP->wantedPostScriptRhombNum = NULL;}
		else
			{fprintf(stderr, "\n\ntiling_export(): error. Weirdly, wantedPostScriptRhombNum == NULL. Continuing.\n\n\n");  fflush(stderr);}

		if( tlngP->wantedPostScriptPathNum != NULL )
			{free(tlngP->wantedPostScriptPathNum);  tlngP->wantedPostScriptPathNum = NULL;}
		else
			{fprintf(stderr, "\n\ntiling_export(): error. Weirdly, wantedPostScriptPathNum == NULL. Continuing.\n\n\n");  fflush(stderr);}

		break;  // from PS_data  (inside tiling_export)



	case JSON:
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "{\n" );
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		seed_type_name(tempStr,  tlngP->seedType);

		sprintf(scratchString,
			"\"TilingId\":%" PRIi8   // Time local, not necessarily UTC, hence no trailing Z.
			",   \"SeedType\":\"%s\",   \"AxisAligned\":%s,     \"EdgeLength\":1,\n",
			tilingId,  tempStr,  tlngP->axisAligned ? "true" : "false"
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString,
			"\"NumFats\":%li,   \"NumThins\":%li,   \"NumThins_0T4F\":%li,   \"NumThins_1T3F\":%li"
			",   \"NumPathsClosed\":%li,   \"NumPathsOpen\":%li,   \"NumPathStats\":%li,   \"BoundingPathNumVertices\":%lli,"   "\n",
			tlngP->numFats,  tlngP->numThins,  tlngP->numThins_0T4F,  tlngP->numThins_1T3F,
			tlngP->numPathsClosed,  tlngP->numPathsOpen,  tlngP->numPathStats,  tlngP->boundingPathNumVertices
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"\"MinX_rhId\":%li,"   "  \"MaxX_rhId\":%li,"   "  \"MinY_rhId\":%li,"   "  \"MaxY_rhId\":%li,"   "\n",
			tlngP->xMin_rhId,  tlngP->xMax_rhId,  tlngP->yMin_rhId,  tlngP->yMax_rhId
		);  // fprintf()
		(*numLinesThisFileP) ++;

		sprintf(scratchString,
			"\"MinX\":%.9lf,   \"MaxX\":%.9lf,   \"MinY\":%.9lf,   \"MaxY\":%.9lf,   \"RadiusMax\":%.9lf,   \"RadiusShortOpen\":%.9lf,"  "\n",
			tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength,
			tlngP->radiusMax                       / tlngP->edgeLength,
			tlngP->radiusShortOpen                 / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);

		sprintf(scratchString,
			"\"DataAsOf\":\"%04d-%02d-%02dT%02d:%02d:%02d\""   // Time local, not necessarily UTC, hence no trailing Z.
			",   \"SecondsToStartExportFromStartFirstTiling\":%0.6lf,   \"SecondsToStartExportFromStartThisTiling\":%0.6lf,   \"InternalToC_EdgeLength\":%.16E"
			",   \"FileTimeString\":\"%s\",   \"PersistentSumSimple_malloc\":%zu,\n",
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  tlngP->timeData->tm_sec,
			tlngP->SecondsToStartExportFromStartFirstTiling,  tlngP->SecondsToStartExportFromStartThisTiling,   tlngP->edgeLength,
			tlngP->timeString,  tlngP->mallocsPersistentSumSimple
		);  // sprintf()
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);

		sprintf(scratchString,
			"\"Licence\":\"%s\",   \"URL\":\"%s\",  \"Author\":\"%s\",\n",
			TextLicence, TextURL, TextAuthor
		);  // sprintf()
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);

		sprintf(scratchString,
			"\"WantedPostScriptCentreX\":%.9lf,  \"WantedPostScriptCentreY\":%.9lf"
			",  \"WantedPostScriptHalfWidth\":%.9lf,   \"wantedPostScriptHalfHeight\":%.9lf,  \"WantedwantedPostScriptAspect\":%.9lf"
			",  \"WantedPostScriptNumRhombi\":%li,   \"WantedPostScriptNumPaths\":%li,"   "\n",
			tlngP->wantedPostScriptCentre.x,
			tlngP->wantedPostScriptCentre.y,
			tlngP->wantedPostScriptHalfWidth,
			tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect,
			tlngP->wantedPostScriptAspect,  tlngP->wantedPostScriptNumRhombi,  tlngP->wantedPostScriptNumPaths
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) ++;

		if( exportQ(pathStats, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "\"PathStats\":[\n");
			(*numLinesThisFileP) ++;
			for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
				pathStat_export(fp,
					exportFormat,
					tlngP,
					&(tlngP->pathStat[pathStatId]),
					indentDepth,
					(pathStatId < tlngP->numPathStats - 1),
					numLinesThisFileP,
					numCharsThisFileP
				);  // pathStat_export()
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "]");
		}  // if export pathStats

		if( exportQ(paths, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, ",\n");
			(*numLinesThisFileP) ++;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "\"Paths\":[\n");
			(*numLinesThisFileP) ++;
			for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
			{
				(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
				path_export(fp,
					exportFormat, tlngP, &(tlngP->path[pathId]), indentDepth,
					(pathId < tlngP->numPathsClosed + tlngP->numPathsOpen - 1),
					numLinesThisFileP, numCharsThisFileP
				);  // path_export()
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathId
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "]");
		}  // if export Paths

		if( exportQ(rhombi, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, ",\n");
			(*numLinesThisFileP) ++;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "\"Rhombi\":[\n");
			(*numLinesThisFileP) ++;
			for(rhId=0;  rhId < tlngP->numFats + tlngP->numThins;  rhId++)
			{
				(*numCharsThisFileP) += fIndent(fp, 2 + indentDepth);
				rhombus_export( fp, exportFormat, tlngP,  &(tlngP->rhombi[rhId]),  (rhId < tlngP->numFats + tlngP->numThins - 1), numCharsThisFileP);
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // rhId

			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "]");
		}  // if exportRhombi

		if( exportQ(boundingPath, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, ",\n");
			(*numLinesThisFileP) ++;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "\"RhombiBoundingPath\":[\n");
			(*numLinesThisFileP) ++;
			tiling_export_PaintBoundary(fp,
				JSON,
				1.0 / tlngP->edgeLength,
				tlngP,
				2 + indentDepth,
				numLinesThisFileP,
				numCharsThisFileP
			);  // tiling_export_PaintBoundary()
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "]");
		}  // if boundingPath

		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, notLast ? "},\n" : "}\n");
		(*numLinesThisFileP) ++;

		break;  // from JSON  (inside tiling_export)


	case TSV:
		// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
		(*numCharsThisFileP) += fprintf(fp,
			"T_%02"   PRIi8 ".TilingId"       "\tT_%02" PRIi8 ".SeedType"     "\tT_%02" PRIi8 ".AxisAligned"    "\tT_%02" PRIi8 ".EdgeLength"
			"\tT_%02" PRIi8 ".NumFats"        "\tT_%02" PRIi8 ".NumThins"     "\tT_%02" PRIi8 ".NumThins_0T4F"  "\tT_%02" PRIi8 ".NumThins_1T3F"
			"\tT_%02" PRIi8 ".NumPathsClosed" "\tT_%02" PRIi8 ".NumPathsOpen" "\tT_%02" PRIi8 ".NumPathStats"   "\tT_%02" PRIi8 ".BoundingPathNumVertices"
			"\tT_%02" PRIi8 ".MinX_rhId"      "\tT_%02" PRIi8 ".MaxX_rhId"    "\tT_%02" PRIi8 ".MinY_rhId"      "\tT_%02" PRIi8 ".MaxY_rhId"
			"\tT_%02" PRIi8 ".MinX"           "\tT_%02" PRIi8 ".MaxX"         "\tT_%02" PRIi8 ".MinY"           "\tT_%02" PRIi8 ".MaxY"      "\tT_%02" PRIi8 ".RadiusMax"      "\tT_%02" PRIi8 ".RadiusShortOpen"
			"\tT_%02" PRIi8 ".DataAsOf"                "\tT_%02" PRIi8 ".SecondsToStartExportFromStartFirstTiling" "\tT_%02" PRIi8 ".SecondsToStartExportFromStartThisTiling" "\tT_%02" PRIi8 ".FileTimeString"
			"\tT_%02" PRIi8 ".PersistentSumSimple_malloc"
			"\tT_%02" PRIi8 ".InternalToC_EdgeLength"  "\tT_%02" PRIi8 ".Licence"                    "\tT_%02" PRIi8 ".URL"                        "\tT_%02" PRIi8 ".Author"
			"\tT_%02" PRIi8 ".WantedPostScriptCentreX" "\tT_%02" PRIi8 ".WantedPostScriptCentreY"    "\tT_%02" PRIi8 ".WantedPostScriptHalfWidth"  "\tT_%02" PRIi8 ".WantedPostScriptHalfHeight"
			"\tT_%02" PRIi8 ".WantedPostScriptAspect"  "\tT_%02" PRIi8 ".WantedPostScriptNumRhombi"  "\tT_%02" PRIi8 ".WantedPostScriptNumPaths"
			"\n",
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId
		);  // fprintf()
		(*numLinesThisFileP) ++;

		seed_type_name(tempStr,  tlngP->seedType);
		(*numCharsThisFileP) += fprintf(fp,
			"%" PRIi8 "\t%s"  "\t%s"  "\t1",
			tilingId,
			tempStr,
			tlngP->axisAligned ? "TRUE" : "FALSE"
		);  // fprintf()
		(*numCharsThisFileP) += fprintf(fp,
			"\t%li"    "\t%li"    "\t%li"    "\t%li"
			"\t%li"    "\t%li"    "\t%li"    "\t%lli"
			"\t%li"    "\t%li"    "\t%li"    "\t%li",
			tlngP->numFats,  tlngP->numThins,  tlngP->numThins_0T4F,  tlngP->numThins_1T3F,
			tlngP->numPathsClosed,  tlngP->numPathsOpen,  tlngP->numPathStats,  tlngP->boundingPathNumVertices,
			tlngP->xMin_rhId,  tlngP->xMax_rhId,  tlngP->yMin_rhId,  tlngP->yMax_rhId
		);  // fprintf()
		sprintf(scratchString,
			"\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf",
			tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength,
			tlngP->radiusMax                       / tlngP->edgeLength,
			tlngP->radiusShortOpen                 / tlngP->edgeLength
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		(*numCharsThisFileP) += fprintf(fp,
			"\t%04d-%02d-%02dT%02d:%02d:%02d"
			"\t%0.6lf"  "\t%0.6lf"   "\t%s"
			"\t%zu"
			"\t%.16E"   "\t%s"    "\t%s"    "\t%s",
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec),
			tlngP->SecondsToStartExportFromStartFirstTiling,  tlngP->SecondsToStartExportFromStartThisTiling,  tlngP->timeString,
			tlngP->mallocsPersistentSumSimple,
			tlngP->edgeLength,  TextLicence,  TextURL,  TextAuthor
		);  // fprintf()

		sprintf(scratchString,
			"\t%.9lf"  "\t%.9lf"  "\t%.9lf"  "\t%.9lf"
			"\t%.9lf"  "\t%li"  "\t%li",
			tlngP->wantedPostScriptCentre.x,
			tlngP->wantedPostScriptCentre.y,
			tlngP->wantedPostScriptHalfWidth,
			tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect,
			tlngP->wantedPostScriptAspect,  tlngP->wantedPostScriptNumRhombi,  tlngP->wantedPostScriptNumPaths
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s\n\n", scratchString);
		(*numLinesThisFileP) += 2;

		if( tlngP->numPathStats > 0  &&  exportQ(pathStats, exportFormat, tlngP, *numLinesThisFileP) )
		{
			pathStat_export(fp, exportFormat, tlngP, (PathStats *)NULL, indentDepth, false, numLinesThisFileP, numCharsThisFileP);  // NULL pointer to Rhombus means print just header, no data.
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			for(pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
			{
				pathStat_export(
					fp,
					exportFormat,
					tlngP,
					&(tlngP->pathStat[pathStatId]),
					indentDepth,
					(pathStatId < tlngP->numPathStats - 1),
					numLinesThisFileP, numCharsThisFileP
				);  // pathStat_export()
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathStatId
		}  // if ... exportQ(pathStats, ...)

		if( tlngP->numPathsClosed + tlngP->numPathsOpen > 0  &&  exportQ(paths, exportFormat, tlngP, *numLinesThisFileP) )
		{
			path_export(fp, exportFormat, tlngP, (Path *)NULL, indentDepth, false, numLinesThisFileP, numCharsThisFileP);  // NULL pointer to Path means print just header, no data.
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
			{
				path_export(fp,
					exportFormat, tlngP, &(tlngP->path[pathId]), indentDepth,
					(pathId < tlngP->numPathsClosed + tlngP->numPathsOpen - 1),
					numLinesThisFileP, numCharsThisFileP
				);  // pathStat_export()
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathId
		}  // if ... exportQ(Paths, ...)

		if( (tlngP->numThins > 0 || tlngP->numFats > 0)  &&  exportQ(rhombi, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			rhombus_export(fp, exportFormat, tlngP, (Rhombus *)NULL, false, numCharsThisFileP);  // NULL pointer to Rhombus means print just header, no data.
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			for(rhId=0;  rhId < tlngP->numFats + tlngP->numThins;  rhId++)
			{
				rhombus_export(fp, exportFormat, tlngP, &(tlngP->rhombi[rhId]), (rhId < tlngP->numFats + tlngP->numThins - 1), numCharsThisFileP);
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // rhId
		}  // if ... exportQ(Rhombi, ...)


		if( (tlngP->numThins > 0 || tlngP->numFats > 0)  &&  exportQ(boundingPath, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fprintf(fp, "\n");
			(*numLinesThisFileP) ++;
			tiling_export_PaintBoundary(
				fp,
				TSV,
				1.0 / tlngP->edgeLength,
				tlngP,
				0,
				numLinesThisFileP,
				numCharsThisFileP
			);  // tiling_export_PaintBoundary()
		}  // boundingPath

		(*numCharsThisFileP) += fprintf(fp, "\n\n\n");
		(*numLinesThisFileP) += 3;

		// One more than longest row, for ease of navigation within Excel.
		int i;
		for( i = 0  ;  i < 40  ;  i++ )
			(*numCharsThisFileP) += fprintf(fp,
				"%s# End tilingId=%02" PRIi8 " #",
				i>0?"\t":"",   tlngP->tilingId
			);  // fprintf()

		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++;

		break;  // from TSV  (inside tiling_export)

	}  // switch(exportFormat)


	fflush(fp);
}  // tiling_export
