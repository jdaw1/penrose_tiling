// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// tilings.c, in PenroseC

#include "penrose.h"

static clock_t timeBeginFirstTiling = -1;

void tiling_descendant(
	Tiling * const tlngDescendantP,
	Tiling const * const tlngAncestorP
)
{
	int rhId_Ancestor;
	clock_t timeBeginThisTiling, timeBeginPart;
	unsigned long int numRhAtPreviousPurgeDuplicates = 0;
	long int numSpecialDeduplications = 0;

	timeBeginThisTiling = clock();
	if(timeBeginFirstTiling < 0)
		timeBeginFirstTiling = timeBeginThisTiling;

	tlngDescendantP->tilingId  = tlngAncestorP->tilingId + 1;
	printf("tiling_descendant(): tilingId=%" PRIi8 ", starting tiling_descendant().\n", tlngDescendantP->tilingId);  fflush(stdout);

	tlngDescendantP->mallocsPersistentSumSimple = 0;
	tlngDescendantP->seedType = tlngAncestorP->seedType;
	tlngDescendantP->numTilings = tlngAncestorP->numTilings;
	tlngDescendantP->rhombi_NumMax = 0;
	tlngDescendantP->rhombi = NULL;
	tlngDescendantP->numFats = 0;
	tlngDescendantP->numThins = 0;
	tlngDescendantP->path_NumMax = 0;
	tlngDescendantP->path = NULL;
	tlngDescendantP->numPathsClosed = 0;
	tlngDescendantP->numPathsOpen = 0;
	tlngDescendantP->pathStats_NumMax = 0;
	tlngDescendantP->pathStat = NULL;
	tlngDescendantP->wantedPostScriptRhombNum = NULL;
	tlngDescendantP->wantedPostScriptPathNum = NULL;
	tlngDescendantP->numPathStats = 0;  // This needed when paths_sort() with pathStat not yet assigned.
	tlngDescendantP->axisAligned = tlngAncestorP->axisAligned;
	tlngDescendantP->wantedPostScriptHalfWidth    = 0;
	tlngDescendantP->wantedPostScriptNumRhombi = 0;
	tlngDescendantP->wantedPostScriptNumPaths  = 0;
	tlngDescendantP->populated = false;
	tlngDescendantP->radiusMax = -1;
	tlngDescendantP->radiusShortOpen = -1;

	tlngDescendantP->edgeLength = tlngAncestorP->edgeLength * GoldenRatioReciprocal;

	// Scope to hold three temporary long int.
	{
		long int const rhombi_NumMax_A  // Based on the number of rhombi that will appear, with small spare. This is memory efficient.
			= (tlngAncestorP->numFats + tlngAncestorP->numThins) * (GoldenRatioSquare * 1.1)   // Final number descendants for ordinary recursion, *1.1 for a little spare. There is here a clash between memory efficiency and speed; 1.1 might be reasonable compromise.
			+ 6 * sqrt(tlngAncestorP->numFats + tlngAncestorP->numThins)                       // some extras, proportional to boundary length, for hole filling.
			+ 256;                                                                             // some more spare, especially for small tilings, and to make this irrelevant for very small tilings.
		long int const rhombi_NumMax_B  // Based on the number of rhombi that will appear, with small spare. This is memory efficient.
			= (tlngAncestorP->numFats + tlngAncestorP->numThins) * (GoldenRatioSquare * 1.01)  // Final number descendants for ordinary recursion, *1.01 for a tiny little spare.
			+ 6 * sqrt(tlngAncestorP->numFats + tlngAncestorP->numThins)                       // some extras, proportional to boundary length, for hole filling.
			+ 262144;                                                                          // Small proportional extra, large fixed extra. Boundary at Ancestor having 1,112,285 rhombi, Descendant having 3,209,784.
		long int const rhombi_NumMax_C  // Based on maximum from recursion algorithm.
			= 5 * tlngAncestorP->numFats   // Recursion makes five new rhombi for each previous fat,  though many subsequently de-duplicated.
			+ 4 * tlngAncestorP->numThins  // Recursion makes four new rhombi for each previous thin, though many subsequently de-duplicated.
			+ 64;                          // The extra 64 for, in early recursions, holes_Fill().
		tlngDescendantP->rhombi_NumMax     = rhombi_NumMax_A;
		if( tlngDescendantP->rhombi_NumMax > rhombi_NumMax_B )  {tlngDescendantP->rhombi_NumMax = rhombi_NumMax_B;}
		if( tlngDescendantP->rhombi_NumMax > rhombi_NumMax_C )  {tlngDescendantP->rhombi_NumMax = rhombi_NumMax_C;}
	}  // Temporary scope

	{  // scope mallocThis
		size_t const mallocThis = tlngDescendantP->rhombi_NumMax  *  sizeof(Rhombus);
		tlngDescendantP->rhombi = malloc(mallocThis);
		if( NULL == tlngDescendantP->rhombi )
		{
			fprintf(stderr, "tiling_descendant(): !!! NULL == tlng.rhombi !!!\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}  // NULL == tlngDescendantP->rhombi
		tlngDescendantP->mallocsPersistentSumSimple += mallocThis;
	}  // scope mallocThis

	timeBeginPart = clock();
	for( rhId_Ancestor = 0;  rhId_Ancestor < tlngAncestorP->numFats + tlngAncestorP->numThins;  rhId_Ancestor ++ )
	{
		// Purpose: if huge memory assigned, best not to use it all as that might entail copying to and from disk.
		// An occasional purge of duplicates might lessen this. Also allows less memory to have been malloc'd.
		if( tlngDescendantP->numFats + tlngDescendantP->numThins >= numRhAtPreviousPurgeDuplicates + 14128176  // 4G of memory, being arbitrarily occasional.
		||  tlngDescendantP->numFats + tlngDescendantP->numThins >= tlngDescendantP->rhombi_NumMax - 8 )      // Tight against boundary: really need to purge duplicates.
		{
			rhombi_purgeDuplicates(tlngDescendantP);
			numSpecialDeduplications ++;
			numRhAtPreviousPurgeDuplicates = tlngDescendantP->numFats + tlngDescendantP->numThins;
		}  // if( 'purge needed' )
		rhombus_append_descendants( tlngDescendantP,  tlngAncestorP->rhombi + rhId_Ancestor );
	}  // for( rhId_Ancestor ... )
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for %li calls of rhombus_append_descendants(), and %li call%s of rhombi_purgeDuplicates(), resulting in #Fats+#Thins=%li\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngAncestorP->numFats + tlngAncestorP->numThins,
		numSpecialDeduplications,  numSpecialDeduplications == 1 ? "" : "s",
		tlngDescendantP->numThins + tlngDescendantP->numFats
	);  fflush(stdout);

	timeBeginPart = clock();
	rhombi_purgeDuplicates(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for rhombi_purgeDuplicates(), with #Fats=%li #Thins=%li, #Fats+#Thins=%li, /prev~=%.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numFats,  tlngDescendantP->numThins,  tlngDescendantP->numFats + tlngDescendantP->numThins,
		((double)(tlngDescendantP->numThins + tlngDescendantP->numFats)) / (tlngAncestorP->numFats + tlngAncestorP->numThins)
	);  fflush(stdout);

	timeBeginPart = clock();
	neighbours_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for neighbours_populate()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	if( holesFillQ(tlngDescendantP) )
	{
		timeBeginPart = clock();
		holesFill(tlngDescendantP);
		printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for holesFill()\n",
			tlngDescendantP->tilingId,
			((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
		);  fflush(stdout);
	}
	else
		neighbours_populate(tlngDescendantP);

	timeBeginPart = clock();
	paths_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for paths_populate(), with #PathsClosed=%li,  #PathsOpen=%li,  C+O=%li,  (C+O)/prev~=%.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numPathsClosed, tlngDescendantP->numPathsOpen,  tlngDescendantP->numPathsClosed + tlngDescendantP->numPathsOpen,
		(double)(tlngDescendantP->numPathsClosed + tlngDescendantP->numPathsOpen) / (double)(tlngAncestorP->numPathsClosed + tlngAncestorP->numPathsOpen)
	);  fflush(stdout);

	timeBeginPart = clock();
	insideness_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for insideness_populate()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	timeBeginPart = clock();
	pathStats_populate(tlngDescendantP);
	printf(
		"tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for pathStats_populate() with #PathStats=%li,  /prev~=%.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numPathStats,
		(double)tlngDescendantP->numPathStats / (double)tlngAncestorP->numPathStats
	);  fflush(stdout);

	rhombi_sort(tlngDescendantP,  &rhombiGt_ByPath,  true);
	bounding_box_tiling(tlngDescendantP);
	radii_populate(tlngDescendantP);
	thins_0T4F_1T3F_count(tlngDescendantP);

	timeBeginPart = clock();
	tlngDescendantP->boundingPathNumVertices = tiling_export_PaintBoundary(
		NULL,
		TSV,   // irrelevant
		0,     // irrelevant
		tlngDescendantP,
		0,     // irrelevant
		NULL,  // not to output
		NULL   // irrelevant
	);  // tiling_export_PaintBoundary()
	if( tlngDescendantP->boundingPathNumVertices <= 0 )
		 fprintf(stderr, "tiling_export_PaintBoundary(), tiling_export_PaintBoundary() returned %lli, which is weirdly non-positive. Continuing.\n", tlngDescendantP->boundingPathNumVertices);
	printf(
		"tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for tiling_export_PaintBoundary(NULL, ...) to return %lli, the \"NULL\" meaning not to print anything.\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->boundingPathNumVertices
	);  fflush(stdout);

	tlngDescendantP->wantedPostScriptCentre = wantedPostScriptCentre(tlngDescendantP);
	tlngDescendantP->wantedPostScriptAspect = max_2(wantedPostScriptAspect(tlngDescendantP), 0.0001);  // Strictly positive, even if wantedPostScriptAspect(0) is hostile
	wanted_populate(tlngDescendantP);
	printf(
		"tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for wanted_populate(), "
		"with wantedPostScriptCentreX=%.15G, CentreY=%.15G, Aspect=%.15G "
		"==> HalfWidth=%.15G, NumRh=%li, NumPaths=%li\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->wantedPostScriptCentre.x,
		tlngDescendantP->wantedPostScriptCentre.y,
		tlngDescendantP->wantedPostScriptAspect,
		tlngDescendantP->wantedPostScriptHalfWidth,
		tlngDescendantP->wantedPostScriptNumRhombi,
		tlngDescendantP->wantedPostScriptNumPaths
	);  fflush(stdout);

	tlngDescendantP->populated = true;

	timeBeginPart = clock();
	verifyHypothesisedProperties(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %.3lfs for verifyHypothesisedProperties()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	// Simple checks: start
	RhombId    rhId;
	PathId     pathId;
	PathStatId pathStatId;
	long int numFats_rhombi, numFats_paths, numFats_pathsStats;
	numFats_rhombi = 0;
	for( rhId = 0  ;  rhId < tlngDescendantP->numFats + tlngDescendantP->numThins  ;  rhId ++ )
		if( Fat == tlngDescendantP->rhombi[rhId].physique )
			numFats_rhombi++;
	numFats_paths = 0;
	for( pathId = 0  ;  pathId < tlngDescendantP->numPathsClosed + tlngDescendantP->numPathsOpen  ;  pathId ++ )
		numFats_paths += tlngDescendantP->path[pathId].pathLength;
	numFats_pathsStats = 0;
	for( pathStatId = 0  ;  pathStatId < tlngDescendantP->numPathStats  ;  pathStatId ++ )
		numFats_pathsStats += tlngDescendantP->pathStat[pathStatId].pathLength * tlngDescendantP->pathStat[pathStatId].numPaths;
	if( numFats_rhombi != numFats_pathsStats  ||  numFats_rhombi != numFats_paths  ||  numFats_rhombi != tlngDescendantP->numFats )
		fprintf(stderr,
			"\ntiling_descendant(): !!! Error !!! tilingId=%" PRIi8 ", numFats_scalar=%li; numFats_rhombi=%li; numFats_paths=%li; numFats_pathsStats=%li\n\n",
			tlngDescendantP->tilingId, tlngDescendantP->numFats, numFats_rhombi, numFats_paths, numFats_pathsStats
		);  fflush(stderr);
	// Simple checks: end

	tlngDescendantP->SecondsToStartExportFromStartThisTiling = ((double)clock() - timeBeginThisTiling) / CLOCKS_PER_SEC;
	tlngDescendantP->SecondsToStartExportFromStartFirstTiling
		= tlngDescendantP->SecondsToStartExportFromStartThisTiling  // ==> times consistent
		+ (timeBeginThisTiling - timeBeginFirstTiling) / CLOCKS_PER_SEC;

	printf("tiling_descendant(): tilingId=%" PRIi8 ", numFats=%li, numThins=%li, numPathsClosed=%li, numPathsOpen=%li, numPathStats=%li"
		", execution time ~= %.3lf seconds; all tilings' time = %.3lfs. (Both excl. this t's disk-writing time.)\n",
		tlngDescendantP->tilingId, tlngDescendantP->numFats, tlngDescendantP->numThins,
		tlngDescendantP->numPathsClosed, tlngDescendantP->numPathsOpen, tlngDescendantP->numPathStats,
		tlngDescendantP->SecondsToStartExportFromStartThisTiling,
		tlngDescendantP->SecondsToStartExportFromStartFirstTiling
	);  fflush(stdout);

	export_soloTiling(tlngDescendantP);

	printf("tiling_descendant(): tilingId=%" PRIi8 ", ending tiling_descendant().\n", tlngDescendantP->tilingId);  fflush(stdout);
}  // tiling_descendant()





void tiling_initial(Tiling * const tlngP)
{
	clock_t timeBeginThisTiling;
	RhombId rhId_new;
	double xN, yN, xS, yS;

	timeBeginThisTiling = clock();
	if(timeBeginFirstTiling < 0)
		timeBeginFirstTiling = timeBeginThisTiling;

	tlngP->mallocsPersistentSumSimple = 0;
	tlngP->rhombi = NULL;
	tlngP->numFats  = 0;
	tlngP->numThins = 0;
	tlngP->path = NULL;
	tlngP->numPathsClosed = 0;
	tlngP->numPathsOpen   = 0;
	tlngP->pathStat = NULL;
	tlngP->wantedPostScriptRhombNum = NULL;
	tlngP->wantedPostScriptPathNum = NULL;
	tlngP->numPathStats = 0;  // This needed when paths_sort() with pathStat not yet assigned.
	tlngP->boundingPathNumVertices  = -1 ;
	tlngP->populated = false;

	tlngP->wantedPostScriptHalfWidth = 0;
	tlngP->wantedPostScriptNumRhombi = 0;
	tlngP->wantedPostScriptNumPaths  = 0;

	tlngP->rhombi_NumMax = 24;
	tlngP->path_NumMax = 8;
	tlngP->pathStats_NumMax = 4;

	tlngP->radiusMax = -1;
	tlngP->radiusShortOpen = -1;

	{  // scope mallocThis
		size_t const mallocThis = tlngP->rhombi_NumMax  *  sizeof(Rhombus);
		tlngP->rhombi = malloc(mallocThis);
		if( NULL == tlngP->rhombi )
		{
			fprintf(stderr, "tiling_initial(): !!! NULL == tlng.rhombi !!!\n");
			fflush(stderr);
			exit(EXIT_FAILURE);
		}  // if( NULL == tlngP->rhombi )
		tlngP->mallocsPersistentSumSimple += mallocThis;
	}  // scope mallocThis

	tlngP->edgeLength = 1 ;

	switch(tlngP->seedType)  // Possibilities: oneThin, oneFat, round5
	{
	case oneThin:
		xN = xS = 0;
		yS =  tlngP->edgeLength == 1.0  ?  Cos72  :  Cos72 * tlngP->edgeLength;
		yN = -yS;
		rhId_new = rhombus_append(tlngP,  Thin,  false,  xN, yN, xS, yS);  // false ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, oneThin. !!!\n");  exit(EXIT_FAILURE);}
		break;

	case oneFat:
		xS =  tlngP->edgeLength == 1.0  ?  Cos36  :  Cos36 * tlngP->edgeLength;
		xN = -xS;
		yN = yS = 0;
		rhId_new = rhombus_append(tlngP,  Fat,  false,  xN, yN, xS, yS);  // false ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, oneFat. !!!\n");  exit(EXIT_FAILURE);}
		break;

	case round5:
		xS = 0;
		yS = 0;
			
		xN = 0;
		yN = -GoldenRatio;
		rhId_new = rhombus_append(tlngP,  Fat,  0,  xN, yN, xS, yS);  // 0 ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, round5, 0. !!!\n");  exit(EXIT_FAILURE);}

		yN = -0.5 * tlngP->edgeLength;
		xN = 1.5388417685876267012851452880184549120033510717688962135195781251874316442475454592272968608335527 * tlngP->edgeLength;  // 2 Cos[36 Degree] * Cos[18 Degree]
		rhId_new = rhombus_append(tlngP,  Fat,  0,  xN, yN, xS, yS);  // false ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, round5, 1. !!!\n");  exit(EXIT_FAILURE);}
		rhId_new = rhombus_append(tlngP,  Fat,  0, -xN, yN, xS, yS);  // 0 ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, round5, 2. !!!\n");  exit(EXIT_FAILURE);}

		xN = Cos18 * tlngP->edgeLength;
		yN = 1.30901699437494742410229341718281905886015458990288143106772431135263023140945122485360360209470 * tlngP->edgeLength;  // 2 Cos[36 Degree] * Cos[36 Degree]
		rhId_new = rhombus_append(tlngP,  Fat,  0,  xN, yN, xS, yS);  // 0 ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, round5, 3. !!!\n");  exit(EXIT_FAILURE);}
		rhId_new = rhombus_append(tlngP,  Fat,  0, -xN, yN, xS, yS);  // 0 ==> not filled hole
		if( rhId_new < 0 ) {fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed, round5, 4. !!!\n");  exit(EXIT_FAILURE);}

		break;


	default:
		fprintf(stderr, "!!! Error in tiling_initial(): switch(tlngP->seedType), unknown value. !!!\n");
		exit(EXIT_FAILURE);
	};  // switch(tlngP->seedType)

	if( ! tiling_simple_tests(tlngP) )
		exit(EXIT_FAILURE);

	tlngP->anyPathsVeryClosed = false;

	rhombi_sort(tlngP,  &rhombiGt_ByY,  false);
	rhombi_purgeDuplicates(tlngP);
	neighbours_populate(tlngP);

	if( holesFillQ(tlngP) )
		holesFill(tlngP);

	paths_populate(tlngP);
	insideness_populate(tlngP);
	pathStats_populate(tlngP);
	rhombi_sort(tlngP,  &rhombiGt_ByPath,  true);
	bounding_box_tiling(tlngP);

	radii_populate(tlngP);
	thins_0T4F_1T3F_count(tlngP);

	tlngP->boundingPathNumVertices = tiling_export_PaintBoundary(
		NULL,   // irrelevant
		TSV,    // irrelevant
		0,      // irrelevant
		tlngP,
		0,      // irrelevant
		NULL,   // signals not to output
		NULL
	);  // tiling_export_PaintBoundary()
	if( tlngP->boundingPathNumVertices <= 0 )
	{
		 fprintf(stderr,
			"tiling_initial(), tiling_export_PaintBoundary() returned %lli, which is weirdly non-positive.\n",
			tlngP->boundingPathNumVertices
		);
		exit(EXIT_FAILURE);
	}
		

	double const angMultiple = tlngP->rhombi[0].angleDegrees / 18;
	tlngP->axisAligned = ( fabs(round(angMultiple) - angMultiple) < 0.000005 );  // A multiple of 18 degrees, to within 0.0935 dots across A3 at 3600dpi.

	tlngP->wantedPostScriptCentre = wantedPostScriptCentre(tlngP);
	tlngP->wantedPostScriptAspect = max_2(wantedPostScriptAspect(tlngP), 0.001);  // Strictly posiitve, even if wantedPostScriptAspect(0 is hostile
	wanted_populate(tlngP);

	tlngP->populated = true;

	tlngP->SecondsToStartExportFromStartThisTiling = ((double)clock() - timeBeginThisTiling) / CLOCKS_PER_SEC;
	tlngP->SecondsToStartExportFromStartFirstTiling
		= tlngP->SecondsToStartExportFromStartThisTiling  // ==> times consistent
		+ (timeBeginThisTiling - timeBeginFirstTiling) / CLOCKS_PER_SEC;

	export_soloTiling(tlngP);
}  // tiling_initial()
