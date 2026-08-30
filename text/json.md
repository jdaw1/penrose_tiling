# Penrose Tilings: JSON output #

## Contents ##

Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;[C&nbsp;code](c.md);&nbsp; 
&bull;&nbsp;**JSON&nbsp;output**;&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(data)](postscript_data.md);&nbsp; 
&bull;&nbsp;[Physical&nbsp;tiling](physical_tiling.md).

## Example output ##

[Penrose_09.json.zip](../data/Penrose_09.json.zip) is 8,208,262 bytes.

Subsequent recursions of JSON files are big (table in page about [C&nbsp;code](c.md) has uncompressed sizes). 
Hence transmitting subsequent JSONs would be less efficient than regenerating them. 
If you need more, compile and regenerate.


## JSON ##

The <kbd>.json</kbd> export format is the maximal output: all generated data that might possibly be useful are in the JSON. 
For post&#8209;processing including selection, the <kbd>.json</kbd> is the most complete and most machine&#8209;comprehensible export format. 
Hence it is the best format for being absorbed by another system.

* The <kbd>.json</kbd> is an array of tilings. 

* Each tiling is a dictionary, containing 
    - strings `DataAsOf`, `Licence`, `URL`, and `Author`;
    - integers `TilingId`, `NumFats`, `NumThins`, `NumThins_0T4F` (the number of thins with zero thin neighbours and four fat neighbours), `NumThins_1T3F` (one thin, three fats), `NumPathsClosed`, `NumPathsOpen`, `NumPathStats`, `BoundingPathNumVertices`;
    - number `EdgeLength` always being `1`;
    - number `InternalToC_EdgeLength`, being the pre&#8209;export edgelength within the C;
    - string `SeedType` describing `tilingId` 0;
    - Boolean `AxisAligned`, being true if all orientations are integer multiples of 18&deg;;
    - integers `MinX_rhId`, `MaxX_rhId`, `MinY_rhId`, `MaxY_rhId`, being the Ids of the extremal rhombi;
    - numbers `MinX`, `MaxX`, `MinY`, `MaxY` (which for some purposes could need to be adjusted by half of a line width);
    - numbers `RadiusMax` being the distance of the point furthest from (0,0), and `RadiusShortOpen` being the largest radius that has paths that are either closed, or of length equal to the longest open path;
    - integer `PersistentSumSimple_malloc` as an approximation to the C&rsquo;s memory usage, and also various C timing numbers;
    - numbers relating the output chosen for the PostScript-as-data format, because of its 65535 limit; 
    - array `Rhombi` (described below);
    - array `Paths` (described below);
    - array `PathStats` (described below);
    - array `RhombiBoundingPath` (described below).


* Each element of `Rhombi`, each rhombus, is a dictionary. All rhombi dictionaries contain:
    - integer `RhId`, zero&#8209;based Id;
    - integer `Physique`, value either `36` (thin&nbsp;&Longleftrightarrow;&nbsp;36&deg;) or `72` (fat&nbsp;&Longleftrightarrow;&nbsp;72&deg;);
    - Boolean `WantedPS`,  whether this is one of the &le;&#8239;65535 rhombi to be exported in the `PS_data` format;
    - numbers `Xn`, `Yn`, `Xs`, `Ys`, `Xe`, `Ye`, `Xw`, `Yw`, being the *x* and *y* coordinates of the north, south, east and west vertices;
    - number `AngleDeg`, being the orientation of the rhombus.
    - array `Neighbours`, each being a dictionary containing:
        * integer `RhId` (the Id of the neighbour),
        * string `Edge`, being `"NE"` or `"SE"`or `"SW"` or `"NW"`,
        * integer `NNN`, &ge;&#8239;0 and &le;&#8239;3, _neighbour&rsquo;s neighbour number_, being the item of the neighbour&rsquo;s `Neighbours` that points back to this rhombus.

* Fat rhombus&rsquo;s dictionaries also contain:
    - integer `PathId`, being the path of which this fat is a member;
    - integer `WithinPathNum`, &ge;&#8239;0 and &le;&#8239;path&nbsp;length&nbsp;&minus;&nbsp;1.
    - string `EdgeClosestToPathCentre`, being `"NE"` or `"SE"`or `"SW"` or `"NW"`.

