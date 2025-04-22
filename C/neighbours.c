// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// neighbours.c, in PenroseC

#include "penrose.h"

void oneRhombi_Neighbourify(
	Tiling * const tlngP,
	RhombId const rhId_This,
	RhombId rhId_Start,
	RhombId const rhId_End,
	bool const isSortedWithin
)
{
	register Rhombus * const rhP_This = &(tlngP->rhombi[rhId_This]);

	if( rhP_This->numNeighbours >= 4 )
		return;

	const double threshold = tlngP->edgeLength / 16;

	if( isSortedWithin )
	{
		RhombId rhId_Start_Max = rhId_End;
		RhombId rhId_Start_Guess;
		for(  ;  rhId_Start_Max > 1 + rhId_Start  ;  )
		{
			rhId_Start_Guess = (rhId_Start_Max / 2)  +  (rhId_Start / 2)  +  (rhId_Start % 2);  // Mid, slightly robust to overflow
			if( tlngP->rhombi[rhId_Start_Guess].centre.y < rhP_This->centre.y - threshold )
				rhId_Start = rhId_Start_Guess;
			else
				rhId_Start_Max = rhId_Start_Guess;
		}  // 'while()'
	}  // if( isSortedWithin )

	register RhombId rhId;
	register Rhombus * rhP;
	for( rhId = rhId_Start  ;  rhId <= rhId_End  ;  rhId ++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( isSortedWithin  &&  rhP->centre.y > rhP_This->centre.y + threshold )
			break;
		if( rhP->numNeighbours < 4  // Quick test, failed by most, so speed-optimal to be first.
		&&  fabs(rhP->centre.y - rhP_This->centre.y) <= threshold  // test could be passed to points_same_2(),
		&&  fabs(rhP->centre.x - rhP_This->centre.x) <= threshold  // but faster to compute threshold only once.
		&&  rhId_This != rhId )  // Quick test, but passed by almost all, so speed-optimal to be last.
		{
			if( rhP->neighbours[0].rhId == rhId_This  // Test safe as all neighbours[].rhId initialised to -1.
			||  rhP->neighbours[1].rhId == rhId_This
			||  rhP->neighbours[2].rhId == rhId_This )  // Can't be 4 neighbours.
				continue;

			twoRhombi_Neighbourify(tlngP, rhId_This, rhId);
		}  // if()
	}  // for( rhId ... )
}  // oneRhombi_Neighbourify()



