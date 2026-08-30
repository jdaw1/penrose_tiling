// By and copyright Julian D. A. Wiseman of www.jdawiseman.com, August 2026
// Released under GNU General Public License, Version 3, https://www.gnu.org/licenses/gpl-3.0.txt
// penrose.h, in PenroseC

#include <inttypes.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <string.h>

// What is your binding constraint: memory or speed?
// Value of true saves almost 10% of memory usage: sizeof(Rhombus) being 270 instead of 296.
// But value of false seems to be faster execution by a factor of ~= 1.4.
// Breaking alignment conventions might, depending on machine and compiler, cause crashes: if so, set to false.
#define MEMORY_FRUGALITY_OUTRANKS_SPEED false

#define scratchStringLength 65535

// Useful constants, to >=96dp. Comparisons, approximate: Universe size is 10^42 proton widths; octuple arithmetic is 71 d.p.
// Done in Mathematica Engine with "WriteString["stdout", N[ ... , 98], "\n"] ".
static double const DegreesPerRadian          = 57.295779513082320876798154814105170332405472466564321549160243861202847148321552632440968995851111  ;  // 180 / Pi
static double const Pi                        =  3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421171 ;  // Pi
static double const GoldenRatioSquare         =  2.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911 ;  // (Sqrt[5] + 3) / 2
static double const GoldenRatio               =  1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911 ;  // (Sqrt[5] + 1) / 2
static double const GoldenRatioReciprocal     =  0.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911 ;  // (Sqrt[5] - 1) / 2
static double const HalfRoot5Plus2Sqrt5       =  1.5388417685876267012851452880184549120033510717688962135195781251874316442475454592272968608335527 ;  // Sqrt[2*Sqrt[5] + 5] / 2
static double const Cos18                     =  0.95105651629515357211643933337938214340569863412575022244730564443015317008519350171879281097081138;  // Sqrt[2*Sqrt[5] + 10] / 4
static double const Cos36                     =  0.80901699437494742410229341718281905886015458990288143106772431135263023140945122485360360209469557;  // (Sqrt[5] + 1) / 4
static double const Cos72                     =  0.30901699437494742410229341718281905886015458990288143106772431135263023140945122485360360209469557;  // (Sqrt[5] - 1) / 4 = GoldenRatioReciprocal / 2
static double const Quarter5MinusSqrt5        =  0.69098300562505257589770658281718094113984541009711856893227568864736976859054877514639639790530443;  // (5 - Sqrt[5]) / 4
static double const Quarter3MinusSqrt5        =  0.19098300562505257589770658281718094113984541009711856893227568864736976859054877514639639790530443;  // (3 - Sqrt[5]) / 4 = Half3MinusSqrt5 / 2
static double const Sin36                     =  0.58778525229247312916870595463907276859765243764314599107227248075727847416235195750850404986274134;  // Sqrt[(5 - Sqrt[5]) / 8]
static double const Half3MinusSqrt5           =  0.38196601125010515179541316563436188227969082019423713786455137729473953718109755029279279581060886;  // (3 - Sqrt[5]) / 2  =  2 - GoldenRatio
static double const HalfRoot5Minus2Sqrt5      =  0.36327126400268044294773337874030937480804619648260423137503316367287469592284154421028876110807005;  // Sqrt[5 - 2*Sqrt[5]] / 2
static double const QuarterRoot50Minus22Sqrt5 =  0.22451398828979268622097257589876339378960624116054175969723931708440377823951041329821528875467129;  // Sqrt[50 - 22*Sqrt[5]] / 4

static char   const TextURL[]     = "https://github.com/jdaw1/penrose_tiling/";    // No internal double quotes; no unbalanced parentheses;
static char   const TextAuthor[]  = "Julian D. A. Wiseman of www.jdawiseman.com";  // escaping insufficient to survive output as PostScript etc.
static char   const TextLicence[] = "GNU General Public License, Version 3, 29 June 2007";

