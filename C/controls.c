// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, September 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// main.c, in PenroseC

#include "penrose.h"

// Change to match your directory structure, and location of applications.
static char const filePath_staticConst[] = "/Volumes/JDAW_A/Documents/programming/C/Penrose_C/Output/";

/*
	What to output? Lots of Booleans.
	But clauses resembling "if(false)" cause compiler warnings about code never being executed.
	So instead there are placeholder conditions, such as:
		if( tlngP->tilingId >= -1 )  // always true
		if( tlngP->tilingId <= -1 )  // always false
	which can easily be changed by a user, either to the other one, or to something non-constant.
*/

// Location and name of application should match your machine's arrangement. Or can become empty do-nothing.
void execute_SVG_PostProcessing(
	const Tiling * const tlngP,
	const unsigned long int numLinesThisFile,
	const unsigned long long int numCharsThisFile,
	const char * const fileName,
	ExportFormat const ef
)
{
	if( tlngP->tilingId >= -1 )  // Placeholder condition, as an explicit constant causes a compiler warning.
	{
		extern char scratchString[];

		if( ef == SVG_arcs  ||  numCharsThisFile < 10737418240 )  // 10 MiB. Limit very different for arcs and rhombi. https://issues.chromium.org/issues/390969197
		{
			// Change next line to invoke your preferred browser or SVG viewer.
			sprintf(scratchString, "open -a '/Applications/Google Chrome.app' 'file://%s'", fileName);  // Obvs., change for non-macOS or non-Chrome.
			
			// In general, system() should be seen as a serious security risk.
			// If unable to be sure of safety, delete next two lines.
			if( system(scratchString) != 0 )
				printf("\nWarning: system(\"%s\") returned non-0. Continuing.\n", scratchString);
		}  // Not huge rhombii
	}  // AnyPostProcessing
}  // execute_SVG_PostProcessing()

// Location and name of application should match your machine's arrangement. Or can become empty do-nothing.
void execute_PostScript_PostProcessing(
	const Tiling * const tlngP,
	const unsigned long int numLinesThisFile,
	const unsigned long long int numCharsThisFile,
	const char * const fileName,
	ExportFormat const ef
)
{
	if(
		tlngP->tilingId >= -1  // Placeholder condition
		&&  (ef == PS_rhomb || ef == PS_arcs)
		&&  tlngP->numFats + tlngP->numThins <= 393216  // 3 * 2^17
	)
	{
		extern char scratchString[];

		// Change following to invoke your preferred PS-to-PDF application, likely either Distiller or GhostScript.
		char fileNamePDF[1024];
		filename_new_extension(fileName, fileNamePDF, "pdf");
		sprintf(scratchString,
			"/opt/homebrew/bin/ps2pdf"          // GhostScript.
			" -dAutoRotatePages=/None"          // Don't auto-rotate pages.
			" -q"                               // Don't send PostScript log back to XCode.
			" -dNOPAUSE"                        // Don't wait for pointless user action.
			" '%s' '%s'"                        // Files.
			" > /dev/null 2>&1"                 // More hush!
			" ;  open -a Preview '%s'",         // Then open PDF in Preview (obvs., change for non-macOS).
			fileName, fileNamePDF, fileNamePDF  // Files: GhostScript in; GhostScript out; Preview in.
		);  // sprintf()

		// In general, system() should be seen as a serious security risk.
		// If unable to be sure of safety, delete next two lines.
		if( system(scratchString) != 0 )
			printf("\nWarning: system(\"%s\") returned non-0. Continuing.\n", scratchString);
	}  // Post-process PostScript
}  // execute_PostScript_PostProcessing()



// User-changeable function: what should be output? Can matter because output can be very big.
bool exportQ(ExportWhat const exprtWhat,  ExportFormat const exportFormat,  const Tiling * const tlngP,  const unsigned long int numLinesThisFile)
{
	long int const ExcelMaxNumRows = 1048576;  // Maximum number of rows in Excel versions from 2013 until at least 2026.

	switch(exportFormat)
	{
	case JSON:
		return( tlngP->tilingId >= -1 );  // Placeholder condition.

	case PS_data:
	{
		const TilingId tilingId_farEnough = 14;  // At 14==tilingId, bounding path has ~=16k points, so array of length ~=48k.
		return(
			tlngP->tilingId >= -1  // Placeholder condition.
			&& (
				tlngP->tilingId == min_2(tilingId_farEnough,  tlngP->numTilings - 1)
				||  exprtWhat != boundingPath
			)
		);  // return()
	}

	case PS_rhomb:
		return(
			tlngP->tilingId >= -1  // Placeholder condition.
			&&  tlngP->tilingId <= 12
		);  // return()

	case PS_arcs:
		return(
			tlngP->tilingId >= -1  // Placeholder condition.
			&&  tlngP->tilingId <= 12
		);  // return()

	case SVG_rhomb:
		if( tlngP->tilingId >= -1  )  // Placeholder condition.
		{
			switch(exprtWhat)
			{
			case anything:
			case rhombi:
				return true;
			case boundingPath:
				return false;  // different to SVG_arcs

			case pathStats:
			case paths:
				return false;  // This never reached; included to quieten compiler grumbling.
			}  // switch(exprtWhat)
		}  // if()
		else
			return false;

	case SVG_arcs:
		if( tlngP->tilingId >= -1  )  // Placeholder condition.
		{
			switch(exprtWhat)
			{
			case anything:
			case rhombi:
			case boundingPath:
				return true;

			case pathStats:
			case paths:
				return false;  // This never reached; included to suppress compiler grumbling.
			}  // switch(exprtWhat)
		}
		else
			return false;

	case TSV:
		if( tlngP->tilingId >= -1 )  // Placeholder condition.
			switch(exprtWhat)
			{
			case anything:
				return( 18 + numLinesThisFile < ExcelMaxNumRows );
			case pathStats:
				return( 10 + numLinesThisFile + tlngP->numPathStats                         < ExcelMaxNumRows );
			case paths:
				return( 10 + numLinesThisFile + tlngP->numPathsClosed + tlngP->numPathsOpen < ExcelMaxNumRows - 10240 );  // Leaving space for subsequent pathStats
			case rhombi:
				return( 10 + numLinesThisFile + tlngP->numFats + tlngP->numThins            < ExcelMaxNumRows - 10240 );  // Leaving space for subsequent pathStats
			case boundingPath:
				return( 10 + numLinesThisFile + 5 * sqrt(tlngP->numFats + tlngP->numThins)  < ExcelMaxNumRows - 10240 );  // Leaving space for subsequent pathStats
			}  // switch(exprtWhat)
		else
			return false;


		break;  // Redundant
	}  // switch(exportFormat)

	return false;  // Redundant
}  // exportQ()



