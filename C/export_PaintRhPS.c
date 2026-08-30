// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// export_PaintRhPS.c, in PenroseC

#include "penrose.h"

#define maybeTesting "%%"  // Replace with empty string not to comment the PostScript useful for debugging.

/*
	The ExportFormats export as data: arrays of stuff.
	But PostScript's maximum array length is 65535.
	So this code just outputs raw PostScript instructions, one after the other.
	All painting is determined one rhombus at a time: rhombi are not stored in a way that allows reasoning at a less-local level than one-at-a-time.
	There is some room for post-C hand editing of the PostScript, but much less flexibly the the other PostScript data format.
	There limit on the number of rhombi is printer memory and resolution, so much much larger than 2^16.

	Because this is just raw PostScript commands, this exporting C code does not have the structure of the other formats' C code.
	It is different and stand-alone.
*/

void tiling_export_PaintRhombiPS(
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
	const Rhombus   *rhThisP;
	const Path      *pathP;
	const PathStats *pathStatP;

	const double toPaint_xMin = postScript_toPaint_xMin(tlngP);  // in controls.c
	const double toPaint_xMax = postScript_toPaint_xMax(tlngP);
	const double toPaint_yMin = postScript_toPaint_yMin(tlngP);
	const double toPaint_yMax = postScript_toPaint_yMax(tlngP);

	// PostScript definitions, preamble, comment about what is changeable, etc.
	preamble_export_PS(fp,  tlngP,  numLinesThisFileP,  numCharsThisFileP);

	// Using \t tabs, because an actual 9-tab might get converted to spaces by an evil IDE.
	sprintf(scratchString,
		"\n"
		"\n\n\n"
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
	);  // sprintf()
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
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	sprintf(scratchString, "\n/ShowTileCounts false def\n\n");
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	tiling_export_subroutines_PS(fp,  PS_rhomb,  numLinesThisFileP,  numCharsThisFileP);

	if( !deBugMode )
	{
		for( pathStatId = 0  ;  pathStatId < (deBugMode ? 1 : tlngP->numPathStats)  ;  pathStatId ++ )
		{
			pathStatP = &(tlngP->pathStat[pathStatId]);
			(*numCharsThisFileP) += fprintf(fp, "(%s\\t%li\\t%li",
				pathStatP->pathClosed ? "Closed" : "Open",
				pathStatP->pathLength  *  ( pathStatP->pathClosed  &&  5==pathStatP->pathLength  &&  !pathStatP->pointy ? -1 : 1),  // Much abbreviated output.
				pathStatP->numPaths
			);  // fprintf()
			if( pathStatP->pathClosed)
			{
				(*numCharsThisFileP) += fprintf(fp,
					"\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%li\\t%.9f\\t%.9f\\t%.9f\\t%.9f",
					pathStatP->insideThis_MaxNumFats,
					pathStatP->insideDeep_MaxNumFats,
					pathStatP->insideThis_MaxNumThins,
					pathStatP->insideDeep_MaxNumThins,
					pathStatP->insideThis_MaxNumFats_Num,
					pathStatP->insideDeep_MaxNumFats_Num,
					pathStatP->insideThis_MaxNumThins_Num,
					pathStatP->insideDeep_MaxNumThins_Num,
					pathStatP->radiusMin / tlngP->edgeLength,  // Not stringClean()'d, for more even visual spacing in output.
					pathStatP->radiusMax / tlngP->edgeLength,
					pathStatP->widthMax  / tlngP->edgeLength,
					pathStatP->heightMax / tlngP->edgeLength
				);  // fprintf()
			}  // pathClosed
			(*numCharsThisFileP) += fprintf(fp, ") =\n");
			(*numLinesThisFileP) ++;
		}  // for( pathStatId ... )
	}  // !deBugMode

	sprintf(scratchString,
		"\n"
		"flush\n"
		"\n"
		"/PaintedNumByType 0 dict def\n"
		"\n"
		"FontName FontSize selectfont  %% Useful default if PaintByRhombus is to show, for example, the RhId.\n"
		"0 setgray  2 setlinejoin  [] 0 setdash\n"
		"PaperMatrix setmatrix  EdgeLength 16 div ScaleFactor mul  setlinewidth\n"
		"\n"
		"%% PaintByRhombus collects the items on the stack, and then paints as appropriate.\n"
		"%% Most of its inner may be rewritten, e.g. for different decoration, or to aid debugging such as by show'ing RhId.\n"
		"/PaintByRhombus\n"
			"{\n"
			"\t17 dict begin  %% Populating this dictionary with values on stack.\n"
			"\t/IsFat exch def  /RhId exch def\n"
			"\tIsFat {[/PathId /PathStatId /WithinPathNum /Pointy /PathLength /PathClosed] {exch def} forall} if  %% Fats only\n"
			"\t[/FilledType /NumNeighbours /AngDeg /NorthY /NorthX /EastY /EastX /SouthY /SouthX /WestY /WestX] {exch def} forall  %% All rhombi\n"
			"\t//true  %% Test whether this rhombus is inside the Wanted box.\n"
			"\t1 {\n"
			"\t\tNorthX //Actual_XMin le  EastX //Actual_XMin le  SouthX //Actual_XMin le  WestX //Actual_XMin le  and and and {pop //false exit} if\n"
			"\t\tNorthY //Actual_YMin le  EastY //Actual_YMin le  SouthY //Actual_YMin le  WestY //Actual_YMin le  and and and {pop //false exit} if\n"
			"\t\tNorthX //Actual_XMax ge  EastX //Actual_XMax ge  SouthX //Actual_XMax ge  WestX //Actual_XMax ge  and and and {pop //false exit} if\n"
			"\t\tNorthY //Actual_YMax ge  EastY //Actual_YMax ge  SouthY //Actual_YMax ge  WestY //Actual_YMax ge  and and and {pop //false exit} if\n"
			"\t} repeat  %% 1\n"
			"\t{\n"
				"\t\t/CentreX NorthX SouthX add 2 div def  /CentreY NorthY SouthY add 2 div def\n"
				"\t\t//TileMatrix setmatrix\n"
				"\t\t%% Boolean: and for this bool example variations follow, the first slightly complicated, one second very simple.\n"
				"\t\t%% CentreX ToPaint_XMin 0.52 mul 0.48 ToPaint_XMax mul add le  CentreY ToPaint_YMin 0.54 mul 0.46 ToPaint_YMax mul add le  or  %% If uncommented, replaces following line.\n"
				"\t\t//true  %% Paint rhombi? Perhaps comment out this line, then uncomment and edit previous line.\n"
				"\t\t{\n"
					"\t\t\tgsave\n"
					"\t\t\ttrue   CentreX dup mul CentreY dup mul add RadiusShortOpen dup mul ge  and  %% Boolean: fade the far-away rhombi? But not rendered correctly by Preview; use Reader.\n"
					"\t\t\t{\n"
						"\t\t\t\t[ currenttransfer   /exec cvx   1   /exch cvx   /sub cvx  0.4  /mul cvx   1   /exch cvx   /sub cvx ] cvx bind settransfer\n"
					"\t\t\t} if  %% Fade the far-away rhombi? But not rendered correctly by Preview; use Reader.\n"
					"\t\t\tNorthX NorthY moveto  EastX EastY lineto  SouthX SouthY lineto  WestX WestY lineto  closepath\n"
					"\t\t\tIsFat\n"
					"\t\t\t{\n"
						"\t\t\t\t%% Fat\n"
						"\t\t\t\tPathClosed {PathLength 5 eq {Pointy {(p_)} {(r_)} ifelse} {(c_)} ifelse} {(o_)} ifelse PathLength 10 string cvs Concatenate cvn\n"
						"\t\t\t\t//PaintedNumByType exch  2 copy known {2 copy get 1 add} {1} ifelse put\n"
						"\t\t\t\tgsave\n"
						"\t\t\t\t//false\n"
						"\t\t\t\t1 {  %% repeat\n"
						"\t\t\t\t\t%% These colours user changeable. If not specified then not fill'd.\n"
						"\t\t\t\t\t%% Closed ==> 5 5 15 25 55 105 215 425 855 1705 3415 6825 13655 27305 54615 109225 218455 436905 873815 1747625 3495255 6990505 ... ([n-1] + 2*[n-2])\n"
						"\t\t\t\t\tPathLength    5 eq PathClosed and Pointy and {0.6 1   0.6 setrgbcolor pop //true exit} if  %% Very light green\n"
						"\t\t\t\t\tPathLength   15 eq PathClosed and  {0   0.3 0   setrgbcolor pop //true exit} if  %% Very dark green\n"
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
						"\t\t\t\t%% Code that highlights the north corners of fat rhombi. Used in debugging. By default commented out.\n"
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
						"\t\t\t\tShowTileCounts\n"
						"\t\t\t\t{\n"
							"\t\t\t\t\t//TileMatrix setmatrix NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto\n"
							"\t\t\t\t\t0 PaintedNumByType {exch /thin ne {add} {pop} ifelse} forall  10 string cvs  dup\n"
							"\t\t\t\t\t//PaperMatrix setmatrix  stringwidth pop -2 div   FontSize -0.4 mul  rmoveto\n"
							"\t\t\t\t\ttrue charpath gsave FontSize 8 div setlinewidth 1 setgray 1 setlinejoin stroke grestore 0 setgray fill\n"
						"\t\t\t\t} if  %% ShowTileCounts\n"
						"\t\t\t\t%% Code that shows text about each fat rhombus. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\tPathClosed not  pop true  %% Choose: all, or only fats in open paths?\n"
						maybeTesting "\t\t\t\t{\n"
							maybeTesting "\t\t\t\t\t//TileMatrix setmatrix  NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto  0 setgray\n"
							maybeTesting "\t\t\t\t\t//PaperMatrix setmatrix mark RhId FilledType dup 0 gt {(,) exch} {pop} ifelse ConcatenateToMark dup stringwidth pop -2 div //FontSize -0.3 mul rmoveto show\n"
						maybeTesting "\t\t\t\t} if  %% PathClosed not\n"
					"\t\t\t}{\n"
						"\t\t\t\t%% Thin\n"
						"\t\t\t\t//PaintedNumByType /thin 2 copy known {2 copy get 1 add} {1} ifelse put\n"
						"\t\t\t\tgsave 0.8 setgray fill grestore\n"
						"\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						"\t\t\t\tShowTileCounts\n"
						"\t\t\t\t{\n"
							"\t\t\t\t\t//TileMatrix setmatrix NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto\n"
							"\t\t\t\t\t//PaintedNumByType /thin get  10 string cvs  dup\n"
							"\t\t\t\t\t//PaperMatrix setmatrix  stringwidth pop -2 div   FontSize -0.4 mul  rmoveto\n"
							"\t\t\t\t\ttrue charpath gsave FontSize 8 div setlinewidth 1 setgray 1 setlinejoin stroke grestore 0 setgray fill\n"
						"\t\t\t\t} if  %% ShowTileCounts\n"
						"\t\t\t\t%% Code that highlights the north corners of thin rhombi. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\tgsave newpath //TileMatrix setmatrix  NorthX NorthY 2 copy  moveto\n"
						maybeTesting "\t\t\t\tEdgeLength 4 div  AngDeg 180 IsFat {36} {72} ifelse sub add  dup IsFat {72} {144} ifelse add  arc  closepath\n"
						maybeTesting "\t\t\t\t0.5 setgray  fill  grestore\n"
						"\t\t\t\t%% Code that shows text about each thin rhombus. Used in debugging. By default commented out.\n"
						maybeTesting "\t\t\t\t//PaperMatrix setmatrix  0 setgray  stroke\n"
						maybeTesting "\t\t\t\t//TileMatrix setmatrix  NorthX SouthX add 2 div  NorthY SouthY add 2 div  moveto  0 setgray\n"
						maybeTesting "\t\t\t\t//PaperMatrix setmatrix mark RhId FilledType dup 0 gt {(,) exch} {pop} ifelse ConcatenateToMark dup stringwidth pop -2 div //FontSize -0.3 mul rmoveto show\n"
					"\t\t\t} ifelse  %% IsFat\n"
					"\t\t\tgrestore\n"
					"\t\t\tnewpath\n"
				"\t\t} if  %% painting rhombi\n"
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
					"\t\t\t//PaperMatrix setmatrix  0 setgray  1 setlinecap  1 setlinejoin  [] 0 setdash\n"
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
					"\t\t\t0 setgray  PaperMatrix setmatrix  stroke\n"
				"\t\t} if  %% stroking diagonal\n"
			"\t} if  %% Inside Wanted box\n"
			"\tend  %% 15 dict\n"
		"} bind def  %% /PaintByRhombus\n"
		"\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	if( exportQ(boundingPath, PS_rhomb, tlngP, *numLinesThisFileP) )
	{
		(*numCharsThisFileP) += fprintf(fp, "\n\nTileMatrix setmatrix\n");
		(*numCharsThisFileP) += fprintf(fp, "%% Bounding path\n");
		(*numLinesThisFileP) += 4;
		tiling_export_PaintBoundary(
			fp,
			PS_rhomb,
			1.0 / tlngP->edgeLength,
			tlngP,
			0,
			numLinesThisFileP,numCharsThisFileP
		);  // tiling_export_PaintBoundary()
		sprintf(scratchString,
			"closepath\n"
			"gsave\n"
				"\tPaperMatrix setmatrix\n"
				"\tEdgeLength 0.3125 mul ScaleFactor mul setlinewidth\n"
				"\t0.4 setgray\n"
				"\t1 setlinejoin  %% round, PLRM3 p673-4\n"
				"\t1 setlinecap  %% In GhostScript 10.06.0 the closepath seems not to work for tilingId>=6. Presumably long path length. This achieves optical equivalent.\n"
				"\tstroke\n"
			"grestore\n"
			"1 setgray  fill\n"
		);  // sprintf()
		(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
		(*numLinesThisFileP) += newlinesInString(scratchString);
	}  // if boundingPath


	sprintf(scratchString,
		"\n"
		"\n"
		"%% Formats for Thin and Fat rhombi:\n"
		"\t%% WestX WestY SouthX SouthY EastX EastY NorthX NorthY Angle /RhId false  PaintByRhombus  %% ==> Thin\n"
		"\t%% WestX WestY SouthX SouthY EastX EastY NorthX NorthY Angle PathClosed PathLength Pointy WithinPathNum /PathStatId /PathId /RhId true  PaintByRhombus  %% ==> Fat\n"
		"\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	// Thins
	for( rhId_This = 0  ;  rhId_This < tlngP->numFats + tlngP->numThins  ;  rhId_This ++ )
	{
		rhThisP = &(tlngP->rhombi[rhId_This]);
		if( Thin == rhThisP->physique
		&&  rhThisP->xMax > toPaint_xMin
		&&  rhThisP->xMin < toPaint_xMax
		&&  rhThisP->yMax > toPaint_yMin
		&&  rhThisP->yMin < toPaint_yMax )
		{
			sprintf(scratchString,
				"%.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.10lf",
				rhThisP->west.x  / tlngP->edgeLength,
				rhThisP->west.y  / tlngP->edgeLength,
				rhThisP->south.x / tlngP->edgeLength,
				rhThisP->south.y / tlngP->edgeLength,
				rhThisP->east.x  / tlngP->edgeLength,
				rhThisP->east.y  / tlngP->edgeLength,
				rhThisP->north.x / tlngP->edgeLength,
				rhThisP->north.y / tlngP->edgeLength,
				rhThisP->angleDegrees
			);  // sprintf()
			stringClean(scratchString);
			(*numCharsThisFileP) += fprintf(fp,
				"%s %" PRIi8 " %" PRIi8 " %li false PaintByRhombus\n",
				scratchString, rhThisP->numNeighbours,  rhThisP->filledType, rhId_This
			);  // fprintf()
			(*numLinesThisFileP) ++;
		}  // Thin and inside toPaint_...
	}  // for( rhId ... )


	// Fats ordered reverse by PathStat (so open paths first, within which shorter paths first).
	// So that rhombi which might be most important are painted last.
	pathP = NULL;
	pathStatP = NULL;
	for( pathStatId = (deBugMode ? 0 : tlngP->numPathStats - 1)  ;  pathStatId >= 0  ;  pathStatId -- )
	{
		if( !deBugMode )
			pathStatP = &(tlngP->pathStat[pathStatId]) ;

		for( rhId_This = tlngP->numFats + tlngP->numThins - 1 ;  rhId_This >= 0  ;  rhId_This -- )
		{
			rhThisP = &(tlngP->rhombi[rhId_This]);
			if( Fat == rhThisP->physique )
			{
				pathP = &(tlngP->path[ rhThisP->pathId ]);  // Known to be fat
				if( pathStatP->pathClosed == pathP->pathClosed
				&&  pathStatP->pathLength == pathP->pathLength
				&&  ( 5 != pathP->pathLength  ||  (!pathP->pathClosed)  ||  pathStatP->pointy == pathP->pointy )
				&&  rhThisP->xMax > toPaint_xMin
				&&  rhThisP->xMin < toPaint_xMax
				&&  rhThisP->yMax > toPaint_yMin
				&&  rhThisP->yMin < toPaint_yMax )
				{
					sprintf(scratchString,
						"%.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.9lf %.10lf",
						rhThisP->west.x  / tlngP->edgeLength,
						rhThisP->west.y  / tlngP->edgeLength,
						rhThisP->south.x / tlngP->edgeLength,
						rhThisP->south.y / tlngP->edgeLength,
						rhThisP->east.x  / tlngP->edgeLength,
						rhThisP->east.y  / tlngP->edgeLength,
						rhThisP->north.x / tlngP->edgeLength,
						rhThisP->north.y / tlngP->edgeLength,
						rhThisP->angleDegrees
					);  // sprintf()
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
					);  // fprintf()
					(*numLinesThisFileP) ++;
				}  // if( 'pathId' == 'pathStatP' ) and inside toPaint_...
			}  // Fat
		}  // for( rhId_This ... )
	}  // for( pathStatId ... )

	sprintf(scratchString,
		"false   SeedType /Round_5 eq and  RadiusShortOpen 0 gt and  %% Boolean: paint circle excluding fats in short open paths\n"
		"{\n"
			"\tTileMatrix setmatrix   0 30 330 {/ang exch def  0  0  RadiusShortOpen  ang  dup 30 add  arc} for  %% 30 degree pieces for pointless accuracy.\n"
			"\tPaperMatrix setmatrix  1 setlinejoin\n"
			"\tgsave 3 setlinewidth 1 setgray stroke grestore\n"
			"\t1 setlinewidth 0 setgray stroke\n"
		"} if  %% circle bounding interesting well-populated rhombii\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);

	tiling_export_Gridlines(fp,  tlngP,  PS_rhomb,  numLinesThisFileP,  numCharsThisFileP);  // Code always present; whether or not active controlled by showGridlines()
			
	sprintf(scratchString,
		"TilingId -9 eq  %% Boolean: paint kitchen? Perhaps change \"-9\" to \"9\".\n"
		"{\n"
			"\t4 dict begin\n\n"

			"\t/mmTileEdge 150.6 def  %% Tile edges must have matching lengths. This is that length, in millimetres, allowing for grouting.\n\n"
			
			"\t%% Going to do a translation, in TileSpace, in which a tile is of length EdgeLength = 1.\n"
			"\t%% Almost certainly, you want to translate to a point within ToPaint_XMin etc.\n"
			"\t%% This point is the origin, the zero point, for the drawing of the room.\n"
			"\t%% Suggestion: make it the point from which physical tiling will start, so the point of certain tile alignment,\n"
			"\t%% so allowing (small) changes in mmTileEdge to correctly adjust for mis-judgements of grouting thickness.\n"
			"\tTileMatrix setmatrix  -11.3 -21.8 translate\n"
			"\tEdgeLength mmTileEdge div dup scale  /KitchenMatrix matrix currentmatrix def\n\n"
			
			"\tfalse  %% Boolean: identify zero location (advice: use to assist choice of it, then disable).\n"
			"\t{\n"
				"\t\t2 dict begin\n"
				"\t\t/FontSize 5 def\n"
				"\t\tKitchenMatrix setmatrix   mmTileEdge EdgeLength div dup scale   matrix currentmatrix\n"
				"\t\t[ 10000 6000 4000 3000 2000 1500 1000 600 400 300 200 150 100 60 40 30 20 15 10 6 4 3 2 1 0.01 ]\n"
				"\t\tdup\n"
				"\t\t{\n"
					"\t\t\t/U_p exch def\n"
					"\t\t\tU_p  Actual_XMax Actual_XMin sub  lt\n"
					"\t\t\tU_p  Actual_YMax Actual_YMin sub  lt   or\n"
					"\t\t\t{\n"
						"\t\t\t\t/V_p 0.666666 U_p mul def  /U_n U_p neg def  /V_n V_p neg def  %% U, V; positive, negative\n"
						"\t\t\t\tU_p V_p moveto  U_p U_p  2 copy  V_p U_p  curveto\n"
						"\t\t\t\tV_n U_p lineto  U_n U_p  2 copy  U_n V_p  curveto\n"
						"\t\t\t\tU_n V_n lineto  U_n U_n  2 copy  V_n U_n  curveto\n"
						"\t\t\t\tV_p U_n lineto  U_p U_n  2 copy  U_p V_n  curveto closepath\n"
					"\t\t\t} if  %% U_p <= page size\n"
				"\t\t} forall  %% U_p\n"
				"\t\tPaperMatrix setmatrix gsave 1.44 setlinewidth 0.8 setgray stroke grestore 0.48 setlinewidth 0.2 setgray stroke\n"
				"\t\t{  %% forall with fixed array, again\n"
					"\t\t\t/U exch def\n"
					"\t\t\tU Actual_XMax Actual_XMin sub lt   U Actual_YMax Actual_YMin sub lt   or   U 0.5 gt  and\n"
					"\t\t\t{\n"
					 	"\t\t\t\tdup setmatrix  U neg U moveto\n"
					 	"\t\t\t\tPaperMatrix setmatrix 0 FontSize -0.4 mul rmoveto\n"
						"\t\t\t\tU 20 string cvs  FontName FontSize 3 -1 roll  ShowOutlined\n"
					"\t\t\t} if  %% U <= page size\n"
				"\t\t} forall  %% U\n"
				"\t\tpop   end\n"
			"\t} if  %% Test zero location\n\n"

			"\t%% Room specified in the mm, per mmTileEdge.\n"
			"\t%% The starting 'moveto' should ideally be to a point near 0,0; i.e., near the place chosen the previous paragraph.\n"
			"\t%% Carefully measure room; carefully echo those measurements to this section. Start with a 'moveto', then better to use 'rlineto's.\n"
			"\t/KitchenRoomPath\n"
			"\t{\n"
				"\t\t%% in mm\n"
				"\t\t    0      0  moveto\n"  // Living room north
				"\t\t-3735      0  rlineto\n"
				"\t\t    0   +320  rlineto\n"
				"\t\t -190      0  rlineto\n"
				"\t\t    0   +305  rlineto\n"  // Downpipe box: Check
				"\t\t +225      0  rlineto\n"
				"\t\t    0  +5160  rlineto\n"
				"\t\t-2590      0  rlineto\n"  // table holding
				"\t\t    0   -140  rlineto\n"
				"\t\t -355      0  rlineto\n"
				"\t\t    0   -585  rlineto\n"
				"\t\t -237      0  rlineto\n"
				"\t\t    0  -3587  rlineto\n"  // New windows
				"\t\t +237      0  rlineto\n"
				"\t\t    0  -1973  rlineto\n"
				"\t\t -250      0  rlineto\n"
				"\t\t    0  -3370  rlineto\n"
				"\t\t +250      0  rlineto\n"
				"\t\t    0   -845  rlineto\n"
				"\t\t+6500      0  rlineto\n"
				"\t\t    0   +475  rlineto\n"
				"\t\t +215      0  rlineto\n"
				"\t\t    0  +3280  rlineto\n"
				"\t\t  -70      0  rlineto\n"
				"\t\t    0   +960  rlineto\n"
				"\t\tclosepath\n"
			"\t} bind def  %% /KitchenRoomPath\n\n"

			"\t/KitchenFurniturePath\n"
			"\t{\n"
				"\t\t%% in mm\n"
				"\t\t-1380  -2050  moveto  %% island\n"
				"\t\t    0   -650  rlineto\n"
				"\t\t-1000      0  rlineto\n"
				"\t\t    0   -220  rlineto\n"
				"\t\t-1600      0  rlineto\n"
				"\t\t    0  +1090  rlineto\n"
				"\t\t+1050      0  rlineto\n"
				"\t\t    0   -220  rlineto\n"
				"\t\tclosepath\n\n"

				"\t\t-2030      0  moveto  %% cupboard\n"
				"\t\t    0   -150  rlineto\n"
				"\t\t-1070      0  rlineto\n"
				"\t\t    0    150  rlineto\n\n"

				"\t\t-6645	 -500  moveto  %% fridge box\n"
				"\t\t +640      0  rlineto\n"
				"\t\t    0   +760  rlineto\n"
				"\t\t -640      0  rlineto\n\n"

				"\t\t-6645  -4180  moveto  %% cabinets\n"
				"\t\t 6715      0  rlineto\n"
			"\t} bind def  %% /KitchenFurniturePath\n\n"

			"\tgsave\n"
			"\t[\n"
				"\t\t{3 setlinewidth  1 setgray  1 setlinejoin  0 setlinecap}  %% setlinewidth in PaperMatrix, so in points\n"
				"\t\t{1 setlinewidth  0 setgray  0 setlinejoin  1 setlinecap}\n"
			"\t]\n"
			"\t{\n"
				"\t\t/KitchenFormatting exch def\n"
				"\t\t[ {KitchenRoomPath} {KitchenFurniturePath} ]\n"
				"\t\t{\n"
					"\t\t\tKitchenMatrix setmatrix   exec\n"
					"\t\t\tPaperMatrix setmatrix  KitchenFormatting  stroke\n"
				"\t\t}\n"
				"\t\tforall  %% Paths\n"
			"\t} forall  %% Formatting\n"
			"\tgrestore\n"
			"\tend\n"
		"} if  %% Paint kitchen?\n\n\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	sprintf(scratchString,
		"false   %% Boolean: label orientation, do or don\'t?\n"
		"{\n"
			"\t1 dict begin\n"
			"\t/FontSize 14 def\n\n"

			"\tPaperMatrix setmatrix   FontName FontSize selectfont\n\n"

			"\tFontName FontSize  [(Label at top: e.g., ) /quotedblleft (Garden) /quotedblright (.)]\n"
			"\tPageWidth 2 div  PageHeight Margin FontSize add sub  moveto   dup StringWidthGeneral -2 div 0 rmoveto   ShowOutlined\n\n"

			"\tFontName FontSize  [(Label at bottom: e.g., ) /quotedblleft (North) /quotedblright (.)]\n"
			"\tPageWidth 2 div  Margin  moveto   dup StringWidthGeneral -2 div 0 rmoveto   ShowOutlined\n\n"

			"\tFontName FontSize  [(Label at left: e.g., ) /quotedblleft (South) /quotedblright (.)]\n"
			"\tgsave\n"
			"\tMargin FontSize add  PageHeight 2 div translate 90 rotate\n"
			"\t0 0 moveto   dup StringWidthGeneral -2 div 0 rmoveto   ShowOutlined\n"
			"\tgrestore\n\n"

			"\tFontName FontSize  [(Label at right: e.g., ) /quotedblleft (Street) /quotedblright (.)]\n"
			"\tgsave\n"
			"\tPageWidth Margin FontSize add sub  PageHeight 2 div translate -90 rotate\n"
			"\t0 0 moveto   dup StringWidthGeneral -2 div 0 rmoveto   ShowOutlined\n"
			"\tgrestore\n\n"

			"\tend\n"
		"} if   %% Show label orientations?\n\n\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	sprintf(scratchString,
		"false   %% Boolean: decorative text, do or don\'t?\n"
		"{\n"
			"\t7 dict begin\n"
			"\tPaperMatrix setmatrix\n"
			"\t/FontSize 16 def\n"
			"\t/X 0.52 PageWidth  mul def  %% User changeable.\n"
			"\t/Y 0.96 PageHeight mul def  %% User changeable.\n"
			"\t/LineHeight  FontSize  1.3 mul def  %% User changeable.\n\n"

			"\tFontName FontSize selectfont   0 setgray\n"
			"\t/indent  %% by longer of these two strings\n"
				"\t\t(Round 5)  StringWidthGeneral\n"
				"\t\t(Pointy 5) StringWidthGeneral  2 copy lt {exch} if pop\n"
			"\tdef  %% /indent\n\n"

			"\t%% In TextLines, each item is an array,\n"
			"\t%% each subarray containing strings, glyphs, or executables.\n"
			"\t/TextLines [\n"
				"\t\t[(Path colours:)]\n"
				"\t\t[ {indent  (Round 5) StringWidthGeneral sub 0 rmoveto}  (Round 5 = white with circle;)]\n"
				"\t\t[ {indent (Pointy 5) StringWidthGeneral sub 0 rmoveto} (Pointy 5 = pale green;)]\n"
				"\t\t[ {indent       (15) StringWidthGeneral sub 0 rmoveto}       (15 = dark green;)]\n"
				"\t\t[ {indent       (25) StringWidthGeneral sub 0 rmoveto}       (25 = dark blue;)]\n"
				"\t\t[ {indent       (55) StringWidthGeneral sub 0 rmoveto}       (55 = bright blue;)]\n"
				"\t\t[ {indent      (105) StringWidthGeneral sub 0 rmoveto}      (105 = pink;)]\n"
				"\t\t[ {indent      (215) StringWidthGeneral sub 0 rmoveto}      (215 = maroon;)]\n"
				"\t\t[ {indent      (425) StringWidthGeneral sub 0 rmoveto}      (425 = medium green;)]\n"
				"\t\t[ {indent     (1705) StringWidthGeneral sub 0 rmoveto}     (1705 = orange;)]\n"
				"\t\t[ {indent     (Open) StringWidthGeneral sub 0 rmoveto}     (Open = bright red.)]\n"
				"\t\t[]\n"
				"\t\t[(GitHub.com/jdaw1/penrose_tiling)]\n"
				"\t\t[(has code, including for this page.)]\n"
				"\t\t[]\n"
				"\t\t[/emdash ( J. D. A. Wiseman, August 2026)]\n"
			"\t] def  %% /TextLines\n\n"

			"\t0  1  TextLines length 1 sub\n"
			"\t{\n"
				"\t\t/lineNum exch def\n"
				"\t\tX  Y lineNum  LineHeight mul sub  moveto\n"
				"\t\tFontName   FontSize   TextLines lineNum get   ShowOutlined\n"
			"\t} for  %% lineNum\n"
			"\tend\n"
		"} if  %% Show decorative text?\n\n\n"
	);  // sprintf()
	(*numCharsThisFileP) += fprintf(fp, "%s", scratchString);
	(*numLinesThisFileP) += newlinesInString(scratchString);


	sprintf(scratchString,
		"showpage\n\n\n"


		"mark  (\\n\\n\\nNum rhombi painted:)\n"
		"(  thin=) PaintedNumByType /thin 2 copy known {get} {pop pop 0} ifelse\n"
		"(;  fat=) 0 PaintedNumByType {exch /thin ne {add} {pop} ifelse} forall\n"
		"(.\\n\\nPainted fat rhombi by path type \\(r_ound, p_ointy, c_losed, o_pen\\):\\n)\n"
		"[ (r) (p) (c) (o) ]  %% Partial sorting of this log output.\n"
		"{\n"
			"\t/MetaType exch 0 get def\n"
			"\t//PaintedNumByType\n"
			"\t{\n"
				"\t\t1 index dup  /thin ne  exch\n"
				"\t\t12 string cvs 0 get MetaType eq  and\n"
				"\t\t{\n"
					"\t\t\t12 string cvs (=) exch (,  )\n"
				"\t\t} {pop pop} ifelse  %% /thin ne, MetaType eq\n"
			"\t} forall  %% PaintedNumByType\n"
		"} forall  %% MetaType\n"
		"pop (.\\n) ConcatenateToMark =\n"
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

}  // tiling_export_PaintRhombiPS