* Some thin rhombus&rsquo;s dictionaries also contain:
    - integer `PathId_ShortestOuter`, being the shortest, i.e. most immediate, path enclosed this thin. Absent if there is no enclosing path.

* Some rhombus&rsquo;s dictionaries also contain:
    - integer `FilledType`, being, for those rhombi inserted with `holesFill()`, the type of fill used.


* Each element of `Paths`, each path, is a dictionary. All path dictionaries contain:
    - integer `PathId`, zero&#8209;based Id;
    - Boolean `Closed`;
    - Boolean `VeryClosed`, meaning that it is `Closed` and all its neighbours have precisely four neighbours, and hence can be part of a closed arc;
    - integer `Length`, if the path is `Closed` the `Length` being one of 5, 15, 25, 55, 105, 215, 425, 855, 1705, 3415, 6825, 13655, 27305, 54615, 109225, 218455, 436905, 873815, 1747625, 3495255, 6990505, 13981015, 27962025, 55924055, 111848105, 223696215, 447392425, 894784855, &hellip;,&nbsp; =&nbsp;(2<small><sup>*n*</sup></small>&nbsp;&minus;&nbsp;(&minus;1)<small><sup>*n*</sup></small>)&times;5&frasl;3;
    - numbers `CentreX`, `CentreY`, `MinX`, `MaxX`, `MinY`, `MaxY` (using British spelling);
    - integer `RhPathStart`, being the rhombus in this path for which `WithinPathNum` is `0`;
    - integer `rhId_PathCentreFurthest`, being one of the rhombi that is furthest from the centre (for closed paths, `RhPathStart` being one of those that is closest).
    - integer `PathStatId`, being the zero&#8209;based pointer into the array `PathStats` (described below). 

* Each element of `Paths`, that is a `Closed` path of `Length` &Equal; `5`, also contains:
    - Boolean `Pointy`, identifying which of the two types of closed 5&#8209;path this is.

* Each element of `Paths`, that is a `Closed` path of `Length` &ge; `15`, also contains:
    - integer `rhId_ThinWithin_First`, being the least of the `RhId` of those thins immediately inside the path;
    - integer `rhId_ThinWithin_Last`, being the largest of the `RhId` of those thins immediately inside the path;.

* Each element of `Paths`, that is a `Closed` path, also contains:
    - numbers `RadiusMin` and `RadiusMax`, both measured from path centre to the centre of each rhombus;
    - integer `InsideThis_NumFats` and `InsideThis_NumThins`, being the number of tiles immediately inside this path, so not including those inside a path inside this path, and not including those in the path itself;
    - integer `InsideDeep_NumFats` and `InsideDeep_NumThins`, being the number of tiles immediately inside this path at arbitrary depth, but not including those in the path itself;

* Some closed path&rsquo;s dictionaries also contain:
    - integer `PathId_ShortestOuter`, being the shortest, i.e. most immediate, path enclosed this path. Absent if there is no enclosing path.

* Each element of `Paths`, that is open &Longleftrightarrow; `! Closed`, also contains:
    - integer `rhId_OpenPathEnd`, being the fat rhombus end of the open path opposite `RhPathStart`.


* The array `PathStats` is intended to answer questions such as &ldquo;How many closed length&#8209;105 paths are there?&rdquo;. It has one item for each {`Length`, `Closed`, and where relevant `Pointy`}. Each element contains:
    - Boolean `PathClosed`;
    - integer `PathLength`;
    - for closed paths of length `5`, a Boolean `Pointy`;
    - integer `NumPaths`, being a count always &ge;&#8239;1;
    - for closed paths, integers `MaxNumThisFats`, `MaxNumThisThins`, `MaxNumDeepFats`, `MaxNumDeepThins`, which should equal the values in every matching path, and were computed as error&#8209;checking for the insideness code;
    - numbers `WidthMax_EdgeLengths`, `HeightMax_EdgeLengths`, `RadiusMinMin_EdgeLengths`, `RadiusMaxMax_EdgeLengths`, which have been divided by `EdgeLength` for ease of comparison between tilings;
    - array of integers `PathIds`, being pointers into the `Paths` array.

* The array `RhombiBoundingPath` is a simple array of points on the outermost path, perhaps to to construct an opaque&#8209;white background. 
Each element contains:
    - integer `RhId`;
    - string `Corner`;
    - double `X`;
    - double `Y`.