void twoRhombi_Neighbourify(Tiling * const tlngP,  RhombId const rhId_A,  RhombId const rhId_B)
{
	if( rhId_A < 0 )
		{printf("twoRhombi_Neighbourify(): !!! Error, sub-zero rhId_A,"    " in twoRhombi_Neighbourify(tilingId=%" PRIi8 ",  rhId_A=%li,  rhId_B=%li) !!!\n", tlngP->tilingId, rhId_A, rhId_B);  fflush(stdout);   return;}
	if( rhId_B < 0 )
		{printf("twoRhombi_Neighbourify(): !!! Error, sub-zero rhId_B,"    " in twoRhombi_Neighbourify(tilingId=%" PRIi8 ",  rhId_A=%li,  rhId_B=%li) !!!\n", tlngP->tilingId, rhId_A, rhId_B);  fflush(stdout);   return;}
	if( rhId_A == rhId_B)
		{printf("twoRhombi_Neighbourify(): !!! Error, duplicated rhombus," " in twoRhombi_Neighbourify(tilingId=%" PRIi8 ",  rhId_A=%li,  rhId_B=%li) !!!\n", tlngP->tilingId, rhId_A, rhId_B);  fflush(stdout);   return;}

	Rhombus * const rhP_A = &(tlngP->rhombi[rhId_A]);
	Rhombus * const rhP_B = &(tlngP->rhombi[rhId_B]);
	bool edge_A_N = true, edge_A_E = true, edge_B_N = true, edge_B_E = true;  // Initialisation to suppress erroneous compiler warning about possibly not set.
	int8_t pointsSame = 0;

	do {

		do {
			if( points_same_2(tlngP,  rhP_A->north,  rhP_B->north) ) {edge_A_N = true ;  edge_B_N = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->north,  rhP_B->south) ) {edge_A_N = true ;  edge_B_N = false;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->north,  rhP_B->east ) ) {edge_A_N = true ;  edge_B_E = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->north,  rhP_B->west ) ) {edge_A_N = true ;  edge_B_E = false;  pointsSame++;  break;}
		} while(false);  // Minimal break'able loop construct

		do {
			if( points_same_2(tlngP,  rhP_A->south,  rhP_B->north) ) {edge_A_N = false;  edge_B_N = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->south,  rhP_B->south) ) {edge_A_N = false;  edge_B_N = false;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->south,  rhP_B->east ) ) {edge_A_N = false;  edge_B_E = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->south,  rhP_B->west ) ) {edge_A_N = false;  edge_B_E = false;  pointsSame++;  break;}
		} while(false);  // Minimal break'able loop construct

		if( pointsSame == 2 )
			break;

		do {
			if( points_same_2(tlngP,  rhP_A->east ,  rhP_B->north) ) {edge_A_E = true ;  edge_B_N = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->east ,  rhP_B->south) ) {edge_A_E = true ;  edge_B_N = false;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->east ,  rhP_B->east ) ) {edge_A_E = true ;  edge_B_E = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->east ,  rhP_B->west ) ) {edge_A_E = true ;  edge_B_E = false;  pointsSame++;  break;}
		} while(false);  // Minimal break'able loop construct

		if( pointsSame == 2 )
			break;

		do {
			if( points_same_2(tlngP,  rhP_A->west ,  rhP_B->north) ) {edge_A_E = false;  edge_B_N = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->west ,  rhP_B->south) ) {edge_A_E = false;  edge_B_N = false;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->west ,  rhP_B->east ) ) {edge_A_E = false;  edge_B_E = true ;  pointsSame++;  break;}
			if( points_same_2(tlngP,  rhP_A->west ,  rhP_B->west ) ) {edge_A_E = false;  edge_B_E = false;  pointsSame++;  break;}
		} while(false);  // Minimal break'able loop construct

	} while(false);  // Minimal break'able loop construct

	if( pointsSame != 2 )
		{fprintf(stderr, "!!! Error, pointsSame=%" PRIi8 ", in twoRhombi_Neighbourify(tlngId=%" PRIi8 ",  %li,  %li) !!!\n", pointsSame, tlngP->tilingId, rhId_A, rhId_B);  fflush(stderr);   exit(EXIT_FAILURE);}

	Neighbour *nghbrP;

	nghbrP = &(rhP_A->neighbours[ rhP_A->numNeighbours ]);
	nghbrP->touchesN       = edge_A_N;
	nghbrP->touchesE       = edge_A_E;
	nghbrP->rhId           = rhId_B;
	nghbrP->physique       = rhP_B->physique;
	nghbrP->nghbrsNghbrNum = rhP_B->numNeighbours;
	nghbrP->pathId         = -1;
	nghbrP->withinPathNum  = -1;

	nghbrP = &(rhP_B->neighbours[ rhP_B->numNeighbours ]);
	nghbrP->touchesN       = edge_B_N;
	nghbrP->touchesE       = edge_B_E;
	nghbrP->rhId           = rhId_A;
	nghbrP->physique       = rhP_A->physique;
	nghbrP->nghbrsNghbrNum = rhP_A->numNeighbours;
	nghbrP->pathId         = -1;
	nghbrP->withinPathNum  = -1;

	(rhP_A->numNeighbours) ++;
	(rhP_B->numNeighbours) ++;
}  // twoRhombi_Neighbourify()


bool neighbours_error_true(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFileP) {return true;}