static long int const PostScriptArrayLengthMax = 65535;


// Types

typedef enum {oneThin = 36,  oneFat = 72,  round5 = 5} SeedType;

typedef enum {anything,  pathStats,  paths,  rhombi,  boundingPath} ExportWhat;

typedef enum  // ExportFormat
{
	// PostScript, by Adobe, first released 1984, https://en.wikipedia.org/wiki/PostScript , PLRM3 = http://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf
	PS_data   = 1982,  // Deeply nested arrays and dictionaries, echoing other data formats, for processing by PostScript-as-a-programming-language. But array-size limit is 65535, so typically only subset of rhombi can be output.
	PS_rhomb  = 1983,  // Directly distillable PostScript, painting rhombi. Some post-C editing and changes possible, but to a much lesser extent than as data. Not as arrays, so not bothered by array-size limit of 65535.
	PS_arcs   = 1984,  // Directly distillable PostScript, painting arcs. Some post-C editing and changes possible, but to a much lesser extent than as data. Not as arrays, so not bothered by array-size limit of 65535.
	SVG_rhomb = 1999,  // Rhombi, browser-viewable Scalable Vector Graphics, so some but only some some post-C editing is feasible. https://en.wikipedia.org/wiki/SVG
	SVG_arcs  = 2000,  // Arcs, in SVG. https://en.wikipedia.org/wiki/SVG
	JSON      = 2001,  // JavaScript Object Notation = JSON, April 2001. All the data, for subsequent machine processing. https://en.wikipedia.org/wiki/JSON
	TSV       =    9   // Tab-Separated Values, for Excel, opening by or copy-pasting subsets into; column titles being good range names. Limited to 2^20 rows. https://en.wikipedia.org/wiki/Tab-separated_values
	// If a new format is added, most of the work that needs doing will be flagged by compiler grumbles re incomplete switch statements.
	// But, not caught by the compiler, will be a need to add new section(s) to main(), within which search for "exportFormat = ".
} ExportFormat;

typedef enum  // Physique
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{  // Physique
	Thin = 36,  // Rhombus with angles, in degrees, 36 144 36 144, area = Sin[36 deg] * edge^2 ~= 0.587785252292 * edge^2
	Fat  = 72   // Rhombus with angles, in degrees, 72 108 72 108, area = Sin[72 deg] * edge^2 ~= 0.951056516295 * edge^2, so GoldenRatio * Thin
} Physique;




typedef struct  // XY
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{double x;  double y;} XY;

typedef  long  int  RhombId   ;
typedef  long  int  PathId    ;
typedef  long  int  PathStatId;
typedef  int8_t     TilingId  ;

typedef struct  // Neighbour
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{  // Neighbour
	RhombId   rhId;
	PathId    pathId;
	long int  withinPathNum ;  // Counting within a path: 0, 1, ..., pathLength - 1.
	Physique  physique;
	int8_t    nghbrsNghbrNum;  // Neighbour's Neighbour Num, Abbreviated to NNN in output: I am the neighbour[NNN] of my neighbour. Fussy to use int8_t, but saves 4 bytes per rhombus, > 1%.
	bool      touchesN;
	bool      touchesE;
} Neighbour;

// These large structs, of which large arays malloc'd, have elements ordered to reduce used memory.

typedef struct  // Rhombus
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{  // Rhombus
	XY         north;  // Orientation local to rhombus.  because meaningful points, these are XY struct.
	XY         south;
	XY         east;
	XY         west;
	XY         centre;

	double     xMin;  // (xMin,yMin) is not a meaningful point, so separate scalars.
	double     xMax;
	double     yMin;
	double     yMax;

	double     angleDegrees;

	RhombId    rhId;

	PathId     pathId;
	long int   withinPathNum;
	PathId     pathId_ShortestOuter;  // Thins only, as Fats done at level of Path.

	Neighbour  neighbours[4];
	int8_t     numNeighbours;

	Physique   physique;
	int8_t     filledType;

	bool       closerPathCentreN;
	bool       closerPathCentreE;

	bool       wantedPostScript;
} Rhombus;


