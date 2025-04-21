// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportRh.c, in PenroseC

#include "penrose.h"


// If exportFormat==TSV && NULL==rhP, output header row not data.
void rhombus_export(
	FILE * const fp,
	ExportFormat const exportFormat,
	Tiling const * const tlngP,
	Rhombus const * const rhP,
	bool const notLast,
	unsigned long long int * const numCharsThisFileP
)
{
	int8_t      nghbrNum;
	extern char scratchString[];

	switch(exportFormat)
	{
	case PS_rhomb:
	case PS_arcs:
	case SVG_rhomb:
	case SVG_arcs:
		break;  // Not applicable here.

	case PS_data:
		if( NULL == rhP )
			break;  // from PS  (inside rhombus_export, NULL == rhP, which should happen only for TSV)

		sprintf(scratchString,
			"<< /RhId %li"
			"  /WantedPS %s  /Physique %i"
			"  /Xn %.9f  /Yn %.9f  /Xs %.9f  /Ys %.9f  /Xe %.9f  /Ye %.9f  /Xw %.9f  /Yw %.9f  /AngDeg %.9f"
			"  /Neighbours [ ",
			tlngP->wantedPostScriptRhombNum[ rhP->rhId ],
			(rhP->wantedPostScript?"true":"false"),  rhP->physique,
			rhP->north.x, rhP->north.y,
			rhP->south.x, rhP->south.y,
			rhP->east.x,  rhP->east.y,
			rhP->west.x,  rhP->west.y,
			rhP->angleDegrees  // In PostScript all angles are in degrees (e.g.: sin, cos, atan, rotate, sethalftone, setscreen, setcolorscreen, ItalicAngle).
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

		for(nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours ;  nghbrNum++)
		{
			(*numCharsThisFileP) += fprintf(fp, "<< /Edge /%s%s  /RhId %li  /NNN %i >> ",
				rhP->neighbours[nghbrNum].touchesN ? "N" : "S",
				rhP->neighbours[nghbrNum].touchesE ? "E" : "W",
				tlngP->wantedPostScriptRhombNum[ rhP->neighbours[nghbrNum].rhId ],
				rhP->neighbours[nghbrNum].nghbrsNghbrNum
			);
		}
		(*numCharsThisFileP) += fprintf(fp, "] ");

		if( Fat == rhP->physique )
		{
			(*numCharsThisFileP) += fprintf(fp,
				" /PathId %li  /PathRhombNum %li ",
				tlngP->wantedPostScriptPathNum[ rhP->pathId ],
				rhP->withinPathNum
			);
			if( tlngP->path[rhP->pathId].pathClosed  &&  0 == rhP->withinPathNum )
				(*numCharsThisFileP) += fprintf(fp,  " /EdgeClosestToPathCentre /%s%s ",  rhP->closerPathCentreN ? "N" : "S",  rhP->closerPathCentreE ? "E" : "W");
		}
		else
		{
			if( rhP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp, 
					" /PathId_ShortestOuter %li ",
					tlngP->wantedPostScriptPathNum[ rhP->pathId_ShortestOuter ]
				);  // Thins only, path containing
		}
		(*numCharsThisFileP) += fprintf(fp,  ">>");

		break;  // from PS  (inside rhombus_export)



	case JSON:
		if( NULL == rhP )
			break;  // from JSON  (inside rhombus_export, NULL == rhP, which should happen only for TSV)

		sprintf(scratchString,
			"{ \"RhId\":%li, \"WantedPS\":%s, \"Physique\":%i"
			", \"Xn\":%.12f, \"Yn\":%.12f, \"Xs\":%.12f, \"Ys\":%.12f, \"Xe\":%.12f, \"Ye\":%.12f, \"Xw\":%.12f, \"Yw\":%.12f, \"AngleDeg\":%.10f",
			rhP->rhId,
			rhP->wantedPostScript ? "true" : "false",
			rhP->physique,
			rhP->north.x,  rhP->north.y,
			rhP->south.x,  rhP->south.y,
			rhP->east.x,   rhP->east.y,
			rhP->west.x,   rhP->west.y,
			rhP->angleDegrees  // Angles seem to be wrong by about +- 1 * 10^-12; so printing to 10d.p. seems to work.
		);
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s,  \"Neighbours\":[", scratchString);

		for(nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours ;  nghbrNum++)
			(*numCharsThisFileP) += fprintf(fp, "{\"Edge\":\"%s%s\", \"RhId\":%li, \"NNN\":%i}%s ",
				rhP->neighbours[nghbrNum].touchesN ? "N" : "S",
				rhP->neighbours[nghbrNum].touchesE ? "E" : "W",
				rhP->neighbours[nghbrNum].rhId,
				rhP->neighbours[nghbrNum].nghbrsNghbrNum,
				nghbrNum < rhP->numNeighbours - 1 ? ", " : ""
			);
		(*numCharsThisFileP) += fprintf(fp, "]");

		if( Fat == rhP->physique )
		{
			// Path
			(*numCharsThisFileP) += fprintf(fp, ",  \"PathId\":%li,  \"WithinPathNum\":%li", rhP->pathId, rhP->withinPathNum);
			if( tlngP->path[rhP->pathId].pathClosed )
				(*numCharsThisFileP) += fprintf(fp,  ", \"EdgeClosestToPathCentre\":\"%s%s\"",  rhP->closerPathCentreN ? "N" : "S",  rhP->closerPathCentreE ? "E" : "W");
		}
		else
			if( rhP->pathId_ShortestOuter >= 0 )
				(*numCharsThisFileP) += fprintf(fp, ",  \"PathId_ShortestOuter\":%li ", rhP->pathId_ShortestOuter);  // Thins only, path containing

		if( rhP->filledType > 0 )  // Only those made by holesFill
			(*numCharsThisFileP) += fprintf(fp, ",  \"FilledType\":%" PRIi8 " ", rhP->filledType);


		(*numCharsThisFileP) += fprintf(fp, " }%s",  notLast ? "," : "");

		break;  // from JSON  (inside rhombus_export)



	case TSV:
		if( NULL == rhP )
		{
			// No data, just headers. Header code here to be near to the data-outputting code.
			// These strings are intended to be unique range names for creation and use within Excel (Formula > Defined Names > Create from Selection).
			(*numCharsThisFileP) += fprintf(fp,
				"Rh_%02" PRIi8 ".TilingId"  "\tRh_%02" PRIi8 ".RhId"  "\tRh_%02" PRIi8 ".Wantedness_PostScript"
				"\tRh_%02" PRIi8 ".Physique"    "\tRh_%02" PRIi8 ".FilledType"
				"\tRh_%02" PRIi8 ".Xn"  "\tRh_%02" PRIi8 ".Yn"  "\tRh_%02" PRIi8 ".Xs"  "\tRh_%02" PRIi8 ".Ys"      "\tRh_%02" PRIi8 ".Xe"  "\tRh_%02" PRIi8 ".Ye"  "\tRh_%02" PRIi8 ".Xw"  "\tRh_%02" PRIi8 ".Yw"
				"\tRh_%02" PRIi8 ".AngleDeg"  "\tRh_%02" PRIi8 ".NumNeighbours"
				"\tRh_%02" PRIi8 ".NeighbourEdge_0"  "\tRh_%02" PRIi8 ".NeighbourEdge_1"          "\tRh_%02" PRIi8 ".NeighbourEdge_2"  "\tRh_%02" PRIi8 ".NeighbourEdge_3"
				"\tRh_%02" PRIi8 ".NeighbourNum_0"   "\tRh_%02" PRIi8 ".NeighbourNum_1"           "\tRh_%02" PRIi8 ".NeighbourNum_2"   "\tRh_%02" PRIi8 ".NeighbourNum_3"
				"\tRh_%02" PRIi8 ".NNN_0"            "\tRh_%02" PRIi8 ".NNN_1"                    "\tRh_%02" PRIi8 ".NNN_2"            "\tRh_%02" PRIi8 ".NNN_3"
				"\tRh_%02" PRIi8 ".PathId"           "\tRh_%02" PRIi8 ".WithinPathNum"    "\tRh_%02" PRIi8 ".EdgeClosestToPathCentre"  "\tRh_%02" PRIi8 ".PathId_ShortestOuter",
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId,
				tlngP->tilingId, tlngP->tilingId, tlngP->tilingId, tlngP->tilingId
			);
			break;  // from TSV  (inside rhombus_export, NULL == rhP )
		}  // if( NULL == rhP )
		{
			sprintf(scratchString,
				"%" PRIi8  "\t%li"  "\t%s"
				"\t%i"  "\t%" PRIi8
				"\t%.12f"  "\t%.12f"
				"\t%.12f"  "\t%.12f"
				"\t%.12f"  "\t%.12f"
				"\t%.12f"  "\t%.12f"
				"\t%.10f"  "\t%i",
				tlngP->tilingId,  rhP->rhId,  rhP->wantedPostScript ? "TRUE" : "FALSE",
				rhP->physique,  rhP->filledType,
				rhP->north.x, rhP->north.y,  rhP->south.x, rhP->south.y,  rhP->east.x,  rhP->east.y,  rhP->west.x,  rhP->west.y,
				rhP->angleDegrees,  rhP->numNeighbours
			);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);

			for(nghbrNum=0 ;  nghbrNum < 4 ;  nghbrNum++)
			{
				if( nghbrNum < rhP->numNeighbours )
					(*numCharsThisFileP) += fprintf(fp, "\t%s%s",
						rhP->neighbours[nghbrNum].touchesN ? "N" : "S",
						rhP->neighbours[nghbrNum].touchesE ? "E" : "W"
					);
				else
					(*numCharsThisFileP) += fprintf(fp, "\t#N/A");
			}

			for(nghbrNum=0 ;  nghbrNum < 4 ;  nghbrNum++)
			{
				if( nghbrNum < rhP->numNeighbours )
					(*numCharsThisFileP) += fprintf(fp, "\t%li", rhP->neighbours[nghbrNum].rhId);
				else
					(*numCharsThisFileP) += fprintf(fp, "\t#N/A");
			}

			for(nghbrNum=0 ;  nghbrNum < 4 ;  nghbrNum++)
			{
				if( nghbrNum < rhP->numNeighbours )
					(*numCharsThisFileP) += fprintf(fp, "\t%i", rhP->neighbours[nghbrNum].nghbrsNghbrNum);
				else
					(*numCharsThisFileP) += fprintf(fp, "\t#N/A");
			}

			if( Fat == rhP->physique )
			{
				(*numCharsThisFileP) += fprintf(fp, "\t%li\t%li", rhP->pathId, rhP->withinPathNum);
				if( rhP->pathId >= 0 )
				{
					if( tlngP->path[rhP->pathId].pathClosed )
						(*numCharsThisFileP) += fprintf(fp, "\t%s%s",  rhP->closerPathCentreN ? "N" : "S",  rhP->closerPathCentreE ? "E" : "W");
					else
						(*numCharsThisFileP) += fprintf(fp, "\t#N/A");

					if( tlngP->path[rhP->pathId].pathClosed
					&&  tlngP->path[rhP->pathId].pathId_ShortestOuter >= 0 )  // Stored at level of path, but surely better to output it than a "#N/A".
						(*numCharsThisFileP) += fprintf(fp, "\t%li", tlngP->path[rhP->pathId].pathId_ShortestOuter);
					else
						(*numCharsThisFileP) += fprintf(fp, "\t#N/A");
				}
				else
					(*numCharsThisFileP) += fprintf(fp, "\t\tError");
			}
			else
			{
				// Thins only, so no pathId, and no closest edge.
				(*numCharsThisFileP) += fprintf(fp, "\t#N/A\t#N/A\t#N/A");

				// Thins only, path containing.
				// Fats' pathId_ShortestOuter held at level of path, not rhombus.
				if( rhP->pathId_ShortestOuter >= 0 )
					(*numCharsThisFileP) += fprintf(fp, "\t%li", rhP->pathId_ShortestOuter);
				else
					(*numCharsThisFileP) += fprintf(fp, "\t#N/A");
			}

			break ;  // from TSV  (inside rhombus_export)
		}
		break ;  // from TSV  (inside rhombus_export, rhP has data )

	}  // switch(exportFormat)
}  // rhombus_export
