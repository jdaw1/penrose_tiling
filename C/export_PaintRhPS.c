// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintRhPS.c, in PenroseC

#include "penrose.h"

#define maybeTesting "%%"  // Replace with empty string not to comment the PostScript useful for debugging.

/*
	The ExportFormats export as data: arrays of stuff.
	But PostScript's maximum array length is 65535.
	So this code just outputs raw PostScript instructions, one after the other.
	All painting is determined one rhombus at a time: rhombii are not stored in a way that allows reasoning at a less-local level than one-at-a-time.
	There is some room for post-C hand editing of the PostScript, but much less flexibly the the other PostScript data format.
	There limit on the number of rhombii is printer memory and resolution, so much much larger than 2^16.

	Because this is just raw PostScript commands, this exporting C code does not have the structure of the other formats' C code.
	It is different and stand-alone.
*/

void tiling_export_PaintRhombiiPS(
	FILE* const fp,
	Tiling const      * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP,
	bool const deBugMode
)
{
	extern char scratchString[];
	RhombId rhId_This;
	PathStatId pathStatId;
	const Rhombus *rhThisP;
	const Path *pathP;
	PathStats *pathStatP;

	// PostScript definitions, preamble, comment about what is changeable, etc.
	sprintf(scratchString,
		"%%!PS\n"
		"\n"
		"/DataAsOf (D:%04d%02d%02d%02d%02d%02d) def\n"
		"/Licence (%s) def\n"
		"/URL (%s) def\n"
		"/Author (%s) def\n"
		"/TilingId %" PRIi8 " def\n"
		"/NumFats %li def\n"
		"/NumThins %li def\n"
		"/EdgeLength %.16E def\n"
		"\n"
		"/XMin %.16G def\n"
		"/XMax %.16G def\n"
		"/YMin %.16G def\n"
		"/YMax %.16G def\n"
		"\n"
		"/ToPaint_XMin %+li def  %% User changeable\n"
		"/ToPaint_XMax %+li def\n"
		"/ToPaint_YMin %+li def\n"
		"/ToPaint_YMax %+li def\n",
		(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
		tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec),
		TextLicence, TextURL, TextAuthor,
		tlngP->tilingId,  tlngP->numFats,  tlngP->numThins,
		tlngP->edgeLength,
		tlngP->xMin, tlngP->xMax, tlngP->yMin, tlngP->yMax,
		(long int)floor(tlngP->xMin), (long int)ceil(tlngP->xMax), (long int)floor(tlngP->yMin), (long int)ceil(tlngP->yMax)
	);
	stringClean(scratchString);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	// Using \t tabs, because an actual 9-tab might get converted to spaces by an evil IDE.
	sprintf(scratchString,
		"\n"
		"\n"
		"/PageWidth  297 360 mul 127 div def  %% A3, long side."  "  User alterable: for US Tabloid use \"11 72 mul\".\n"
		"/PageHeight 420 360 mul 127 div def  %% A3, short side." "  User alterable: for US Tabloid use \"17 72 mul\".\n"
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
		"def  %% /ScaleFactor. User alterable. Could be of form \"6 EdgeLength div\", making an edge be 6pt on the paper.\n"
		"/FontName /Helvetica def  %% /TrebuchetMS /GothamNarrow\n"
		"/FontSize EdgeLength ScaleFactor mul 0.3 mul def\n"
		"\n"
		"<< /PageSize [PageWidth PageHeight] >> setpagedevice\n"
		"/PaperMatrix matrix currentmatrix def\n"
		"PageWidth 0.5 mul PageHeight 0.5 mul translate  %% User alterable. This has tiling positioned wrt centre of paper page.\n"
		"ScaleFactor dup scale\n"
		"Actual_XMax Actual_XMin add -2 div  Actual_YMax Actual_YMin add -2 div  translate  %% User alterable. Optional specification of which point in tiling is to be in the specifed point of page.\n"
		"%% 0 rotate  %% User alterable. Optional rotation, angle in degrees, positive rotating image anti-clockise. By default commented out.\n"
		"/TileMatrix matrix currentmatrix def\n"
		"\n"
	);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	sprintf(scratchString,
		"mark\n"
			"\t/CreationDate (D:%04d%02d%02d%02d%02d%02d)\n"
			"\t/Title (Penrose tiling)\n"
			"\t/Author (Julian D. A. Wiseman)\n"
			"\t/Subject (http://github.com/jdaw1/penrose_tiling/)\n"
			"\t/Keywords (Penrose tiling rhombi)\n"
			"\t/Creator (C and PostScript coded by Julian D. A. Wiseman of www.jdawiseman.com/author.html)\n"
		"/DOCINFO pdfmark\n"
		"\n",
		(1900 + tlngP->timeData->tm_year),  (1 + tlngP->timeData->tm_mon),  tlngP->timeData->tm_mday,
		tlngP->timeData->tm_hour,  tlngP->timeData->tm_min,  (int)(tlngP->timeData->tm_sec)
	);
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
		"/NumRhombii NumFats NumThins add def"
		"[/TilingId /DataAsOf /NumFats /NumThins /NumRhombii /EdgeLength  /XMin /XMax /YMin /YMax  /ToPaint_XMin /ToPaint_XMax /ToPaint_YMin /ToPaint_YMax]\n"
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
		"mark (FontName = ) FontName (; FontSize = ) FontSize ConcatenateToMark =\n"

		"\n\n(\\n\\nPathStats:\\n\\nPathClosed\\tPathLength\\tNumPaths\\tMaxNumThisFats\\tMaxNumDeepFats\\tMaxNumThisThins\\tMaxNumDeepThins\\tMaxNumThisFats_Num\\tMaxNumDeepFats_Num\\tMaxNumThisThins_Num\\tMaxNumDeepThins_Num\\tRadiusMinMin_EdgeLengths\\tRadiusMaxMax_EdgeLengths\\tWidthMax_EdgeLengths\\tHeightMax_EdgeLengths) =\n"
	);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	if( !deBugMode )
	{
		for( pathStatId = 0  ;  pathStatId < (deBugMode ? 1 : tlngP->numPathStats)  ;  pathStatId ++ )
		{
			pathStatP = &(tlngP->pathStat[pathStatId]);
			(*numCharsThisFileP) += fprintf(fp, "(%s\\t%li\\t%li",
				pathStatP->pathClosed ? "Closed" : "Open",
				pathStatP->pathLength  *  ( pathStatP->pathClosed  &&  5==pathStatP->pathLength  &&  !pathStatP->pointy ? -1 : 1),  // Much abbreviated output.
				pathStatP->numPaths
			);
			if( pathStatP->pathClosed)
			{
				(*numCharsThisFileP) += fprintf(fp, "\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%.8f\\t%.8f\\t%.8f\\t%.8f",
					pathStatP->insideThis_MaxNumFats,
					pathStatP->insideDeep_MaxNumFats,
					pathStatP->insideThis_MaxNumThins,
					pathStatP->insideDeep_MaxNumThins,
					pathStatP->insideThis_MaxNumFats_Num,
					pathStatP->insideDeep_MaxNumFats_Num,
					pathStatP->insideThis_MaxNumThins_Num,
					pathStatP->insideDeep_MaxNumThins_Num,
					pathStatP->radiusMinMin / tlngP->edgeLength,  // Not stringClean()'d, for more even visual spacing in output.
					pathStatP->radiusMaxMax / tlngP->edgeLength,
					pathStatP->widthMax     / tlngP->edgeLength,
					pathStatP->heightMax    / tlngP->edgeLength
				);
			}  // pathClosed
			(*numCharsThisFileP) += fprintf(fp, ") =\n");
			(*numLinesThisFileP) ++;
		}  // for( pathStatId ... )
	}  // !deBugMode

	sprintf(scratchString,
		"\n"
		"flush\n"
		"\n"
		"FontName FontSize selectfont  %% Useful default if PaintByRhombus is to show, for example, the RhId.\n"
		"0 setgray  2 setlinejoin  [] 0 setdash\n"
		"PaperMatrix setmatrix  EdgeLength 60 div ScaleFactor mul  setlinewidth\n"
		"\n"
		"%% PaintByRhombus collects the items on the stack, and then paints as appropriate.\n"
		"%% Most of its inner may be rewritten, e.g. for different decoration, or to aid debugging such as by show'ing RhId.\n"
		"/PaintByRhombus\n"
			"{\n"
			"\t15 dict begin  %% Populating this dictionary with values on stack.\n"
			"\t/IsFat exch def  /RhId exch def\n"
			"\tIsFat {[/PathId /PathStatId /WithinPathNum /Pointy /PathLength /PathClosed] {exch def} forall} if  %% Fats only\n"
			"\t[/FilledType /NumNeighbours /AngDeg /NorthY /NorthX /EastY /EastX /SouthY /SouthX /WestY /WestX] {exch def} forall  %% All rhombii\n"
			"\t//true  %% Test whether this rhombus is inside the Wanted box.\n"
			"\t1 {\n"
			"\t\tNorthX //Actual_XMin le  EastX //Actual_XMin le  SouthX //Actual_XMin le  WestX //Actual_XMin le  and and and {pop //false exit} if\n"
			"\t\tNorthY //Actual_YMin le  EastY //Actual_YMin le  SouthY //Actual_YMin le  WestY //Actual_YMin le  and and and {pop //false exit} if\n"
			"\t\tNorthX //Actual_XMax ge  EastX //Actual_XMax ge  SouthX //Actual_XMax ge  WestX //Actual_XMax ge  and and and {pop //false exit} if\n"
			"\t\tNorthY //Actual_YMax ge  EastY //Actual_YMax ge  SouthY //Actual_YMax ge  WestY //Actual_YMax ge  and and and {pop //false exit} if\n"
			"\t} repeat  %% 1\n"
			"\t{\n"
				"\t\t%% Code inside here is intended to be user alterable.\n"
				"\t\t//TileMatrix setmatrix\n"
				"\t\t//true  %% paint rhombii?\n"
				"\t\t{\n"
					"\t\t\tNorthX NorthY moveto  EastX EastY lineto  SouthX SouthY lineto  WestX WestY lineto  closepath\n"
					"\t\t\tIsFat\n"
					"\t\t\t{\n"
						"\t\t\t\t%% Fat\n"
						"\t\t\t\tgsave\n"
						"\t\t\t\t//false\n"
						"\t\t\t\t1 {  %% repeat\n"
						"\t\t\t\t\t%% These colours use changeable. If not specified then not fill'd.\n"
						"\t\t\t\t\t%% Closed ==> 5 5 15 25 55 105 215 425 855 1705 3415 6825 13655 27305 54615 109225 218455 436905 873815 ... ([n-1] + 2*[n-2])\n"
						"\t\t\t\t\tPathLength    5 eq PathClosed and Pointy and {0.6 1   0.6 setrgbcolor pop //true exit} if  %% Very light green\n"
						"\t\t\t\t\tPathLength   15 eq PathClosed and  {0   0.3 0   setrgbcolor pop //true exit} if  %% very dark green\n"
						"\t\t\t\t\tPathLength   25 eq PathClosed and  {0   0   0.6 setrgbcolor pop //true exit} if  %% Dark blue\n"
						"\t\t\t\t\tPathLength   55 eq PathClosed and  {0   0   1   setrgbcolor pop //true exit} if  %% Blue\n"
						"\t\t\t\t\tPathLength  105 eq PathClosed and  {1   0.4 1   setrgbcolor pop //true exit} if  %% Pink\n"
						"\t\t\t\t\tPathLength  215 eq PathClosed and  {0.5 0   0   setrgbcolor pop //true exit} if  %% Maroon\n"
						"\t\t\t\t\tPathLength  425 eq PathClosed and  {0   0.6 0   setrgbcolor pop //true exit} if  %% Dark green\n"
						"\t\t\t\t\tPathLength  855 eq PathClosed and  {0   0.6 1   setrgbcolor pop //true exit} if  %% Light blue\n"
						"\t\t\t\t\tPathLength 1705 eq PathClosed and  {1   0.4 0   setrgbcolor pop //true exit} if  %% Orange\n"
						"\t\t\t\t\tPathLength 3415 ge PathClosed and  {0.1         setgray     pop //true exit} if  %% Very dark grey; ge ==> very long closed paths.\n"
						"\t\t\t\t\tPathLength  172 gt {PathLength 1.2 div ln 2 ln div floor cvi 4 mod 0.1 mul 0.7 add 0 0 setrgbcolor pop //true exit} if  %% Bright red  %% 172 = 215 *4/5\n"
						"\t\t\t\t} repeat  %% 1\n"
						"\t\t\t\t{fill} if\n"
						"\t\t\t\t%% Code that highlights the north corners of fat rhombii. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\tPathClosed not  pop true  %% Choose: all, or only fats in open paths?\n"
						maybeTesting "\t\t\t\t{\n"
							maybeTesting "\t\t\t\t\tnewpath  NorthX NorthY 2 copy  moveto\n"
							maybeTesting "\t\t\t\t\tEdgeLength 3 div  AngDeg 180 IsFat {36} {72} ifelse sub add  dup IsFat {72} {144} ifelse add  arc  closepath\n"
							maybeTesting "\t\t\t\t\t0.5 setgray  fill\n"
						maybeTesting "\t\t\t\t} if  %% PathClosed not\n"
						"\t\t\t\tgrestore\n"
						"\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						"\t\t\t\tPathClosed 5 PathLength eq and {Pointy not WithinPathNum 0 eq and} {//false} ifelse\n"
						"\t\t\t\t{\n"
							"\t\t\t\t\t%% Show small circles in non-Pointy 5 Closed paths.\n"
							"\t\t\t\t\t//TileMatrix setmatrix\n"
							"\t\t\t\t\tSouthX  SouthY  //EdgeLength 4 div  0  360  arc  closepath\n"
							"\t\t\t\t\tgsave 1 setgray fill grestore\n"
							"\t\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						"\t\t\t\t} if  %% Last rhombus in Closed, Length=5, non-Pointy\n"
						"\t\t\t\t%% Code that shows text about each fat rhombus. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\tPathClosed not  pop true  %% Choose: all, or only fats in open paths?\n"
						maybeTesting "\t\t\t\t{\n"
							maybeTesting "\t\t\t\t\t//TileMatrix setmatrix  NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto  0 setgray\n"
							maybeTesting "\t\t\t\t\t//PaperMatrix setmatrix mark RhId FilledType dup 0 gt {(,) exch} {pop} ifelse ConcatenateToMark dup stringwidth pop -2 div //FontSize -0.3 mul rmoveto show\n"
						maybeTesting "\t\t\t\t} if  %% PathClosed not\n"
					"\t\t\t}{\n"
						"\t\t\t\t%% Thin\n"
						"\t\t\t\tgsave 0.8 setgray fill grestore\n"
						"\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						"\t\t\t\t%% Code that highlights the north corners of thin rhombii. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\tgsave newpath //TileMatrix setmatrix  NorthX NorthY 2 copy  moveto\n"
						maybeTesting "\t\t\t\tEdgeLength 4 div  AngDeg 180 IsFat {36} {72} ifelse sub add  dup IsFat {72} {144} ifelse add  arc  closepath\n"
						maybeTesting "\t\t\t\t0.5 setgray  fill  grestore\n"
						"\t\t\t\t%% Code that shows text about each thin rhombus. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						maybeTesting "\t\t\t\t//TileMatrix setmatrix  NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto  0 setgray\n"
						maybeTesting "\t\t\t\t//PaperMatrix setmatrix mark RhId FilledType dup 0 gt {(,) exch} {pop} ifelse ConcatenateToMark dup stringwidth pop -2 div //FontSize -0.3 mul rmoveto show\n"
					"\t\t\t} ifelse  %% IsFat\n"
					"\t\t\tnewpath\n"
				"\t\t} if  %% painting rhombii\n"
				"\t\t//false  %% paint triangles?\n"
				"\t\t{\n"
					"\t\t\tEastX 0.25 mul WestX 0.75 mul add\n"
					"\t\t\tEastY 0.25 mul WestY 0.75 mul add\n"
					"\t\t\tWestX 0.25 mul EastX 0.75 mul add\n"
					"\t\t\tWestY 0.25 mul EastY 0.75 mul add\n"
					"\t\t\tNorthX NorthY moveto lineto lineto closepath\n"
					"\t\t\t0.8 setgray fill\n"
				"\t\t} if  %% painting triangles\n"
				"\t\t//false  %% stroke arcs?\n"
				"\t\t{\n"
					"\t\t\t//PaperMatrix setmatrix  0.06 setlinewidth  0 setgray  1 setlinecap  1 setlinejoin  [] 0 setdash\n"
					"\t\t\tWestX WestY  EdgeLength 2 div   EastX EastY  2 index    AngDeg dup dup dup\n"
					"\t\t\tIsFat\n"
						"\t\t\t\t{144 sub exch 36 sub   7 2 roll   36 add exch 144 add}\n"
						"\t\t\t\t{108 sub exch 72 sub   7 2 roll   72 add exch 108 add}\n"
					"\t\t\tifelse  %% IsFat\n"
					"\t\t\t2 {//TileMatrix setmatrix arc  //PaperMatrix setmatrix  stroke} repeat\n"
				"\t\t} if  %% stroking arcs\n"
				"\t\t//false  %% stroke diagonal?\n"
				"\t\t{\n"
					"\t\t\tTileMatrix setmatrix\n"
					"\t\t\tNorthX NorthY moveto  SouthX SouthY lineto\n"
					"\t\t\tPaperMatrix setmatrix  0 setgray  stroke\n"
				"\t\t} if  %% stroking diagonal\n"
			"\t} if  %% Inside Wanted box\n"
			"\tend  %% 15 dict\n"
		"} bind def  %% /PaintByRhombus\n"
		"\n"
		"\n"
		"\n"
		"%% Formats for Thin and Fat rhombii:\n"
		"\t%% WestX WestY SouthX SouthY EastX EastY NorthX NorthY Angle /RhId false  PaintByRhombus  %% ==> Thin\n"
		"\t%% WestX WestY SouthX SouthY EastX EastY NorthX NorthY Angle PathClosed PathLength Pointy WithinPathNum /PathStatId /PathId /RhId true  PaintByRhombus  %% ==> Fat\n"
		"\n"
	);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	// Thins
	for( rhId_This = tlngP->numFats + tlngP->numThins - 1  ;  rhId_This >= 0  ;  rhId_This -- )
	{
		rhThisP = &(tlngP->rhombii[rhId_This]);
		if( Thin == rhThisP->physique )
		{
			sprintf(scratchString,
				"%0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf",
				rhThisP->west.x, rhThisP->west.y, rhThisP->south.x, rhThisP->south.y,
				rhThisP->east.x, rhThisP->east.y, rhThisP->north.x, rhThisP->north.y,
				rhThisP->angleDegrees
			);
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp,
				"%s %" PRIi8 " %" PRIi8 " %li false PaintByRhombus\n",
				scratchString, rhThisP->numNeighbours,  rhThisP->filledType, rhId_This
			);
			(*numLinesThisFileP) ++;
		}  // Thin
	}  // for( rhId ... )

	// Fats ordered reverse by PathStat (so open paths first, within which shorter paths first).
	// So that rhombii which might be most important are painted last.
	pathP = NULL;
	pathStatP = NULL;
	for( pathStatId = (deBugMode ? 0 : tlngP->numPathStats - 1)  ;  pathStatId >= 0  ;  pathStatId -- )
	{
		if( !deBugMode )
			pathStatP = &(tlngP->pathStat[pathStatId]) ;

		for( rhId_This = tlngP->numFats + tlngP->numThins - 1 ;  rhId_This >= 0  ;  rhId_This -- )
			if( Fat == tlngP->rhombii[rhId_This].physique )
			{
				rhThisP = &(tlngP->rhombii[rhId_This]);
				pathP   = &(tlngP->path[ rhThisP->pathId ]);
				if(
					pathStatP->pathClosed == pathP->pathClosed  &&
					pathStatP->pathLength == pathP->pathLength  &&
					( 5 != pathP->pathLength  ||  (!pathP->pathClosed)  ||  pathStatP->pointy == pathP->pointy )
				)
				{
					sprintf(scratchString,
						"%0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf %0.9lf",
						rhThisP->west.x, rhThisP->west.y, rhThisP->south.x, rhThisP->south.y,
						rhThisP->east.x, rhThisP->east.y, rhThisP->north.x, rhThisP->north.y,
						rhThisP->angleDegrees
					);
					stringClean(scratchString);
					(*numCharsThisFileP) += fprintf(fp,
						"%s %" PRIi8 " %" PRIi8 " %s %li %s %li %li %li %li true PaintByRhombus\n",
						scratchString,
						rhThisP->numNeighbours,   rhThisP->filledType,
						deBugMode ? "true" : (pathP->pathClosed ? "true" : "false"),
						deBugMode ? 0 : (pathP->pathLength),
						deBugMode ? "false" : ((5 == pathP->pathLength && pathP->pathClosed && pathP->pointy) ? "true" : "false"),
						deBugMode ? -1 : rhThisP->withinPathNum,
						pathStatId, rhThisP->pathId, rhId_This
					);
					(*numLinesThisFileP) ++;
				}  // if( 'pathId' == 'pathStatP' )
			}  // Fat
	}  // for( pathStatId ... )


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
	);
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

}  // tiling_export_PaintRhombiiPS