void neighbours_populate(Tiling * const tlngP)
{
	RhombId      rhId1, rhId2;
	double       xDiff, yDiff;

	double const thresholdNeighbour = 0.9511 * tlngP->edgeLength;  // slightly bigger than Cos(18 degrees) = sqrt(10+2*sqrt(5))/4 ~= 0.951056516295
	double const thresholdNeighbourSqrd = thresholdNeighbour * thresholdNeighbour;

	for( rhId1 = 0  ;  rhId1 < tlngP->numFats + tlngP->numThins  ;  rhId1++ )
	{
		tlngP->rhombi[rhId1].numNeighbours = 0;  // Reset previous neighbouring, if any
		tlngP->rhombi[rhId1].neighbours[0].rhId = -1;
		tlngP->rhombi[rhId1].neighbours[1].rhId = -1;
		tlngP->rhombi[rhId1].neighbours[2].rhId = -1;
		tlngP->rhombi[rhId1].neighbours[3].rhId = -1;
	}

	rhombi_sort(tlngP,  &rhombiGt_ByY,  false);

	for( rhId1 = 1  ;  rhId1 < tlngP->numFats + tlngP->numThins  ;  rhId1++ )
	{
		for( rhId2 = rhId1 - 1  ;  rhId2 >= 0  ;  rhId2-- )
		{
			yDiff =
				tlngP->rhombi[rhId1].centre.y -
				tlngP->rhombi[rhId2].centre.y ;

			if( yDiff > thresholdNeighbour )
				break;

			xDiff =
				tlngP->rhombi[rhId1].centre.x -
				tlngP->rhombi[rhId2].centre.x ;

			if( xDiff * xDiff  +  yDiff * yDiff  <=  thresholdNeighbourSqrd )
			{
				if( tlngP->rhombi[rhId1].numNeighbours >= 4
				||  tlngP->rhombi[rhId2].numNeighbours >= 4 )
				{
					extern char scratchString[];
					FILE * fp;
					unsigned long int numLinesThisFileP;
					unsigned long long int numCharsThisFileP;

					fprintf(stderr,
						"\n\nneighbours_populate(): error, need to connect two rhombi, but >=1 already has >=4 neighbours: tilingId=%" PRIi8 ";\n"
						"\trhId1=%li, numNeighbours=%hi = [%li,%li,%li,%li], centre = (%0.9lf, %0.9lf);\n"
						"\trhId2=%li, numNeighbours=%hi = [%li,%li,%li,%li], centre = (%0.9lf, %0.9lf).\n"
						"Too many neighbours likely to be caused by duplicate rhombi. "
						"This likely because holesFill() has made the same rhombus twice. "
						"If a type of filling is known to do that, as one is, then, after, it should call rhombi_purgeDuplicates(). "
						"Believed that this is done sufficiently, but the appearance of this error suggests some optimism of belief. "
						"Ouch!\n\n",
						tlngP->tilingId,
						rhId1,  tlngP->rhombi[rhId1].numNeighbours,  tlngP->rhombi[rhId1].neighbours[0].rhId,  tlngP->rhombi[rhId1].neighbours[1].rhId,  tlngP->rhombi[rhId1].neighbours[2].rhId,  tlngP->rhombi[rhId1].neighbours[3].rhId,  tlngP->rhombi[rhId1].centre.x,  tlngP->rhombi[rhId1].centre.y,
						rhId2,  tlngP->rhombi[rhId2].numNeighbours,  tlngP->rhombi[rhId2].neighbours[0].rhId,  tlngP->rhombi[rhId2].neighbours[1].rhId,  tlngP->rhombi[rhId2].neighbours[2].rhId,  tlngP->rhombi[rhId2].neighbours[3].rhId,  tlngP->rhombi[rhId2].centre.x,  tlngP->rhombi[rhId2].centre.y
					);
					fflush(stderr);
					sprintf(scratchString, "%serror_neighbours_populate.tsv", tlngP->filePath);
					fp = fopen(scratchString, "w");

					tiling_export(
						fp,
						TSV,
						&neighbours_error_true,
						tlngP,
						0,  // indentDepth,
						false,  // notLast,
						tlngP->tilingId,
						&numLinesThisFileP,
						&numCharsThisFileP
					);
					fflush(fp);
					fclose(fp);
					exit(EXIT_FAILURE);
				}

				twoRhombi_Neighbourify(tlngP,  rhId1,  rhId2);
			}  // So close that must be neighbours

		}  // for( rhId2 ... )
	}  // for( rhId1 ... )

}  // neighbours_populate
