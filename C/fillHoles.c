// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// fillHoles.c, in PenroseC

#include "penrose.h"

static RhombId rhId_lastSorted;

/*
	A tiling made from only the recursion rule will have, at or near its edges, holes and gaps.
	Using local information, some of these have a unique extension.
	This small set of extensions has two purposes.

	The author is as interested in paths as much as tiles.
	Mostly, the addition of fats can join or complete paths.

	Of course, by adding tiles in small tilings, many tiles are thereby added to descendant
	tilings. Also, the full collection of additions makes the resulting pattern convex, at any
	resolution bigger than an edge length. Indeed it makes a neat rhombus, rather than an
	awkward crinkly shape, only the deep interior of which might help with an actual tiling.

	It seems that some of the rules have been made redundant by the presence of others.
	But if tiles outside an wakard shape were somehow removed, then it might be that,
	for some boundary shapes, rules otherwise redundant become useful again.
	Hence all rules retained. The speed cost is slight.
*/


static inline int8_t fillTypeA(Tiling * const tlngP, RhombId const rhId_A)
{
	/*
	A is a thin, to its south neighbouring only fats.
	A's SE edge touches a fat, B, with nothing on its SE edge. If its NE edge has a thin, that is D.
	A's SW edge touches a fat, C, with nothing on its SW edge. If its NW edge has a thin, that is E.
	Between theese there should be a fat, A-B-C south vertices in common.
	*/

	/*
<svg width='667.78' height='486' viewBox='-333.89 -135 667.78 486' id='EdgeGap_3' xmlns='http://www.w3.org/2000/svg'>

<path stroke='#000' stroke-width='3' stroke-linejoin='round' fill='none' d='M332.39 108L205.43 -66.75L0 0L126.96 174.75Z M-332.39 108L-205.43 -66.75L0 0L-126.96 174.75Z M0 349.5L-126.96 174.75L0 0L126.96 174.75Z'/>

<path stroke='#000' stroke-width='3' stroke-linejoin='round' fill='#CFC' paint-order='fill' d='M0 -133.5L205.43 -66.75L0 0L-205.43 -66.75Z'/>

<path stroke='#000' stroke-width='3' stroke-linejoin='round' fill='#FEE' paint-order='fill' d='M126.96 174.75L332.39 108L205.43 282.75L0 349.5Z M-126.96 174.75L-332.39 108L-205.43 282.75L0 349.5Z'/>

<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='0' y='-66.75'>A</text>
<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='166.19' y='54'>B</text>
<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='-166.19' y='54'>C</text>
<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='166.19' y='228.75'>D?</text>
<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='-166.19' y='228.75'>E?</text>
<text text-anchor='middle' alignment-baseline='central' fill='#090' font-size ='98' x='0' y='174.75'>New</text>

<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='0' y='-111.25'>N</text>
<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='290.84' y='94.5'>N</text>
<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='-290.84' y='94.5'>N</text>
<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='140.04' y='192.75'>N</text>
<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='-140.04' y='192.75'>N</text>
<text text-anchor='middle' alignment-baseline='central' font-size ='34' x='0' y='305.81'>N</text>

</svg>
	*/

	RhombId rhId_B = -1, rhId_C = -1, rhId_D = -1, rhId_E = -1;
	int8_t    nghbrNum;
	Neighbour *nghbrP;

	if( Thin != tlngP->rhombii[rhId_A].physique  ||  tlngP->rhombii[rhId_A].numNeighbours < 2 )  // Required qualities
		return 0;

	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_A].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_A].neighbours[nghbrNum]);
		if( ! nghbrP->touchesN )
		{
			// South
			if( Fat != nghbrP->physique )
				return 0;
			if( nghbrP->touchesE )
				rhId_B = nghbrP->rhId;  // A's SE edge
			else
				rhId_C = nghbrP->rhId;  // A's SW edge
		}  // South
	}  // for( nghbrNum ... )

	if( rhId_B < 0  ||  rhId_C < 0 )  // Both must exist
		return 0;

	// B's neighbours
	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_B].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_B].neighbours[nghbrNum]);
		if( nghbrP->touchesN )
		{
			if( nghbrP->touchesE )  // North-east
			{
				rhId_D = nghbrP->rhId;
				if( tlngP->rhombii[rhId_D].physique != Thin
				||  tlngP->rhombii[rhId_D].neighbours[ nghbrP->nghbrsNghbrNum ].rhId     != rhId_B
				||  tlngP->rhombii[rhId_D].neighbours[ nghbrP->nghbrsNghbrNum ].touchesN != true
				||  tlngP->rhombii[rhId_D].neighbours[ nghbrP->nghbrsNghbrNum ].touchesE != true )
					return 0;
			}  // North-east
			else
			{
				// North-west
				if( Fat != nghbrP->physique )
					return 0;
			}  // North-west
		}  // North
		else
		{
			// South
			if( nghbrP->touchesE )
				return 0;  // South-east neighbour should not yet exist
			else
			{
				if( rhId_A != nghbrP->rhId )
					return 0;
			}  // South-west
		}  // South
	}  // for( nghbrNum ... )

	// C's neighbours
	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_C].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_C].neighbours[nghbrNum]);
		if( nghbrP->touchesN )
		{
			if( nghbrP->touchesE )  // North-east
			{
				if( Fat != nghbrP->physique )
					return 0;
			}  // North-east
			else
			{
				// North-west
				rhId_E = nghbrP->rhId;
				if( tlngP->rhombii[rhId_E].physique != Thin
				||  tlngP->rhombii[rhId_E].neighbours[ nghbrP->nghbrsNghbrNum ].rhId     != rhId_C
				||  tlngP->rhombii[rhId_E].neighbours[ nghbrP->nghbrsNghbrNum ].touchesN != true
				||  tlngP->rhombii[rhId_E].neighbours[ nghbrP->nghbrsNghbrNum ].touchesE != false )
					return 0;
			}  // North-west
		}  // North
		else
		{
			// South
			if( nghbrP->touchesE )
			{
				// South-east
				if( rhId_A != nghbrP->rhId )
					return 0;
			}  // South-east
			else
				return 0;  // South-west neighbour should not yet exist
		}  // South
	}  // for( nghbrNum ... )

	if( points_different_3(tlngP,  tlngP->rhombii[rhId_A].south,  tlngP->rhombii[rhId_B].south,  tlngP->rhombii[rhId_C].south) )
		return 0;  // This test should be redundant

	double const new_S_x = median_3( tlngP->rhombii[rhId_A].south.x,  tlngP->rhombii[rhId_B].south.x,  tlngP->rhombii[rhId_C].south.x);  // Precision fussiness
	double const new_S_y = median_3( tlngP->rhombii[rhId_A].south.y,  tlngP->rhombii[rhId_B].south.y,  tlngP->rhombii[rhId_C].south.y);

	RhombId const rhId_new = rhombus_append(
		tlngP,
		Fat,
		'A' + 1 - 'A',  // filledType
		tlngP->rhombii[rhId_B].east.x  +  tlngP->rhombii[rhId_C].west.x  -  new_S_x,
		tlngP->rhombii[rhId_B].east.y  +  tlngP->rhombii[rhId_C].west.y  -  new_S_y,
		new_S_x,
		new_S_y
	);
	if( rhId_new < 0 )
		return 0;

	twoRhombii_Neighbourify(tlngP, rhId_new, rhId_B);
	twoRhombii_Neighbourify(tlngP, rhId_new, rhId_C);
	if( rhId_D >= 0 )
		twoRhombii_Neighbourify(tlngP, rhId_new, rhId_D);
	if( rhId_E >= 0 )
		twoRhombii_Neighbourify(tlngP, rhId_new, rhId_E);

	return 1;
}  // fillTypeA()



