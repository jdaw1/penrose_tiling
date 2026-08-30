// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// tiling_empty.c, in PenroseC

#include "penrose.h"

void tiling_empty(Tiling * const tlngP)
{
	tlngP->populated = false;

	if( NULL != tlngP->pathStat )
	{
		free( tlngP->pathStat );
		tlngP->pathStat = NULL;
	}
	tlngP->pathStats_NumMax = 0;
	tlngP->numPathStats     = 0;


	if( NULL != tlngP->path )
	{
		free( tlngP->path );
		tlngP->path = NULL;
	}
	tlngP->path_NumMax    = 0 ;
	tlngP->numPathsClosed = 0 ;
	tlngP->numPathsOpen   = 0 ;
	tlngP->anyPathsVeryClosed = false;

	if( NULL != tlngP->rhombi )
	{
		free( tlngP->rhombi );
		tlngP->rhombi = NULL;
	}
	tlngP->rhombi_NumMax = 0 ;
	tlngP->numFats       = 0;
	tlngP->numThins      = 0;
	tlngP->numThins_0T4F = 0;
	tlngP->numThins_1T3F = 0;
	tlngP->wantedPostScriptNumRhombi = 0;
	tlngP->wantedPostScriptNumPaths  = 0;

	tlngP->edgeLength   = 0;
	tlngP->mallocsPersistentSumSimple = 0;
	tlngP->boundingPathNumVertices = -1;
	tlngP->xMin_rhId = -1 ;
	tlngP->xMax_rhId = -1 ;
	tlngP->yMin_rhId = -1 ;
	tlngP->yMax_rhId = -1 ;

	tlngP->radiusMax = -999999;
	tlngP->radiusShortOpen = -999999;
	tlngP->SecondsToStartExportFromStartThisTiling = -999;
	tlngP->SecondsToStartExportFromStartFirstTiling = -999;
}  // tiling_empty()
