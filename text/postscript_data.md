# Penrose Tilings: PostScript (Data) output #

## Contents ##

Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;[C&nbsp;code](c.md);&nbsp; 
&bull;&nbsp;[JSON&nbsp;output](json.md);&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md);&nbsp; 
&bull;&nbsp;**PostScript&nbsp;output&nbsp;(data)**;&nbsp; 
&bull;&nbsp;[Physical&nbsp;tiling](physical_tiling.md).


## Output ##

PostScript ([wikipedia](http://en.wikipedia.org/wiki/PostScript), [Adobe&rsquo;s&nbsp;PostScript Language Reference 3<small><sup>rd</sup></small>&nbsp;edition](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf)) is an an early-1980s reverse-Polish stack-based weakly-typed printer-control language. 
It is a delightful simple clean text-based user-alterable [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) means of making [PDF](https://en.wikipedia.org/wiki/PDF)s. 
But, even in the most flattering light, PostScript shows its age.
For example, arrays and data structures have a maximum length of 2<small><sup>16</sup></small>&nbsp;&minus;&nbsp;1 =&nbsp;65,535 ([PLRM3, Appendix&nbsp;B, p739](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf#page=753), table&nbsp;B.1, row&nbsp;3). 

Nonetheless, the enum `ExportFormat` includes the value `PS_data`. 
Initially, this was the primary format for PostScript. 
But the development of `ExportFormat`s `PS_rhomb` and `PS_arcs` means that `PS_data` is heading for deprecation. 
(New software, at point of release, has a feature described as &ldquo;heading for deprecation&rdquo;: cool, or dim-witted?)
For `PS_data` the data is exported as arrays (in PostScript, <code>[&nbsp;&hellip;&nbsp;]</code>) of dictionaries (<code><<&nbsp;&hellip;&nbsp;>></code>), each of which is limited to at most 65,535 items. 

So a &lsquo;wanted&rsquo; subset of rhombi is chosen. 
In <kbd>controls.c</kbd> there are functions `wantedPostScriptCentre()` which returns the centre of the desired region, and `wantedPostScriptAspect()` which returns the desired _y_/_x_ aspect ratio. 
The largest possible area is then chosen, such that there are &le;&#8239;65535 rhombi (sometimes _x_ or _y_ equalities compel the number of rhombi to be slightly less than 65535).

The `RhId` and `PathId` are renumbered, such that numbers point within the shorter PostScript arrays, rather than within the longer C arrays used in all other export formats.


## A PostScript program ##

This PostScript data is intended to be used by a PostScript program, and one might start with the supplied [example program](../postscript_pdf/Penrose_uses_data.ps). 
Starting about about line 77 this defines a large array 

```PostScript
/Tilings [
	% Many many lines omitted
] def  % /Tilings
```

If there is new C output, this array would need to be replaced. 

After this there are obvious definitions of variables such as `PageWidth`, `PageHeight`, `Margin`. 

Much of the useful work is done by the routine `Tiles_Paint`, which takes an array of &lsquo;representations&rsquo;, which are then painted in the array order. 
Some of these resemble directly usable output from the C (`/Rhombi_fill`, `/Rhombi_stroke`, `/Rhombi_circles5`, `Arcs_fill`, `/Arcs_stroke`); others are different representations (`/Diagonals_stroke`, `/Triangles_fill`); and others are labellings for debugging.

This is called below. 
There is a loop over `Tilings`, looping over several possible such arrays of representations.

All of this is changeble and alterable. 
Indeed, in the [PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md) output the routine `PaintByRhombus` could be altered to do the work in `/Diagonals_stroke` or that in `/Triangles_fill`. 
Even though the PostScript-data output is used less than it was, this is all good play.

## Speed and size ##

Generally, if working with PostScript, [Adobe&nbsp;Distiller](https://en.wikipedia.org/wiki/Adobe_Distiller) is superior to [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript): quicker execution, smaller PDF size, even obscure parameters are completely ujderstood. 
But this PostScript-data confounds Distiller, which can be much much slower than Ghostscript. 
However, the Distiller PDF sizes are still smaller than those output by Ghostscript.