static inline int8_t fillTypeB(Tiling * const tlngP, RhombId const rhId_Start)
{
	// Simple hole: four but not five fats share south corners. The fifth fat would complete the round five path.
	// Start is westernmost fat, so for 0 to 2 next fat is at south-east edge; and for 1 to 3 previous fat is at south-west edge.
	// Must be enclosed: all North edges must neighbour a thin.

	int8_t fatNum, nghbrNum;
	bool thin_NE, thin_NW;
	Neighbour *nghbrP;
	RhombId thin_0 = -1, thin_3 = -1, rhIds[4];

	rhIds[0] = rhId_Start;  rhIds[1] = -1;  rhIds[2] = -1;  rhIds[3] = -1;
	for( fatNum = 0  ;  fatNum < 4  ;  fatNum ++ )
	{
		if( Fat != tlngP->rhombii[rhIds[fatNum]].physique )
			return 0;
		if( tlngP->rhombii[rhIds[fatNum]].numNeighbours != (fatNum == 1 || fatNum == 2  ?  4  :  3) )
			return 0;

		thin_NE = thin_NW = false;
		for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhIds[fatNum]].numNeighbours  ;  nghbrNum ++ )
		{
			nghbrP = &(tlngP->rhombii[rhIds[fatNum]].neighbours[nghbrNum]);
			if( nghbrP->touchesN )
			{
				// North
				if( Fat == nghbrP->physique )  // Must touch thin
					return 0;

				// North, thin

				if( nghbrP->touchesE )
				{
					// NE, thin
					thin_NE = true;
					if( fatNum == 3 )
					{
						thin_3 = nghbrP->rhId;
						if( tlngP->rhombii[thin_3].numNeighbours >= 4 )
							return 0;
					}  // if( fatNum == 3 )
				}  // East
				else
				{
					// NW, thin
					thin_NW = true;
					if( fatNum == 0 )
					{
						thin_0 = nghbrP->rhId;
						if( tlngP->rhombii[thin_0].numNeighbours >= 4 )
							return 0;
					}  // if( fatNum == 0 )
				}  // West
			}  // North
			else
			{
				// S
				if( Fat != nghbrP->physique )  // Must touch fat
					return 0;
				if( nghbrP->touchesE )
				{
					// SE
					if( fatNum < 3 )
							rhIds[fatNum + 1] = nghbrP->rhId;
					else
						return 0;
				}  // East
				else
				{
					// SW
					if( fatNum > 0  &&  rhIds[fatNum - 1] != nghbrP->rhId )
						return 0;
				}  // West
			}  // South
		}  // for( nghbrNum ... )

		if( (!thin_NE)  ||  (!thin_NW) )
			return 0;

	}  // for( fatNum ... )

	if( thin_0 < 0  ||  thin_3 < 0 )  // Should be redundant
		return 0;

	if( points_different_4(tlngP,
		tlngP->rhombii[rhIds[0]].south,
		tlngP->rhombii[rhIds[1]].south,
		tlngP->rhombii[rhIds[2]].south,
		tlngP->rhombii[rhIds[3]].south
	) )
		return 0;

	RhombId const rhId_new = rhombus_append(
		tlngP,
		Fat,
		'B' + 1 - 'A',  // filledType
		avg_2( tlngP->rhombii[thin_0].east.x,  tlngP->rhombii[thin_3].west.x ),
		avg_2( tlngP->rhombii[thin_0].east.y,  tlngP->rhombii[thin_3].west.y ),
		median_4(
			tlngP->rhombii[rhIds[0]].south.x,
			tlngP->rhombii[rhIds[1]].south.x,
			tlngP->rhombii[rhIds[2]].south.x,
			tlngP->rhombii[rhIds[3]].south.x
		),
		median_4(
			tlngP->rhombii[rhIds[0]].south.y,
			tlngP->rhombii[rhIds[1]].south.y,
			tlngP->rhombii[rhIds[2]].south.y,
			tlngP->rhombii[rhIds[3]].south.y
		)
	);
	if( rhId_new < 0 )
		return 0;

	twoRhombii_Neighbourify(tlngP, rhId_new, rhIds[0]);
	twoRhombii_Neighbourify(tlngP, rhId_new, rhIds[3]);
	twoRhombii_Neighbourify(tlngP, rhId_new, thin_0);
	twoRhombii_Neighbourify(tlngP, rhId_new, thin_3);

	return 1;
}  // fillTypeB()



