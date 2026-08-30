// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintArcsPS.c, in PenroseC

#include "penrose.h"

#define maybeTesting "%%"  // Replace with empty string to not comment the PostScript useful for debugging.

/*
	The ExportFormats export as data: arrays of stuff.
	But PostScript's maximum array length is 65535 (https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf#page=753  PLRM3, Appendix B, p739, table B.1, row 3).
	So this code just outputs raw PostScript instructions, one after the other.
	All painting is determined one rhombus at a time: rhombi are not stored in a way that allows reasoning at a less-local level than one-at-a-time.
	There is some room for post-C hand editing of the PostScript, but much less flexibly the the other PostScript data format.
	The limit on the number of rhombi is printer memory and resolution, so much much larger than 2^16.

	Because this is just raw PostScript commands, this exporting C code does not have the structure of the other formats' C code.
	It is different and stand-alone.
*/

void tiling_export_PaintArcsPS(
	FILE* const fp,
	Tiling const      * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];
	RhombId rhId, rhIdStart;
	PathId pathId, pathId_goingOut;
	bool edgeN, edgeE, edgeStartE, foundNeighbour, *arcdEast, *arcdWest, either;
	const Path    * pathP;
	const Rhombus * rhP;
	XY arcCentrePrev, arcCentreThis;
	double angThisStart, angThisEnd, xMin, xMax, yMin, yMax;
	int8_t nghbrNum, nnn;
	char seedTypeName[32];

	const double toPaint_xMin = postScript_toPaint_xMin(tlngP);  // in controls.c
	const double toPaint_yMin = postScript_toPaint_yMin(tlngP);
	const double toPaint_xMax = postScript_toPaint_xMax(tlngP);
	const double toPaint_yMax = postScript_toPaint_yMax(tlngP);

	arcdEast = malloc( (tlngP->numFats + tlngP->numThins) * sizeof(bool) );
	arcdWest = malloc( (tlngP->numFats + tlngP->numThins) * sizeof(bool) );
	if( NULL == arcdEast ) fprintf(stderr, "tiling_export_PaintArcsPS(): !!! NULL == arcdEast, with tilingId=%" PRIi8 ". Continuing, with slight misbehaviour. !!!\n", tlngP->tilingId);
	if( NULL == arcdWest ) fprintf(stderr, "tiling_export_PaintArcsPS(): !!! NULL == arcdWest, with tilingId=%" PRIi8 ". Continuing, with slight misbehaviour. !!!\n", tlngP->tilingId);
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
	{
		if( NULL != arcdEast ) arcdEast[rhId] = false;
		if( NULL != arcdWest ) arcdWest[rhId] = false;
	}

	// PostScript definitions, preamble, comment about what is changeable, etc.
	preamble_export_PS(fp,  tlngP,  numLinesThisFileP,  numCharsThisFileP);

	seed_type_name(seedTypeName,  tlngP->seedType);
	sprintf(scratchString,
		"/R 0.5 def\n"
		"\n/LongestPathToBeFilled 215 def  %% User alterable. If being increased, might need to add more colours to PaintPath.\n\n\n"
	);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	// Using \t tabs, because an actual 9-tab might get converted to spaces by an evil IDE.
	sprintf(scratchString,
		"/OnPageTileCentreX PageWidth  0.5 mul def  %% User alterable. This has tiling positioned wrt centre of paper page.\n"
		"/OnPageTileCentreY PageHeight 0.5 mul def  %% User alterable. This has tiling positioned wrt centre of paper page.\n"
		"\n"
		"<< /PageSize [PageWidth PageHeight] >> setpagedevice\n"
		"/PaperMatrix matrix currentmatrix def\n"
		"OnPageTileCentreX OnPageTileCentreY translate   ScaleFactor dup scale\n"
		"Actual_XMax Actual_XMin add -2 div  Actual_YMax Actual_YMin add -2 div  translate  %% User alterable. Optional specification of which point in tiling is to be in the specifed point of page.\n"
		"%% 0 rotate  %% User alterable. Optional rotation, angle in degrees, positive rotating image anti-clockise. By default commented out.\n"
		"/TileMatrix matrix currentmatrix def\n"
		"\n"
		"/StrokeMulti  %% User changable. Allows multi-layer strokes, such as black outer and white inner.\n"
		"{\n"
			"\t//TileMatrix setmatrix   1 setlinejoin  [] 0 setdash\n"
			"\t[\n"
				"\t\t%% {gsave  0 setgray  0 setlinecap  PaperMatrix setmatrix  EdgeLength 16 div ScaleFactor mul setlinewidth  stroke  grestore}  %% #gsave ... grestore for each stroke, except the last. At finish path should be empty.\n"
				"\t\t%% {       1 setgray  1 setlinecap  PaperMatrix setmatrix  EdgeLength 48 div ScaleFactor mul setlinewidth  stroke          }  %% This pair paints thick black, then thin inner white. If uncommented, delete next line.\n"
				"\t\t{0 setgray  1 setlinecap  PaperMatrix setmatrix  EdgeLength 24 div ScaleFactor mul setlinewidth  stroke}  %% Thin black only.\n"
			"\t] {exec} forall\n"
		"} bind def  %% StrokeMulti\n"
		"\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	sprintf(scratchString,
	  "mark\n"
		  "\t/CreationDate (D:%04d%02d%02d%02d%02d%02d)\n"
		  "\t/Title (Penrose tiling)\n"
		  "\t/Author (Julian D. A. Wiseman)\n"
		  "\t/Subject (http://github.com/jdaw1/penrose_tiling/)\n"
		  "\t/Keywords (Penrose tiling arcs)\n"
		  "\t/Creator (C and PostScript coded by Julian D. A. Wiseman of www.jdawiseman.com/author.html)\n"
	  "/DOCINFO pdfmark\n"
	  "\n",
	  (1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
	  tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec)
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	sprintf(scratchString,
		"%% Functions taken from https://github.com/jdaw1/placemat/blob/main/PostScript/placemat.ps\n"
		"/ToString\n"
		"{\n"
		"\t1 dict begin  dup type cvlit /Type exch def\n"
		"\t1 {\n"
		"\t\tType /integertype  eq {11 string cvs exit ( ) Concatenate} if\n"
		"\t\tType /realtype     eq {16 string cvs exit ( ) Concatenate} if\n"
		"\t\tType /nametype     eq {dup length string cvs exit} if\n"
		"\t\tType /stringtype   eq {exit} if\n"
		"\t\tType /booleantype  eq {5 string cvs exit} if\n"
		"\t\tType /operatortype eq {127 string cvs exit} if\n"
		"\t\tType /marktype     eq {pop (mark) exit} if\n"
		"\t\tType /nulltype     eq {pop (null) exit} if\n"
		"\t\tpop (-- not handled --)  %% fall-back\n"
		"\t} repeat  end\n"
		"} bind def  %% /ToString\n"
		"\n"
		"/Concatenate {2 copy length exch length dup 3 1 roll add string dup dup 5 3 roll exch putinterval 3 -1 roll 0 exch putinterval} bind def\n"
		"\n"
		"%% mark string|number|other ... string|number|other  ConcatenateToMark  string\n"
		"/ConcatenateToMark\n"
		"{\n"
		"\t4 dict begin\n"
		"\tcounttomark /ctm exch def  /n 0 def\n"
		"\tctm {ToString  dup length n add /n exch def  ctm 1 roll} repeat\n"
		"\t/p 0 def  /s n 65535 2 copy gt {exch} if pop string def\n"
		"\tctm  -1  0\n"
		"\t{\n"
		"\t\tdup 0 gt\n"
		"\t\t{\n"
		"\t\t\t-1 roll   dup length p add  65532 le\n"
		"\t\t\t\t{s exch p exch dup length p add /p exch def putinterval}\n"
		"\t\t\t\t{s exch p exch 0 65532 p sub getinterval putinterval  s 65532 (...) putinterval  cleartomark  s  exit}\n"
		"\t\t\tifelse  %% too long\n"
		"\t\t} {pop pop s} ifelse\n"
		"\t} for\n"
		"\tend\n"
		"} bind def  %% /ConcatenateToMark\n"
		"\n"
		"() =\n"
		"/NumRhombi NumFats NumThins add def\n"
		"[/TilingId /DataAsOf /NumFats /NumThins /NumRhombi /EdgeLength  /XMin /XMax /YMin /YMax  /ToPaint_XMin /ToPaint_XMax /ToPaint_YMin /ToPaint_YMax]\n"
		"{\n"
			"\tdup 12 string cvs ( = ) Concatenate  exch load\n"
			"\tdup type /stringtype eq {(\") exch (\") Concatenate Concatenate} {16 string cvs} ifelse\n"
			"\tConcatenate =\n"
		"} forall\n"
		"mark (\\nPageWidth = ) PageWidth  (pt = ) PageWidth 72 div  (\" = ) PageWidth  127 mul 360 div (mm) ConcatenateToMark =\n"
		"mark   (PageHeight = ) PageHeight (pt = ) PageHeight 72 div (\" = ) PageHeight 127 mul 360 div (mm) ConcatenateToMark =\n"
		"mark (T, B, L, and R margins each = ) Margin (pt = ) Margin 72 div (\" = ) Margin 127 mul 360 div (mm) ConcatenateToMark =\n"
		"mark (ScaleFactor = ) ScaleFactor ConcatenateToMark =\n"
		"mark (On paper, length of an edge is ) EdgeLength ScaleFactor mul (pt = ) 1 index 72 div (\" = 1\"/) 1 2 index div ( = ) 5 index 127 mul 360 div (mm = 1mm/) 1 2 index div ConcatenateToMark =\n"
		"mark (Actual_XMin = ) Actual_XMin (;  Actual_XMax = ) Actual_XMax (;  Actual_YMin = ) Actual_YMin (;  Actual_YMax = ) Actual_YMax ConcatenateToMark =\n"
		"\n"
		"false  %% Testing\n"
		"{\n"
			"\tgsave  TileMatrix setmatrix\n"
			"\tActual_XMin Actual_YMin moveto\n"
			"\tActual_XMin Actual_YMax lineto\n"
			"\tActual_XMax Actual_YMax lineto\n"
			"\tActual_XMax Actual_YMin lineto\n"
			"\tclosepath  PaperMatrix setmatrix  18 setlinewidth  0.8 setgray  stroke\n"
			"\tgrestore\n"
		"} if   %% Testing\n"
		"\n"
		"\n"
		"/PaintPath\n"
		"{\n"
			"\t6 dict begin\n"
			"\t/LengthsGoingOut exch def  /PathId exch def  /YMax exch def  /XMax exch def  /YMin exch def  /XMin exch def\n"
			"\tXMax Actual_XMin ge  XMin Actual_XMax le  YMax Actual_YMin ge  YMin Actual_YMax le  and and and\n"
			"\t{\n"
				"\t\tLengthsGoingOut length 1 sub  -1  0\n"
				"\t\t{\n"
					"\t\t\tdup  LengthsGoingOut exch get //LongestPathToBeFilled le\n"
					"\t\t\t{\n"
						"\t\t\t\t1 add  LengthsGoingOut exch  0 exch  getinterval\n"
						"\t\t\t\t/LengthsGoingOut exch def  exit\n"
					"\t\t\t} {pop} ifelse\n"
				"\t\t} for\n"
				"\n"
				"\t\t//PaperMatrix setmatrix\n"
				"\t\t1 {\n"
					"\t\t\tLengthsGoingOut 0 get  //LongestPathToBeFilled  gt {exit} if\n"
					"\t\t\tLengthsGoingOut length 2 mod 0 eq       {gsave 0 0 0 0 setcmykcolor fill grestore exit} if\n"
					"\t\t\t%% Because of odd-even rules, colours never touch +-2 that are strictly longer than 5. All other pairs can neighbour.\n"
					"\t\t\t%%                                  Cyan Mgnt Ylw Blk\n"
					"\t\t\tLengthsGoingOut 0 get   5 eq {gsave 1    0.6  0  0.4  setcmykcolor fill grestore exit} if  %% Dark blue\n"
					"\t\t\tLengthsGoingOut 0 get  15 eq {gsave 0    1    1  0.3  setcmykcolor fill grestore exit} if  %% Dark red\n"
					"\t\t\tLengthsGoingOut 0 get  25 eq {gsave 1    0    1  0    setcmykcolor fill grestore exit} if  %% Green\n"
					"\t\t\tLengthsGoingOut 0 get  55 eq {gsave 0    0.2  1  0    setcmykcolor fill grestore exit} if  %% Yellow-gold\n"
					"\t\t\tLengthsGoingOut 0 get 105 eq {gsave 0.7  0    0  0    setcmykcolor fill grestore exit} if  %% Light cyan\n"
					"\t\t\tLengthsGoingOut 0 get 215 eq {gsave 0    0.1  0  0.3  setcmykcolor fill grestore exit} if  %% Grey-grey-red\n"
					"\t\t\t%% This list needs to go as far as LongestPathToBeFilled ==> bigger LongestPathToBeFilled can require more colours here.\n"
				"\t\t} repeat  %% 1, as exit'able.\n"
				"\t\tStrokeMulti\n"
			"\t} {newpath} ifelse  %% In box\n"
			"\tend\n"
		"} bind def  %% /PaintPath\n"
		"\n"
		"\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	if( exportQ(boundingPath, PS_arcs, tlngP, *numLinesThisFileP) )
	{
		(*numCharsThisFileP) += fprintf(fp, "\n\n%% Bounding path\n");
		(*numLinesThisFileP) += 3;
		tiling_export_PaintBoundary(
			fp,
			PS_arcs,
			1.0 / tlngP->edgeLength,
			tlngP,
			0,
			numLinesThisFileP,
			numCharsThisFileP
		);  // tiling_export_PaintBoundary()
		(*numCharsThisFileP) += fprintf(fp, "newpath  %% gsave 0.8 setgray fill grestore 0.4 setgray 1 setlinejoin PaperMatrix setmatrix 1.92 setlinewidth stroke \n");
		(*numLinesThisFileP) ++;
	}  // if boundingPath

	for( pathId = 0  ;  pathId < tlngP->numPathsClosed + tlngP->numPathsOpen  ;  pathId++ )
	{
		pathP = &(tlngP->path[pathId]);
		if( (! pathP->pathClosed)  ||  pathP->pathClosedTypeNum < 2 )  // open, or 5r. But 5 closed passes.
			break;

		if( (! pathP->pathVeryClosed)  // Close to edge, such that arc might not be complete
		||  tlngP->rhombi[ pathP->xMin_rhId ].xMin > toPaint_xMax
		||  tlngP->rhombi[ pathP->xMax_rhId ].xMax < toPaint_xMin
		||  tlngP->rhombi[ pathP->yMin_rhId ].yMin > toPaint_yMax
		||  tlngP->rhombi[ pathP->yMax_rhId ].yMax < toPaint_yMin )
			continue;

		arcCentrePrev.x = DBL_MAX;  // So certain to be different to first rhombus in path
		arcCentrePrev.y = DBL_MAX;
		if( 1 == pathP->pathClosedTypeNum % 2 )
		{
			rhIdStart = rhId = pathP->rhId_PathCentreClosest;
			rhP = &(tlngP->rhombi[rhIdStart]);
			edgeStartE = edgeE = (
				pow(rhP->east.x - pathP->centre.x, 2) + pow(rhP->east.y - pathP->centre.y, 2) <=
				pow(rhP->west.x - pathP->centre.x, 2) + pow(rhP->west.y - pathP->centre.y, 2)
			);  // edgeStartE, edgeE
		}  // pathLength is 5r, 15, 55, 215, 855, 3415, 13655, 54615, 218455, 873815, ...
		else
		{
			rhIdStart = rhId = pathP->rhId_PathCentreFurthest;
			rhP = &(tlngP->rhombi[rhIdStart]);
			edgeStartE = edgeE = (
				pow(rhP->east.x - pathP->centre.x, 2) + pow(rhP->east.y - pathP->centre.y, 2) >=
				pow(rhP->west.x - pathP->centre.x, 2) + pow(rhP->west.y - pathP->centre.y, 2)
			);  // edgeStartE, edgeE
		}  // pathLength is 5p, 25, 105, 425, 1705, 6825, 27305, 109225, 436905, . ..
		edgeN = false ;  // Arcs broadly follow south side, so always start south.
		xMin = xMax = (edgeE ? rhP->east.x : rhP->west.x);
		yMin = yMax = (edgeE ? rhP->east.y : rhP->west.y);

		sprintf(scratchString,
			"\nTileMatrix setmatrix  %.9lf %.9lf moveto  %% pathId=%li, length=%li%s",
			( (edgeN ? rhP->north.x : rhP->south.x)  +  (edgeE ? rhP->east.x : rhP->west.x) ) / 2 / tlngP->edgeLength,
			( (edgeN ? rhP->north.y : rhP->south.y)  +  (edgeE ? rhP->east.y : rhP->west.y) ) / 2 / tlngP->edgeLength,
			pathId,  pathP->pathLength,
			pathP->pathLength == 5 ? (pathP->pointy ? " pointy" : " round") : ""  // if 5 then should always be pointy
		);  // sprintf()
		stringClean(scratchString);
		(*numCharsThisFileP) += fprintf(fp, "%s",scratchString);
		(*numLinesThisFileP) ++;
		if( pathP->pathId_ShortestOuter >= 0 )
			(*numCharsThisFileP) += fprintf(fp,
				", pathId_ShortestOuter=%li of length %li",
				pathP->pathId_ShortestOuter,   tlngP->path[ pathP->pathId_ShortestOuter ].pathLength
			);  // fprintf()
		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++;

		do
		{
			if( edgeE )
			{
				if( xMin > rhP->east.x )  xMin = rhP->east.x;
				if( yMin > rhP->east.y )  yMin = rhP->east.y;
				if( xMax < rhP->east.x )  xMax = rhP->east.x;
				if( yMax < rhP->east.y )  yMax = rhP->east.y;
				if( NULL != arcdEast )  arcdEast[rhId] = true;
			}
			else
			{
				if( xMin > rhP->west.x )  xMin = rhP->west.x;
				if( yMin > rhP->west.y )  yMin = rhP->west.y;
				if( xMax < rhP->west.x )  xMax = rhP->west.x;
				if( yMax < rhP->west.y )  yMax = rhP->west.y;
				if( NULL != arcdWest ) arcdWest[rhId] = true;
			}

			arcCentreThis = (edgeE ? rhP->east : rhP->west);
			if(72 == rhP->physique)
			{
				angThisStart = rhP->angleDegrees  +  ( edgeN ? (edgeE ? 36: -36) : (edgeE ? 144 : 216) );  // Fat
				if( angThisStart >   180 ) angThisStart -= 360;
				if( angThisStart <= -180 ) angThisStart += 360;
				angThisEnd = angThisStart + (edgeN == edgeE ? +108 : -108);
			}  // Fat
			else
			{
				angThisStart = rhP->angleDegrees  +  ( edgeN ? (edgeE ? 72: -72) : (edgeE ? 108 : -108) );  // Fat
				if( angThisStart >   180 ) angThisStart -= 360;
				if( angThisStart <= -180 ) angThisStart += 360;
				angThisEnd = angThisStart + (edgeN == edgeE ? +36 : -36);
			}  // Thin
			sprintf(scratchString,
				"%.9lf %.9lf R %.9lf %.9lf arc%s\n",
				arcCentreThis.x / tlngP->edgeLength,
				arcCentreThis.y / tlngP->edgeLength,
				angThisStart, angThisEnd,
				edgeN == edgeE ? "" : "n"
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s",scratchString);
			(*numLinesThisFileP) ++;

			foundNeighbour = false;
			for( nghbrNum = 0  ;  nghbrNum < rhP->numNeighbours  ;  nghbrNum++ )  // VeryClosed  ==>  4==numNeighbours
			{
				if( edgeN != rhP->neighbours[nghbrNum].touchesN  // Because exit N|S is opposite to entry N|S
				&&  edgeE == rhP->neighbours[nghbrNum].touchesE )
				{
					foundNeighbour = true;
					nnn = rhP->neighbours[nghbrNum].nghbrsNghbrNum;
					rhId = rhP->neighbours[nghbrNum].rhId;
					rhP = &(tlngP->rhombi[rhId]);
					edgeN = rhP->neighbours[ nnn ].touchesN;
					edgeE = rhP->neighbours[ nnn ].touchesE;
					break;  // nghbrNum loop
				}  // correct nghbrNum, correct edge
			}  // for( nghbrNum ... )

			if( ! foundNeighbour )
			{
				fprintf(stderr,
					"tiling_export_PaintArcsPS(): !!! impossible failure to find neighbour, pathStatId=%li, pathId=%li, pathLength=%li, %s, rhId=%li, physique=%" PRIi8 " numNeighbours=%" PRIi8 " !!!\n",
					pathP->pathStatId,  pathId,  pathP->pathLength,  pathP->pathClosed ? "closed" : "open",  rhId,  rhP->physique,  rhP->numNeighbours
				);  // fprintf()
				fflush(fp);
				fclose(fp);
				exit(EXIT_FAILURE);
			}

		}  // do
		while( rhIdStart != rhId  ||  edgeStartE != edgeE);

		sprintf(scratchString,
			"closepath  %.9lf %.9lf %.9lf %.9lf  %li  [ ",
			(xMin  -  tlngP->edgeLength / 2) / tlngP->edgeLength,
			(yMin  -  tlngP->edgeLength / 2) / tlngP->edgeLength,
			(xMax  +  tlngP->edgeLength / 2) / tlngP->edgeLength,
			(yMax  +  tlngP->edgeLength / 2) / tlngP->edgeLength,
			pathId
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		pathId_goingOut = pathId;
		do
		{
			(*numCharsThisFileP) += fprintf(fp, "%li ",  tlngP->path[pathId_goingOut].pathLength);
			pathId_goingOut = tlngP->path[pathId_goingOut].pathId_ShortestOuter;
		} while( pathId_goingOut >= 0 ) ;
		(*numCharsThisFileP) += fprintf(fp, "]  PaintPath\n");
		(*numLinesThisFileP) ++;
		fflush(fp);
	}  // for( pathId ... )

	(*numCharsThisFileP) += fprintf(fp, "\n\n//TileMatrix setmatrix\n");
	(*numLinesThisFileP) += 3 ;
	for( rhId = 0  ;  rhId < tlngP->numFats + tlngP->numThins  ;  rhId ++ )
	{
		rhP = &(tlngP->rhombi[rhId]);

		if( rhP->xMax < toPaint_xMin
		||  rhP->xMin > toPaint_xMax
		||  rhP->yMax < toPaint_yMin
		||  rhP->yMin > toPaint_yMax )
			continue;

		either = false;
		angThisStart = rhP->angleDegrees  +  (Fat == rhP->physique  ?   36  :  72 );
		if(angThisStart >= 180) angThisStart -= 360;
		if( (NULL == arcdEast  ||  (! arcdEast[rhId]))
		&& rhP->east.x  +  tlngP->edgeLength / 2  >  toPaint_xMin
		&& rhP->east.x  -  tlngP->edgeLength / 2  <  toPaint_xMax
		&& rhP->east.y  +  tlngP->edgeLength / 2  >  toPaint_yMin
		&& rhP->east.y  -  tlngP->edgeLength / 2  <  toPaint_yMax )  // This is slightly generous; harmlessly, orientation-dependent, some near misses will be admitted.
		{
			sprintf(scratchString,
				"%.9lf %.9lf moveto  %.9lf %.9lf R %.9lf %.9lf arc",
				(rhP->east.x + rhP->north.x) / 2 / tlngP->edgeLength,
				(rhP->east.y + rhP->north.y) / 2 / tlngP->edgeLength,
				rhP->east.x / tlngP->edgeLength,   rhP->east.y / tlngP->edgeLength,
				angThisStart,  angThisStart + (Fat == rhP->physique  ?  108  :  36 )
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s",scratchString);
			either = true;
		}  // ! arcdEast[], inside toPaint_...
		if( (NULL == arcdWest  ||  (! arcdWest[rhId]))
		&& rhP->west.x  +  tlngP->edgeLength / 2  >  toPaint_xMin
		&& rhP->west.x  -  tlngP->edgeLength / 2  <  toPaint_xMax
		&& rhP->west.y  +  tlngP->edgeLength / 2  >  toPaint_yMin
		&& rhP->west.y  -  tlngP->edgeLength / 2  <  toPaint_yMax )  // This is slightly generous; harmlessly, orientation-dependent, some near misses will be admitted.
		{
			angThisStart += (angThisStart > 90 ? -180 : 180);
			sprintf(scratchString,
				"%s%.9lf %.9lf moveto  %.9lf %.9lf R %.9lf %.9lf arc",
				either ? "   " : "",
				(rhP->west.x + rhP->south.x) / 2 / tlngP->edgeLength,
				(rhP->west.y + rhP->south.y) / 2 / tlngP->edgeLength,
				rhP->west.x / tlngP->edgeLength,   rhP->west.y / tlngP->edgeLength,
				angThisStart,  angThisStart + (Fat == rhP->physique  ?  108  :  36 )
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp, "%s",scratchString);
			either = true;
		}  // ! arcdWest[], inside toPaint_...
		if( either )
		{
			(*numCharsThisFileP) += fprintf(fp, "\n");  fflush(fp);
			(*numLinesThisFileP) ++ ;
		}  // either
	}  // for( rhId ... )
	(*numCharsThisFileP) += fprintf(fp, "PaperMatrix setmatrix  StrokeMulti\n");
	(*numLinesThisFileP) ++ ;

	tiling_export_subroutines_PS(fp,  PS_rhomb,  numLinesThisFileP,  numCharsThisFileP);

	(*numCharsThisFileP) += fprintf(fp, "\n\n/FontName /Helvetica def\n\n");
	(*numLinesThisFileP) +=4;

	tiling_export_Gridlines(fp,  tlngP,  PS_arcs,  numLinesThisFileP,  numCharsThisFileP);  // Code always present; whether or not active controlled by showGridlines()

	sprintf(scratchString,
		"\n"
		"showpage\n"
		"\n"
		"(\\n\\n) =\n"
		"count              (      = count: this really should be 0\\n)   dup 0  4 -1 roll  6 string cvs  putinterval\n"
		"countdictstack     (      = countdictstack: this should be 3\\n) dup 0  4 -1 roll  6 string cvs  putinterval  Concatenate =\n"
		"count 0 gt {(+pstack) = pstack (-pstack) =} if\n"
		"\n"
		"{countdictstack 3 gt {8 {() =} repeat currentdict {exch == =} forall end} {exit} ifelse} bind loop  %% Final debugging\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	if( NULL != arcdEast ) {free(arcdEast);  arcdEast = NULL;}
	if( NULL != arcdWest ) {free(arcdWest);  arcdWest = NULL;}
}  // tiling_export_PaintArcsPS()
