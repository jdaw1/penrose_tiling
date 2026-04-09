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

[Penrose_09.json.zip](../data/Penrose_09.json.zip) is &asymp;&#8239;4562&#8239;KiB. 

Subsequent recursions of JSON files are big (table in page about [C&nbsp;code](c.md) has uncompressed sizes). 
Hence transmitting subsequent JSONs would be less efficient than regenerating them. 
If you need more, compile and regenerate.


## JSON ##

The <kbd>.json</kbd> export format is the maximal output: all generated data that might possibly be useful are in the JSON. 
For post-processing including selection, the <kbd>.json</kbd> is the most complete and most machine-comprehensible export format. 
Hence it is the best format for being absorbed by another system.

* The <kbd>.json</kbd> is an array of tilings. 

* Each tiling is a dictionary, containing 
    - strings `DataAsOf`, `Licence`, `URL`, and `Author`;
    - integers `TilingId`, `NumFats`, `NumThins`, `NumPathsClosed`, `NumPathsOpen`;
    - number `EdgeLength`, unusually in scientific notation as accuracy is important relative to its own size, rather than relative to the size of the canvas;
    - Boolean `AxisAligned`, being true if all orientations are integer multiples of 18&deg;;
    - numbers `MinX`, `MaxX`, `MinY`, `MaxY` (which for some purposes could need to be adjusted by half of a line width);
    - array `Rhombi` (described below);
    - array `Paths` (described below);
    - array `PathStats` (described below).

* Each element of `Rhombi`, each rhombus, is a dictionary. All rhombi dictionaries contain:
    - integer `RhId`, zero-based Id;
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
    - integer `PathId`, zero-based Id;
    - Boolean `Closed`;
    - Boolean `VeryClosed`, meaning that it is `Closed` and all its neighbours have precisely four neighbours, and hence can be part of a closed arc;
    - integer `Length`, if the path is `Closed` the `Length` being one of 5, 15, 25, 55, 105, 215, 425, 855, 1705, 3415, 6825, 13655, 27305, 54615, 109225, 218455, 436905, 873815, 1747625, 3495255, 6990505, 13981015, 27962025, 55924055, 111848105, 223696215, 447392425, 894784855, &hellip;,&nbsp; =&nbsp;(2<small><sup>*n*</sup></small>&nbsp;&minus;&nbsp;(&minus;1)<small><sup>*n*</sup></small>)&times;5&frasl;3;
    - numbers `CentreX`, `CentreY`, `MinX`, `MaxX`, `MinY`, `MaxY` (using British spelling);
    - integer `RhPathStart`, being the rhombus in this path for which `WithinPathNum` is `0`;
    - integer `rhId_PathCentreFurthest`, being one of the rhombi that is furthest from the centre (for closed paths, `RhPathStart` being one of those that is closest).
    - integer `PathStatId`, being the zero-based pointer into the array `PathStats` (described below). 

* Each element of `Paths`, that is a `Closed` path of `Length` &Equal; `5`, also contains:
    - Boolean `Pointy`, identifying which of the two types of closed 5-path this is.

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


* The array `PathStats` is intended to answer questions such as &ldquo;How many closed length-105 paths are there?&rdquo;. It has one item for each {`Length`, `Closed`, and where relevant `Pointy`}. Each element contains:
    - Boolean `PathClosed`;
    - integer `PathLength`;
    - for closed paths of length `5`, a Boolean `Pointy`;
    - integer `NumPaths`, being a count always &ge;&#8239;1;
    - for closed paths, integers `MaxNumThisFats`, `MaxNumThisThins`, `MaxNumDeepFats`, `MaxNumDeepThins`, which should equal the values in every matching path, and were computed as error-checking for the insideness code;
    - numbers `WidthMax_EdgeLengths`, `HeightMax_EdgeLengths`, `RadiusMinMin_EdgeLengths`, `RadiusMaxMax_EdgeLengths`, which have been divided by `EdgeLength` for ease of comparison between tilings;
    - array of integers `PathIds`, being pointers into the `Paths` array.

---


## Formal JSON Schema ##

The following is a manually edited version of an automatically-generated schema.