static inline int8_t fillTypeC(Tiling * const tlngP, RhombId const rhId_Orig)
{
	// Fat has three neighbours: two thins at south; north being one fat and one nothing.
	// Missing neighbour must be a fat, its north vertex being original's north vertex.
	// But what is its other neighbour. Must walk anti-clockwise around original's north vertex, traversing either all fats, or two fats and two thins, to each end.
	// End is neighbour of new.

	if( Fat != tlngP->rhombii[rhId_Orig].physique  ||  tlngP->rhombii[rhId_Orig].numNeighbours != 3 )  // Required qualities
		return 0;

	int8_t nghbrNum, numThins = 0;
	Neighbour *nghbrP;
	RhombId rhId_Next = -1;
	bool OrigsNghbrTouchesE = true;  // Assignment to prevent incorrect compiler compiler warning sayig that it might be unassigned.

	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_Orig].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_Orig].neighbours[nghbrNum]);
		if( nghbrP->touchesN )
		{
			// North
			if( Fat == nghbrP->physique )
			{
				if( rhId_Next >= 0 )  // This is second fat, which is disallowed
					return 0;
				rhId_Next = nghbrP->rhId;
				OrigsNghbrTouchesE = nghbrP->touchesE;
			}
			else
				return 0;  // Northerly thins disallowed.
		}
		else
		{
			// South
			if( Thin == nghbrP->physique )
				numThins ++;
			else
				return 0;  // Southerly fats disallowed.
		}
	}  // for( nghbrNum ... )
	if( numThins != 2  ||  rhId_Next < 0 )  // Given that there are 3 neighbours, these conditions might be equivalent.
		return 0;

	short int totalAngle;
	RhombId rhId_Prev, rhId_This = rhId_Orig;
	bool foundNext;
	XY const xyOrigNorth = tlngP->rhombii[rhId_Orig].north;

	for( totalAngle = 0  ;  totalAngle < 144  ;  totalAngle += tlngP->rhombii[rhId_This].physique )
	{
		rhId_Prev = rhId_This;
		rhId_This = rhId_Next;

		foundNext = false;
		for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_This].numNeighbours  ;  nghbrNum ++ )
		{
			nghbrP = &(tlngP->rhombii[rhId_This].neighbours[nghbrNum]);
			rhId_Next = nghbrP->rhId;
			if( rhId_Next == rhId_Prev )
				continue;  // next nghbrNum
			if( nghbrP->physique == Fat )
			{
				if( points_same_2(tlngP,  xyOrigNorth,  tlngP->rhombii[rhId_Next].north) )
				{
					foundNext = true;
					break;
				}
			}  // Fat
			else
			{
				// Thin
				if( points_same_2(tlngP,  xyOrigNorth,  tlngP->rhombii[rhId_Next].east)
				||  points_same_2(tlngP,  xyOrigNorth,  tlngP->rhombii[rhId_Next].west) )
				{
					foundNext = true;
					break;
				}
			}  // Thin
		}  // for( nghbrNum ... )
		if( !foundNext )
			return 0;
	}  // for( totalAngle ... )

	if( Fat != tlngP->rhombii[rhId_Next].physique  ||  totalAngle != 144 )  // Required qualities
		return 0;

	double const new_N_x = avg_2( tlngP->rhombii[rhId_Orig].north.x,  tlngP->rhombii[rhId_Next].north.x);  // Better precision might use all five points.
	double const new_N_y = avg_2( tlngP->rhombii[rhId_Orig].north.y,  tlngP->rhombii[rhId_Next].north.y);

	RhombId const rhId_new = rhombus_append(
		tlngP,
		Fat,
		'C' + 1 - 'A',  // filledType
		new_N_x,
		new_N_y,
		(OrigsNghbrTouchesE
			? tlngP->rhombii[rhId_Orig].west.x + tlngP->rhombii[rhId_Next].east.x
			: tlngP->rhombii[rhId_Orig].east.x + tlngP->rhombii[rhId_Next].west.x) - new_N_x,
		(OrigsNghbrTouchesE
			? tlngP->rhombii[rhId_Orig].west.y + tlngP->rhombii[rhId_Next].east.y
			: tlngP->rhombii[rhId_Orig].east.y + tlngP->rhombii[rhId_Next].west.y) - new_N_y
	);
	if( rhId_new < 0 )
		return 0;

	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_Orig);
	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_Next);
	oneRhombii_Neighbourify(tlngP,  rhId_new,  0,                    rhId_lastSorted,                       true );
	oneRhombii_Neighbourify(tlngP,  rhId_new,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);

	return 1;
}  // fillTypeC()


