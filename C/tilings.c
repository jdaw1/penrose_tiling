// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// tilings.c, in PenroseC

#include "penrose.h"

static clock_t prevTime  = -1;
static clock_t singleStartTime = -1;

void tiling_descendant(
	Tiling * const tlngDescendantP,
	Tiling const * const tlngAncestorP
)
{
	int rhId_Ancestor;
	clock_t const timeBeginDescendant = clock();
	clock_t timeBeginPart;
	unsigned long int numRhAtPreviousPurgeDuplicates = 0;
	long int numSpecialDeduplications = 0;

	if(singleStartTime == -1)
		singleStartTime = clock();

	tlngDescendantP->tilingId  = tlngAncestorP->tilingId + 1;
	printf("tiling_descendant(): tilingId=%" PRIi8 ", starting tiling_descendant().\n", tlngDescendantP->tilingId);  fflush(stdout);

	tlngDescendantP->numTilings = tlngAncestorP->numTilings;
	tlngDescendantP->rhombii_NumMax = 0;
	tlngDescendantP->rhombii = NULL;
	tlngDescendantP->numFats = 0;
	tlngDescendantP->numThins = 0;
	tlngDescendantP->path_NumMax = 0;
	tlngDescendantP->path = NULL;
	tlngDescendantP->numPathsClosed = 0;
	tlngDescendantP->numPathsOpen = 0;
	tlngDescendantP->pathStats_NumMax = 0;
	tlngDescendantP->pathStat = NULL;
	tlngDescendantP->numPathStats = 0;  // This needed when paths_sort() with pathStat not yet assigned.
	tlngDescendantP->axisAligned = tlngAncestorP->axisAligned;
	tlngDescendantP->wantedPostScriptCentre        = tlngAncestorP->wantedPostScriptCentre;
	tlngDescendantP->wantedPostScriptAspect        = tlngAncestorP->wantedPostScriptAspect;
	tlngDescendantP->wantedPostScriptHalfWidth     = 0;
	tlngDescendantP->wantedPostScriptNumberRhombii = 0;
	tlngDescendantP->wantedPostScriptNumberPaths   = 0;

	tlngDescendantP->edgeLength = tlngAncestorP->edgeLength * GoldenRatioReciprocal;

	// Scope to hold three temporary long int.
	{
		long int const rhombii_NumMax_A  // Based on the number of rhombii that will appear, with small spare. This is memory efficient.
			= (tlngAncestorP->numFats + tlngAncestorP->numThins) * (GoldenRatioSquare * 1.1)   // Final number descendants for ordinary recursion, *1.1 for a little spare. There is here a clash between memory efficiency and speed; 1.1 might be reasonable compromise.
			+ 6 * sqrt(tlngAncestorP->numFats + tlngAncestorP->numThins)                       // some extras, proportional to boundary length, for hole filling.
			+ 256;                                                                             // some more spare, especially for small tilings, and to make this irrelevant for very small tilings.
		long int const rhombii_NumMax_B  // Based on the number of rhombii that will appear, with small spare. This is memory efficient.
			= (tlngAncestorP->numFats + tlngAncestorP->numThins) * (GoldenRatioSquare * 1.01)  // Final number descendants for ordinary recursion, *1.01 for a tiny little spare.
			+ 6 * sqrt(tlngAncestorP->numFats + tlngAncestorP->numThins)                       // some extras, proportional to boundary length, for hole filling.
			+ 262144;                                                                          // Small proportional extra, large fixed extra. Boundary at Ancestor having 1,112,285 rhombii, Descendant having 3,209,784.
		long int const rhombii_NumMax_C  // Based on maximum from recursion algorithm.
			= 5 * tlngAncestorP->numFats   // Recursion makes five new rhombii for each previous fat,  though many subsequently de-duplicated.
			+ 4 * tlngAncestorP->numThins  // Recursion makes four new rhombii for each previous thin, though many subsequently de-duplicated.
			+ 64;                          // The extra 64 for, in early recursions, holes_Fill().
		tlngDescendantP->rhombii_NumMax = rhombii_NumMax_A;
		if( tlngDescendantP->rhombii_NumMax > rhombii_NumMax_B )  tlngDescendantP->rhombii_NumMax = rhombii_NumMax_B;
		if( tlngDescendantP->rhombii_NumMax > rhombii_NumMax_C )  tlngDescendantP->rhombii_NumMax = rhombii_NumMax_C;
	}

	tlngDescendantP->rhombii = malloc( tlngDescendantP->rhombii_NumMax  *  sizeof(Rhombus) );
	if( NULL == tlngDescendantP->rhombii )
	{
		fprintf(stderr, "tiling_descendant(): !!! NULL == tlng.rhombii !!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // NULL == tlngDescendantP->rhombii

	timeBeginPart = clock();
	for( rhId_Ancestor = 0;  rhId_Ancestor < tlngAncestorP->numFats + tlngAncestorP->numThins;  rhId_Ancestor ++ )
	{
		// Purpose: if huge memory assigned, best not to use it all as that might entail copying to and from disk.
		// An occasional purge of duplicates might lessen this. Also allows less memory to have been malloc'd.
		if( tlngDescendantP->numFats + tlngDescendantP->numThins >= numRhAtPreviousPurgeDuplicates + 14128176  // 4G of memory, being arbitrarily occasional.
		||  tlngDescendantP->numFats + tlngDescendantP->numThins >= tlngDescendantP->rhombii_NumMax - 8 )      // Tight against boundary: really need to purge duplicates.
		{
			rhombii_purgeDuplicates(tlngDescendantP);
			numSpecialDeduplications ++;
			numRhAtPreviousPurgeDuplicates = tlngDescendantP->numFats + tlngDescendantP->numThins;
		}  // if( 'purge needed' )
		rhombus_append_descendants( tlngDescendantP,  tlngAncestorP->rhombii + rhId_Ancestor );
	}  // for( rhId_Ancestor ... )
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for %li calls of rhombus_append_descendants(), and %li call%s of rhombii_purgeDuplicates(), resulting in #Fats+#Thins=%li\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngAncestorP->numFats + tlngAncestorP->numThins,
		numSpecialDeduplications,  numSpecialDeduplications == 1 ? "" : "s",
		tlngDescendantP->numThins + tlngDescendantP->numFats
	);  fflush(stdout);

	timeBeginPart = clock();
	rhombii_purgeDuplicates(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for rhombii_sorted_purgeDuplicates(), with #Fats=%li #Thins=%li, #Fats+#Thins=%li, /prev~=%0.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numFats,  tlngDescendantP->numThins,  tlngDescendantP->numFats + tlngDescendantP->numThins,
		((double)(tlngDescendantP->numThins + tlngDescendantP->numFats)) / (tlngAncestorP->numFats + tlngAncestorP->numThins)
	);  fflush(stdout);

	timeBeginPart = clock();
	neighbours_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for neighbours_populate()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	if( holesFillQ(tlngDescendantP) )
	{
		timeBeginPart = clock();
		holesFill(tlngDescendantP);
		printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for holesFill()\n",
			tlngDescendantP->tilingId,
			((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
		);  fflush(stdout);
	}
	else
		neighbours_populate(tlngDescendantP);

	timeBeginPart = clock();
	paths_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for paths_populate(), with #PathsClosed=%li,  #PathsOpen=%li,  (C+O)/prev~=%0.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numPathsClosed, tlngDescendantP->numPathsOpen,
		(double)(tlngDescendantP->numPathsClosed + tlngDescendantP->numPathsOpen) / (double)(tlngAncestorP->numPathsClosed + tlngAncestorP->numPathsOpen)
	);  fflush(stdout);

	timeBeginPart = clock();
	wanted_populate(tlngDescendantP);
	printf(
		"tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for wanted_populate(), "
		"with wantedPostScriptCentreX=%.15G, CentreY=%.15G, Aspect=%.15G "
		"==> HalfWidth=%0.15G, NumRh=%li, NumPaths=%li\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->wantedPostScriptCentre.x,
		tlngDescendantP->wantedPostScriptCentre.y,
		tlngDescendantP->wantedPostScriptAspect,
		tlngDescendantP->wantedPostScriptHalfWidth,
		tlngDescendantP->wantedPostScriptNumberRhombii,
		tlngDescendantP->wantedPostScriptNumberPaths
	);  fflush(stdout);

	timeBeginPart = clock();
	insideness_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for insideness_populate()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	timeBeginPart = clock();
	pathStats_populate(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for pathStats_populate() with #PathStats=%li,  /prev~=%0.4lg\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC,
		tlngDescendantP->numPathStats,
		(double)tlngDescendantP->numPathStats / (double)tlngAncestorP->numPathStats
	);  fflush(stdout);

	timeBeginPart = clock();
	verifyHypothesisedProperties(tlngDescendantP);
	printf("tiling_descendant(): tilingId=%" PRIi8 ", %0.3lfs for verifyHypothesisedProperties()\n",
		tlngDescendantP->tilingId,
		((double)clock() - timeBeginPart) / CLOCKS_PER_SEC
	);  fflush(stdout);

	// Simple checks: start
	RhombId    rhId;
	PathId     pathId;
	PathStatId pathStatId;
	long int numFats_rhombii, numFats_paths, numFats_pathsStats;
	numFats_rhombii = 0;
	for( rhId = 0  ;  rhId < tlngDescendantP->numFats + tlngDescendantP->numThins  ;  rhId ++ )
		if( Fat == tlngDescendantP->rhombii[rhId].physique )
			numFats_rhombii++;
	numFats_paths = 0;
	for( pathId = 0  ;  pathId < tlngDescendantP->numPathsClosed + tlngDescendantP->numPathsOpen  ;  pathId ++ )
		numFats_paths += tlngDescendantP->path[pathId].pathLength;
	numFats_pathsStats = 0;
	for( pathStatId = 0  ;  pathStatId < tlngDescendantP->numPathStats  ;  pathStatId ++ )
		numFats_pathsStats += tlngDescendantP->pathStat[pathStatId].pathLength * tlngDescendantP->pathStat[pathStatId].numPaths;
	if( numFats_rhombii != numFats_pathsStats  ||  numFats_rhombii != numFats_paths  ||  numFats_rhombii != tlngDescendantP->numFats )
		fprintf(stderr,
			"\ntiling_descendant(): !!! tilingId=%" PRIi8 ", numFats_scalar=%li; numFats_rhombii=%li; numFats_paths=%li; numFats_pathsStats=%li\n\n",
			tlngDescendantP->tilingId, tlngDescendantP->numFats, numFats_rhombii, numFats_paths, numFats_pathsStats
		);  fflush(stderr);
	// Simple checks: end


	double thisTime = (double)(clock() - timeBeginDescendant) / CLOCKS_PER_SEC;
	printf("tiling_descendant(): tilingId=%" PRIi8 ", numFats=%li, numThins=%li, numPathsClosed=%li, numPathsOpen=%li, numPathStats=%li"
		", execution time ~= %0.3lf seconds; all tilings' time = %0.3lfs. (Both excl. this t's disk-writing time.)\n",
		tlngDescendantP->tilingId, tlngDescendantP->numFats, tlngDescendantP->numThins,
		tlngDescendantP->numPathsClosed, tlngDescendantP->numPathsOpen, tlngDescendantP->numPathStats,
		thisTime,
		((double)clock() - singleStartTime) / CLOCKS_PER_SEC
	);  fflush(stdout);

	export_soloTiling(tlngDescendantP,  timeBeginDescendant);

	prevTime = thisTime;
	printf("tiling_descendant(): tilingId=%" PRIi8 ", ending tiling_descendant().\n", tlngDescendantP->tilingId);  fflush(stdout);
}  // tiling_descendant()



void tiling_initial(
	Tiling * const tlngP,
	double   const init_thin_xNorth,  double const init_thin_xSouth,
	double   const init_thin_yNorth,  double const init_thin_ySouth,
	XY const wantedPostScriptCentre,  double const wantedPostScriptAspect
)
{
	clock_t const timeBegin = clock();

	tlngP->rhombii_NumMax = 0;
	tlngP->rhombii = NULL;
	tlngP->numFats = 0;
	tlngP->numThins = 0;
	tlngP->path_NumMax = 0;
	tlngP->path = NULL;
	tlngP->numPathsClosed = 0;
	tlngP->numPathsOpen = 0;
	tlngP->pathStats_NumMax = 0;
	tlngP->pathStat = NULL;
	tlngP->numPathStats = 0;  // This needed when paths_sort() with pathStat not yet assigned.

	tlngP->rhombii_NumMax = 4;  // Initial thin, + two fats added by holesFill(), + one spare.

	tlngP->wantedPostScriptCentre        = wantedPostScriptCentre;
	tlngP->wantedPostScriptAspect        = wantedPostScriptAspect;
	tlngP->wantedPostScriptHalfWidth     = 0;
	tlngP->wantedPostScriptNumberRhombii = 0;
	tlngP->wantedPostScriptNumberPaths   = 0;

	if     ( init_thin_yNorth == init_thin_ySouth ) tlngP->edgeLength = fabs(init_thin_xNorth - init_thin_xSouth) * GoldenRatio;
	else if( init_thin_xNorth == init_thin_xSouth ) tlngP->edgeLength = fabs(init_thin_yNorth - init_thin_ySouth) * GoldenRatio;
	else tlngP->edgeLength = sqrt( pow(init_thin_xNorth - init_thin_xSouth, 2) + pow(init_thin_yNorth - init_thin_ySouth, 2) ) * GoldenRatio;

	tlngP->rhombii = malloc( tlngP->rhombii_NumMax  *  sizeof(Rhombus) );
	if( NULL == tlngP->rhombii )
	{
		fprintf(stderr, "tiling_initial(): !!! NULL == tlng.rhombii !!!\n");
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // if( NULL == tlngP->rhombii )
	tlngP->anyPathsVeryClosed = false;
	RhombId const rhId_new = rhombus_append(
		tlngP,
		Thin,
		false,  // not filled hole
		init_thin_xNorth, init_thin_yNorth, init_thin_xSouth, init_thin_ySouth
	);
	if( rhId_new < 0 )
	{
		fprintf(stderr, "!!! Error in tiling_initial(): rhombus_append() failed. !!!\n");
		exit(EXIT_FAILURE);
	}
	tlngP->xMin = tlngP->rhombii[rhId_new].xMin;
	tlngP->yMin = tlngP->rhombii[rhId_new].yMin;
	tlngP->xMax = tlngP->rhombii[rhId_new].xMax;
	tlngP->yMax = tlngP->rhombii[rhId_new].yMax;

	// Most of the following redundant, unless initial tiling made more complicated.
	rhombii_sort(tlngP,  &rhombiiGt_ByY,  false);
	rhombii_purgeDuplicates(tlngP);
	neighbours_populate(tlngP);

	if( holesFillQ(tlngP) )
		holesFill(tlngP);

	paths_populate(tlngP);
	wanted_populate(tlngP);
	insideness_populate(tlngP);
	pathStats_populate(tlngP);

	double const angMultiple = tlngP->rhombii[0].angleDegrees / 18;
	tlngP->axisAligned = ( fabs(round(angMultiple) - angMultiple) < 0.000005 );  // A multiple of 18 degrees, to within 0.0935 dots across A3 at 3600dpi.

	export_soloTiling(tlngP,  timeBegin);
}  // tiling_initial()



void tiling_empty(Tiling * const tlngP)
{
	if( NULL != tlngP->pathStat )
	{
		free( tlngP->pathStat );
		tlngP->pathStat = NULL;
	}
	tlngP->pathStats_NumMax = 0;
	tlngP->numPathStats = 0;


	if( NULL != tlngP->path )
	{
		free( tlngP->path );
		tlngP->path = NULL;
	}
	tlngP->path_NumMax = 0 ;
	tlngP->numPathsClosed = 0 ;
	tlngP->numPathsOpen = 0 ;
	tlngP->anyPathsVeryClosed = false;

	if( NULL != tlngP->rhombii )
	{
		free( tlngP->rhombii );
		tlngP->rhombii = NULL;
	}
	tlngP->rhombii_NumMax      = 0 ;
	tlngP->numFats             = 0;
	tlngP->numThins            = 0;
	tlngP->wantedPostScriptNumberRhombii = 0;
	tlngP->wantedPostScriptNumberPaths   = 0;


	tlngP->edgeLength   = 0;
}  // tiling_empty()
