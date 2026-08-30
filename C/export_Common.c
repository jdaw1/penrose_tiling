// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_Common.c, in PenroseC

#include "penrose.h"

void tiling_export_Gridlines(
	FILE* const fp,
	Tiling const * const tlngP,
	ExportFormat const ef,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];

	switch (ef)
	{
	case PS_rhomb:
	case PS_arcs:
	{
		sprintf(scratchString,
			"\n\n"
			"%s  %% Boolean: paint gridlines?\n"
			"{\n"
				"\t14 dict begin\n\n"
				"%s"
				"\t/SizeX Actual_XMax Actual_XMin sub def\n"
				"\t/SizeY Actual_YMax Actual_YMin sub def\n"
				"\t/GridStepX\n"
				"\t1 {\n"
					"\t\tSizeX   16 le {   1 exit} if\n"
					"\t\tSizeX   32 le {   2 exit} if\n"
					"\t\tSizeX   80 le {   5 exit} if\n"
					"\t\tSizeX  160 le {  10 exit} if\n"
					"\t\tSizeX  320 le {  20 exit} if\n"
					"\t\tSizeX  800 le {  50 exit} if\n"
					"\t\tSizeX 1600 le { 100 exit} if\n"
					"\t\tSizeX 3200 le { 500 exit} if\n"
					"\t\tSizeX 8000 le {1000 exit} if\n"
					"\t\tSixeX log 0.3 sub floor cvi 10 exch exp exit\n"
				"\t} repeat def  %% 1, /GridStepX\n"
				"\t/GridStepY\n"
				"\t1 {\n"
					"\t\tSizeY   16 le {   1 exit} if\n"
					"\t\tSizeY   32 le {   2 exit} if\n"
					"\t\tSizeY   80 le {   5 exit} if\n"
					"\t\tSizeY  160 le {  10 exit} if\n"
					"\t\tSizeY  320 le {  20 exit} if\n"
					"\t\tSizeY  800 le {  50 exit} if\n"
					"\t\tSizeY 1600 le { 100 exit} if\n"
					"\t\tSizeY 3200 le { 500 exit} if\n"
					"\t\tSizeY 8000 le {1000 exit} if\n"
					"\t\tSixeY log 0.3 sub floor cvi 10 exch exp exit\n"
				"\t} repeat def  %% 1, /GridStepY\n"
				"\t/StartX  Actual_XMin GridStepX div ceiling cvi GridStepX mul  def\n"
				"\t/StartY  Actual_YMin GridStepY div ceiling cvi GridStepY mul  def\n"
				"\n"
				"\t//TileMatrix setmatrix\n"
				"\n"
				"\tStartY  GridStepY  Actual_YMax\n"
				"\t{\n"
				"\t\t/GridVal exch def\n"
					"\t\tActual_XMin GridVal moveto  Actual_XMax GridVal lineto\n"
				"\t} for  %% GridVal, horizontal lines\n"
				"\tStartX  GridStepX  Actual_XMax\n"
				"\t{\n"
				"\t\t/GridVal exch def\n"
					"\t\tGridVal Actual_YMin moveto  GridVal Actual_YMax lineto\n"
				"\t} for  %% GridVal, vertical lines\n"
				"\tmatrix currentmatrix  PaperMatrix setmatrix\n"
				"\tgsave 0.72 setlinewidth 1 setgray 1 setlinecap stroke grestore\n"
				"\t0.24 setlinewidth 0 setgray 0 setlinecap stroke\n"
				"\tsetmatrix\n"
				"\n"
				"\t/FontSize Actual_YMax Actual_YMin sub 60 div def\n"
				"\tFontName FontSize selectfont\n"
				"\tStartY  GridStepY  Actual_YMax\n"
				"\t{\n"
					"\t\t/GridVal exch def\n"
					"\t\tActual_XMax  GridVal FontSize 0.3 mul sub  moveto\n"
					"\t\tFontName FontSize\n"
					"\t\tGridVal 0 lt {[/minus GridVal abs 16 string cvs]} {GridVal 16 string cvs} ifelse\n"
					"\t\tdup StringWidthGeneral neg 0 rmoveto  ShowOutlined\n"
				"\t} for  %% GridVal, y labels\n"
				"\tStartX  GridStepX  Actual_XMax\n"
				"\t{\n"
					"\t\t/GridVal exch def\n"
					"\t\tGridVal  Actual_YMin  moveto\n"
					"\t\tFontName FontSize\n"
					"\t\tGridVal 0 lt {[/minus GridVal abs 16 string cvs]} {GridVal 16 string cvs} ifelse\n"
					"\t\tdup StringWidthGeneral -2 div 0 rmoveto  ShowOutlined\n"
				"\t} for  %% GridVal, x labels\n"
				"\tend\n"
			"} if  %% Paint gridlines?\n\n\n",
			showGridlines(tlngP, ef) ? "true" : "false",
			PS_rhomb == ef ? "" :  PS_arcs == ef ? "/FontName /Helvetica-Bold def\n" :  ""  // Last possibility ought to be unreachable.
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);
		break;
	}  // PS_rhomb, PS_arcs


	case SVG_rhomb:
	case SVG_arcs:
	{
		double xLow, yLow, xBig, yBig, xSize, ySize, fontSize;
		long int gridVal, xStart, yStart, xGridStep, yGridStep;
		bool isFirst;
		const int numDecimalPlaces = 4;
		xLow = tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength;  xBig = tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength;  xSize = xBig - xLow;
		yLow = tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength;  yBig = tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength;  ySize = yBig - yLow;
		xGridStep =
			xSize <=   16 ?    1 :
			xSize <=   32 ?    2 :
			xSize <=   80 ?    5 :
			xSize <=  160 ?   10 :
			xSize <=  320 ?   20 :
			xSize <=  800 ?   50 :
			xSize <= 1600 ?  100 :
			xSize <= 3200 ?  200 :
			xSize <= 8000 ?  500 :  (long int)round(pow(10, floor(log10(xSize) - 0.3)));
		yGridStep =
			ySize <=   16 ?    1 :
			ySize <=   32 ?    2 :
			ySize <=   80 ?    5 :
			ySize <=  160 ?   10 :
			ySize <=  320 ?   20 :
			ySize <=  800 ?   50 :
			ySize <= 1600 ?  100 :
			ySize <= 3200 ?  200 :
			ySize <= 8000 ?  500 :  (long int)round(pow(10, floor(log10(ySize) - 0.3)));
		xStart = (long int)ceil(xLow / xGridStep) * xGridStep;
		yStart = (long int)ceil(yLow / yGridStep) * yGridStep;
		if( ! showGridlines(tlngP, ef) )
		{
			(*numCharsThisFileP) += fprintf(fp, "<!-- To uncomment gridlines delete this line, and the line at end of this 'paragraph'.\n");
			(*numLinesThisFileP) ++ ;
		}
		(*numCharsThisFileP) += fprintf(fp, "<path vector-effect='non-scaling-stroke' stroke-width='3px' stroke='#000' opacity='0.25' d='\n");
		(*numLinesThisFileP) ++ ;

		isFirst = true;
		for( gridVal = (long int)ceil(yStart)  ;  gridVal <= yBig  ;  gridVal += yGridStep )
		{
			(*numCharsThisFileP) += fprintf(fp, "%sM %0.5lf %li  h %0.5lf",  isFirst ? "\t" : "   ",  xLow,  gridVal,  xSize);
			isFirst = false;
		}  // for( gridVal ... )
		(*numCharsThisFileP) += fprintf(fp, "\n");
		(*numLinesThisFileP) ++ ;
		isFirst = true;
		for( gridVal = (long int)ceil(xStart)  ;  gridVal <= xBig  ;  gridVal += xGridStep )
		{
			(*numCharsThisFileP) += fprintf(fp, "%sM %li %0.5lf  v %0.5lf",  isFirst ? "\t" : "   ",  gridVal,  yLow,  ySize);
			isFirst = false;
		}  // for( gridVal ... )
		
		fontSize = (yBig - yLow) / 60;
		(*numCharsThisFileP) += fprintf(fp,
			"\n'/>\n<g font-size='%0.3lf' font-weight='bold' paint-order='stroke fill' stroke-width='%0.3lf' stroke='#FFF' stroke-linejoin='round' fill='#000' opacity='1' text-anchor='middle'>\n",
			fontSize,  fontSize / 6.0
		);  // fprintf()
		(*numLinesThisFileP) += 3 ;
		
		for( gridVal = (long int)ceil(yStart)  ;  gridVal <= yBig  ;  gridVal += yGridStep )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"\t<g transform='translate(%0.*lf,%li)'><text text-anchor='end' alignment-baseline='central' transform='scale(1,-1)'>%s%li</text></g>\n",
				numDecimalPlaces, xBig  -  (SVG_arcs == ef ? 0.25 : 0),  gridVal,
				gridVal >= 0 ? "" : "&#8722;",  labs(gridVal)  // #8722 is is Unicode minus sign. Alas "&minus;" seems not to work: YMMV.
			);  // fprintf()
			(*numLinesThisFileP) ++ ;
		}  // y loop of gridVal

		for( gridVal = (long int)ceil(xStart)  ;  gridVal <= xBig  ;  gridVal += xGridStep )
		{
			(*numCharsThisFileP) += fprintf(fp,
				"\t<g transform='translate(%li,%0.*lf)'><text text-anchor='middle' alignment-baseline='text-bottom' transform='scale(1,-1)'>%s%li</text></g>\n",
				gridVal, numDecimalPlaces, yLow + (SVG_arcs == ef ? 0.25 : 0),
				gridVal >= 0 ? "" : "&#8722;",  labs(gridVal)  // #8722 is is Unicode minus sign. Alas "&minus;" seems not to work: YMMV.
			);  // fprintf()
			(*numLinesThisFileP) ++ ;
		}  // x loop of gridVal

		(*numCharsThisFileP) += fprintf(fp, "</g>\n");
		(*numLinesThisFileP) ++ ;

		if( ! showGridlines(tlngP, ef) )
		{
			(*numCharsThisFileP) += fprintf(fp, "To uncomment gridlines delete this line, and the line at start of this 'paragraph'. -->\n\n");
			(*numLinesThisFileP) += 2 ;
		}
		break;
	}  // SVG_rhomb, SVG_arcs


	default:
		break;
	}  // switch(ef)

}  // tiling_export_Gridlines()