static inline int8_t fillTypeD(Tiling * const tlngP, RhombId const rhId_Thin1)
{
	// Two neighbouring thins must meet at their south vertex.
	// Also touching their south vertex must be the south vertex of a fat.
	// If missing, insert.
	// But subsequent neighbourification is difficult, so instead whole tiling reneighbourified.

	if( Thin != tlngP->rhombii[rhId_Thin1].physique  ||  tlngP->rhombii[rhId_Thin1].numNeighbours == 4 )
		return 0;

	int8_t nghbrNum;
	Neighbour *nghbrP;
	RhombId rhId_Thin2 = -1;
	bool touch1East = true;  // Redundant initialisation, but prevents compiler warning about possibly uninitialised.

	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_Thin1].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_Thin1].neighbours[nghbrNum]);
		if( ! nghbrP->touchesN )
		{
			// South
			if( Thin == nghbrP->physique )
			{
				rhId_Thin2 = nghbrP->rhId;
				touch1East = nghbrP->touchesE;
			}  // Thin
			else
				return 0;  // Fat
		}  // South
	}

	if( rhId_Thin2 < 0 )
		return 0;

	double const new_south_x = avg_2(tlngP->rhombii[rhId_Thin1].south.x, tlngP->rhombii[rhId_Thin2].south.x);
	double const new_south_y = avg_2(tlngP->rhombii[rhId_Thin1].south.y, tlngP->rhombii[rhId_Thin2].south.y);

	double const new_north_x = (touch1East
		? tlngP->rhombii[rhId_Thin1].west.x + tlngP->rhombii[rhId_Thin2].east.x
		: tlngP->rhombii[rhId_Thin1].east.x + tlngP->rhombii[rhId_Thin2].west.x
	) - new_south_x;

	double const new_north_y = (touch1East
		? tlngP->rhombii[rhId_Thin1].west.y + tlngP->rhombii[rhId_Thin2].east.y
		: tlngP->rhombii[rhId_Thin1].east.y + tlngP->rhombii[rhId_Thin2].west.y
	) - new_south_y;

	RhombId const rhId_new = rhombus_append(
		tlngP,
		Fat,
		'D' + 1 - 'A',  // filledType
		new_north_x,
		new_north_y,
		new_south_x,
		new_south_y
	);
	if( rhId_new < 0 )
		return 0;
	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_Thin1);
	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_Thin2);
	oneRhombii_Neighbourify(tlngP,  rhId_new,  0,                    rhId_lastSorted,                       true );
	oneRhombii_Neighbourify(tlngP,  rhId_new,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);

	return 1;
}  // fillTypeD()


