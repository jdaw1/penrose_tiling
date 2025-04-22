// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportTiling.c, in PenroseC

#include "penrose.h"

void tiling_export(
	FILE* const fp,
	ExportFormat        const exportFormat,
	bool exportQ(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFileP),
	Tiling            * const tlngP,
	int                 const indentDepth,
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

	if( ! exportQ(Anything, exportFormat, tlngP, *numLinesThisFileP) )
		return;

	switch(exportFormat)
	{
	case PS_rhomb:
	case PS_arcs:
	case SVG_rhomb:
	case SVG_arcs:
		break;  // Not applicable here.

	case PS_data:
		tlngP->wantedPostScriptRhombNum = malloc( (tlngP->numFats + tlngP->numThins) * sizeof(RhombId) );
		if( NULL == tlngP->wantedPostScriptRhombNum )
		{
			fprintf(stderr, "tiling_export(): error, tilingId=%hi, malloc() failure, for wantedPostScriptRhombNum. Not outputting PS_data.\n", tlngP->tilingId);
			fflush(stderr);
			return ; // Not exit()ing
		}

		tlngP->wantedPostScriptPathNum = malloc( (tlngP->numPathsClosed + tlngP->numPathsOpen) * sizeof(PathId) );
		if( NULL == tlngP->wantedPostScriptPathNum )
		{
			fprintf(stderr, "tiling_export(): error, tilingId=%hi, malloc() failure, for wantedPostScriptPathNum. Not outputting PS_data.\n", tlngP->tilingId);
			fflush(stderr);
			if( NULL != tlngP->wantedPostScriptRhombNum )
				free(tlngP->wantedPostScriptRhombNum);
			return ; // Not exit()ing
		}

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
			tlngP->wantedPostScriptNumberRhombi, tlngP->wantedPostScriptNumberPaths
		);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "/DataAsOf (D:%04d%02d%02d%02d%02d%02d)   /Licence (%s)  /URL (%s)  /Author (%s)  /TilingId %" PRIi8 "   /NumFats %li   /NumThins %li",
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec),
			TextLicence, TextURL, TextAuthor,
			tilingId,  tlngP->numFats,  tlngP->numThins
		);
		sprintf(scratchString, "/EdgeLength %.16E", tlngP->edgeLength);  // Extra precision
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "  %s  /AxisAligned %s\n",  scratchString, tlngP->axisAligned ? "true" : "false");
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString, "/MinX %.9f   /MaxX %.9f   /MinY %.9f   /MaxY %.9f",
			tlngP->xMin,
			tlngP->xMax,
			tlngP->yMin,
			tlngP->yMax
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s\n", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString,
			"/wantedPostScriptCentreX %.9f   /WantedPostScriptCentreY %.9f   /WantedPostScriptAspect %.9f   /WantedPostScriptHalfWidth %.9f   /WantedPostScriptHalfHeight %.9f",
			tlngP->wantedPostScriptCentre.x,  tlngP->wantedPostScriptCentre.y,  tlngP->wantedPostScriptAspect,  tlngP->wantedPostScriptHalfWidth,  tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s\n", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"/wantedPostScriptNumberRhombi %li   /wantedPostScriptNumberPaths %li  %% [ WhollyOutside RhombOutPathIn RhombPartlyIn RhombWhollyIn ]\n",
			tlngP->wantedPostScriptNumberRhombi,  tlngP->wantedPostScriptNumberPaths
		);
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

		if(exportQ(Paths, exportFormat, tlngP, *numLinesThisFileP) )
		{
			pathsNumToBeOutput = tlngP->numPathsClosed + tlngP->numPathsOpen < 65535 ? tlngP->numPathsClosed + tlngP->numPathsOpen : 65535 ;
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/Paths %li array  %% + /Paths: tilingId=%" PRIi8 ", numPaths=%li, numPathsOutput=%li",
				pathsNumToBeOutput, tlngP->tilingId, tlngP->numPathsClosed + tlngP->numPathsOpen, pathsNumToBeOutput
			);
			if( tlngP->numPathStats > 0  &&  tlngP->pathStat[0].pathClosed  &&  tlngP->pathStat[0].numPaths > 0 )
				(*numCharsThisFileP) += fprintf(fp,
					", the longest closed path being of length %li, of which there %s %li.\n",
					tlngP->pathStat[0].pathLength,  tlngP->pathStat[0].numPaths > 1 ? "are" : "is only",  tlngP->pathStat[0].numPaths
				);
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
				);
				if( tlngP->numPathStats > 0  &&  tlngP->pathStat[0].pathClosed  &&  tlngP->pathStat[0].numPaths > 0 )
					(*numCharsThisFileP) += fprintf(fp,
						", the longest closed path being of length %li, of which there %s %li.\n",
						tlngP->pathStat[0].pathLength,  tlngP->pathStat[0].numPaths > 1 ? "are" : "is only",  tlngP->pathStat[0].numPaths
					);
				else
					(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // if( numPaths > 0 )
		}  // if exportPaths


		if( exportQ(Rhombi, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp,
				"/Rhombi %li array  %% + /Rhombi: tilingId=%" PRIi8 ", numFats=%li, numThins=%li, wantedPostScriptNumberRhombi=%li, wantedPostScriptNumberPaths=%li\n",
				tlngP->wantedPostScriptNumberRhombi,
				tlngP->tilingId, tlngP->numFats, tlngP->numThins,
				tlngP->wantedPostScriptNumberRhombi,  tlngP->wantedPostScriptNumberPaths
			);
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
				"numPathsOpen=%li, wantedPostScriptNumberRhombi=%li, wantedPostScriptNumberPaths=%li\n",
				tilingId,  tlngP->numFats,  tlngP->numThins, tlngP->numPathsClosed, tlngP->numPathsOpen,
				tlngP->wantedPostScriptNumberRhombi, tlngP->wantedPostScriptNumberPaths
			);
			(*numLinesThisFileP) ++;
			(*numCharsThisFileP) += fprintf(fp,
				"/Licence (%s)\n"
				"/URL (%s)\n"
				"/Author (%s)\n",
				TextLicence, TextURL, TextAuthor
			);
			(*numLinesThisFileP) += 3;
		}  // if exportRhombi

		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, ">>  %% - tilingId=%" PRIi8 "\n", tilingId);
		(*numLinesThisFileP) ++;
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"(After ) usertime usertimeStart sub 1000 div 12 string cvs Concatenate ( seconds execution, interpreted tilingId=%" PRIi8 ", containing %li rhombi) Concatenate = flush\n",
			tilingId, tlngP->wantedPostScriptNumberRhombi
		);
		(*numLinesThisFileP) ++;

		if( tlngP->wantedPostScriptRhombNum != NULL )
		{
			free(tlngP->wantedPostScriptRhombNum);
			tlngP->wantedPostScriptRhombNum = NULL;
		}
		else
		{
			fprintf(stderr, "\n\ntiling_export(): error. Weirdly, wantedPostScriptRhombNum == NULL. Continuing.\n\n\n");
			fflush(stderr);
		}
		if( tlngP->wantedPostScriptPathNum != NULL )
		{
			free(tlngP->wantedPostScriptPathNum);
			tlngP->wantedPostScriptPathNum = NULL;
		}
		else
		{
			fprintf(stderr, "\n\ntiling_export(): error. Weirdly, wantedPostScriptPathNum == NULL. Continuing.\n\n\n");
			fflush(stderr);
		}

		break;  // from PS  (inside tiling_export)



	case JSON:
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "{\n" );
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"\"DataAsOf\":\"%04d-%02d-%02dT%02d:%02d:%02d\",  \"TilingId\":%" PRIi8   // Time local, not necessarily UTC, hence no trailing Z.
			",   \"NumFats\":%li,   \"NumThins\":%li"
			",   \"NumPathsClosed\":%li,   \"NumPathsOpen\":%li",
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  tlngP->timeData->tm_sec,
			tilingId,  tlngP->numFats,  tlngP->numThins,  tlngP->numPathsClosed,  tlngP->numPathsOpen
		);
		sprintf(scratchString, ",  \"EdgeLength\":%.16E", tlngP->edgeLength);  // Extra precision
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp,
			"%s,  \"AxisAligned\":%s,"  "\n",
			scratchString, tlngP->axisAligned ? "true" : "false"
		);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString,
			"\"MinX\":%.12f,   \"MaxX\":%.12f,   \"MinY\":%.12f,   \"MaxY\":%.12f,"  "\n",
			tlngP->xMin, tlngP->xMax, tlngP->yMin, tlngP->yMax
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		sprintf(scratchString,
			"\"WantedPostScriptCentreX\":%.12f,  \"WantedPostScriptCentreY\":%.12f,  \"WantedwantedPostScriptAspect\":%.12f,  \"WantedPostScriptHalfWidth\":%.12f,   \"wantedPostScriptHalfHeight\":%.12f,"  "\n",
			tlngP->wantedPostScriptCentre.x,  tlngP->wantedPostScriptCentre.y,
			tlngP->wantedPostScriptAspect,  tlngP->wantedPostScriptHalfWidth,  
			tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) ++;

		(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"\"WantedPostScriptNumberRhombi\":%li,   \"WantedPostScriptNumberPaths\":%li,"  
			"\"Licence\":\"%s\",   \"URL\":\"%s\",  \"Author\":\"%s\","  "\n",
			tlngP->wantedPostScriptNumberRhombi,  tlngP->wantedPostScriptNumberPaths,
			TextLicence, TextURL, TextAuthor
		);
		(*numLinesThisFileP) ++;

		if( exportQ(pathStats, exportFormat, tlngP, *numLinesThisFileP) )
		{
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "\"PathStats\":[\n");
			(*numLinesThisFileP) ++;
			for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
				pathStat_export(fp, exportFormat, tlngP, &(tlngP->pathStat[pathStatId]), indentDepth, (pathStatId < tlngP->numPathStats - 1), numLinesThisFileP, numCharsThisFileP);
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "],\n");
			(*numLinesThisFileP) ++;
		}  // if exportpathStats

		if( exportQ(Paths, exportFormat, tlngP, *numLinesThisFileP) )
		{
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
				);
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathId
			(*numCharsThisFileP) += fIndent(fp, 1 + indentDepth);
			(*numCharsThisFileP) += fprintf(fp, "],\n");
			(*numLinesThisFileP) ++;
		}  // if exportPaths

		if( exportQ(Rhombi, exportFormat, tlngP, *numLinesThisFileP) )
		{
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
			(*numCharsThisFileP) += fprintf(fp, "]\n");
			(*numLinesThisFileP) ++;
		}  // if exportRhombi

		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, notLast ? "},\n" :  "}\n");
		(*numLinesThisFileP) ++;

		break;  // from JSON  (inside tiling_export)


	case TSV:
		// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
		(*numCharsThisFileP) += fprintf(fp,
			"T_%02" PRIi8 ".TilingId"                  "\tT_%02" PRIi8 ".AxisAligned"             "\tT_%02" PRIi8 ".EdgeLength"
			"\tT_%02" PRIi8 ".NumFats"                 "\tT_%02" PRIi8 ".NumThins"                "\tT_%02" PRIi8 ".NumPathsClosed"         "\tT_%02" PRIi8 ".NumPathsOpen"         "\tT_%02" PRIi8 ".NumPathStats"
			"\tT_%02" PRIi8 ".MinX"                    "\tT_%02" PRIi8 ".MaxX"                    "\tT_%02" PRIi8 ".MinY"                   "\tT_%02" PRIi8 ".MaxY"
			"\tT_%02" PRIi8 ".WantedPostScriptCentreX" "\tT_%02" PRIi8 ".WantedPostScriptCentreY" "\tT_%02" PRIi8 ".WantedPostScriptAspect" "\tT_%02" PRIi8 ".WantedPostScriptHalfWidth" "\tT_%02" PRIi8 ".WantedPostScriptHalfHeight"
			"\tT_%02" PRIi8 ".WantedPostScriptNumberRhombi"   "\tT_%02" PRIi8 ".WantedPostScriptNumberPaths"
			"\tT_%02" PRIi8 ".Licence" "\tT_%02" PRIi8 ".URL" "\tT_%02" PRIi8 ".Author" "\n",
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId,
			tlngP->tilingId, tlngP->tilingId, tlngP->tilingId
		);
		(*numLinesThisFileP) ++;
		sprintf(scratchString,
			"%" PRIi8      "\t%s"     "\t%.16E"
			"\t%li"    "\t%li"    "\t%li"    "\t%li"    "\t%li"
			"\t%.12f"  "\t%.12f"  "\t%.12f"  "\t%.12f"
			"\t%.12f"  "\t%.12f"  "\t%.12f"  "\t%.12f"  "\t%.12f",
			tilingId,  tlngP->axisAligned ? "TRUE" : "FALSE",  tlngP->edgeLength,
			tlngP->numFats, tlngP->numThins, tlngP->numPathsClosed, tlngP->numPathsOpen, tlngP->numPathStats,
			tlngP->xMin, tlngP->xMax, tlngP->yMin, tlngP->yMax,
			tlngP->wantedPostScriptCentre.x, tlngP->wantedPostScriptCentre.y, tlngP->wantedPostScriptAspect, tlngP->wantedPostScriptHalfWidth, tlngP->wantedPostScriptHalfWidth * tlngP->wantedPostScriptAspect
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numCharsThisFileP) += fprintf(fp,
			"\t%li"    "\t%li   \t%s"    "\t%s"    "\t%s"   "\n\n",
			tlngP->wantedPostScriptNumberRhombi, tlngP->wantedPostScriptNumberPaths,
			TextLicence, TextURL, TextAuthor  // Do not stringClean() these, as that would mess with dots.
		);
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
				);
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathStatId
		}  // if ... exportQ(pathStats, ...)

		if( tlngP->numPathsClosed + tlngP->numPathsOpen > 0  &&  exportQ(Paths, exportFormat, tlngP, *numLinesThisFileP) )
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
				);
				(*numCharsThisFileP) += fprintf(fp, "\n");
				(*numLinesThisFileP) ++;
			}  // pathId
		}  // if ... exportQ(Paths, ...)

		if( (tlngP->numThins > 0 || tlngP->numFats > 0)  &&  exportQ(Rhombi, exportFormat, tlngP, *numLinesThisFileP) )
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

		(*numCharsThisFileP) += fprintf(fp, "\n\n\n");
		(*numLinesThisFileP) += 3;

		// One more than longest row, for ease of navigation within Excel.
		int i;
		for( i = 0  ;  i < 32  ;  i++ )
			(*numCharsThisFileP) += fprintf(fp, "%s# #%02" PRIi8 " End tilingId=%02" PRIi8 " End #%02" PRIi8 " #",  i>0?"\t":"",  tlngP->tilingId,  tlngP->tilingId,  tlngP->tilingId);

		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++;

		break;  // from TSV  (inside tiling_export)

	}  // switch(exportFormat)


	fflush(fp);
}  // tiling_export
