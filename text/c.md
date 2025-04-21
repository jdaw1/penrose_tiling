# Penrose Tilings: C code #

## Contents ##

Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;**C&nbsp;code**;&nbsp; 
&bull;&nbsp;[JSON&nbsp;output](json.md);&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(data)](postscript_data.md);&nbsp; 
&bull;&nbsp;[Physical&nbsp;tiling](physical_tiling.md).


## Security ##

[C](https://en.wikipedia.org/wiki/C_(programming_language)) is a fabulous low-level programming language, dating back to 1972. 
The language has great power over the computer on which it is running, able to inspect and alter memory, and likewise read and write from disks. 
This used to be thought fast and powerful, but is now described as insecure. 
One should be careful about code downloaded from the internet; one should be more careful about C&nbsp;code downloaded from the internet. 
Indeed, since 17<small><sup>th</sup></small>&nbsp;January 2025 the US Government has been [discouraging the use&nbsp;of&nbsp;C](https://www.cisa.gov/resources-tools/resources/product-security-bad-practices): 

> **1)** The development of new product lines for use in service of critical infrastructure or NCFs in a memory-unsafe language (e.g., C or C++) where readily available alternative memory-safe languages could be used is dangerous and significantly elevates risk to national security, national economic security, and national public health and safety.

So, even though this is not &ldquo;critical infrastructure&rdquo;, be careful. 
Run this code only in your compiler&rsquo;s debug mode, as that will block pointer mischief. 
Yes it will execute slower, but execution is a once-off to generate data: pay the slower to gain the full paranoia.

Also, this code needs to output many files. 
Give it write access only to one chosen output directory, and block all read access.

Yes, I know that this code is not naughty. 
But you cannot know that I truthfully know that, so if executing this C, indeed any downloaded C, you ought to be paranoid.

Version: this [C](https://en.wikipedia.org/wiki/C_(programming_language)) code uses inline comments `//`, types `bool`, `int8_t` and `long long int`, and an inline struct initialisation. 
Hence this code needs at least [C99](https://en.wikipedia.org/wiki/C99). 
This code should not compile pedanticly as original ANSI&nbsp;C =&nbsp;C89.


## Changes to the C, mandatory and optional ##

The C code cannot quite be executed directly out of the box: a few trivial customisations are needed, which are in the [<kbd>controls.c</kbd>](../C/controls.c) file.

* **Most important**: to what directory should output go, as used by <code>fopen(&hellip;, "w")</code>? 
This is in `filePath_staticConst[]`, which has default value `"/Users/JDAW/Documents/outputPenrose/"`. 
Almost certainly, you should change this default.

* After an SVG or a distillable PostScript file is finished, should an application be called to display it or distill it? 
This is done by <code>execute_SVG_PostProcessing(&hellip;)</code> and <code>execute_PostScript_PostProcessing(&hellip;)</code>. 
By default, these call Chrome and Adobe Distiller, in my computer&rsquo;s location of these applications. 
These calls are made by the <code>system(&hellip;)</code> command, which could in theory do anything, so check that your (and indeed, my) invocations are safe. 
Or, to have them do nothing, replace both of these <code>execute_&hellip;</code> routines with a do&#8209;nothing `{return;}`.

The optional customisations are also within the [<kbd>controls.c</kbd>](../C/controls.c) file.

* There are many file types of possible output, which could be output for all the `tilingId`s. 
Which is to be output is controlled by `exportQ()`. 
To output everything, it suffices for this function to <code>{return&nbsp;true;}</code>.

* For the SVGs, one might want to zoom into a particular region, not outputting the constructs wholly outside. 
E.g., the images in the [Introduction](introduction.md) and [Paths](paths.md) pages of this documentation, and some of those on the [SVG&nbsp;output](svg.md) page. 
For this, set `svg_toPaint_xMin()` etc to suitable values. 
Perhaps also choose the displayed width of the SVG with `svg_displayWidth()`, and consider `svg_strokeWidth()`.
(For similar control over PDF from the [distillable PostScript output](postscript_distillable.md), the controls are within the PostScript file: `/ToPaint_XMin -7 def`, etc.)

In `main()` a call of `fscanf()` asks how many loops of `TilingId` should there be. 
This is an important question, answering which takes the next section of this page.


## Loops of TilingId ##

The C code creates an initial tiling, then loops creating each new tiling from the previous. 
From one loop to the next the number of tiles grows, approximately, by a factor &gap;&nbsp;&phi;&sup2;&nbsp;&asymp;&nbsp;2.618 &asymp;&nbsp;10<small><sup>0.42</sup></small> &asymp;&nbsp;10<small><sup>(1&#8239;&divide;&#8239;2.4)</sup></small>. 
The table below shows the numbers of fat and thin tiles in each tiling. 

[My kitchen](../images/20230923_kitchen_tiles.png) uses tiles with an edge length of 150mm. 
Areas in the following table assume tiles of the same size, with &asymp;&nbsp;1mm of grouting, so fat and thin are assumed to occupy areas of (0.151m)&sup2;&#8239;&times;&#8239;Sin(72&deg;)&nbsp;&asymp;&nbsp;0.02168&#8239;m&sup2; &asymp;&nbsp;1m&sup2;&#8239;/&#8239;46.1 and (0.151m)&sup2;&#8239;&times;&#8239;Sin(36&deg;)&nbsp;&asymp;&nbsp;0.01340&#8239;m&sup2; &asymp;&nbsp;1m&sup2;&#8239;/&#8239;74.6.




| Tl&rsquo;g<br>Id | Num<br>Fats | Num<br>Thins | Long<br>-est<br>closed<br>path | Long<br>-est<br>open<br>path | Area<br>metric | Area<br>imperial<br>&CupCap;&nbsp;US | JSON<br>size | &Sum;<small><sub>0&#10141;_n_</sub></small><br>exec.<br>time |
|----------------------------------:|-----------:|-----------:|-------:|--------:|--------------------:|----------------------:|--------------:|:------------:|
|  [0](../images/Penrose_Rh_00.svg) |          2 |          1 |        |       2 |                     |                       |   2&#8239;KiB |              |
|  [1](../images/Penrose_Rh_01.svg) |          9 |          4 |        |       3 |                     |                       |   8&#8239;KiB |              |
|  [2](../images/Penrose_Rh_02.svg) |         32 |         18 |      5 |       8 |   0.9&#8239;m&sup2; |     10&#8239;ft&sup2; |  31&#8239;KiB |              |
|  [3](../images/Penrose_Rh_03.svg) |         98 |         55 |      5 |      15 |   2.9&#8239;m&sup2; |     31&#8239;ft&sup2; |  98&#8239;KiB |              |
|  [4](../images/Penrose_Rh_04.svg) |        281 |        166 |     15 |      32 |     8&#8239;m&sup2; |     90&#8239;ft&sup2; | 297&#8239;KiB |              |
|  [5](../images/Penrose_Rh_05.svg) |        776 |        464 |     25 |      63 |    23&#8239;m&sup2; |    248&#8239;ft&sup2; | 855&#8239;KiB |              |
|  [6](../images/Penrose_Rh_06.svg) |      2,098 |      1,273 |     55 |     128 |    63&#8239;m&sup2; |    673&#8239;ft&sup2; | 2.3&#8239;MiB |              |
|  [7](../images/Penrose_Rh_07.svg) |      5,601 |      3,420 |    105 |     255 |   167&#8239;m&sup2; |  1,801&#8239;ft&sup2; | 6.4&#8239;MiB |              |
|  [8](../images/Penrose_Rh_08.svg) |     14,840 |      9,106 |    215 |     512 |   444&#8239;m&sup2; |  4,778&#8239;ft&sup2; |  17&#8239;MiB |  2&#8239;s   |
|  [9](../images/Penrose_Rh_09.svg) |     39,138 |     24,079 |    425 |   1,023 | 1,171&#8239;m&sup2; | 12,609&#8239;ft&sup2; |  47&#8239;MiB |  6&#8239;s   |
| [10](../images/Penrose_Rh_10.svg) |    102,929 |     63,438 |    855 |   2,048 | 3,082&#8239;m&sup2; | 33,177&#8239;ft&sup2; | 125&#8239;MiB | 16&#8239;s   |
| [11](../images/Penrose_Rh_11.svg) |    270,224 |    166,720 |  1,705 |   4,095 | 8,094&#8239;m&sup2; |          2.0&#8239;ac | 331&#8239;MiB | 40&#8239;s   |
|  12                               |    708,674 |    437,521 |  3,415 |   8,192 |        2.1&#8239;ha |          5.2&#8239;ac | 876&#8239;MiB |  2&#8239;min |
|  13                               |  1,857,305 |  1,147,124 |  6,825 |  16,383 |        5.6&#8239;ha |           14&#8239;ac | 2.3&#8239;GiB |  5&#8239;min |
|  14                               |  4,865,680 |  3,005,938 | 13,655 |  32,768 |         15&#8239;ha |           36&#8239;ac | 6.0&#8239;GiB | 20&#8239;min |
|  15                               | 12,743,682 |  7,874,055 | 27,305 |  65,535 |         38&#8239;ha |           94&#8239;ac |  16&#8239;GiB | 1.3&#8239;hr |
|  16                               | 33,371,753 | 20,621,686 | 54,615 | 131,072 |        100&#8239;ha |          247&#8239;ac |  42&#8239;GiB |  6&#8239;hr  |


### Area comparisons ###

To compare to the areas in the table (which assume an edge length of 151mm &asymp;&nbsp;5.9&Prime;), the following are areas of familiar or famous spaces.

* A large domestic room could be 23&#8239;feet&#8239;&times;&#8239;23&#8239;feet =&nbsp;529&#8239;ft&sup2; &asymp; (7&#8239;m)&sup2; &asymp;&nbsp;49&#8239;m&sup2;.
* A [tennis court](https://www.tiauk.org/app/uploads/2021/03/key-dimensions-court-guidance-lta.pdf) is 78&#8239;feet&#8239;&times;&#8239;36&nbsp;feet =&nbsp;2,808&#8239;ft&sup2; &asymp;&nbsp;261&#8239;m&sup2;;
* An NBA [basketball court](https://en.wikipedia.org/wiki/Basketball_court) is 94&#8239;feet&#8239;&times;&#8239;50&nbsp;feet =&nbsp;4,700&#8239;ft&sup2; &asymp;&nbsp;437&#8239;m&sup2;;
* A category-4 [UEFA stadium field](https://en.wikipedia.org/wiki/UEFA_stadium_categories#Differences_between_categories) is 105&#8239;m&#8239;&times;&#8239;68&#8239;m =&nbsp;7,140&#8239;m&sup2; =&nbsp;0.714&#8239;ha &asymp;&nbsp;76,854&#8239;ft&sup2; &asymp;&nbsp;1.76&#8239;ac;
* [Trafalgar Square](https://en.wikipedia.org/wiki/Trafalgar_Square) is approximately 1.2&#8239;ha &asymp;&nbsp;3&#8239;ac; 
* The [Forbidden City](https://en.wikipedia.org/wiki/Forbidden_City) (&#32043;&#31105;&#22478;) &asymp;&nbsp;72&#8239;ha &asymp;&nbsp;178&#8239;ac.
* In New York, [Central Park](https://en.wikipedia.org/wiki/Central_Park) &asymp;&nbsp;341&#8239;ha &asymp;&nbsp;843&#8239;ac.



### Elegant subsets ###

The output has top&#8209;bottom mirror symmetry. 
It might be that, to emphasise aperiodicity, only half of this is wanted. 
And the space to be tiled likely won&rsquo;t match the remaining output&rsquo;s triangle (approx.&nbsp;54&deg;,&nbsp;54&deg;,&nbsp;72&deg;): the largest square that can fit into such a half-rhombus has area &asymp;&#8239;24% of the whole rhombus. 
So it might well be that, with 150mm-edge tiles, the areas that can asymmetrically and practicably be tiled are about one fifth of the areas in the table. 


### Larger tiles ###

Of course, if tiling a grand outdoor space, then larger tiles should be used. 
Increasing the edge length to 500mm&nbsp;&asymp;&nbsp;19.7&Prime; would increase the covered area by a factor of about (501&#8239;/&#8239;151)&sup2; &asymp;&nbsp;11&times;. 
With the previous paragraph&rsquo;s constraints, the proportionate increase in areas would product to &asymp;&nbsp;2&times;. 
That is, with tiles of edge length 500mm, `TilingId` 16 could asymmetrically and practicably cover &asymp;&#8239;2&#8239;km&sup2; &asymp;&#8239;0.85&#8239;mile&sup2;. 
But, of course, your space might not be square, and your tiles might not be 500mm.

### Limits ###

The quoted execution times are from a 2.7&#8239;GHz quad-core iMac with 32&#8239;GiB of memory. 
Execution was in [Xcode](https://developer.apple.com/xcode/)&rsquo;s debug mode to catch possible stray pointer misbehaviours (of which there were none).

A deeper recursion would need more memory. 
It might be that one more recursion, so &asymp;&#8239;0.14&nbsp;billion rhombii, would require 64&#8239;GiB of memory; and two more, &asymp;&#8239;0.37&nbsp;billion rhombii, would require &asymp;&#8239;160&#8239;GiB, and execution would likely take a few weeks. 
Also consider: the disk space to hold the JSON; the disk space to hold the database used for post-processing such as selection of relevant rhombii; the storage space and software on the computer in the tile-laying machines. 
Indeed, I doubt that your physical area really needs many more tens of millions of tiles.

Somewhere not far beyond `numTilings = 19`, there might be a need to enlarge some types from `long int` (&le;&nbsp;2<small><sup>31</sup></small>&#8239;&minus;&#8239;1 &asymp;&nbsp;2&nbsp;billion, and which should not be `unsigned` as it needs to be able to hold `-1`) to `long long int` (&le;&nbsp;2<small><sup>63</sup></small>&#8239;&minus;&#8239;1 &asymp;&nbsp;9&nbsp;quintillion =&nbsp;9&#8239;&times;&#8239;10<small><sup>18</sup></small>).

Anyway, by me, not tested beyond `numTilings = 17`&nbsp; &DoubleLongRightArrow;&nbsp;&nbsp;0&#8239;&le;&#8239;`tilingId`&#8239;&le;&#8239;16, the largest of which has almost fifty&#8209;four million rhombii.


## Code workings ##

The following is an overrview, but only an overview, of the workings of the code. 

### The header ###

There is a single header file, [<kbd>penrose.h</kbd>](../C/penrose.h), which:
* `#include`s multiple library headers;
* declares and defines various `static double const`, 
* declares types, both simple aliases, `enum`s, and `struct`s;
* declares all functions accessed outside the file in which defined.


### Functions ###

As explained above, in [<kbd>controls.c</kbd>](../C/controls.c) are multiple definitions needing, or perhaps needing, user attention. 

[<kbd>main.c</kbd>](../C/main.c) asks <samp>"What is to be the recursion depth = numTilings?"</samp>, with an `fscanf()` into `numTilings`. 
There is a `malloc()` of this length, and trivial initialisation. 
The showtime loop: <code>if(&nbsp;tilingId&nbsp;==&nbsp;0&nbsp;)</code> it calls `tiling_initial()`; otherwise passing the previous tiling into `tiling_descendant()`.

These two, `tiling_initial()` and `tiling_descendant()`, are in [<kbd>tilings.c</kbd>](../C/tilings.c). 
The latter is more complicated; the former creates an initial rhombus and does much as the former. 
These invoke much work.

* Computes `rhombii_NumMax`, and `malloc()` enough space for that many rhombii. 

* For each rhombus in the ancestor tiling, appends its descendants to the descendant tiling by repeated calls of `rhombus_append_descendants` (which is in [<kbd>rhombii.c</kbd>](../C/rhombii.c)). 
When almost `rhombii_NumMax` rhombii have been created, space is freed by `rhombii_purgeDuplicates()` (which is in [<kbd>purgeDuplicates.c</kbd>](../C/purgeDuplicates.c)), which is called again after the loop. 

* Then `neighbours_populate()` (which is in [<kbd>neighbours.c</kbd>](../C/neighbours.c)), and self-evidently populates the neighbours of each rhombus. 
Both this and the earlier duplicate-purging work with rhombii sorted by the *y* position, as done by `rhombii_sort()` in [<kbd>sortRhombii.c</kbd>](../C/sortRhombii.c).

* The next work is the most interesting. There is a call of `holesFill()`. 
Let&rsquo;s start with two examples, both using [the example tiling](../images/Penrose_Rh_10_clipped_norths.svg) at the top of the [Paths page](paths.md). 
Observe the thin tiles. 
On each one, one of its obtuse 144&deg; is &lsquo;north&rsquo;, and marked with a small circle. 
For every thin tile, each of its two north edges touch a fat, each of these two fats have a south edge touching the other, and each fat&rsquo;s north corner touches the thin. 
So if a thin has a neighbourless north edge, that fat rhombus can be added with uniquely defined orientation. 
Also, if four but not five fats south corners are all at the same location, then the round-5 path can be completed by adding the final fat. 
(But this does not work for north corners: the missing 72&deg; can be completed with a fat, or with two thins.)
And there are other positions for which holes or gaps have a unique completion, all called by `holesFill()`. 

    It is easiest to see the action, so images have beeen generated. 
    For all these output images, `tilingId`&nbsp;is&nbsp;8. 
    For each, `holesFill()` was called only for some values of `tilingId`. 
    To see this, in order, command-click each to open in a new tab, and then command-page-down or -up to move between them (or control instead of command):&nbsp; 
    [none](../images/Penrose_Rh_08_holesFill_none.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [only&nbsp;0](../images/Penrose_Rh_08_holesFill_only_00.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;1](../images/Penrose_Rh_08_holesFill_to_01.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;2](../images/Penrose_Rh_08_holesFill_to_02.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;3](../images/Penrose_Rh_08_holesFill_to_03.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;4](../images/Penrose_Rh_08_holesFill_to_04.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;5](../images/Penrose_Rh_08_holesFill_to_05.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;6](../images/Penrose_Rh_08_holesFill_to_06.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&le;&nbsp;7](../images/Penrose_Rh_08_holesFill_to_07.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [all](../images/Penrose_Rh_08_holesFill_all.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [&ge;&nbsp;1](../images/Penrose_Rh_08_holesFill_from_01.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [even](../images/Penrose_Rh_08_holesFill_even.svg?raw=1),&nbsp;&nbsp;&nbsp; 
    [odd](../images/Penrose_Rh_08_holesFill_odd.svg?raw=1).


    These extra rhombii require neighbourification, and for some of the `holesFill()` patterns, re-de-duplication. 

* [<kbd>paths.c</kbd>](../C/paths.c) finds paths. Naturally enough, start at an as-yet-unpathed fat rhombus, and trace using neighbours. If it is an open path, then on coming to an end it restarts from there. For closed paths, there a rhombus must be chosen to have zero `.withinPathNum`, so to be the first rhombus of the path. This is chosen by `rhWithinPathMoreSpecial()` to be one of the rhombii that is as close as possible to the centre of the path, and in the first quadrant.

* Which is the smallest enclosing path?  But the level of the question varies.  
    - Open paths cannot have an enclosing path.
    - Closed paths can, though not all do. So for fat rhombii, enclosing-path is at the level of the path.
    - But for thin rhombii, it is at the level of the rhombus. For thin rhombii, find a touching fat, and either the thin is immediately inside that fat&rsquo;s path, or the answer is the same as for that fat&rsquo;s path.

    Insideness is computed in [<kbd>insideness.c</kbd>](../C/insideness.c), which makes much use of [<kbd>windingNumber.c</kbd>](../C/windingNumber.c). 
    Because there are many candidate enclosing paths to be tested, this is potentially slow.

* For each length of path, how many paths are there? 
    How many fats and thins are enclosed? 
    What are the minimum and maximum radii? 
    This data is gathered by [<kbd>pathStats.c</kbd>](../C/pathStats.c).

* When making `holesFill()`, some properties were observed, and were checked over some good-size tilings by a routine `verifyHypothesisedProperties()` in [<kbd>propertyVerifying.c</kbd>](../C/propertyVerifying.c). 
    The routine remains, in case similar is needed, but currently does nothing. 

* Arrays needs sorting, requiring renumbering of pointers into into them. Already mentioned are `rhombii_sort()` in [<kbd>sortRhombii.c</kbd>](../C/sortRhombii.c); also `paths_sort()` in [<kbd>sortPaths.c</kbd>](../C/sortPaths.c); and `pathStats_sort()` in [<kbd>sortPathStats.c</kbd>](../C/sortPathStats.c).

* PostScript ([wikipedia](http://en.wikipedia.org/wiki/PostScript), [Adobe&rsquo;s&nbsp;PostScript Language Reference 3<small><sup>rd</sup></small>&nbsp;edition](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf)) is an an early-1980s printer-control language. 
    It is a delightful simple clean text-based user-alterable [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) means of making [PDF](https://en.wikipedia.org/wiki/PDF)s. 
    It is also an early-1980s printer-control language that is really showing its age. 
    Nurse&mdash;please, my stick! 
    For example, arrays and data structures have a maximum length of 2<small><sup>16</sup></small>&nbsp;&minus;&nbsp;1 =&nbsp;65,535 ([PLRM3, Appendix&nbsp;B, p739](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf#page=753), table&nbsp;B.1, row&nbsp;3). 
    That is, for all `tilingId`&#8239;&ge;&#8239;10, it is not possible to output all the data in a PostScript object. 
    Instead a &lsquo;wanted&rsquo; subset is chosen. 
    In [<kbd>controls.c</kbd>](../C/controls.c) there are functions `wantedPostScriptCentre()` which returns the centre of the desired region, and `wantedPostScriptAspect()` which returns the desired _y_/_x_ aspect ratio. 
    The largest possible area is then chosen, such that there are &le;&#8239;65535 rhombii (sometimes _x_ or _y_ equalities compel the number of rhombii to be slightly less than 65535). 
    (The other two PostScript output formats merely paint one thing after another, so never need a large PostScript array, and hence can cope with lots of stuff.)

* Some minor and small administration functions are in [<kbd>smalls.c</kbd>](../C/smalls.c). Numbers comparisons (which perhaps should have been overloaded, but which weren&rsquo;t): `max_2()`, `min_2()`, `max_4()`, `min_4()`, `avg_2()`, `median_3()`, `median_4()`. Point comparisons (ditto): `points_different_2()`, `points_same_2()`, `points_different_3()`, `points_same_3()`, `points_different_4()`, `points_same_4()`, `collinear()`. Small exporting helpers: `newlinesInString()`, `fileExtension_from_ExportFormat()`. 


### Exporting code ###

* There is occasional need to `sprintf()` to a string, for subsequent use. So [<kbd>exportTilings.c</kbd>](../C/exportTilings.c) creates `scratchString[]` of length 32767, which many export routines access with `extern char scratchString[];`.

* Some of the output files are large. There should be some effort to not enlarge them needlessly. In particular, the likes of &ldquo;<samp>1.000000000</samp>&rdquo; should be trimmed to &ldquo;<samp>1</samp>&rdquo;. This should not be done with scientific notation: if an _x_ or _y_ value is almost zero, then it should be &ldquo;<samp>0</samp>&rdquo; rather than &ldquo;<samp>-1.234567E-89</samp>&rdquo;. So there is a routine `stringClean()`, in [<kbd>stringClean.c</kbd>](../C/stringClean.c), which cleans a string in this style. Almost always, the string passed to this is `scratchString`. 


There are two very different types of export format. 

* Those with `enum` `ExportFormat` values {`JSON`, `TSV`, `PS_data`} hold data from multiple tilings, for subsequent processing. In [<kbd>exportTilings.c</kbd>](../C/exportTilings.c) it loops over tilings, calling code in [<kbd>exportTiling.c</kbd>](../C/exportTiling.c). Each of these loops over arrays, so repeatedly calling code in [<kbd>exportPathStats.c</kbd>](../C/exportPathStats.c), in [<kbd>exportPath.c</kbd>](../C/exportPath.c), and in [<kbd>exportRh.c</kbd>](../C/exportRh.c). For `TSV`the passed item can be a `NULL` pointer, instructing that the header row be output.

* Those with `enum` `ExportFormat` values {`PS_rhomb`, `PS_arcs`, `SVG_rhomb`, `SVG_arcs`} output something useful as-is. That is, an SVG can be viewed; a PostScript file can be distilled (using [Adobe Distiller](https://en.wikipedia.org/wiki/Adobe_Distiller) or [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript)) into a PDF. For these, code in [<kbd>export_SoloTiling.c</kbd>](../C/export_SoloTiling.c) calls the appropriate one of [<kbd>export_PaintArcsPS.c</kbd>](../C/export_PaintArcsPS.c), [<kbd>export_PaintArcsSVG.c</kbd>](../C/export_PaintArcsSVG.c), [<kbd>export_PaintRhPS.c</kbd>](../C/export_PaintRhPS.c), [<kbd>export_PaintRhSVG.c</kbd>](../C/export_PaintRhSVG.c). Some SVG processing is in [<kbd>Smalls_SVG.c</kbd>](../C/Smalls_SVG.c).


---

There is more detail about each type of output. 
Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;**C&nbsp;code**;&nbsp; 
&bull;&nbsp;[JSON&nbsp;output](json.md);&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md);&nbsp;
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(data)](postscript_data.md).