static inline int8_t fillTypeE(Tiling * const tlngP, RhombId const rhId_Start)
{
	// Five fats, sharing their south vertex, so this is a round 5-path, which is travelled eastwards.
	// Should be surrounded by 5 thins, fat NE touching thin NE, and fat NW touching thin NW.
	// But subsequent neighbourification is difficult, so instead whole tiling reneighbourified.

	if( Fat != tlngP->rhombii[rhId_Start].physique
	||  tlngP->rhombii[rhId_Start].numNeighbours <= 1
	||  tlngP->rhombii[rhId_Start].north.x <= tlngP->rhombii[rhId_Start].south.x
	||  tlngP->rhombii[rhId_Start].north.y <= tlngP->rhombii[rhId_Start].south.y )  // Orientation constraint prevents repeating same loop multiple times
		return 0;

	int8_t nghbrNum, fatNum, fatNumNext;
	RhombId rhIds[5];
	bool thin_NE[5], thin_NW[5],  allFiveThinsPresent;
	Neighbour *nghbrP;

	rhIds[0] = rhId_Start;  rhIds[1] = -1;  rhIds[2] = -1;  rhIds[3] = -1;  rhIds[4] = -1;
	allFiveThinsPresent = true;
	for( fatNum = 0  ;  fatNum < 5  ;  fatNum ++ )
	{
		if( rhIds[fatNum] < 0
		||  Fat != tlngP->rhombii[rhIds[fatNum]].physique
		||  tlngP->rhombii[rhIds[fatNum]].numNeighbours <= 1
		||  ( fatNum > 0  &&  points_different_2(tlngP, tlngP->rhombii[rhId_Start].south, tlngP->rhombii[rhIds[fatNum]].south) )  )
			return 0;

		thin_NE[fatNum] = false;
		thin_NW[fatNum] = false;

		for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhIds[fatNum]].numNeighbours  ;  nghbrNum ++ )
		{
			nghbrP = &(tlngP->rhombii[rhIds[fatNum]].neighbours[nghbrNum]);

			if( nghbrP->touchesN )
			{
				// North
				if( Fat == nghbrP->physique )  // Cannot neighbour fat
					return 0;

				// North, thin
				if( nghbrP->touchesE )
					thin_NE[fatNum] = true;
				else
					thin_NW[fatNum] = true;
			}  // North
			else
			{
				// South
				if( Fat != nghbrP->physique )  // Must touch fat
					return 0;

				if( nghbrP->touchesE )
				{
					// South-East
					if( fatNum < 4 )
						rhIds[fatNum + 1] = nghbrP->rhId;
					else
					{
						if( rhIds[0] != nghbrP->rhId )
						{
							printf("fillTypeE(): !!! impossibility !!! #####\n");
							exit(EXIT_FAILURE);
						}
					}  // FatNum == 4
				}  // East
				else
				{
					// South-West, fat
					if( fatNum > 0  &&  rhIds[fatNum - 1] != nghbrP->rhId )
						return 0;
				}  // West
			}  // South
		}  // for( nghbrNum ... )
		if( !thin_NE[fatNum]  ||  !thin_NW[fatNum] )
			allFiveThinsPresent = false;
	}  // for( fatNum ... )

	if( allFiveThinsPresent )
		return 0;

	double new_x, new_y;

	int8_t numInserted = 0;

	for( fatNum = 0  ;  fatNum < 5  ;  fatNum ++ )
	{
		fatNumNext = (fatNum + 1) % 5;
		if( !thin_NE[fatNum]  &&  !thin_NW[fatNumNext] )
		{
			new_x = avg_2( tlngP->rhombii[rhIds[fatNum]].east.x,  tlngP->rhombii[rhIds[fatNumNext]].west.x );
			new_y = avg_2( tlngP->rhombii[rhIds[fatNum]].east.y,  tlngP->rhombii[rhIds[fatNumNext]].west.y );
			RhombId const rhId_new = rhombus_append(
				tlngP,
				Thin,
				'E' + 1 - 'A',  // filledType
				new_x,
				new_y,
				tlngP->rhombii[rhIds[fatNum]].north.x + tlngP->rhombii[rhIds[fatNumNext]].north.x - new_x,
				tlngP->rhombii[rhIds[fatNum]].north.y + tlngP->rhombii[rhIds[fatNumNext]].north.y - new_y
			);
			if( rhId_new >= 0 )
			{
				numInserted ++;
				twoRhombii_Neighbourify(tlngP, rhId_new, rhIds[fatNum    ]);
				twoRhombii_Neighbourify(tlngP, rhId_new, rhIds[fatNumNext]);

				oneRhombii_Neighbourify(tlngP,  fatNum    ,  0,  rhId_lastSorted,  true);
				oneRhombii_Neighbourify(tlngP,  fatNumNext,  0,  rhId_lastSorted,  true);
				oneRhombii_Neighbourify(tlngP,  fatNum    ,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);
				oneRhombii_Neighbourify(tlngP,  fatNumNext,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);
			} // if( rhId_new >= 0 )
		}  // !thin...
	}  // for( fatNum ... )
	return numInserted;
}  // fillTypeE()