typedef struct  // Path
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{  // Path
	PathId      pathId;
	long int    pathLength;
	double      orientationDegrees;  // Allows SVG to <def> one closed path of each length, and then to <use ... transform='... rotate(orientationDegrees - orientationDegreesTemplate)'/>
	PathStatId  pathStatId;

	RhombId     rhId_PathCentreClosest;   // Rhombus with corner closest to path's centre, so used as path's starting rhombus with withinPathNum == 0
	RhombId     rhId_PathCentreFurthest;  // Rhombus with corner furthest from path's centre.
	RhombId     rhId_openPathEnd;
	RhombId     rhId_ThinWithin_First;
	RhombId     rhId_ThinWithin_Last;

	XY          centre;  // This an XY struct because this a meaningful point; not true of extremal corners.
	RhombId     xMin_rhId;  // Pointer to rhombus, rather than double, because user might want centre, might want extreme.
	RhombId     xMax_rhId;
	RhombId     yMin_rhId;
	RhombId     yMax_rhId;

	double      radiusMin;  // To innermost corner of any of the rhombi.
	double      radiusMax;  // To outermost corner of any of the rhombi.

	long int    insideThis_NumFats ;
	long int    insideDeep_NumFats ;
	long int    insideThis_NumThins;
	long int    insideDeep_NumThins;

	PathId      pathId_ShortestOuter;  // Closed paths only. The smallest enclosing path.
	PathId      pathId_LongestInner ;  // Closed paths only. The unique largest enclosed path. This has same centre, and Inner.Length = (Outer.Length +- 5) / 4.

	bool        pathClosed;
	bool        pathVeryClosed;  // All neighbouring tiles have four neightbours. I.e., far from outside of whole tiling.
	bool        pointy;  // If pathClosed && 5==pathLength then: true ==> ten neighbouring thins in a pointy star; false ==> five neighbouring thins lying flat to it.
	int8_t      pathClosedTypeNum;  // -1=open; 1=5r, 2=5p, 3=15, 4=25, 5=55, ...
	bool        wantedPostScript;  // <==> any of its fats are wanted
} Path;


typedef struct  // PathStats
#if MEMORY_FRUGALITY_OUTRANKS_SPEED
__attribute__ ((__packed__))
#endif  // MEMORY_FRUGALITY_OUTRANKS_SPEED
{  // PathStats
	PathStatId pathStatId;
	long int   pathLength;
	long int   numPaths;
	PathId     examplePathId ;
	long int   insideThis_MaxNumFats ;
	long int   insideDeep_MaxNumFats ;
	long int   insideThis_MaxNumThins;
	long int   insideDeep_MaxNumThins;
	long int   insideThis_MaxNumFats_Num ;
	long int   insideDeep_MaxNumFats_Num ;
	long int   insideThis_MaxNumThins_Num;
	long int   insideDeep_MaxNumThins_Num;
	double     radiusMin;  // To innermost middle rhombus.
	double     radiusMax;  // To outermost middle rhombus.
	double     widthMax;
	double     heightMax;
	bool       pathClosed;
	bool       pointy;  // If pathClosed && 5==pathLength then: true ==> ten neighbouring thins in a pointy star; false ==> five neighbouring thins lying flat to it.
	int8_t     pathClosedTypeNum;
} PathStats;


