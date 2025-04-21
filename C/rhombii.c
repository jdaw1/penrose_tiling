// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// rhombii.c, in PenroseC

#include "penrose.h"

RhombId rhombus_append(
	Tiling * const tlngP,  Physique const physique,  int8_t filledType,
	double const xNorth,  double const yNorth,  double const xSouth,  double const ySouth
)
{
	if (! rhombus_keep(tlngP,  physique,  xNorth,  yNorth,  xSouth,  ySouth) )
		return -1;

	RhombId const rhId = tlngP->numFats + tlngP->numThins ;
	int8_t nghbrNum;

	if( rhId >= tlngP->rhombii_NumMax )
	{
		fprintf(stderr,
			"!!! Error in rhombus_append() with tilingId=%" PRIi8 ", numFats=%li, numThins=%li: rhombii_NumMax = %li is too small.\n",
			tlngP->tilingId,  tlngP->numFats,  tlngP->numThins,  tlngP->rhombii_NumMax
		);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}

	Rhombus * const rhP = &(tlngP->rhombii[rhId]) ;
	rhP->rhId       = rhId;
	rhP->physique   = physique;
	rhP->filledType = filledType;
	rhP->north.x    = xNorth;
	rhP->north.y    = yNorth;
	rhP->south.x    = xSouth;
	rhP->south.y    = ySouth;

	rhP->centre.x   = rhP->east.x = rhP->west.x = (xNorth + xSouth) / 2;
	rhP->centre.y   = rhP->east.y = rhP->west.y = (yNorth + ySouth) / 2;

	rhP->angleDegrees = DegreesPerRadian * atan2(yNorth - ySouth, xNorth - xSouth);
	while(rhP->angleDegrees <  -90)  {rhP->angleDegrees += 360;}
	while(rhP->angleDegrees >= 270)  {rhP->angleDegrees -= 360;}

	double const otherDiagonalFactor = (Fat == physique  ?  HalfRoot5Minus2Sqrt5  :  HalfRoot5Plus2Sqrt5);  // 0.36327 or 1.5388

	rhP->east.x  +=  ( (yNorth - ySouth)  *  otherDiagonalFactor );
	rhP->west.x  -=  ( (yNorth - ySouth)  *  otherDiagonalFactor );
	rhP->east.y  -=  ( (xNorth - xSouth)  *  otherDiagonalFactor );
	rhP->west.y  +=  ( (xNorth - xSouth)  *  otherDiagonalFactor );

	rhP->xMax = max_4(rhP->north.x, rhP->south.x, rhP->east.x, rhP->west.x);
	rhP->yMax = max_4(rhP->north.y, rhP->south.y, rhP->east.y, rhP->west.y);
	rhP->xMin = min_4(rhP->north.x, rhP->south.x, rhP->east.x, rhP->west.x);
	rhP->yMin = min_4(rhP->north.y, rhP->south.y, rhP->east.y, rhP->west.y);

	rhP->numNeighbours = 0;
	for( nghbrNum = 0  ;  nghbrNum < 4  ;  nghbrNum ++ )
	{
		rhP->neighbours[nghbrNum].rhId           = -1;  // I.e., invalid
		rhP->neighbours[nghbrNum].pathId         = -1;  // I.e., invalid
		rhP->neighbours[nghbrNum].withinPathNum  = -1;  // I.e., invalid
		rhP->neighbours[nghbrNum].nghbrsNghbrNum = -1;  // I.e., invalid
	}  // for( nghbrNum ... )
	rhP->pathId               = -1;  // I.e., invalid
	rhP->withinPathNum        = -1;  // I.e., invalid
	rhP->pathId_ShortestOuter = -1;  // I.e., invalid
	rhP->wantedPostScript = false;

	if( 0 == tlngP->numFats + tlngP->numThins )
	{
		tlngP->xMax = rhP->xMax;
		tlngP->yMax = rhP->yMax;
		tlngP->xMin = rhP->xMin;
		tlngP->yMin = rhP->yMin;
	}
	else
	{
		if(tlngP->xMax < rhP->xMax) {tlngP->xMax = rhP->xMax;}
		if(tlngP->yMax < rhP->yMax) {tlngP->yMax = rhP->yMax;}
		if(tlngP->xMin > rhP->xMin) {tlngP->xMin = rhP->xMin;}
		if(tlngP->yMin > rhP->yMin) {tlngP->yMin = rhP->yMin;}
	}

	if( Fat == physique)
		tlngP->numFats ++ ;
	else
		tlngP->numThins ++ ;

	return rhId ;
}  // rhombus_append()