static inline int8_t fillTypeF(Tiling * const tlngP, RhombId const rhId_Start)
{
	// Start with a fat with <=3 neighbours, and no north-west neighbour.
	// Traverse around the original fat's north vertex. Could be traversing in either direction.
	// A fat met in traversing has the same north vertex as original.
	// A thin met in traversing has a matching east or west matching original's north.
	// Count number thins with east matching original north, num thins with west matching original's north.
	// Score 36 for a thin, 72 for a fat. If total is 324, then missing just a thin.
	// New thin has the east or west matching original north according to which is one fewer.
	// But subsequent neighbourification is difficult, so instead whole tiling reneighbourified.

	if( Fat != tlngP->rhombii[rhId_Start].physique  ||  tlngP->rhombii[rhId_Start].numNeighbours == 4 )
		return 0;

	XY const origNorth = tlngP->rhombii[rhId_Start].north;
	short int physiqueTotal = 0;
	int8_t nghbrNum, numThinsMatchingEast = 0, numThinsMatchingWest = 0;
	RhombId rhId_Prev,  rhId_This = - 1,  rhId_Next = rhId_Start;
	Neighbour *nghbrP;
	bool thereIsGoodNghbr;

	while( true )
	{
		rhId_Prev = rhId_This;
		rhId_This = rhId_Next;

		physiqueTotal += tlngP->rhombii[rhId_This].physique;
		if( physiqueTotal > 324 )
			return 0;

		thereIsGoodNghbr = false;
		for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_This].numNeighbours  ;  nghbrNum ++ )
		{
			nghbrP = &(tlngP->rhombii[rhId_This].neighbours[nghbrNum]);
			rhId_Next = nghbrP->rhId;
			if( rhId_Next != rhId_Prev)
			{
				if( Fat == tlngP->rhombii[rhId_Next].physique )
				{
					// Fat
					if( points_same_2(tlngP, origNorth, tlngP->rhombii[rhId_Next].north) )
					{
						thereIsGoodNghbr = true;
						break;  // for( nghbrNum ... )
					}
				}  // Fat
				else
				{
					// Thin
					if( points_same_2(tlngP, origNorth, tlngP->rhombii[rhId_Next].east) )
					{
						numThinsMatchingEast ++;
						thereIsGoodNghbr = true;
						break;  // for( nghbrNum ... )
					}  // orig north == this east
					if( points_same_2(tlngP, origNorth, tlngP->rhombii[rhId_Next].west) )
					{
						numThinsMatchingWest ++;
						thereIsGoodNghbr = true;
						break;  // for( nghbrNum ... )
					}  // orig north == this east
				}  // Thin
			}  // if( rhId_Next != rhId_Prev)
		}  // for( nghbrNum ... )

		if( !thereIsGoodNghbr )
		{
			if( physiqueTotal == 360 - 36 )
				break;  // while( true )
			else
				return 0;
		}  // if( !thereIsGoodNghbr )

	}  // while( true )

	if( 1 != abs(numThinsMatchingEast - numThinsMatchingWest)  ||  Thin != tlngP->rhombii[rhId_This].physique )
	{
		fprintf(stderr,
			"fillTypeF(): !!! impossibility !!!  rhId_Start = %li;  rhId_Prev = %li;  rhId_This = %li;  rhId_Next = %li;  numThinsMatchingEast = %" PRIi8 ";  numThinsMatchingWest = %" PRIi8 ";  physiqueTotal = %hi  #####\n",
			rhId_Start, rhId_Prev, rhId_This, rhId_Next, numThinsMatchingEast, numThinsMatchingWest, physiqueTotal
		);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}  // Two impossibilities

	RhombId const rhId_new = rhombus_append(
		tlngP,
		Thin,
		'F' + 1 - 'A',  // filledType
		numThinsMatchingEast > numThinsMatchingWest  ?  tlngP->rhombii[rhId_Start].west.x  :  tlngP->rhombii[rhId_Start].east.x,
		numThinsMatchingEast > numThinsMatchingWest  ?  tlngP->rhombii[rhId_Start].west.y  :  tlngP->rhombii[rhId_Start].east.y,
		tlngP->rhombii[rhId_This].south.x,
		tlngP->rhombii[rhId_This].south.y
	);
	if( rhId_new < 0 )
		return 0;

	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_Start);
	twoRhombii_Neighbourify(tlngP,  rhId_new,  rhId_This );
	oneRhombii_Neighbourify(tlngP,  rhId_new,  0,                    rhId_lastSorted,                       true );
	oneRhombii_Neighbourify(tlngP,  rhId_new,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);

	return 1;
}  // fillTypeF()



