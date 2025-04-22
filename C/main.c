// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// main.c, in PenroseC

#include "penrose.h"

static int8_t const numTilings_Max = 17;  // Hard-wired contraint to catch CPU-expensive mistyping. Sensible limit for 32GiB machine. YMMV.

char scratchString[scratchStringLength] ;  // Mostly used for post-processing of doubles: removal of trailing 0s and trailing decimal points. Size harmlessly generous: 6k would have been sufficient, the boundary need being the preamble in tiling_export_PaintRhombiPS.

int main(void)
{
	/* Seed thin rhombus. No need to change. */
	double const init_thin_xNorth = 0 ;
	double const init_thin_xSouth = 2 ;
	double const init_thin_yNorth = 0 ;
	double const init_thin_ySouth = init_thin_yNorth;
	int8_t numTilings;

	// User-changeable constants above.

	Tiling *tlngs;
	FILE *fp;
	TilingId tilingId;
	time_t prog_start;
	struct tm *timeData;
	char timeString[250], fileName[256], fileExtension[16];

	printf(
		"main(): sizeof(Rhombus)=%li;  sizeof(Path)=%li;  sizeof(PathStats)=%li;  sizeof(Tiling)=%li\n\n",
		(long int)(sizeof(Rhombus)),  (long int)(sizeof(Path)),  (long int)(sizeof(PathStats)),  (long int)(sizeof(Tiling))
	);

	printf(
		"\n\a\n"  // Bell sound! Good luck.
		"What is to be the recursion depth = numTilings?\n"
		"For testing choose in range 8 to 12.\n"
		"By memory constraint on a 32GiB machine, maximum is 17, run time being, on author's computer, about 5 hours. Obviously, YMMV.\n"
	);  fflush(stdout);
	do
	{
		printf("Compile-time constraint: must be >=1 && <=%" PRIi8 ".\n", numTilings_Max);  fflush(stdout);
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
	sprintf(timeString, "%04d%02d%02d_%02d%02d%02d",
		(1900 + timeData->tm_year),  (1 + timeData->tm_mon),  timeData->tm_mday,
		timeData->tm_hour,  timeData->tm_min,  (int)(timeData->tm_sec)
	);
	
	clock_t const timeBeginConstruction = clock();

	for( tilingId = 0;  tilingId < numTilings;  tilingId++ )
	{
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
		tlngs[tilingId].xMin                     = DBL_MAX;
		tlngs[tilingId].yMin                     = DBL_MAX;
		tlngs[tilingId].xMax                     = - DBL_MAX;
		tlngs[tilingId].yMax                     = - DBL_MAX;
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
			tiling_initial(
				&(tlngs[tilingId]),
				init_thin_xNorth, init_thin_xSouth, init_thin_yNorth, init_thin_ySouth,
				wantedPostScriptCentre(), wantedPostScriptAspect()
			);
		}
		else
			tiling_descendant(
				&(tlngs[tilingId]),
				&(tlngs[tilingId - 1])
			);


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

		// If ExportFormat acquires other possibilites, give attention to this, to the "3" in next line, and to the extension calculation.
		// Also, want PS_data last, because of sort-by wanted.
		const ExportFormat exportFormat[3] = {TSV, PS_data, JSON};  // TSV most useful, so first; JSON most massive, so last.
		for( ef_num = 0  ;  ef_num < 3  ; ef_num++ )
		{
			ef = exportFormat[ef_num];

			numLinesThisFile = 0;
			numCharsThisFile = 0;

			anythingToExport = false;
			for( tilingIdTest = 0  ;  tilingIdTest <= tilingId  ;  tilingIdTest++ )
				if( exportQ(Anything, ef, &(tlngs[tilingIdTest]), numLinesThisFile ) )  // If any to be output in this ExportFormat
				{
					anythingToExport = true;
					break;
				}

			if( anythingToExport )
			{
				sprintf(fileName,
					"%sPenrose_%s_Rhombi_%02" PRIi8 ".%s",
					tlngs[tilingId].filePath,  timeString,  tilingId,
					fileExtension_from_ExportFormat(fileExtension, ef)
				);
				numLinesThisFile = 0;
				fp=fopen(fileName, "w");
				tilings_export(
					fp,
					ef,
					&exportQ,
					tlngs,
					0,
					numTilings,
					&numLinesThisFile,
					&numCharsThisFile
				);
				fflush(fp); fclose(fp);
				printf(
					"main(): during tilingId=%" PRIi8 ", exported %lli chars %li lines, so %0.1lf c/l, to %s\n",
					tilingId,  numCharsThisFile,  numLinesThisFile,  (double)numCharsThisFile / (double)numLinesThisFile,  fileName
				);  fflush(stdout);
			}  // If any to be output in this ExportFormat
		}  // for( ef_num ... )

		printf(
			"main(): tilingId=%" PRIi8 " constructed and exported:\n"
			"#Fats=%li; #Thins=%li; #PathsClosed=%li; #PathsOpen=%li; #PathStats=%li; LongestPathClosed=%li; #LongestPathOpen=%li;\n"
			"total execution time = %0.3lfs\n",
			tilingId,  tlngs[tilingId].numFats,  tlngs[tilingId].numThins,  tlngs[tilingId].numPathsClosed,  tlngs[tilingId].numPathsOpen,  tlngs[tilingId].numPathStats,
			longestPathClosed,  longestPathOpen,  ((double)clock() - timeBeginConstruction) / CLOCKS_PER_SEC
		);  fflush(stdout);

		printf("\n\n");  fflush(stdout);
	}  // for( tilingId ... )

	printf("main(): about to tiling_empty().\n");  fflush(stdout);
	for( tilingId = numTilings - 1;  tilingId >= 0;  tilingId-- )
		tiling_empty( &(tlngs[tilingId]) );
	free(tlngs);
	tlngs = NULL;

	printf("main(): finished.\n");  fflush(stdout);
	return EXIT_SUCCESS;

}  // main()
