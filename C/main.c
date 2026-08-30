// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// main.c, in PenroseC

#include "penrose.h"

// Hard-wired contraint to catch CPU-expensive mistyping. Limit sensible for 16 GiB machine.
static int8_t const numTilings_Max = 15;

char scratchString[scratchStringLength] ;  // Mostly used for post-processing of doubles: removal of trailing 0s and trailing decimal points. Size harmlessly generous: 6k would have been sufficient, the boundary need being the preamble in tiling_export_PaintRhombiPS.

int main(void)
{
	/* Seed thin rhombus. No need to change. */
	int8_t numTilings;

	// User-changeable constants above.

	Tiling *tlngs;
	FILE *fp;
	TilingId tilingId;
	time_t prog_start;
	struct tm *timeData;
	char timeString[250], fileName[256], fileExtension[16];
	long int pathStatNum,  numPathStatsClosed;

	printf(
		// https://stackoverflow.com/questions/15610053/correct-printf-format-specifier-for-size-t-zu-or-iu
		"main(): "
		"sizeof(Physique)=%zu;  sizeof(XY)=%zu;  sizeof(Neighbour)=%zu;  sizeof(Rhombus)=%zu;  sizeof(Path)=%zu;  sizeof(PathStats)=%zu;  sizeof(Tiling)=%zu\n\n",
		sizeof( Physique),      sizeof(XY),      sizeof(Neighbour),      sizeof(Rhombus),      sizeof(Path),      sizeof(PathStats),      sizeof(Tiling)
	);  // printf()

	printf(
		"\n\a\n"  // Bell sound! Doesn't work on my Mac -- maybe your system is better.
		"What is to be the recursion depth = numTilings?\n"
		"For testing choose in range 8 to 11.\n"
		"By memory constraint on a 16GiB machine, maximum is 13, run time being, on author's computer, about 2.5 hours. Obviously, YMMV.\n"
	);  fflush(stdout);
	do
	{
		printf("Compile-time constraint: must be >=1 && <=%" PRIi8 ".\n", (int8_t)numTilings_Max);  fflush(stdout);
		fscanf(stdin, "%" SCNi8, &numTilings);
	}
	while( numTilings < 1  ||  numTilings > numTilings_Max );

	printf("\nFrom stdin: numTilings = %" PRIi8 ", hence tilingId = 0 to %" PRIi8 ".\n\n\n",  numTilings,  numTilings - 1);


	tlngs = malloc( numTilings * sizeof(Tiling) );
	if( NULL == tlngs )
	{
		fprintf(stderr, "main(): !!! NULL == tlngs !!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // if( NULL == tlngs )

	time(&prog_start);
	timeData = localtime(&prog_start);
	file_names_timeString_set(timeString, timeData);

	clock_t const timeBeginConstruction = clock();

	for( tilingId = 0;  tilingId < numTilings;  tilingId++ )
	{
		tlngs[tilingId].populated                = false;
		tlngs[tilingId].tilingId                 = tilingId;
		tlngs[tilingId].numTilings               = numTilings;
		tlngs[tilingId].edgeLength               = 0;
		tlngs[tilingId].rhombi_NumMax            = 0;
		tlngs[tilingId].path_NumMax              = 0;
		tlngs[tilingId].pathStats_NumMax         = 0;
		tlngs[tilingId].numFats                  = 0;
		tlngs[tilingId].numThins                 = 0;
		tlngs[tilingId].numPathsClosed           = 0;
		tlngs[tilingId].numPathsOpen             = 0;
		tlngs[tilingId].numPathStats             = 0;
		tlngs[tilingId].xMin_rhId                = (RhombId) 0;
		tlngs[tilingId].xMax_rhId                = (RhombId) 0;
		tlngs[tilingId].yMin_rhId                = (RhombId) 0;
		tlngs[tilingId].yMax_rhId                = (RhombId) 0;
		tlngs[tilingId].rhombi                   = NULL;
		tlngs[tilingId].path                     = NULL;
		tlngs[tilingId].pathStat                 = NULL;
		tlngs[tilingId].wantedPostScriptRhombNum = NULL;
		tlngs[tilingId].wantedPostScriptPathNum  = NULL;
		tlngs[tilingId].filePath                 = filePath();
		tlngs[tilingId].timeData                 = timeData;
		tlngs[tilingId].timeString               = timeString;
	}  // for( tilingId ... )

	printf(
		"main(): filePath = \"%s\", timeString = \"%s\"; about to create tilings.\n",
		tlngs[0].filePath,  tlngs[0].timeString
	);  fflush(stdout);
	for( tilingId = 0;  tilingId < numTilings;  tilingId++ )
	{
		if( tilingId == 0 )
		{
			tlngs[tilingId].seedType = initial_Tiling_Layout();
			tiling_initial( &(tlngs[tilingId]) );
		}
		else
			tiling_descendant( &(tlngs[tilingId]), &(tlngs[tilingId - 1]) );

		PathStatId pathStatId;
		long int longestPathClosed = 0, longestPathOpen = 0;
		for( pathStatId = 0  ;  pathStatId < tlngs[tilingId].numPathStats  ;  pathStatId++ )
			if( tlngs[tilingId].pathStat[pathStatId].pathClosed )
			{
				if(longestPathClosed < tlngs[tilingId].pathStat[pathStatId].pathLength)
					longestPathClosed = tlngs[tilingId].pathStat[pathStatId].pathLength;
			}  // Closed
			else
			{
				if(longestPathOpen < tlngs[tilingId].pathStat[pathStatId].pathLength)
					longestPathOpen = tlngs[tilingId].pathStat[pathStatId].pathLength;
			}  // Open

		printf("main(): about to tilings_export().\n");  fflush(stdout);

		unsigned long int numLinesThisFile;
		unsigned long long numCharsThisFile;
		bool anythingToExport;
		long int tilingIdTest;
		ExportFormat ef;
		int ef_num;

		// If ExportFormat acquires other possibilites, give attention to the next two lines, which define exportFormat[] and numExportFormats.
		const ExportFormat exportFormat[] = {TSV, PS_data, JSON};
		const int numExportFormats = 3;

		for( ef_num = 0  ;  ef_num < numExportFormats  ; ef_num++ )
		{
			ef = exportFormat[ef_num];

			numLinesThisFile = 0;
			numCharsThisFile = 0;

			anythingToExport = false;
			for( tilingIdTest = 0  ;  tilingIdTest <= tilingId  ;  tilingIdTest++ )
				if( exportQ(anything, ef, &(tlngs[tilingIdTest]), numLinesThisFile ) )  // If any to be output in this ExportFormat
				{
					anythingToExport = true;
					break;
				}

			if( anythingToExport )
			{
				sprintf(fileName,
					"%sPenrose%s_Rhombi_%02" PRIi8 ".%s",
					tlngs[tilingId].filePath,  timeString,  tilingId,
					fileExtension_from_ExportFormat(fileExtension, ef)
				);  // sprintf()
				numLinesThisFile = 0;
				fp = file_open(fileName, "w", "main");
				tilings_export(
					fp,
					ef,
					&exportQ,
					tlngs,
					0,
					numTilings,
					&numLinesThisFile,
					&numCharsThisFile
				);  // tilings_export()
				fflush(fp); fclose(fp);
				printf(
					"main(): during tilingId=%" PRIi8 ", exported %lli chars %li lines, so %.1lf c/l, to %s\n",
					   tilingId,  numCharsThisFile,  numLinesThisFile,  numLinesThisFile > 0 ? (double)numCharsThisFile / (double)numLinesThisFile : 0,  fileName
				);  fflush(stdout);
			}  // If any to be output in this ExportFormat
		}  // for( ef_num ... )

		numPathStatsClosed = 0;
		for( pathStatNum = 0  ;  pathStatNum < tlngs[tilingId].numPathStats  ;  pathStatNum ++ )
			if( tlngs[tilingId].pathStat[pathStatNum].pathClosed )
				numPathStatsClosed ++;

		fprintf(stdout,  "main(): tilingId=%" PRIi8 " constructed and exported:\n",  tilingId);
		fprintf(stdout,
			"tId=%" PRIi8 ": #Fats=%li; #Thins=%li; F+T=%li;"
			"  #PathsClosed=%li; #PathsOpen=%li; C+O=%li;"
			"  #PathStats=%li;  #PathStats(C)=%li;  #PathStats(O)=%li;"
			"  LongestPathClosed=%li; #LongestPathOpen=%li;  boundingPathNumVertices=%lli.\n",
			tilingId,  tlngs[tilingId].numFats,  tlngs[tilingId].numThins,  tlngs[tilingId].numFats + tlngs[tilingId].numThins,
			tlngs[tilingId].numPathsClosed,  tlngs[tilingId].numPathsOpen,  tlngs[tilingId].numPathsClosed + tlngs[tilingId].numPathsOpen,
			tlngs[tilingId].numPathStats,  numPathStatsClosed,  tlngs[tilingId].numPathStats - numPathStatsClosed,
			longestPathClosed,  longestPathOpen,
			tlngs[tilingId].boundingPathNumVertices
		);  // fprintf()
		fprintf(stdout,
			"main(): tilingId=%" PRIi8 ", malloc()'s = %zu, this tiling simple total, so ignoring boundary and page alignments\n",
			tilingId,  tlngs[tilingId].mallocsPersistentSumSimple
		);  // fprintf()
		fflush(stdout);
		fprintf(stdout,
			"tId=%" PRIi8 ": xMin = %0.6lf;  xMax = %0.6lf;  yMin = %0.6lf;  yMax = %0.6lf.\n", tilingId,
			tlngs[tilingId].rhombi[ tlngs[tilingId].xMin_rhId ].xMin / tlngs[tilingId].edgeLength,
			tlngs[tilingId].rhombi[ tlngs[tilingId].xMax_rhId ].xMax / tlngs[tilingId].edgeLength,
			tlngs[tilingId].rhombi[ tlngs[tilingId].yMin_rhId ].yMin / tlngs[tilingId].edgeLength,
			tlngs[tilingId].rhombi[ tlngs[tilingId].yMax_rhId ].yMax / tlngs[tilingId].edgeLength
		);  // fprintf()
		fprintf(stdout,
			"Over all tilings with tilngId <= %" PRIi8 ", total execution time = %.3lfs\n\n\n",
			tilingId,   ((double)clock() - timeBeginConstruction) / CLOCKS_PER_SEC
		);  // fprintf()
		fflush(stdout);
	}  // for( tilingId ... )

	printf("main(): about to tiling_empty().\n");  fflush(stdout);
	for( tilingId = numTilings - 1;  tilingId >= 0;  tilingId-- )
		tiling_empty( &(tlngs[tilingId]) );
	free(tlngs);
	tlngs = NULL;

	printf("main(): finished.\n");  fflush(stdout);
	return EXIT_SUCCESS;

}  // main()