typedef struct  // Tiling, never packed because <=20 of them, yet frequent access to members, especially edgeLength, numFats, numThins, numPathsClosed, numPathsOpen.
{
	size_t     mallocsPersistentSumSimple;
	TilingId   tilingId;
	SeedType   seedType;
	int8_t     numTilings;
	bool       axisAligned;
	double     edgeLength;
	XY         wantedPostScriptCentre;  // As if edge length is 1.
	double     wantedPostScriptAspect;  // Height over Width
	double     wantedPostScriptHalfWidth;   // As if edge length is 1; 'wantedPostScriptHalfHeight' = wantedPostScriptHalfWidth * wantedPostScriptAspect
	long int   wantedPostScriptNumRhombi;
	long int   wantedPostScriptNumPaths;
	long int   numFats;
	long int   numThins;
	long int   numThins_0T4F;
	long int   numThins_1T3F;
	long int   rhombi_NumMax;
	long long int boundingPathNumVertices;  // Could, in theory, be two plus twice num rhombi, hence long long. In practice, of order sqrt(num rhombi).
	Rhombus    * rhombi;
	RhombId    xMin_rhId;  // Pointer to rhombus, rather than double, because user might want centre, might want extremal corner.
	RhombId    xMax_rhId;
	RhombId    yMin_rhId;
	RhombId    yMax_rhId;
	double     xMaxSuggestedZoom;
	double     xMinSuggestedZoom;
	double     yMaxSuggestedZoom;
	double     yMinSuggestedZoom;
	double     radiusMax;
	double     radiusShortOpen;

	long int   path_NumMax;
	Path       * path;
	long int   numPathsClosed;
	long int   numPathsOpen;
	bool       anyPathsVeryClosed;

	long int   pathStats_NumMax;
	PathStats  * pathStat;
	long int   numPathStats;

	char       const * filePath;
	struct tm  const * timeData;
	char       const * timeString;

	RhombId    * wantedPostScriptRhombNum;
	PathId     * wantedPostScriptPathNum;

	double     SecondsToStartExportFromStartThisTiling;
	double     SecondsToStartExportFromStartFirstTiling;
	bool       populated;
} Tiling;



// Functions and sub-routines

FILE * file_open(const char *filename,  const char * mode,  const char * errorString);

extern double    max_2(double const d0, double const d1);
extern double    min_2(double const d0, double const d1);
extern double    min_4(double const d0, double const d1, double const d2, double const d3);
extern double    max_4(double const d0, double const d1, double const d2, double const d3);
extern double    avg_2(double const d0, double const d1);
extern double median_3(double const d0, double const d1, double const d2);
extern double median_4(double const d0, double const d1, double const d2, double const d3);
extern bool points_diff_2(double const edgeLength, XY const xy0, XY const xy1);
extern bool points_diff_3(double const edgeLength, XY const xy0, XY const xy1, XY const xy2);
extern bool points_diff_4(double const edgeLength, XY const xy0, XY const xy1, XY const xy2, XY const xy3);
extern bool points_same_2(double const edgeLength, XY const xy0, XY const xy1);
extern bool points_same_3(double const edgeLength, XY const xy0, XY const xy1, XY const xy2);
extern bool points_same_4(double const edgeLength, XY const xy0, XY const xy1, XY const xy2, XY const xy3);

RhombId rhombus_append(
	Tiling  * const tlngP,  // Parent tiling
	Physique  const physique,
	int8_t    const filledType,
	double    const xNorth,
	double    const yNorth,
	double    const xSouth,
	double    const ySouth
);  // rhombus_append()
bool rhombus_keep(
	const Tiling * const tlngP,  Physique const physique,
	double const xNorth,  double const yNorth,  double const xSouth,  double const ySouth
);

void rhombus_append_descendants(Tiling* tlngP, Rhombus* rhP);

void tiling_empty(Tiling * const tlngP);

SeedType initial_Tiling_Layout(void);
void tiling_initial(Tiling * const tlngP);
bool tiling_simple_tests(const Tiling * const tlngP);

void tiling_descendant(
	Tiling * const tlngDescendantP,
	Tiling const * const tlngAncestorP
);  // tiling_descendant()