static inline int8_t fillTypeG(Tiling * const tlngP, RhombId const rhId_Orig)
{
	// For every thin, both north edges touch fats that touch each other. Fats' norths touch thin; fats' souths share a vertex.
	// But subsequent neighbourification is difficult, so instead whole tiling reneighbourified.

	if( Thin != tlngP->rhombii[rhId_Orig].physique  ||  tlngP->rhombii[rhId_Orig].numNeighbours == 4 )  // Required qualities
		return 0;

	int8_t nghbrNum;
	RhombId rhId_E = -1, rhId_W = -1;
	int8_t numInserted = 0;
	Neighbour *nghbrP;

	for( nghbrNum = 0  ;  nghbrNum < tlngP->rhombii[rhId_Orig].numNeighbours  ;  nghbrNum ++ )
	{
		nghbrP = &(tlngP->rhombii[rhId_Orig].neighbours[nghbrNum]);
		if( nghbrP->touchesN )
		{
			if( Fat == nghbrP->physique )
			{
				if( nghbrP->touchesE )
					rhId_E = nghbrP->rhId;
				else
					rhId_W = nghbrP->rhId;
			}
			else
			{
				// North thin: impossible.
				fprintf(stderr, "fillTypeG(): !!! North thin = impossible !!! #####\n");
				fflush(stderr);
				exit(EXIT_FAILURE);
			}  // Fat or Thin
		}  // North
	}  // for( nghbrNum ... )

	if( rhId_E >= 0  &&  rhId_W >= 0 )  // Both already exist
		return 0;

	// debug
/*
	printf("fillTypeG(), tilingId=%02" PRIi8 ",  CentreX=%0.4lf,  CentreY=%0.4lf,  rhId_Orig=%li,  rhId_E=%li,  rhId_W=%li\n",
		tlngP->tilingId,
		(tlngP->rhombii[rhId_Orig].north.x + tlngP->rhombii[rhId_Orig].south.x) / 2,
		(tlngP->rhombii[rhId_Orig].north.y + tlngP->rhombii[rhId_Orig].south.y) / 2,
		rhId_Orig, rhId_E, rhId_W
	);
*/
	double const new_x = tlngP->rhombii[rhId_Orig].north.x * GoldenRatioSquare  -  GoldenRatio * tlngP->rhombii[rhId_Orig].south.x;
	double const new_y = tlngP->rhombii[rhId_Orig].north.y * GoldenRatioSquare  -  GoldenRatio * tlngP->rhombii[rhId_Orig].south.y;

	if( rhId_E < 0 )
	{
		rhId_E = rhombus_append(
			tlngP,
			Fat,
			'G' + 1 - 'A',  // filledType
			tlngP->rhombii[rhId_Orig].east.x,
			tlngP->rhombii[rhId_Orig].east.y,
			new_x,
			new_y
		);
		if( rhId_E >= 0 )
		{
			numInserted ++;
			twoRhombii_Neighbourify(tlngP,  rhId_E,  rhId_Orig);
			oneRhombii_Neighbourify(tlngP,  rhId_E,  0,  rhId_lastSorted,  true);
			oneRhombii_Neighbourify(tlngP,  rhId_E,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);
		}  // if( rhId_E >= 0 )
	}  // if( rhId_E < 0 )

	if( rhId_W < 0 )
	{
		rhId_W = rhombus_append(
			tlngP,
			Fat,
			'G' + 1 - 'A',  // filledType
			tlngP->rhombii[rhId_Orig].west.x,
			tlngP->rhombii[rhId_Orig].west.y,
			new_x,
			new_y
		);
		if( rhId_W >= 0 )
		{
			numInserted ++;
			twoRhombii_Neighbourify(tlngP, rhId_W, rhId_Orig);
			if( rhId_E >= 0 )
				twoRhombii_Neighbourify(tlngP,  rhId_W,  rhId_E);
			oneRhombii_Neighbourify(tlngP,  rhId_W,  0,  rhId_lastSorted,  true);
			oneRhombii_Neighbourify(tlngP,  rhId_W,  rhId_lastSorted + 1,  tlngP->numFats + tlngP->numThins - 1,  false);
		}  // if( rhId_W >= 0 )
	}  // if( rhId_W < 0 )

	return numInserted;
}  // fillTypeG()




