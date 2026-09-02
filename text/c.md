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
The language has great power over the computer on which it is running, at a low level able to inspect and alter memory, and likewise read and write from disks. 
This used to be thought fast and powerful, but is now described as insecure. 
One should be careful about code downloaded from the internet; one should be extra careful about C&nbsp;code downloaded from the internet. 
Indeed, since 17<small><sup>th</sup></small>&nbsp;January 2025 the US Government has been [discouraging the use&nbsp;of&nbsp;C](https://www.cisa.gov/resources-tools/resources/product-security-bad-practices): 

> **1)** The development of new product lines for use in service of critical infrastructure or NCFs in a memory-unsafe language (e.g., C or C++) where readily available alternative memory-safe languages could be used is dangerous and significantly elevates risk to national security, national economic security, and national public health and safety.

So, even though this is not &ldquo;critical infrastructure&rdquo;, be careful. 
Run this code only in your compiler&rsquo;s debug mode, as that will block pointer mischief. 
Yes, it will execute slower, but execution is a once-off to generate data: pay the slower to gain the full paranoia.

Also, this code outputs multiple files ([.json](json.md) [.tsv](tsv.md), [.svg](svg.md), [.ps](postscript_distillable.md), [.ps](postscript_data.md)). 
Allow writing only to the chosen output directory `filePath_staticConst[]`; block all other writing; block all file reading.

I know that this code is not naughty, and I believe that it is not careless. 
But it&rsquo;s 9.9k lines of&nbsp;C, so you cannot easily know whether I truthfully know or believe these. 
(But your AI might know: do ask it.) 
If executing this downloaded C, indeed any downloaded C, you should be paranoid, meaning execute only in your compiler&rsquo;s debug mode, with file access restricted as in the previous&nbsp;&para;.