void   rhombi_sort(
	Tiling * const tlngP,
	int orderedFn(const Rhombus * const, const Rhombus * const),
	bool const alsoRenumber
);  // rhombi_sort()
void     paths_sort(Tiling * const tlngP,  int orderedFn(const Path    * const, const Path    * const) );  // Always renumber
void pathStats_sort(Tiling * const tlngP);  // Only one sort function, always renumber
int rhombiGt_ByY(Rhombus const * const rhP0, Rhombus const * const rhP1);
void rhombi_purgeDuplicates(Tiling * const tlngP);

void twoRhombi_Neighbourify(Tiling * const tlngP,  RhombId const rhId_A,  RhombId const rhId_B);
void oneRhombi_Neighbourify(
	Tiling * const tlngP,
	RhombId const rhId_This,
	RhombId rhId_Start,
	RhombId const rhId_End,
	bool const isSortedWithin
);
void neighbours_populate(Tiling * const tlngP);
void bounding_box_tiling(Tiling * const tlngP);
void radii_populate(Tiling * const tlngP);
void thins_0T4F_1T3F_count(Tiling * const tlngP);

bool holesFillQ(const Tiling * const tlngP);
void holesFill(Tiling * const tlngP);

void verifyHypothesisedProperties(Tiling const * const tlngP);


RhombId NextInPath_RhId(
	const Rhombus * const rhombi,
	const Rhombus * const rhThisP,
	long int        const pathLength,
	bool            const direction
);  // NextInPath_RhId(). Negative return means non-existent. Re direction: true ==> next; false ==> prev.

int  neighbourGt(       Neighbour const * const nP0,     Neighbour const * const nP1   );
int  rhombiGt_ByPath(  Rhombus   const * const rhP0,    Rhombus   const * const rhP1  );
int  pathGt_ByClosedEtc(Path      const * const pathP0,  Path      const * const pathP1);

int  path_winding_number(const Path * const pathP_Inner,  const Path * const pathP_Outer,  const Tiling * const tlngP);
int  point_winding_number(register double const x,  register double const y,  const Path * const pathP_Outer,  const Tiling * const tlngP);
int8_t pathClosedTypeNum(bool pathClosed, long int pathLength, bool pointy);
void paths_populate(Tiling * const tlngP);
bool collinear(XY const xy0, XY const xy1, XY const xy2, const Tiling * const tlngP);

int  rhombus_winding_number(const Rhombus * const rhP,  const Path * const pathP_Outer,  const Tiling * const tlngP);
void insideness_populate(Tiling * const tlngP);

void pathStats_populate(Tiling * const tlngP);

void wanted_populate(Tiling * const tlngP);

extern long int newlinesInString(char const * const str);

const char * filePath(void);
void file_names_timeString_set(char * const timeString, const struct tm * const timeData);
const double svg_toPaint_xMin(const Tiling * const tlngP);
const double svg_toPaint_xMax(const Tiling * const tlngP);
const double svg_toPaint_yMin(const Tiling * const tlngP);
const double svg_toPaint_yMax(const Tiling * const tlngP);
const double svg_displayWidth(const Tiling * const tlngP);
const double svg_strokeWidth( const Tiling * const tlngP);
const bool showGridlines(const Tiling * const tlngP, ExportFormat const ef);
const long int svg_arcs_longestPathToBeColoured(void);
void execute_SVG_PostProcessing(       const Tiling * const tlngP,  const unsigned long int numLinesThisFile,  const unsigned long long int numCharsThisFile, const char * const fileName,  ExportFormat const ef);

const double postScript_toPaint_xMin(const Tiling * const tlngP);
const double postScript_toPaint_xMax(const Tiling * const tlngP);
const double postScript_toPaint_yMin(const Tiling * const tlngP);
const double postScript_toPaint_yMax(const Tiling * const tlngP);
void execute_PostScript_PostProcessing(const Tiling * const tlngP,  const unsigned long int numLinesThisFile,  const unsigned long long int numCharsThisFile, const char * const fileName,  ExportFormat const ef);