// This test was used to make documentation that shows effect of holesFill().
// But in production, want holesFill() always.
bool holesFillQ(const Tiling * const tlngP)
{
	return true;  // Example alternative:   return (tlngP->tilingId <= 2);
}  // holesFillQ()



// These can be changed to show, in SVG, only a subset of the rhombi.
// Comments have example, as used to make image in documentation.
// These y values are as generated by C code; SVG's y values are -1* these.-
// For the bounding box used in the introductory documentation change the next four "-9"s to "9".
const double svg_toPaint_xMin(const Tiling * const tlngP) {return  tlngP->tilingId == -9 ? -21   : -999999;}
const double svg_toPaint_xMax(const Tiling * const tlngP) {return  tlngP->tilingId == -9 ?  42   : +999999;}
const double svg_toPaint_yMin(const Tiling * const tlngP) {return  tlngP->tilingId == -9 ?  59.4 : -999999;}
const double svg_toPaint_yMax(const Tiling * const tlngP) {return  tlngP->tilingId == -9 ?  95.9 : +999999;}
const double svg_displayWidth(const Tiling * const tlngP) {return 960;}  // GitHub seems to use a 1012px column. This that, less a little wiggle room for margin and padding.
const double svg_strokeWidth(const Tiling * const tlngP) {return 1.0 / 16;}
const bool  showGridlines(const Tiling * const tlngP, ExportFormat const ef) {return tlngP->tilingId == -9;}  // If false, gridline code still in the SVG, but commented out.

// These can be changed to output, in the PostScript non-data output, fewer rhombi.
// Purpose is lessening file size. Does not affect counting, nor paths, nor even output of boundary.
const double postScript_toPaint_xMin(const Tiling * const tlngP) {return -999999;}
const double postScript_toPaint_xMax(const Tiling * const tlngP) {return +999999;}
const double postScript_toPaint_yMin(const Tiling * const tlngP) {return -999999;}
const double postScript_toPaint_yMax(const Tiling * const tlngP) {return +999999;}

const long int svg_arcs_longestPathToBeColoured(void)
{
	// If enlarging this might need to add more colours to "<style>" in tiling_export_PaintArcsSVG().
	return 215;
}  // svg_arcs_longestPathToBeColoured



XY wantedPostScriptCentre(Tiling const * const tlngP)  // in units that assume unit edgeLength
{
	const PathStatId pathStatId = 0;
	double rMin;
	return (XY){
		.x = (
			tlngP->numPathStats > pathStatId  &&
			tlngP->pathStat[pathStatId].pathClosed  &&
			(rMin = tlngP->pathStat[pathStatId].radiusMin) < tlngP->radiusMax / 2
		) ? rMin / tlngP->edgeLength :  0,
		.y = 0
	};
}  // wantedPostScriptCentre()

double wantedPostScriptAspect(Tiling const * const tlngP)
{
	return 1.44;  // Portrait A3, less margins of 8.73mm ~= 0.3437 inch: (420 - 2*8.73) / (297 - 2*8.73) ~= 1.44001
}  // wantedPostScriptAspect()

bool rhombus_keep(
	const Tiling * const tlngP,  Physique const physique,
	double const xNorth,  double const yNorth,  double const xSouth,  double const ySouth
)
{
	return true;
	// Example alternative:  return xNorth > (-1 * tlngP->edgeLength);
	// Purpose; consider tiling a region with aspect ratio very very different to 1, such as a road.
	// Hence might be much more efficient to keep only a particular subset of tiles.
}  // rhombus_keep


void file_names_timeString_set(char * const timeString, const struct tm * const timeData)
{
	// My compiler, Xcode 26.2 (17C52), does not warn about this, I think rightly.
	// But your compiler might compile-time evaluate, then grumble that some code is never reached.
	// If so, rather than using a placeholder condition, comment out the test and the unwanted branch.
	if( 2 > 1 )  // Placeholder condition
		sprintf(timeString,  "_%04d%02d%02d_%02d%02d%02d",
			(1900 + timeData->tm_year),  (1 + timeData->tm_mon),  timeData->tm_mday,
			timeData->tm_hour,  timeData->tm_min,  (int)(timeData->tm_sec)
		);  // sprintf()
	else
		sprintf(timeString, "");
}  // file_names_timeString_set()



// Wrappers
const char * filePath(void)
{
	return filePath_staticConst;
}  // filePath()


SeedType initial_Tiling_Layout(void)
{
	// Possibilities: round5, oneThin, oneFat
	return( round5 );
}  // initialTilingLayout()
