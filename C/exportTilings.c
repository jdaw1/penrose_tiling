// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, April 2025
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// exportTilings.c, in PenroseC

#include "penrose.h"
#define indentationString "\t"  // Could be replaced with something like "    ".

int fIndent(FILE* const fp, int const indentDepth)
{
	int indentNum, numChars;
	for(numChars=indentNum=0;  indentNum < indentDepth;  indentNum++)
		numChars += fprintf(fp, indentationString);
	return numChars;
}


void tilings_export(
	FILE              * const fp,
	ExportFormat        const exportFormat,
	bool exportQ(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFile),
	Tiling            * const tlngs,
	int                 const indentDepth,
	long int            const numTilings,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
)
{
	long int i;
	TilingId tilingId;
	extern char scratchString[];

	for( i = 0  ;  i < scratchStringLength  ;  i++ )
		scratchString[i] = 0 ;

	if( PS_data == exportFormat )
	{
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp, "%%!PS\n\n/Tilings [\n\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp,"<<\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp,"\t%% Concatenate from http://github.com/jdaw1/placemat/blob/main/PostScript/placemat.ps, simplified\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp,"\t/Concatenate {2 copy length exch length dup 3 1 roll add string dup dup 5 3 roll exch putinterval 3 -1 roll 0 exch putinterval} bind\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp,"\t/usertimeStart usertime\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp,">> begin\n\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp, "(At usertime=) usertimeStart 12 string cvs Concatenate (, starting interpretation of Tilings) Concatenate = flush\n\n\n\n");
		(*numLinesThisFileP) += 14;
	}
	else if( JSON == exportFormat )
	{
		(*numCharsThisFileP) += fIndent(fp, indentDepth);  (*numCharsThisFileP) += fprintf(fp, "[\n\n\n");
		(*numLinesThisFileP) += 3;
	}


	for( tilingId = 0  ;  tilingId < numTilings;  tilingId++ )
	{
		if( tlngs[tilingId].edgeLength > 0  &&  (tlngs[tilingId].numFats > 0  ||  tlngs[tilingId].numThins > 0)
		&&  exportQ(Anything, exportFormat, &(tlngs[tilingId]), *numLinesThisFileP) )
		{
			tiling_export(
				fp,
				exportFormat,
				exportQ,
				&(tlngs[tilingId]),
				1 + indentDepth,
				(tilingId < numTilings - 1),
				tilingId,
				numLinesThisFileP,
				numCharsThisFileP
			);
			(*numCharsThisFileP) += fprintf(fp, "\n\n\n");
			(*numLinesThisFileP) += 3;
		}  // export Anything
	}  // tilingId


	if( PS_data == exportFormat )
	{
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp,
			"\n\n(After ) usertime usertimeStart sub 1000 div 12 string cvs Concatenate "
			"( seconds execution, finishing interpretation of Tilings) Concatenate = flush\n"
		);
		(*numCharsThisFileP) += fprintf(fp, "end  %% /Concatenate, /usertimeStart\n\n\n");
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "] def  %% /Tilings\n\n");
		(*numLinesThisFileP) += 8;
	}
	else if( JSON == exportFormat )
	{
		(*numCharsThisFileP) += fIndent(fp, indentDepth);
		(*numCharsThisFileP) += fprintf(fp, "]\n");
		(*numLinesThisFileP) ++;
	}

	fflush(fp);
}  // Tilings_Export