void tiling_export_subroutines_PS(
	FILE* const fp,
	ExportFormat const ef,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	extern char scratchString[];

	switch (ef)
	{
	case PS_rhomb:
	case PS_arcs:
		{
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

				"\n\n\n%% Computes the path of a glyph, with the charpath-style boolean flag, moving the currentpoint as if after a show.\n"
				"/GlyphPathMoveto\n"
				"{\n"
					"\t2 dict begin\n"
					"\t/bln exch def\n"
					"\t/GlyphNameThis exch def\n"
					"\tcurrentfont\n"
					"\tcurrentfont length dict begin\n"
					"\tcurrentfont {1 index /FID ne {def} {pop pop} ifelse} forall\n"
					"\t/Encoding [ 256 {/.notdef} repeat ] def\n"
					"\tEncoding 97 GlyphNameThis put  %% ASCII a = 97\n"
					"\tcurrentdict\n"
					"\tend\n"
					"\t/TemporaryFont exch definefont pop\n"
					"\t/TemporaryFont 1 selectfont\n"
					"\tcurrentpoint exch (a) stringwidth pop add exch\n"
					"\t(a) bln charpath %% (a) has been redefined\n"
					"\tmoveto\n"
					"\tsetfont\n"
					"\t/TemporaryFont undefinefont\n"
					"\tend\n"
				"} bind def  %% /GlyphPathMoveto\n\n"

				"%% http://groups.google.com/g/comp.lang.postscript/c/hZmiLPonDl8  \"Possible bug in Mac Distiller\'s charpath?\", 16 Feb 2008\n"
				"%% There is a current point.\n"
				"%% CompoundString bool  CharPathGeneral  -\n"
				"/CharPathGeneral\n"
				"{\n"
					"\t2 dict begin\n"
					"\t/bln exch def\n"
					"\t/param exch def\n"
					"\t1 {\n"
						"\t\t/param load xcheck {[ param ]     bln CharPathGeneral         exit} if  %% executable\n"
						"\t\tparam type /arraytype  eq {param {bln CharPathGeneral} forall exit} if  %% array\n"
						"\t\tparam type /stringtype eq {param  bln                charpath exit} if  %% string\n"
						"\t\tparam type /nametype   eq {param  bln GlyphPathMoveto         exit} if  %% glyph\n"
						"\t\t(\nCharPathGeneral: type error. Continuing.) = param == (\n) =\n"
					"\t} repeat  %% 1\n"
					"\tend\n"
				"} bind def  %% /CharPathGeneral\n\n"

				"/StringWidthGeneral {gsave nulldevice 0 0 moveto true CharPathGeneral currentpoint pop grestore} bind def\n\n"

				"%% FontName FontSize CompoundString  ShowOutlined  -\n"
				"/ShowOutlined\n"
				"{\n"
					"\t3 dict begin\n"
					"\t/CompoundString exch def  /FontSize exch def  /FontName exch def\n"
					"\tFontName FontSize selectfont\n"
					"\tCompoundString true CharPathGeneral\n"
					"\tgsave FontSize 8 div setlinewidth 1 setgray 1 setlinejoin stroke grestore 0 setgray fill\n"
					"\tend\n"
				"} bind def  %% /ShowOutlined\n\n\n"
				
				"() =\n"
				"/NumRhombi NumFats NumThins add def"
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

				"\n\n(\\n\\nPathStats:\\n\\nPathClosed\\tPathLength\\tNumPaths\\tMaxNumThisFats\\tMaxNumDeepFats\\tMaxNumThisThins\\tMaxNumDeepThins\\tMaxNumThisFats_Num\\tMaxNumDeepFats_Num\\tMaxNumThisThins_Num\\tMaxNumDeepThins_Num\\tRadiusMinMin_EdgeLengths\\tRadiusMaxMax_EdgeLengths\\tWidthMax_EdgeLengths\\tHeightMax_EdgeLengths) =\n"
			);  // sprintf()
			(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
			(*numLinesThisFileP) += newlinesInString(scratchString);
			break;
		}  // PS_rhomb, PS_arcs

		default:
			break;
	}  // switch(ef)
}  // tiling_export_subroutines_PS()