void holesFill(Tiling * const tlngP)
{
	RhombId rhId; 
	long int numFilledTypeA = 0, numFilledTypeB = 0, numFilledTypeC = 0, numFilledTypeD = 0, numFilledTypeE = 0, numFilledTypeF = 0, numFilledTypeG = 0;
	bool anyChanges, anyChangesThisType;
	int8_t numInserted;
	const bool debug_holesFill = false;

	rhId_lastSorted = tlngP->numFats + tlngP->numThins;

	do {
		anyChanges = false;


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +A\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeA(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeA += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
			printf("holesFill(), A: tilingId=%" PRIi8 ", numFilledTypeA = %li\n", tlngP->tilingId, numFilledTypeA);
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -A\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +B\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeB(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeB += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
			printf("holesFill(), B: tilingId=%" PRIi8 ", numFilledTypeB = %li\n", tlngP->tilingId, numFilledTypeB);
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -B\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +C\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeC(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeC += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
			printf("holesFill(), C: tilingId=%" PRIi8 ", numFilledTypeC = %li\n", tlngP->tilingId, numFilledTypeC);
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -C\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +D\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeD(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeD += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
		{
			printf("holesFill(), D: tilingId=%" PRIi8 ", numFilledTypeD = %li\n", tlngP->tilingId, numFilledTypeD);
			neighbours_populate(tlngP);
		}  // if( anyChangesThisType )
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -D\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +E\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeE(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeE += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
		{
			printf("holesFill(), E: tilingId=%" PRIi8 ", numFilledTypeE = %li\n", tlngP->tilingId, numFilledTypeE);
			neighbours_populate(tlngP);
		}  // if( anyChangesThisType )
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -E\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +F\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeF(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeF += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
		{
			printf("holesFill(), F: tilingId=%" PRIi8 ", numFilledTypeF = %li\n", tlngP->tilingId, numFilledTypeF);
			neighbours_populate(tlngP);
		}  // if( anyChangesThisType )
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -F\n", tlngP->tilingId);


		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  +G\n", tlngP->tilingId);
		anyChangesThisType = false;
		for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
		{
			numInserted = fillTypeG(tlngP, rhId);
			if( numInserted > 0 )
				{numFilledTypeG += numInserted;  anyChanges = anyChangesThisType = true;}
		}  // for( rhId ... )
		if( anyChangesThisType )
		{
			printf("holesFill(), G: tilingId=%" PRIi8 ", numFilledTypeG = %li\n", tlngP->tilingId, numFilledTypeG);
			rhombii_purgeDuplicates(tlngP);  // Two thins, touching at a vertex, can create overlapping fats.
			neighbours_populate(tlngP);
		}  // if( anyChangesThisType )
		if( debug_holesFill )   printf("holesFill(): tilingId=%02" PRIi8 ",  -G\n", tlngP->tilingId);


	} while(anyChanges);

	printf(
		"holesFill(): tilingId=%" PRIi8 ", extras = [ %li, %li, %li, %li, %li, %li, %li ]; #fats=%li; #thins=%li\n",
		tlngP->tilingId,
		numFilledTypeA,  numFilledTypeB,  numFilledTypeC,  numFilledTypeD,  numFilledTypeE,  numFilledTypeF,  numFilledTypeG,
		tlngP->numFats,  tlngP->numThins
	);  fflush(stdout);

}  // holesFill()