bool exportQ(
	ExportWhat const exprtWhat,
	ExportFormat const exportFormat,
	const Tiling * const tlngP,
	const unsigned long int numLinesThisFile
);  // exportQ()

void stringClean(char *str);
int  fIndent(FILE* const fp, int8_t const indentDepth);
void rhombus_export(
	FILE * const fp,
	ExportFormat const exportFormat,
	Tiling const * const tlngP,
	Rhombus const * const rhP,
	bool const notLast,
	unsigned long long int * const numCharsThisFileP
);  // rhombus_export()

void preamble_export_PS(
	FILE* const fp,
	Tiling const * const tlngP,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);

long long int tiling_export_PaintBoundary(
	FILE * const fp,
	ExportFormat        const exportFormat,
	double              const scalingFactor,
	const Tiling      * const tlngP,
	int8_t              const indentDepth,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);

void path_export(
	FILE * const fp,
	ExportFormat             const exportFormat,
	Tiling           const * const tlngP,
	Path             const * const pathP,
	int8_t                   const indentDepth,
	bool                     const notLast,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // path_export()

void pathStat_export(
	FILE * const fp,
	ExportFormat             const exportFormat,
	const Tiling           * const tlngP,
	PathStats const        * const pathStatP,
	int8_t                   const indentDepth,
	bool                     const notLast,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // pathStat_export()

void export_soloTiling(Tiling * const tlngP);

void tiling_export(
	FILE              * const fp,
	ExportFormat        const exportFormat,
	bool exportQ(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFileP),
	Tiling            * const tlngP,
	int8_t              const indentDepth,
	bool                const notLast,
	TilingId            const tilingId,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tiling_export(), which doesn't need to be in the header file, except that it has been used in debugging.

void tilings_export(
	FILE              * const fp,
	ExportFormat        const exportFormat,
	bool exportQ(ExportWhat const exprtWhat, ExportFormat const exportFormat, const Tiling * const tlngP, const unsigned long int numLinesThisFile),
	Tiling            * const tlngs,
	int8_t              const indentDepth,
	long int            const numTilings,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tilings_export()

void tiling_export_Gridlines(
	FILE* const fp,
	Tiling const      * const tlngP,
	ExportFormat const ef,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tiling_export_Gridlines()

void tiling_export_subroutines_PS(
	FILE* const fp,
	ExportFormat const ef,
	unsigned long int * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tiling_export_subroutines_PS()

void tiling_export_PaintRhombiPS(
	FILE* const fp,
	Tiling           const * const tlngP,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP,
	bool                     const deBugMode
);  // tiling_export_PaintRhombiPS()

void tiling_export_PaintArcsPS(
	FILE* const fp,
	Tiling          const  * const tlngP,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tiling_export_PaintArcsPS()

void exportColourSVG(
	char   * const strA,
	char   * const strB,
	bool   * const isWhiteP,
	Physique const ph,
	bool     const pathClosed,
	long int const pathLength,
	bool     const pointy
);
void tiling_export_PaintRhombiSVG(
	FILE* const fp,
	Tiling           const * const tlngP,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);

void tiling_export_PaintArcsSVG(
	FILE* const fp,
	Tiling           const * const tlngP,
	unsigned long int      * const numLinesThisFileP,
	unsigned long long int * const numCharsThisFileP
);  // tiling_export_PaintArcsSVG()

char * svgTransform(
	char * const str,
	int const numDecimalPlaces,
	double const x,
	double const y,
	double const angDeg,
	short int    angMod
);  // svgTransform()

char * fileExtension_from_ExportFormat(char * const str, ExportFormat const ef);
char * filename_new_extension(const char * const filenameOld,  char * strNew,  const char * const extensioNew);

XY     wantedPostScriptCentre(Tiling const * const tlngP);
double wantedPostScriptAspect(Tiling const * const tlngP);

char * seed_type_name(char * const nameString,  const SeedType seed);
