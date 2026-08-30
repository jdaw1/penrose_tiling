// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// pathStats.c, in PenroseC

#include "penrose.h"

void tiling_count_path_lengths(Tiling * const tlngP);


void pathStats_populate(Tiling * const tlngP)
{
	PathId          pathId;
	PathStatId      pathStatId;
	PathStats * pathStatP;
	bool            pathTypeFound;
	Path          * pathP;

	tlngP->numPathStats = 0;

	tlngP->pathStats_NumMax = 1  +  2 * ( tlngP->numFats > 0  ?  (long int)ceil(pow(1 + tlngP->numFats, 0.6))  :  0 );
	{  // scope mallocThis
		size_t const mallocThis = tlngP->pathStats_NumMax  *  sizeof(PathStats);
		tlngP->pathStat = malloc(mallocThis);
		if(NULL == tlngP->pathStat)
		{
			fprintf(stderr, "pathStats_populate: malloc() failed with tilingId=%" PRIi8 ", pathStats_NumMax=%li\n", tlngP->tilingId, tlngP->pathStats_NumMax);
			fflush(stderr);
			exit( EXIT_FAILURE );
		}  // NULL == tlngP->pathStat
		tlngP->mallocsPersistentSumSimple += mallocThis;
	}  // scope mallocThis

	for( pathStatId = 0  ;  pathStatId < tlngP->pathStats_NumMax  ;  pathStatId++ )
	{
		pathStatP = &(tlngP->pathStat[ pathStatId ]);
		pathStatP->pathStatId = pathStatId;
		pathStatP->pathClosed = false;
		pathStatP->pathLength = -1;
		pathStatP->pointy = false;
		pathStatP->pathClosedTypeNum = -2;  // -1 is open, -2 undefined.
		pathStatP->numPaths = 0;
		pathStatP->examplePathId = -1;  // ==> invalid.
		pathStatP->insideThis_MaxNumFats = -1;
		pathStatP->insideDeep_MaxNumFats = -1;
		pathStatP->insideThis_MaxNumThins = -1;
		pathStatP->insideDeep_MaxNumThins = -1;
		pathStatP->insideThis_MaxNumFats_Num = -1;
		pathStatP->insideDeep_MaxNumFats_Num = -1;
		pathStatP->insideThis_MaxNumThins_Num = -1;
		pathStatP->insideDeep_MaxNumThins_Num = -1;
		pathStatP->radiusMin = DBL_MAX;
		pathStatP->radiusMax = -1;
		pathStatP->widthMax  = -1;
		pathStatP->heightMax = -1;
	}  // for( pathStatId ... )
	
	if( tlngP->path == NULL  ||  tlngP->numPathsClosed + tlngP->numPathsOpen == 0 )
		return;

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
	{
		pathP = &(tlngP->path[pathId]);
		pathTypeFound = false;
		for( pathStatId = 0  ;  pathStatId < tlngP->numPathStats  ;  pathStatId++ )
			if( pathP->pathClosed == tlngP->pathStat[pathStatId].pathClosed
			&&  pathP->pathLength == tlngP->pathStat[pathStatId].pathLength
			&&  ( pathP->pathLength != 5  ||  !(pathP->pathClosed)  ||  pathP->pointy == tlngP->pathStat[pathStatId].pointy )  )
				{pathTypeFound = true;   break;}  // Matching
		
		if( pathTypeFound )
		{
			double temp;
			
			pathStatP = &(tlngP->pathStat[ pathStatId ]);
			pathP->pathStatId = pathStatId;
			(pathStatP->numPaths) ++ ;
			
			if(pathStatP->insideThis_MaxNumFats  < pathP->insideThis_NumFats )
				{pathStatP->insideThis_MaxNumFats  = pathP->insideThis_NumFats ;  pathStatP->insideThis_MaxNumFats_Num  = 1;}
			else if( pathStatP->insideThis_MaxNumFats  == pathP->insideThis_NumFats  )
				(pathStatP->insideThis_MaxNumFats_Num) ++;

			if( pathStatP->insideDeep_MaxNumFats  < pathP->insideDeep_NumFats  )
				{pathStatP->insideDeep_MaxNumFats  = pathP->insideDeep_NumFats ;  pathStatP->insideDeep_MaxNumFats_Num  = 1;}
			else if( pathStatP->insideDeep_MaxNumFats  == pathP->insideDeep_NumFats  )
				(pathStatP->insideDeep_MaxNumFats_Num) ++;

			if( pathStatP->insideThis_MaxNumThins < pathP->insideThis_NumThins )
				{pathStatP->insideThis_MaxNumThins = pathP->insideThis_NumThins;  pathStatP->insideThis_MaxNumThins_Num = 1;}
			else if( pathStatP->insideThis_MaxNumThins == pathP->insideThis_NumThins )
				(pathStatP->insideThis_MaxNumThins_Num) ++;

			if( pathStatP->insideDeep_MaxNumThins < pathP->insideDeep_NumThins )
				{pathStatP->insideDeep_MaxNumThins = pathP->insideDeep_NumThins;  pathStatP->insideDeep_MaxNumThins_Num = 1;}
			else if( pathStatP->insideDeep_MaxNumThins == pathP->insideDeep_NumThins )
				(pathStatP->insideDeep_MaxNumThins_Num) ++;
			
			if(pathStatP->radiusMin > pathP->radiusMin) pathStatP->radiusMin = pathP->radiusMin;
			if(pathStatP->radiusMax < pathP->radiusMax) pathStatP->radiusMax = pathP->radiusMax;
			
			temp = tlngP->rhombi[ pathP->xMax_rhId ].xMax - tlngP->rhombi[ pathP->xMin_rhId ].xMin;  if(pathStatP->widthMax  < temp)  pathStatP->widthMax  = temp;
			temp = tlngP->rhombi[ pathP->yMax_rhId ].yMax - tlngP->rhombi[ pathP->yMin_rhId ].yMin;  if(pathStatP->heightMax < temp)  pathStatP->heightMax = temp;
			
			pathStatP->examplePathId = pathId ;
			
			if(pow(pathP->centre.x, 2) + pow(pathP->centre.y, 2) <
			   pow(tlngP->path[ pathStatP->examplePathId ].centre.x, 2) +
			   pow(tlngP->path[ pathStatP->examplePathId ].centre.y, 2)
			   )
				pathStatP->examplePathId = pathId ;  // Choosing path closest to (0,0), for needless precision optimality.
		}  // pathTypeFound
		else
		{
			// New PathStat
			pathStatId = tlngP->numPathStats;
			if( pathStatId >= tlngP->pathStats_NumMax )
			{
				fprintf(stderr, "pathStats_populate(): !!! pathStatId >= pathStats_NumMax=%li; tilingId=%" PRIi8 "\n", tlngP->pathStats_NumMax, tlngP->tilingId);
				fflush(stderr);
				exit(EXIT_FAILURE);
			}  // pathStatId >= tlngP->pathStats_NumMax
			
			pathStatP = &(tlngP->pathStat[pathStatId]);
			
			pathP->pathStatId = pathStatId;
			pathStatP->pathStatId = pathStatId;
			pathStatP->pathClosed = pathP->pathClosed;
			pathStatP->pathLength = pathP->pathLength;
			pathStatP->pointy = (pathP->pathLength == 5  &&  pathP->pathClosed)  ?  pathP->pointy  :  false ;
			pathStatP->pathClosedTypeNum = pathClosedTypeNum(pathStatP->pathClosed,  pathStatP->pathLength,  pathStatP->pointy) ;
			pathStatP->numPaths = 1 ;
			pathStatP->examplePathId = pathId ;
			pathStatP->insideThis_MaxNumFats  = pathP->insideThis_NumFats ;
			pathStatP->insideDeep_MaxNumFats  = pathP->insideDeep_NumFats ;
			pathStatP->insideThis_MaxNumThins = pathP->insideThis_NumThins;
			pathStatP->insideDeep_MaxNumThins = pathP->insideDeep_NumThins;
			pathStatP->insideThis_MaxNumFats_Num   = 1;
			pathStatP->insideDeep_MaxNumFats_Num   = 1;
			pathStatP->insideThis_MaxNumThins_Num  = 1;
			pathStatP->insideDeep_MaxNumThins_Num  = 1;
			pathStatP->radiusMin = pathP->radiusMin;
			pathStatP->radiusMax = pathP->radiusMax;
			pathStatP->widthMax  = tlngP->rhombi[ pathP->xMax_rhId ].xMax  -  tlngP->rhombi[ pathP->xMin_rhId ].xMin;
			pathStatP->heightMax = tlngP->rhombi[ pathP->yMax_rhId ].yMax  -  tlngP->rhombi[ pathP->yMin_rhId ].yMin;
			
			(tlngP->numPathStats) ++ ;
		}  // pathTypeFound
	}  // for( pathId ... )
	
	pathStats_sort(tlngP);
	
	// avg( Sin(18), sin(36)*cos(54) ) = (Sqrt[5] + 3) / 16. And the precision is gratuitous: two d.p. would be as performant.
	double const wantedOffset = 0.327254248593736856025573354295704764715038647475720357766931077838157557852362806213400900523674;
	PathStatId const includePathsUpToRank = 3;
	double temp;
	
	tlngP->xMinSuggestedZoom  =  tlngP->rhombi[ tlngP->xMax_rhId ].xMax;  // Min infeasibly large;
	tlngP->xMaxSuggestedZoom  =  tlngP->rhombi[ tlngP->xMin_rhId ].xMin;  // Max infeasibly negative;
	tlngP->yMinSuggestedZoom  =  tlngP->rhombi[ tlngP->yMax_rhId ].yMax;  // so will be corrected by data.
	tlngP->yMaxSuggestedZoom  =  tlngP->rhombi[ tlngP->yMin_rhId ].yMin;
	
	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId ++ )
	{
		pathP = &(tlngP->path[pathId]);
		if( pathP->pathClosed  &&  pathP->pathStatId < includePathsUpToRank )
		{
			temp = tlngP->rhombi[ pathP->xMin_rhId ].centre.x;  if(tlngP->xMinSuggestedZoom > temp) tlngP->xMinSuggestedZoom = temp;
			temp = tlngP->rhombi[ pathP->xMax_rhId ].centre.y;  if(tlngP->xMaxSuggestedZoom < temp) tlngP->xMaxSuggestedZoom = temp;
			temp = tlngP->rhombi[ pathP->yMin_rhId ].centre.x;  if(tlngP->yMinSuggestedZoom > temp) tlngP->yMinSuggestedZoom = temp;
			temp = tlngP->rhombi[ pathP->yMax_rhId ].centre.y;  if(tlngP->yMaxSuggestedZoom < temp) tlngP->yMaxSuggestedZoom = temp;
		}  // if( pathP->pathStatId < includePathsUpToRank )
		else
			break;
	}  // for( pathId ... )
	tlngP->xMinSuggestedZoom  =  tlngP->xMinSuggestedZoom / tlngP->edgeLength  +  wantedOffset;
	tlngP->xMaxSuggestedZoom  =  tlngP->xMaxSuggestedZoom / tlngP->edgeLength  -  wantedOffset;
	tlngP->yMinSuggestedZoom  =  tlngP->yMinSuggestedZoom / tlngP->edgeLength  +  wantedOffset;
	tlngP->yMaxSuggestedZoom  =  tlngP->yMaxSuggestedZoom / tlngP->edgeLength  -  wantedOffset;
	
	if( tlngP->xMinSuggestedZoom >= tlngP->xMaxSuggestedZoom
	   ||  tlngP->yMinSuggestedZoom >= tlngP->yMaxSuggestedZoom )
	{
		// Small tiling
		tlngP->xMinSuggestedZoom  =  tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength;
		tlngP->xMaxSuggestedZoom  =  tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength;
		tlngP->yMinSuggestedZoom  =  tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength;
		tlngP->yMaxSuggestedZoom  =  tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength;
	}  // ...Min... >= ...Max...
}  // pathStats_populate()