Language version: this 
[C](https://en.wikipedia.org/wiki/C_(programming_language)) code uses 
inline comments [`//`](https://en.cppreference.com/w/c/comment.html), types 
[`bool`](https://en.cppreference.com/w/c/language/arithmetic_types.html#Boolean_type), 
[`int8_t`](https://en.cppreference.com/w/c/types/integer.html#Types) and 
[<code>long&nbsp;long&nbsp;int</code>](https://en.cppreference.com/w/c/language/arithmetic_types.html#Integer_types), and 
[inline struct initialisation](https://en.cppreference.com/w/c/language/struct_initialization.html). 
Hence this code needs &ge;&nbsp;[C99](https://en.wikipedia.org/wiki/C99). 
This C&nbsp;code is not valid 
original ANSI&nbsp;C&nbsp;=&nbsp;[C89](https://en.wikipedia.org/wiki/ANSI_C#C89), 
nor valid [K&R&nbsp;&lsquo;original&rsquo;](https://en.wikipedia.org/wiki/The_C_Programming_Language).


## Changes to the C, mandatory and optional ##

The C code cannot quite be executed directly out of the box: a few trivial customisations are needed, all of which are in the [<kbd>controls.c</kbd>](../C/controls.c) file.

* **Most important**: to what directory should output go, as used by <code>fopen(&hellip;, "w")</code>? 
This is in `filePath_staticConst[]` and almost certainly you should change this.

* After an SVG or a distillable PostScript file is finished, should an application be called to display it or distill it? 
This is done by <code>execute_SVG_PostProcessing(&hellip;)</code> and <code>execute_PostScript_PostProcessing(&hellip;)</code>. 
By default, these call Chrome and GhostScript, in my computer&rsquo;s location of these applications. 
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
(For similar control over PDF from the [distillable PostScript output](postscript_distillable.md), the controls are within the PostScript file: e.g., `/ToPaint_XMin -7 def`, etc.)

In `main()` a call of `fscanf()` asks how many loops of `TilingId` should there be (which is bounded above by `numTilings_Max` as described below). 
This is an important question, answering which takes the next section of this page.


## Loops of TilingId ##

The C code creates an initial tiling, then loops recursively creating each new tiling from the previous. 
From one loop to the next the number of tiles grows, approximately, by a factor &gap;&nbsp;&phi;&sup2;&nbsp;&asymp;&nbsp;2.618 &asymp;&nbsp;10<small><sup>0.42</sup></small> &asymp;&nbsp;10<small><sup>(1&#8239;&divide;&#8239;2.4)</sup></small>. 
The table below shows the numbers of fat and thin tiles in each tiling. 


| Tl&rsquo;g<br>Id | Num<br>Fats | Num<br>Thins | Long<br>-est<br>closed<br>path | &lsquo;Area&rsquo; | Bound<br>-ary<br>num<br>vertices | JSON<br>size | &asymp;&#8239;&Sum;<small><sub>0&#10141;_n_</sub></small><br>exec.<br>time |
|----------------------------------------:|-----------:|-----------:|-----------------:|--------------:|-------:|----------------:|--------------:|
| [&numsp;0](../images/Penrose_Rh_00.svg) |          5 |          5 | 5&lsquo;r&rsquo; |               |     10 |   5.9&#8239;KiB |               |
| [&numsp;1](../images/Penrose_Rh_01.svg) |         20 |         10 | 5&lsquo;p&rsquo; |               |     20 |  21.0&#8239;KiB |               |
| [&numsp;2](../images/Penrose_Rh_02.svg) |         65 |         40 |               15 |               |     40 |  69.5&#8239;KiB |               |
| [&numsp;3](../images/Penrose_Rh_03.svg) |        190 |        110 |               25 |      184      |     70 |   203&#8239;KiB |               |
| [&numsp;4](../images/Penrose_Rh_04.svg) |        530 |        320 |               55 |      444      |    120 |   581&#8239;KiB |               |
| [&numsp;5](../images/Penrose_Rh_05.svg) |      1,440 |        870 |              105 |     1303      |    200 | 1,613&#8239;KiB |               |
| [&numsp;6](../images/Penrose_Rh_06.svg) |      3,855 |      2,355 |              215 |     3286      |    330 | 4,407&#8239;KiB |               |
| [&numsp;7](../images/Penrose_Rh_07.svg) |     10,230 |      6,270 |              425 |     9&#8239;k |    540 |  11.6&#8239;MiB |               |
| [&numsp;8](../images/Penrose_Rh_08.svg) |     27,005 |     16,610 |              855 |    23&#8239;k |    880 |  31.1&#8239;MiB |               |
| [&numsp;9](../images/Penrose_Rh_09.svg) |     71,060 |     43,780 |            1,705 |    62&#8239;k |  1,430 |  83.1&#8239;MiB |   4&#8239;s   |
|       [10](../images/Penrose_Rh_10.svg) |    186,620 |    115,120 |            3,415 |   162&#8239;k |  2,320 |   222&#8239;MiB |  11&#8239;s   |
|       [11](../images/Penrose_Rh_11.svg) |    489,520 |    302,180 |            6,825 |   427&#8239;k |  3,760 |   588&#8239;MiB |  26&#8239;s   |
|        12                               |  1,283,105 |    792,425 |           13,655 | 1,116&#8239;k |  6,090 | 1,560&#8239;MiB |  89&#8239;s   |
|        13                               |  3,361,680 |  2,076,690 |           27,305 |   2.9&#8239;M |  9,860 | 4,126&#8239;MiB |   4&#8239;min |
|        14                               |  8,804,985 |  5,440,260 |           54,615 |   7.7&#8239;M | 15,960 |  10.7&#8239;GiB |  22&#8239;min |


### &lsquo;Area&rsquo; ###

The &lsquo;Area&rsquo; quoted in the table is not quite of the whole tiling. 
Near the edges of a tiling paths are self-evidently incomplete, their length indeterminate, frustrating a tiling with paths coloured by lengh. 
So the quoted &lsquo;area&rsquo; is of a circle not containing any open paths of length shorter than the longest open path(s). 
That is, within this circle, colour consistently all rhombi in open paths, and it will be and seem consistent. 
The area is denominated in units of square tile edges. 
For `tilingId`&#8239;&gap;&#8239;6, this &lsquo;colourable&rsquo; circular &lsquo;Area&rsquo; is about &frac23; of the whole tiling.

[My kitchen](../images/20230923_kitchen_tiles.png) uses tile edges of length 150mm: to convert square tile edges to metres squared, assuming this tile size, divided by &asymp;&#8239;49.6, 
as the average tile area is &frac14;(&radic;5&nbsp;+&nbsp;&radic;(10&nbsp;&minus;&nbsp;2&radic;5)&nbsp;&minus;&nbsp;1) &asymp;&nbsp;0.8968  &asymp;&nbsp;1/1.1151. 
Tiling of a substantial outdoor space might use tiles of edge length 500mm&#8239;&asymp;&#8239;19.7&Prime;, so for m&sup2; use the &lsquo;Area&rsquo;&#8239;&divide;&#8239;4. 
The tilings have pentagonal symmetry: if it were desired to emphasise aperiodicity by using an asymmetric subset, then the usable part would be much smaller. 


### Speed and limits ###

The quoted execution times are from a ten-core &lap;4.4&#8239;GHz Apple&nbsp;M4 Mac&nbsp;mini with 16&#8239;GiB of memory. 
Execution was in [Xcode](https://developer.apple.com/xcode/)&rsquo;s debug mode to catch possible stray pointer misbehaviours (of which there were none); and with memory&#8209;alignment at its natural value by <code>#define&nbsp;MEMORY_FRUGALITY_OUTRANKS_SPEED&nbsp;false</code> in [<kbd>penrose.h</kbd>](../C/penrose.h). 
Obviously, your execution times might be very different.

For `15`, memory needed exceeded the available 16&#8239;GiB, so the OS started paging memory: execution was cancelled after 12 hours. 
Hence a deeper recursion needs more memory. 
It might be that one more recursion, `15`, so &asymp;&#8239;37&nbsp;million rhombi, would require 24&#8239;GiB of memory; and two more, &asymp;&#8239;97&nbsp;million rhombi, would require &asymp;&#8239;48&#8239;GiB, likely requiring a few days to execute. 
Also consider: the disk space to hold its JSON; the disk space to hold the database used for post-processing such as selection of relevant rhombi; the storage space and software on the computers in the tile&#8209;laying machines. 
And it could fairly be asked whether your physical area _really_ needs many more millions of tiles (likely answer:&nbsp;no).

For more than 2.1&#8239;bn rhombi, so many square kilometres, there would be a need to enlarge some types from 
<code>long&nbsp;int</code> (&le;&nbsp;2<small><sup>31</sup></small>&#8239;&minus;&#8239;1) to 
[<code>long&nbsp;long&nbsp;int</code>](https://en.cppreference.com/w/c/language/arithmetic_types.html#Integer_types) 
(&le;&nbsp;2<small><sup>63</sup></small>&#8239;&minus;&#8239;1 &asymp;&nbsp;9&#8239;&times;&#8239;10<small><sup>18</sup></small> =&nbsp;9&nbsp;quintillion &gap;&nbsp;vast&nbsp;plethora), which needs to be able to hold `-1` so cannot be `unsigned`. 
This would require changing some format strings passed to `fprintf()` and variants, &ldquo;`%li`&rdquo; becoming &ldquo;`%lli`&rdquo;. 
If this is necessary, perhaps also improve the floating-point precision by replacing `double` with 
[<code>long&nbsp;double</code>](https://en.cppreference.com/w/c/language/arithmetic_types.html#Real_floating_types) ([wikipedia](https://en.wikipedia.org/wiki/Long_double)), 
which would necessitate further changes to format strings. 
In [<kbd>main.c</kbd>](../C/main.c) is 
<code>static&nbsp;int8_t&nbsp;const <b>numTilings_Max</b>&nbsp;=&nbsp;15;</code>, which prevents the accidental start, by a typing error such as &ldquo;<kbd>115</kbd>&rdquo;, of a task that is certain to fail eventually. 
If you really want to allow larger values&mdash;but almost certainly you don&rsquo;t&mdash;then increase `numTilings_Max`.

Anyway, by me, not tested beyond the table above, the largest of which has 14,245,245 rhombi.


## Code workings ##

The following is an overview, but only an overview, of the workings of the code.

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
The former creates an initial rhombus and does minor bureaucracy; the latter is more complicated and does much work.

* Computes `rhombi_NumMax`, being a slight overestimate of the number of rhombi for which space needed, and `malloc()` enough space for that many rhombi. 

* For each rhombus in the ancestor tiling, appends its descendants to the descendant tiling by repeated calls of `rhombus_append_descendants` (which is in [<kbd>rhombi.c</kbd>](../C/rhombi.c)). 
When almost `rhombi_NumMax` rhombi have been created, space is freed by `rhombi_purgeDuplicates()` (which is in [<kbd>purgeDuplicates.c</kbd>](../C/purgeDuplicates.c)), which is called again after the loop. 

* Then `neighbours_populate()` (which is in [<kbd>neighbours.c</kbd>](../C/neighbours.c)), and self-evidently populates the neighbours of each rhombus. 
Both this and the earlier duplicate-purging work with rhombi sorted by the *y* position, as done by `rhombi_sort()` in [<kbd>sortRhombi.c</kbd>](../C/sortRhombi.c).

* The next work is the most interesting. There is a call of `holesFill()`. 
Let&rsquo;s start with two examples, both using [the example tiling](../images/Penrose_Rh_09_clipped_norths.svg) at the top of the [Paths page](paths.md). 
Observe the thin tiles. 
On each one, one of its obtuse 144&deg; is &lsquo;north&rsquo;, and marked with a small circle. 
For every thin tile, each of its two north edges touch a fat, each of these two fats have a south edge touching the other, and each fat&rsquo;s north corner touches the thin. 
So if a thin has a neighbourless north edge, that fat rhombus can be added with uniquely defined orientation. 
Also, if four but not five fats south corners are all at the same location, then the round-5 path can be completed by adding the final fat. 
(But this does not work for north corners: the missing 72&deg; can be completed with a fat, or with two thins.)
And there are other positions for which holes or gaps have a unique completion, all called by `holesFill()`. 

    It is easiest to see the action, so for`tilingId`&nbsp;is&nbsp;8, open in adjacent tabs the 
    [regular](../images/Penrose_Rh_08.svg?raw=1) and 
    [holes&nbsp;not&nbsp;filled](../images/Penrose_holesFill_not_Rh_08.svg?raw=1), 
    then command-page-down or -up to move between them (or control instead of command). 

    Whether or not holes are so filled is determined by the Boolean returned by `holesFillQ()`. 
    This could be made complicated, but for production the only sensible value is `true`. 
    Indeed, likely, the only purpose of having this controllable is to make this documentation.

    These extra rhombi require neighbourification, and for some of the `holesFill()` patterns, re&#8209;de&#8209;duplication. 

* [<kbd>paths.c</kbd>](../C/paths.c) finds paths. Naturally enough, start at an as-yet-unpathed fat rhombus, and trace using neighbours. If it is an open path, then on coming to an end it restarts from there. For closed paths, a rhombus must be chosen to have zero `.withinPathNum`, so to be the first rhombus of the path. This is chosen by `rhWithinPathMoreSpecial()` to be one of the rhombi that is as close as possible to the centre of the path, and in the first quadrant.

* Which is the smallest enclosing path?  But the level of the question varies.  
    - Open paths cannot have an enclosing path.
    - Closed paths can, though not all do. So for fat rhombi, enclosing-path is at the level of the path.
    - But for thin rhombi, it is at the level of the rhombus. For thin rhombi, find a touching fat, and either the thin is immediately inside that fat&rsquo;s path, or the answer is the same as for that fat&rsquo;s path.

    Insideness is computed in [<kbd>insideness.c</kbd>](../C/insideness.c), which makes much use of [<kbd>windingNumber.c</kbd>](../C/windingNumber.c). 
    Because there are many candidate enclosing paths to be tested, this is potentially slow.

* For each length of path, how many paths are there? 
    How many fats and thins are enclosed? 
    What are the minimum and maximum radii? 
    This data is gathered by [<kbd>pathStats.c</kbd>](../C/pathStats.c).

* When making `holesFill()`, some properties were observed, and were checked over some good-size tilings by a routine `verifyHypothesisedProperties()` in [<kbd>propertyVerifying.c</kbd>](../C/propertyVerifying.c). 
    The routine remains, in case similar is needed, but currently does nothing. 

* Arrays needs sorting, requiring renumbering of pointers into into them. Already mentioned are `rhombi_sort()` in [<kbd>sortRhombi.c</kbd>](../C/sortRhombi.c); also `paths_sort()` in [<kbd>sortPaths.c</kbd>](../C/sortPaths.c); and `pathStats_sort()` in [<kbd>sortPathStats.c</kbd>](../C/sortPathStats.c).

* PostScript ([wikipedia](http://en.wikipedia.org/wiki/PostScript), [Adobe&rsquo;s&nbsp;PostScript Language Reference 3<small><sup>rd</sup></small>&nbsp;edition](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf)) is an an early-1980s printer-control language. 
    It is a delightful simple clean text-based user-alterable [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) means of making [PDF](https://en.wikipedia.org/wiki/PDF)s. 
    It is also an early-1980s printer-control language that is really showing its age. 
    Nurse&mdash;please, my stick! 
    For example, arrays and data structures have a maximum length of 2<small><sup>16</sup></small>&nbsp;&minus;&nbsp;1 =&nbsp;65,535 ([PLRM3, Appendix&nbsp;B, p739](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf#page=753), table&nbsp;B.1, row&nbsp;3). 
    That is, for all `tilingId`&#8239;&ge;&#8239;9, it is not possible to output all the data in a PostScript object. 
    Instead a &lsquo;wanted&rsquo; subset is chosen. 
    In [<kbd>controls.c</kbd>](../C/controls.c) there are functions `wantedPostScriptCentre()` which returns the centre of the desired region, and `wantedPostScriptAspect()` which returns the desired _y_/_x_ aspect ratio. 
    The largest possible area is then chosen, such that there are &le;&#8239;65535 rhombi (sometimes _x_ or _y_ equalities compel the number of rhombi to be slightly less than 65535). 
    (The other two PostScript output formats merely paint one thing after another, so never need a large PostScript array, and hence can cope with lots of stuff.)

* Some minor and small administration functions are in [<kbd>smalls.c</kbd>](../C/smalls.c). Numbers comparisons (which perhaps should have been overloaded, but which weren&rsquo;t): `max_2()`, `min_2()`, `max_4()`, `min_4()`, `avg_2()`, `median_3()`, `median_4()`. Point comparisons (ditto): `points_different_2()`, `points_same_2()`, `points_different_3()`, `points_same_3()`, `points_different_4()`, `points_same_4()`, `collinear()`. Small exporting helpers: `file_open()` which does `fopen` with error handling, `newlinesInString()`, `fileExtension_from_ExportFormat()`, `filename_new_extension()` which is used to rename a <kbd>.ps</kbd> to a <kbd>.pdf</kbd>.


### Exporting code ###

* There is occasional need to `sprintf()` to a string, for subsequent use. So [<kbd>main.c</kbd>](../C/main.c) creates `scratchString[]` of length 32767, which many export routines access with <code>extern&nbsp;char&nbsp;scratchString[];</code>.

* Some of the output files are large. There should be some effort to not enlarge them needlessly. In particular, the likes of &ldquo;<samp>1.000000000</samp>&rdquo; should be trimmed to &ldquo;<samp>1</samp>&rdquo;. Numbers should not be shown in scientific notation: if an _x_ or _y_ value is almost zero, then it should be &ldquo;<samp>0</samp>&rdquo; rather than &ldquo;<samp>-1.234567E-89</samp>&rdquo;. So there is a routine `stringClean()`, in [<kbd>stringClean.c</kbd>](../C/stringClean.c), which cleans a string in this style. The string passed to this is always `scratchString`.


There are two very different types of export format. 

* Those with `enum` `ExportFormat` values {`JSON`, `TSV`, `PS_data`} hold data from multiple tilings, for subsequent processing. In [<kbd>exportTilings.c</kbd>](../C/exportTilings.c) it loops over tilings, calling code in [<kbd>exportTiling.c</kbd>](../C/exportTiling.c). Each of these loops over arrays, so repeatedly calling code in [<kbd>exportPathStats.c</kbd>](../C/exportPathStats.c), in [<kbd>exportPath.c</kbd>](../C/exportPath.c), and in [<kbd>exportRh.c</kbd>](../C/exportRh.c). For `TSV` the passed item can be a `NULL` pointer, instructing that the header row be output.

* Those with `enum` `ExportFormat` values {`PS_rhomb`, `PS_arcs`, `SVG_rhomb`, `SVG_arcs`} output something useful as-is. That is, an SVG can be viewed; a PostScript file can be distilled (using [Adobe Distiller](https://en.wikipedia.org/wiki/Adobe_Distiller) or [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript)) into a PDF. For these, code in [<kbd>export_SoloTiling.c</kbd>](../C/export_SoloTiling.c) calls the appropriate one of [<kbd>export_PaintArcsPS.c</kbd>](../C/export_PaintArcsPS.c), [<kbd>export_PaintArcsSVG.c</kbd>](../C/export_PaintArcsSVG.c), [<kbd>export_PaintRhPS.c</kbd>](../C/export_PaintRhPS.c), [<kbd>export_PaintRhSVG.c</kbd>](../C/export_PaintRhSVG.c). Some SVG processing is in [<kbd>Smalls_SVG.c</kbd>](../C/Smalls_SVG.c).

* The routine `tiling_export_PaintBoundary()` in [<kbd>export_BoundarySVG.c</kbd>](../C/export_BoundarySVG.c) computes and exports (but does not store internally) the exterior boundary of the rhombi. The purpose is to provide a background colour for the `SVG_arcs`, but it will also work with an `ExportFormat` of `SVG_rhomb`, `PS_arcs`, `PS_rhomb`, `PS_data`, `TSV`, and `JSON`. For large tilings with _n_ tiles, the number of points on the boundary seems to be slightly less than 3&radic;_n_. The first call of [<kbd>export_BoundarySVG.c</kbd>](../C/export_BoundarySVG.c) computes the boundary&rsquo;s length, which is stored, exporting nothing.

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