void rhombus_append_descendants(Tiling *tlngP, Rhombus* rhP)
{
	double const xNorthMinusSouth = rhP->north.x - rhP->south.x;
	double const yNorthMinusSouth = rhP->north.y - rhP->south.y;

	if(Fat == rhP->physique)
	{
		// Fat

		rhombus_append(
			tlngP,
			Fat,
			0,  // not filled hole
			rhP->south.x * GoldenRatioReciprocal  +  rhP->north.x * Half3MinusSqrt5,
			rhP->south.y * GoldenRatioReciprocal  +  rhP->north.y * Half3MinusSqrt5,
			rhP->north.x,
			rhP->north.y
		);

		rhombus_append(
			tlngP,
			Fat,
			0,  // not filled hole
			rhP->south.x,
			rhP->south.y,
			rhP->centre.x  -  yNorthMinusSouth * HalfRoot5Minus2Sqrt5,
			rhP->centre.y  +  xNorthMinusSouth * HalfRoot5Minus2Sqrt5
		);

		rhombus_append(
			tlngP,
			Fat,
			0,  // not filled hole
			rhP->south.x,
			rhP->south.y,
			rhP->centre.x  +  yNorthMinusSouth * HalfRoot5Minus2Sqrt5,
			rhP->centre.y  -  xNorthMinusSouth * HalfRoot5Minus2Sqrt5
		);

		rhombus_append(
			tlngP,
			Thin,
			false,  // not filled hole
			rhP->north.x * Quarter5MinusSqrt5  +  rhP->south.x * Cos72  -  yNorthMinusSouth * QuarterRoot50Minus22Sqrt5,
			rhP->north.y * Quarter5MinusSqrt5  +  rhP->south.y * Cos72  +  xNorthMinusSouth * QuarterRoot50Minus22Sqrt5,
			rhP->centre.x  -  yNorthMinusSouth * HalfRoot5Minus2Sqrt5,
			rhP->centre.y  +  xNorthMinusSouth * HalfRoot5Minus2Sqrt5
		);

		rhombus_append(
			tlngP,
			Thin,
			0,  // not filled hole
			rhP->north.x * Quarter5MinusSqrt5  +  rhP->south.x * Cos72  +  yNorthMinusSouth * QuarterRoot50Minus22Sqrt5,
			rhP->north.y * Quarter5MinusSqrt5  +  rhP->south.y * Cos72  -  xNorthMinusSouth * QuarterRoot50Minus22Sqrt5,
			rhP->centre.x  +  yNorthMinusSouth * HalfRoot5Minus2Sqrt5,
			rhP->centre.y  -  xNorthMinusSouth * HalfRoot5Minus2Sqrt5
		);

	}  // Fat
	else
	{
		// Thin

		rhombus_append(
			tlngP,
			Fat,
			0,  // not filled hole
			rhP->south.x,
			rhP->south.y,
			rhP->centre.x  -  yNorthMinusSouth * HalfRoot5Plus2Sqrt5,
			rhP->centre.y  +  xNorthMinusSouth * HalfRoot5Plus2Sqrt5
		);

		rhombus_append(
			tlngP,
			Fat,
			0,  // not filled hole
			rhP->south.x,
			rhP->south.y,
			rhP->centre.x  +  yNorthMinusSouth * HalfRoot5Plus2Sqrt5,
			rhP->centre.y  -  xNorthMinusSouth * HalfRoot5Plus2Sqrt5
		);

		rhombus_append(
			tlngP,
			Thin,
			0,  // not filled hole
			rhP->north.x * Cos36  +  rhP->south.x * Quarter3MinusSqrt5  -  yNorthMinusSouth * Sin36,
			rhP->north.y * Cos36  +  rhP->south.y * Quarter3MinusSqrt5  +  xNorthMinusSouth * Sin36,
			rhP->north.x,
			rhP->north.y
		);

		rhombus_append(
			tlngP,
			Thin,
			0,  // not filled hole
			rhP->north.x * Cos36  +  rhP->south.x * Quarter3MinusSqrt5  +  yNorthMinusSouth * Sin36,
			rhP->north.y * Cos36  +  rhP->south.y * Quarter3MinusSqrt5  -  xNorthMinusSouth * Sin36,
			rhP->north.x,
			rhP->north.y
		);

	}  // Thin
}  // rhombus_append_descendants()