```json
{
	"$schema": "http://json-schema.org/draft-07/schema#",
	"title": "Schema for Penrose tilings output",
	"type": "array",
	"items": {
		"type": "object",
		"properties": {
			"DataAsOf":                      {"type": "string"},
			"TilingId":                      {"type": "integer"},
			"NumFats":                       {"type": "integer"},
			"NumThins":                      {"type": "integer"},
			"NumPathsClosed":                {"type": "integer"},
			"NumPathsOpen":                  {"type": "integer"},
			"EdgeLength":                    {"type": "number" },
			"AxisAligned":                   {"type": "boolean"},
			"MinX":                          {"type": "number" },
			"MaxX":                          {"type": "number" },
			"MinY":                          {"type": "number" },
			"MaxY":                          {"type": "number" },
			"WantedPostScriptCentreX":       {"type": "number" },
			"WantedPostScriptCentreY":       {"type": "number" },
			"WantedwantedPostScriptAspect":  {"type": "number" },
			"WantedPostScriptHalfWidth":     {"type": "number" },
			"wantedPostScriptHalfHeight":    {"type": "number" },
			"WantedPostScriptNumberRhombi": {"type": "integer"},
			"WantedPostScriptNumberPaths":   {"type": "integer"},
			"Licence":                       {"type": "string" },
			"URL":                           {"type": "string" },
			"Author":                        {"type": "string" },
			"Rhombi": {
				"type": "array",
				"items": {
					"type": "object",
					"properties": {
						"RhId": {"type": "integer"},
						"WantedPS": {"type": "boolean"},
						"Physique": {"type": "integer"},
						"Xn": {"type": "number"},
						"Yn": {"type": "number"},
						"Xs": {"type": "number"},
						"Ys": {"type": "number"},
						"Xe": {"type": "number"},
						"Ye": {"type": "number"},
						"Xw": {"type": "number"},
						"Yw": {"type": "number"},
						"AngleDeg": {"type": "number"},
						"Neighbours": {"type": "array",
							"items": {
								"type": "object",
								"properties": {
									"Edge": {"type": "string"},
									"RhId": {"type": "integer"},
									"NNN":  {"type": "integer"}
								},
								"required": ["Edge", "RhId", "NNN"]
							}
						},
						"PathId": {"type": "integer"},
						"EdgeClosestToPathCentre": {"type": "string"},
						"PathId_ShortestOuter":    {"type": "integer"}
					},
					"required": [
						"RhId", "Xn", "Yn", "Xs", "Ys", "Xe", "Ye", "Xw", "Yw",
						"WantedPS", "Physique", "AngleDeg", "Neighbours"
					]
				}
			},
			"Paths": {
				"type": "array",
				"items": {
					"type": "object",
					"properties": {
						"PathId":                  {"type": "integer"},
						"Closed":                  {"type": "boolean"},
						"VeryClosed":              {"type": "boolean"},
						"Length":                  {"type": "integer"},
						"PathStatId":              {"type": "integer"},
						"RhPathStart":             {"type": "integer"},
						"rhId_PathCentreFurthest": {"type": "integer"},
						"rhId_OpenPathEnd":        {"type": "integer"},
						"CentreX":                 {"type": "number" },
						"CentreY":                 {"type": "number" },
						"MinX":                    {"type": "number" },
						"MaxX":                    {"type": "number" },
						"MinY":                    {"type": "number" },
						"MaxY":                    {"type": "number" },
						"Pointy":                  {"type": "boolean"},
						"Orient":                  {"type": "number" },
						"RadiusMin":               {"type": "number" },
						"RadiusMax":               {"type": "number" },
						"InsideThis_NumFats":      {"type": "integer"},
						"InsideThis_NumThins":     {"type": "integer"},
						"InsideDeep_NumFats":      {"type": "integer"},
						"InsideDeep_NumThins":     {"type": "integer"},
						"ThinWithin_First":        {"type": "integer"},
						"ThinWithin_Last":         {"type": "integer"},
						"PathId_ShortestOuter":    {"type": "integer"}
					},
					"required": [
						"PathId", "Closed", "VeryClosed", "Length",
						"PathStatId", "RhPathStart", "rhId_PathCentreFurthest",
						"rhId_OpenPathEnd", "CentreX", "CentreY", "MinX",
						"MaxX", "MinY", "MaxY"
					]
				}
			},
			"PathStats": {
				"type": "array",
				"items": {
					"type": "object",
					"properties": {
						"PathClosed":               {"type": "boolean"},
						"PathLength":               {"type": "integer"},
						"NumPaths":                 {"type": "integer"},
						"MaxNumThisFats":           {"type": "integer"},
						"MaxNumDeepFats":           {"type": "integer"},
						"MaxNumThisThins":          {"type": "integer"},
						"MaxNumDeepThins":          {"type": "integer"},
						"WidthMax_EdgeLengths":     {"type": "number" },
						"HeightMax_EdgeLengths":    {"type": "number" },
						"PathIds": {
							"type": "array",
							"items ":{"type": "integer"}
						},
						"Pointy":                   {"type": "boolean"},
						"RadiusMinMin_EdgeLengths": {"type": "number" },
						"RadiusMaxMax_EdgeLengths": {"type": "number" }
					},
						"required": [
						"PathClosed", "PathLength", "NumPaths",
						"MaxNumThisFats", "MaxNumDeepFats", "MaxNumThisThins",
						"MaxNumDeepThins", "WidthMax_EdgeLengths",
						"HeightMax_EdgeLengths", "PathIds"
					]
				}
			}
		},
		"required": [
			"DataAsOf", "TilingId", "NumFats", "NumThins", "EdgeLength",
			"AxisAligned", "MinX", "MaxX", "MinY", "MaxY",
			"WantedPostScriptCentreX", "WantedPostScriptCentreY",
			"WantedwantedPostScriptAspect", "WantedPostScriptHalfWidth",
			"wantedPostScriptHalfHeight", "WantedPostScriptNumberRhombi",
			"WantedPostScriptNumberPaths", "Licence", "URL", "Author",
			"Rhombi", "Paths", "PathStats"
		]
	}
}
```
