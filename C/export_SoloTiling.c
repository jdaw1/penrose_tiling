// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_SoloTiling.c, in PenroseC

#include "penrose.h"

void export_soloTiling(
	Tiling * const tlngP,
	clock_t const timeBeginDescendant
)
{
	ExportFormat ef;
	FILE * fp;
	char fileName[256];
	char fileExtension[16];
	unsigned long int numLinesThisFile = 0;
	unsigned long long int numCharsThisFile = 0;
	extern char scratchString[];

	// SVG, PS_rhomb, and PS_arcs all work one tiling at a time. So it can export earlier
	// than the others. And because of the possibility that the last few tilings are very very
	// slow to make, worth exporting these as they become ready.

	rhombi_sort(tlngP,  &rhombiGt_ByY,  true);

	ef = SVG_rhomb;
	if( exportQ(Anything, ef, tlngP, numLinesThisFile) )
	{
		sprintf(fileName,
			"%sPenrose_%s_Rh_%02" PRIi8 ".%s",
			tlngP->filePath,  tlngP->timeString,  tlngP->tilingId,  fileExtension_from_ExportFormat(fileExtension, ef)
		);
		numLinesThisFile = 0;
		numCharsThisFile = 0;
		fp=fopen(fileName, "w");
		tiling_export_PaintRhombiSVG(fp, tlngP, &numLinesThisFile, &numCharsThisFile);
		fflush(fp); fclose(fp);
		printf(
			"export_soloTiling(): tilingId=%" PRIi8 ", exported %lli chars %li lines, so %0.1lf c/l, to %s\n",
			tlngP->tilingId,  numCharsThisFile,  numLinesThisFile,  (double)numCharsThisFile / (double)numLinesThisFile, fileName
		);  fflush(stdout);

		execute_SVG_PostProcessing(tlngP, numLinesThisFile, numCharsThisFile, fileName, ef);
	}  // if exportQ(), SVG_rhomb


	ef = SVG_arcs;
	if( exportQ(Anything, ef, tlngP, numLinesThisFile) )
	{
		sprintf(fileName,
			"%sPenrose_%s_Arcs_%02" PRIi8 ".%s",
			tlngP->filePath,  tlngP->timeString,  tlngP->tilingId,  fileExtension_from_ExportFormat(fileExtension, ef)
		);
		numLinesThisFile = 0;
		numCharsThisFile = 0;
		fp=fopen(fileName, "w");
		tiling_export_PaintArcsSVG(fp, tlngP, &numLinesThisFile, &numCharsThisFile);
		fflush(fp); fclose(fp);
		printf(
			"export_soloTiling(): tilingId=%" PRIi8 ", exported %lli chars %li lines, so %0.1lf c/l, to %s\n",
			tlngP->tilingId,  numCharsThisFile,  numLinesThisFile,  (double)numCharsThisFile / (double)numLinesThisFile, fileName
		);  fflush(stdout);

		execute_SVG_PostProcessing(tlngP, numLinesThisFile, numCharsThisFile, fileName, ef);
	}  // if( exportQ() ), SVG_arcs


	ef = PS_rhomb;
	if( exportQ(Anything, ef, tlngP, numLinesThisFile) )
	{
		rhombi_sort(tlngP,  &rhombiGt_ByPath,  true);
		numCharsThisFile = 0;
		numLinesThisFile = 0;

		sprintf(fileName,
			"%sPenrose_%s_Rh_%02" PRIi8 ".%s",
			tlngP->filePath,  tlngP->timeString,  tlngP->tilingId,  fileExtension_from_ExportFormat(fileExtension, ef)
		);
		numLinesThisFile = 0;
		numCharsThisFile = 0;
		fp=fopen(fileName, "w");
		tiling_export_PaintRhombiPS(fp, tlngP, &numLinesThisFile, &numCharsThisFile, false);
		fflush(fp); fclose(fp);
		printf(
			"export_soloTiling(): tilingId=%" PRIi8 ", exported %lli chars %li lines, so %0.1lf c/l, to %s\n",
			tlngP->tilingId,  numCharsThisFile,  numLinesThisFile,  (double)numCharsThisFile / (double)numLinesThisFile, fileName
		);  fflush(stdout);

		execute_PostScript_PostProcessing(tlngP, numLinesThisFile, numCharsThisFile, fileName, ef);
	}  // if( exportQ() ), PS_rhomb


	ef = PS_arcs;
	if( exportQ(Anything, ef, tlngP, numLinesThisFile) )
	{
		rhombi_sort(tlngP,  &rhombiGt_ByPath,  true);
		sprintf(
			fileName,  "%sPenrose_%s_Arcs_%02" PRIi8 ".%s",
			tlngP->filePath,  tlngP->timeString,  tlngP->tilingId,  fileExtension_from_ExportFormat(fileExtension, ef)
		);
		numLinesThisFile = 0;
		numCharsThisFile = 0;
		fp=fopen(fileName, "w");
		tiling_export_PaintArcsPS(fp, tlngP, &numLinesThisFile, &numCharsThisFile);
		fflush(fp); fclose(fp);
		printf(
			"export_soloTiling(): tilingId=%" PRIi8 ", exported %lli chars %li lines, so %0.1lf c/l, to %s\n",
			tlngP->tilingId,  numCharsThisFile,  numLinesThisFile,  (double)numCharsThisFile / (double)numLinesThisFile, fileName
		);  fflush(stdout);

		execute_PostScript_PostProcessing(tlngP, numLinesThisFile, numCharsThisFile, fileName, ef);
	}  // if( exportQ() ), PS_arcs

}  // export_soloTiling()
