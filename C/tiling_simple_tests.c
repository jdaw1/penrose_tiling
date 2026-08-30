// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// tiling_simple_tests.c, in PenroseC

#include "penrose.h"

bool tiling_simple_tests(const Tiling * const tlngP)
{
	RhombId rhId;
	Rhombus *rhP;
	bool problem = false;

	// For each of the rhombi, check that all four edges are of length very close to 1.
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( fabs( (pow(rhP->north.x - rhP->east.x, 2) + pow(rhP->north.y - rhP->east.y, 2)) / pow(tlngP->edgeLength,2)  -  1 ) > 0.000000001 ) // One part in a billion.
			{problem=true;  fprintf(stderr, "!!! Error !!! tiling_initial(): rhId=%li, north-east edgeLength not 1.\n", rhId);}
		if( fabs( (pow(rhP->north.x - rhP->west.x, 2) + pow(rhP->north.y - rhP->west.y, 2)) / pow(tlngP->edgeLength,2)  -  1 ) > 0.000000001 )
			{problem=true;  fprintf(stderr, "!!! Error !!! tiling_initial(): rhId=%li, north-west edgeLength not 1.\n", rhId);}
		if( fabs( (pow(rhP->south.x - rhP->east.x, 2) + pow(rhP->south.y - rhP->east.y, 2)) / pow(tlngP->edgeLength,2)  -  1 ) > 0.000000001 )
			{problem=true;  fprintf(stderr, "!!! Error !!! tiling_initial(): rhId=%li, south-east edgeLength not 1.\n", rhId);}
		if( fabs( (pow(rhP->south.x - rhP->west.x, 2) + pow(rhP->south.y - rhP->west.y, 2)) / pow(tlngP->edgeLength,2)  -  1 ) > 0.000000001 )
			{problem=true;  fprintf(stderr, "!!! Error !!! tiling_initial(): rhId=%li, south-west edgeLength not 1.\n", rhId);}
	}  // for( rhId ... )
	if( problem )
		return false;

	// For each of the rhombi, check that all four angles are correct. No need to divide by lengths, as they 1.
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )
	{
		rhP = &(tlngP->rhombi[rhId]);
		if( Fat == rhP->physique )
		{
			if( fabs( (rhP->east.x - rhP->north.x)*(rhP->west.x - rhP->north.x) + (rhP->east.y - rhP->north.y)*(rhP->west.y - rhP->north.y) - Cos72 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): fat rhId=%li, north angle not 72 degrees.\n", rhId);}
			if( fabs( (rhP->east.x - rhP->south.x)*(rhP->west.x - rhP->south.x) + (rhP->east.y - rhP->south.y)*(rhP->west.y - rhP->south.y) - Cos72 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): fat rhId=%li, south angle not 72 degrees.\n", rhId);}
			if( fabs( (rhP->north.x - rhP->east.x)*(rhP->south.x - rhP->east.x) + (rhP->north.y - rhP->east.y)*(rhP->south.y - rhP->east.y) + Cos72 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): fat rhId=%li, east angle not 108 degrees.\n", rhId);}
			if( fabs( (rhP->north.x - rhP->west.x)*(rhP->south.x - rhP->west.x) + (rhP->north.y - rhP->west.y)*(rhP->south.y - rhP->west.y) + Cos72 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): fat rhId=%li, west angle not 108 degrees.\n", rhId);}
		}
		else
		{
			if( fabs( (rhP->east.x - rhP->north.x)*(rhP->west.x - rhP->north.x) + (rhP->east.y - rhP->north.y)*(rhP->west.y - rhP->north.y) + Cos36 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): thin rhId=%li, north angle not 144 degrees.\n", rhId);}
			if( fabs( (rhP->east.x - rhP->south.x)*(rhP->west.x - rhP->south.x) + (rhP->east.y - rhP->south.y)*(rhP->west.y - rhP->south.y) + Cos36 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): thin rhId=%li, south angle not 144 degrees.\n", rhId);}
			if( fabs( (rhP->north.x - rhP->east.x)*(rhP->south.x - rhP->east.x) + (rhP->north.y - rhP->east.y)*(rhP->south.y - rhP->east.y) - Cos36 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): thin rhId=%li, east angle not 36 degrees.\n", rhId);}
			if( fabs( (rhP->north.x - rhP->west.x)*(rhP->south.x - rhP->west.x) + (rhP->north.y - rhP->west.y)*(rhP->south.y - rhP->west.y) - Cos36 ) > 0.000000001 )
				{problem=true;  fprintf(stderr, "!!! Error !!! tiling_simple_tests(): thin rhId=%li, west angle not 36 degrees.\n", rhId);}
		}  // physique
	}  // for( rhId ... )
	if( problem )
		return false;

	
	if( tlngP->numFats + tlngP->numThins >= 2 )
	{
		double angMultiple;
		const double angRadians_0 = atan2(
			tlngP->rhombi[0].north.y - tlngP->rhombi[0].south.y,
			tlngP->rhombi[0].north.x - tlngP->rhombi[0].south.x
		);
		for( rhId = 1  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId++ )  // deliberate start at 1
		{
			rhP = &(tlngP->rhombi[rhId]);
			angMultiple = DegreesPerRadian * (atan2(
				rhP->north.y - rhP->south.y,
				rhP->north.x - rhP->south.x
			) - angRadians_0) / 18;

			if( fabs(round(angMultiple) - angMultiple) >= 0.000005 )  // A multiple of 18 degrees, to within 0.0935 dots across A3 at 3600dpi.
			{
				problem = true;
				fprintf(stderr, "!!! Error !!! tiling_simple_tests(): rhId=%li and rhId=0 not aligned by a multiple of 18 degrees.\n", rhId);
			}  // not close to a multiple of 18 degrees
		}  // for( rhId = 1 ... )
		if( problem )
			return false;
	}  // >=2 rhombi
	

	// All the above is necessary, but not sufficient.
	// Not testing whether misplaced, as, in general, that very difficult.
	// Even the angles of touching rhombi could be further tested, but aren't.
	// Nonetheless, the above might catch some careless errors.


	return true;
}  // tiling_simple_tests