void preamble_export_PS(
	FILE* const fp,
	Tiling const * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	// No ExportFormat parmater as fixed-form PostScript.
	extern char scratchString[];
	char seedTypeName[32];
	
	seed_type_name(seedTypeName,  tlngP->seedType);
	sprintf(scratchString,
			"%%!PS\n"
			"\n"
			"/TilingId %" PRIi8 " def\n"
			"/SeedType /%s def\n"
			"/EdgeLength 1 def\n"
			"/AxisAligned %s def\n"
			
			"/NumFats %li def\n"
			"/NumThins %li def\n"
			"/NumThins_0T4F %li def\n"
			"/NumThins_1T3F %li def\n"
			"/NumPathsClosed %li def\n"
			"/NumPathsOpen %li def\n"
			"/NumPathsStats %li def\n"
			"/BoundingPathNumVertices %lli def\n",
			tlngP->tilingId,  seedTypeName,  tlngP->axisAligned ? "true" : "false",
			
			tlngP->numFats,  tlngP->numThins,  tlngP->numThins_0T4F,  tlngP->numThins_1T3F,
			tlngP->numPathsClosed,  tlngP->numPathsOpen,  tlngP->numPathStats,  tlngP->boundingPathNumVertices
			);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	
	sprintf(scratchString,
			"/XMin %0.9lf def\n"
			"/XMax %0.9lf def\n"
			"/YMin %0.9lf def\n"
			"/YMax %0.9lf def\n"
			"/RadiusMax %.9lf def\n"
			"/RadiusShortOpen %.9lf def\n",
			tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength,
			tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength,
			tlngP->radiusMax / tlngP->edgeLength,  tlngP->radiusShortOpen / tlngP->edgeLength
			);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	
	sprintf(scratchString,
			"/DataAsOf (D:%04d%02d%02d%02d%02d%02d) def\n"
			"/SecondsToStartExportFromStartFirstTiling %0.6lf def\n"
			"/SecondsToStartExportFromStartThisTiling %0.6lf def\n"
			"/InternalToC_EdgeLength %.16G def\n"  // G is signifiant figures, aut-choosing decimal or scientific.
			"/Licence (%s) def\n"
			"/URL (%s) def\n"
			"/Author (%s) def\n",
			(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
			tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec),
			tlngP->SecondsToStartExportFromStartFirstTiling,  tlngP->SecondsToStartExportFromStartThisTiling,
			tlngP->edgeLength,  TextLicence,  TextURL,  TextAuthor
			);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	
	sprintf(scratchString,
			"/WantedPostScriptCentreX %.9lf def\n"
			"/WantedPostScriptCentreY %.9lf def\n"
			"/WantedPostScriptHalfWidth %.9lf def\n"
			"/WantedPostScriptHalfHeight %.9lf def\n"
			"/WantedPostScriptAspect %.9lf def\n"
			"/wantedPostScriptNumRhombi %li def\n"
			"/wantedPostScriptNumPaths %li def\n\n",
			tlngP->wantedPostScriptCentre.x,
			tlngP->wantedPostScriptCentre.y,
			tlngP->wantedPostScriptHalfWidth / tlngP->edgeLength,
			tlngP->wantedPostScriptHalfWidth / tlngP->edgeLength * tlngP->wantedPostScriptAspect,
			tlngP->wantedPostScriptAspect,
			tlngP->wantedPostScriptNumRhombi,  tlngP->wantedPostScriptNumPaths
			);  // sprintf()
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	
	double const xMinSuggest =  ceil(tlngP->xMinSuggestedZoom * 1000000) / 1000000;
	double const yMinSuggest =  ceil(tlngP->yMinSuggestedZoom * 1000000) / 1000000;
	double const xMaxSuggest = floor(tlngP->xMaxSuggestedZoom * 1000000) / 1000000;
	double const yMaxSuggest = floor(tlngP->yMaxSuggestedZoom * 1000000) / 1000000;
	
	long int const xMinDefault = (long int) floor(tlngP->rhombi[ tlngP->xMin_rhId ].xMin / tlngP->edgeLength);
	long int const xMaxDefault = (long int)  ceil(tlngP->rhombi[ tlngP->xMax_rhId ].xMax / tlngP->edgeLength);
	long int const yMinDefault = (long int) floor(tlngP->rhombi[ tlngP->yMin_rhId ].yMin / tlngP->edgeLength);
	long int const yMaxDefault = (long int)  ceil(tlngP->rhombi[ tlngP->yMax_rhId ].yMax / tlngP->edgeLength);
	
	short const widthChars = (short int) floor(max_4(
		log10(fabs((double) xMinDefault)),
		log10(fabs((double) xMaxDefault)),
		log10(fabs((double) yMinDefault)),
		log10(fabs((double) yMaxDefault))
	)) + 2;
	
	sprintf(scratchString,
		"\n%% Next four user changeable, for zoom. Commented suggestions are:  original enclosing all tiles;  maximum elegant area,\n"
		"%% enclosing the %li closed path%s of length %li and the %li of length %li;  and for my kitchen.\n"
		"/ToPaint_XMin % *li def  %% % *li  or  % 0.6lf  or  -57.5  %% Last column works with my kitchen, tilingId==9.\n"
		"/ToPaint_XMax % *li def  %% % *li  or  % 0.6lf  or  -10.0\n"
		"/ToPaint_YMin % *li def  %% % *li  or  % 0.6lf  or  -53.0\n"
		"/ToPaint_YMax % *li def  %% % *li  or  % 0.6lf  or   17.0\n"
		"\n",
		tlngP->pathStat[0].numPaths,  tlngP->pathStat[0].numPaths > 1 ? "s" : "",  tlngP->pathStat[0].pathLength,
		tlngP->pathStat[1].numPaths,  tlngP->pathStat[1].pathLength,
		widthChars, xMinDefault,   widthChars, xMinDefault,   xMinSuggest,
		widthChars, xMaxDefault,   widthChars, xMaxDefault,   xMaxSuggest,
		widthChars, yMinDefault,   widthChars, yMinDefault,   yMinSuggest,
		widthChars, yMaxDefault,   widthChars, yMaxDefault,   yMaxSuggest
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);
	
	sprintf(scratchString,
		"\n"
		"\n"
		"/PageWidth  420 360 mul 127 div def  %% A3, long side."  "  User alterable: for US Tabloid use \"11 72 mul\".\n"
		"/PageHeight 297 360 mul 127 div def  %% A3, short side." "  User alterable: for US Tabloid use \"17 72 mul\".\n"
		"/Margin 18 def  %% 18pt = 0.25\" = 6.35mm. Enlarge if the printer can't work so close to the edge. Used only for default value of scale.\n"
		"\n"
		"/Actual_XMin  XMin ToPaint_XMin  2 copy lt {exch} if pop  def\n"
		"/Actual_YMin  YMin ToPaint_YMin  2 copy lt {exch} if pop  def\n"
		"/Actual_XMax  XMax ToPaint_XMax  2 copy gt {exch} if pop  def\n"
		"/Actual_YMax  YMax ToPaint_YMax  2 copy gt {exch} if pop  def\n"
		"/ScaleFactor\n"
			"\tPageWidth  Margin 2 mul sub  Actual_XMax Actual_XMin sub  div  %% Tight-fiting x.\n"
			"\tPageHeight Margin 2 mul sub  Actual_YMax Actual_YMin sub  div  %% Tight-fiting y.\n"
			"\t2 copy gt {exch} if pop  %% Lesser of them.\n"
			"\t%% pop 150  %% For endpapers, on A3, with TilingId=9,  perhaps choose X -3.4 to 2.4,  Y -4 to 4,  then scaling of 150. But YMMV.\n"
		"def  %% /ScaleFactor. User alterable. Could be of form \"6 EdgeLength div\", making an edge be 6pt on the paper.\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

}  // tiling_scalars_export
