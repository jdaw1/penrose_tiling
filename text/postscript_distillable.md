# Penrose Tilings: PostScript (Distillable) output #

## Contents ##

Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;[C&nbsp;code](c.md);&nbsp; 
&bull;&nbsp;[JSON&nbsp;output](json.md);&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;**PostScript&nbsp;output&nbsp;(distillable)**;&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(data)](postscript_data.md);&nbsp; 
&bull;&nbsp;[Physical&nbsp;tiling](physical_tiling.md).


## Some output ##

<table>
<tr>
	<th align="right">.ps, Rhombi:</th>
	<td><a href="../postscript_pdf/Penrose_Rh_00.ps">00</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_01.ps">01</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_02.ps">02</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_03.ps">03</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_04.ps">04</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_05.ps">05</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_06.ps">06</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_07.ps">07</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_08.ps">08</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_09.ps">09</a></td>
	<td></td>
	<td></td>
</tr><tr>
	<th align="right">.pdf, Rhombi:</th>
	<td><a href="../postscript_pdf/Penrose_Rh_00.pdf">00</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_01.pdf">01</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_02.pdf">02</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_03.pdf">03</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_04.pdf">04</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_05.pdf">05</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_06.pdf">06</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_07.pdf">07</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_08.pdf">08</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_09.pdf">09</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_10.pdf">10</a></td>
	<td><a href="../postscript_pdf/Penrose_Rh_11.pdf">11</a></td>
</tr><tr>
	<th align="right">.ps, Arcs:</th>
	<td><a href="../postscript_pdf/Penrose_Arcs_00.ps">00</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_01.ps">01</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_02.ps">02</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_03.ps">03</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_04.ps">04</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_05.ps">05</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_06.ps">06</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_07.ps">07</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_08.ps">08</a></td>
	<td></td>
	<td></td>
	<td></td>
</tr><tr>
	<th align="right">.pdf, Arcs:</th>
	<td><a href="../postscript_pdf/Penrose_Arcs_00.pdf">00</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_01.pdf">01</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_02.pdf">02</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_03.pdf">03</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_04.pdf">04</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_05.pdf">05</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_06.pdf">06</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_07.pdf">07</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_08.pdf">08</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_09.pdf">09</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_10.pdf">10</a></td>
	<td></td>
</tr>
</table>

Endpapers, being a riff on an old-school [marbling](https://en.wikipedia.org/wiki/Paper_marbling): 
[Endpapers_015.pdf](../postscript_pdf/Endpapers_015.pdf) (similar to that used in [Port&nbsp;Vintages](https://academieduvinlibrary.com/products/port-vintages)); 
[Endpapers_215.pdf](../postscript_pdf/Endpapers_215.pdf); and the PostScript from which both generated (&plusmn;&nbsp;tiny variation) [Endpapers_215.ps](../postscript_pdf/Endpapers_215.ps).

Also files making the map of my kitchen:
[Kitchen_Rh_09.**ps**](../postscript_pdf/Kitchen_Rh_09.ps); and 
[Kitchen_Rh_09.**pdf**](../postscript_pdf/Kitchen_Rh_09.pdf); and also
[20230923_kitchen_tiles.**png**](../images/20230923_kitchen_tiles.png). 

## The PostScript Language ##

PostScript ([wikipedia](http://en.wikipedia.org/wiki/PostScript), [Adobe&rsquo;s&nbsp;PostScript Language Reference 3<small><sup>rd</sup></small>&nbsp;edition](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf)) is an an early-1980s reverse-Polish stack-based weakly-typed printer-control language. 
It is a delightful simple clean text-based user-alterable [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) means of making [PDF](https://en.wikipedia.org/wiki/PDF)s. 
But, even in the most flattering light, PostScript is showing its age.

[This author](https://jdawiseman.com/author.html) is fond of PostScript, and has done much work in it. 
(Indeed, I maintain and use a [splendid PostScript program](https://github.com/jdaw1/placemat/blob/main/README.md#readme) of a mere &asymp;&#8239;16k lines.) 

PostScript is a natural and relatively simple way to generate PDFs from a low-level language such as&nbsp;C: the&nbsp;C writes ASCII-text PostScript, which is then converted to PDF. 
That conversion, that &lsquo;distillation&rsquo;, requires an application, likely to be either [Adobe&nbsp;Distiller](https://en.wikipedia.org/wiki/Adobe_Distiller) or [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript) or an online route to Ghostscript such as [ps2pdf.org](https://www.ps2pdf.org/convert.htm). 
For these purposes, any would be satisfactor, except that Adobe is penny&#8209;wise pound&#8209;foolish expensive.


## PostScript Files ##

The C code exports distillable files for both rhombi and arcs. 
(For an explanation of arcs see the [SVG&nbsp;documentation](svg.md).)

Much of the control of these codes are quite similar.

Early is:

```PostScript
/ToPaint_XMin -7 def  % User changeable
/ToPaint_XMax +6 def
/ToPaint_YMin -8 def
/ToPaint_YMax +8 def
```

This is the bounding box of what is to be shown: shrink to zoom.

Both have:
```PostScript
/PageWidth  297 360 mul 127 div def
/PageHeight 420 360 mul 127 div def
```

The `360 mul 127 div` converts millimetres to the units of PostScript, [DTP points](https://en.wikipedia.org/wiki/Point_(typography)). 
A4 is an [ISO&nbsp;216 paper size](https://en.wikipedia.org/wiki/ISO_216), 297&#8239;mm&#8239;&times;&#8239;210&#8239;mm;&nbsp; 
A3 is 420&#8239;mm&#8239;&times;&#8239;297&#8239;mm;&nbsp; 
A2 is 594&#8239;mm&#8239;&times;&#8239;420&#8239;mm.&nbsp; 
For the US size&nbsp;8&frac12;&Prime;&#8239;&times;&#8239;11&Prime; use <code>/PageWidth&nbsp;8.5&nbsp;72&nbsp;mul&nbsp;def</code> and <code>/PageHeight&nbsp;11&nbsp;72&nbsp;mul&nbsp;def</code> (&because;&nbsp;72&#8239;pt&nbsp;&equiv;&nbsp;1&Prime;), and likewise for other inch-specified page sizes.
(*NB*: [1&nbsp;inch is defined](https://en.wikipedia.org/wiki/International_yard_and_pound) to be exactly 25.4&#8239;mm;&nbsp; 
1&nbsp;DTP&nbsp;point is defined to be 1&frasl;72 of an inch;&nbsp; 
&Longleftrightarrow;&nbsp; 1&#8239;pt&nbsp;=&nbsp;127&frasl;360&#8239;mm&nbsp; 
&Longleftrightarrow;&nbsp; 1&#8239;pt&nbsp;=&nbsp;0.35277&#773;&#8239;mm&nbsp; 
&DoubleLongRightArrow;&nbsp; 1&#8239;mm&nbsp;&asymp;&nbsp;2.8346&#8239;pt.)


The <code>/Margin&nbsp;18&nbsp;def</code> says 18&#8239;pt =&nbsp;18&frasl;72&nbsp;inch =&nbsp;&frac13;&Prime; &asymp;&nbsp;8.5&#8239;mm. 
If not liked, change.

In the arcs file, `/LongestPathToBeFilled 215 def` is as explained in the [SVG&nbsp;documentation](svg.md).

The `ScaleFactor` is derived from values computed from the `ToPaint_XMin` etc, but can be overidden by the user.

The rhombus output specifies the routine `PaintByRhombus`, which takes parameters off the stack, and then paints the parameter-specified rhombus. 
It encodes conditions making the choice of which colour, and then a `setrgbcolor` or a `setgray`. 
The [author](https://jdawiseman.com/author.html) has strong shape perception but very weak colour perception, so you might well want to change the colours. 
Really, don&rsquo;t love my colours: be not shy about recolouration.

The rhombus code also has multiple sections of commented-out code that were used in debugging, with nearby explanatory comments. 
If useful to you, play and experiment. 
(In PostScript comments begin with a `%`.) 

It also has pieces of code wrapped in &ldquo;<code><i>Boolean</i>&nbsp;{&nbsp;&hellip;&nbsp;}&nbsp;if</code>&rdquo;, by default disabled with a `false`. 
Generally, nearby coments explain. 
If wanted, enable with `true`.

Is the `stroke` to be a single line, as in the second diagram on the [SVG&nbsp;page](svg.md), or outer-and-inner lines, as in that page&rsquo;s top diagram? 
To allow variations, `stroke`ing is done by the user-alterable routine `StrokeMulti`, which has a commented-out example.

Users might also want to find and change instances of `setlinewidth`. 
When making such changes, it might be simplest to replace the current &ldquo;`setlinewidth`&rdquo; with something of the form &ldquo;<code>2&nbsp;mul&nbsp;setlinewidth</code>&rdquo;.


## Kitchen ##

The files 
[Kitchen_Rh_09.**ps**](../postscript_pdf/Kitchen_Rh_09.ps) and 
[Kitchen_Rh_09.**pdf**](../postscript_pdf/Kitchen_Rh_09.pdf) show the map of my kitchen.

**For those tiling a non-huge space, this PostScript, appropriately amended, might be all that is needed from this repo.**

Obviously, for the particulars of your project, changes are needed. 
After each change re&#8209;distill: first&#8209;draft PostScript is often imperfect. 
To facilitate reverting a step or few, frequently make a copy of your file (on macOS&nbsp;by&nbsp;&#8984;D).

Find the string &ldquo;`Boolean: paint gridlines?`&rdquo;, and set the nearby Boolean to `true`.

Find `mmTileEdge`. 
A few lines up is a Boolean condition. 
If you are editing [my kitchen file](../postscript_pdf/Kitchen_Rh_09.ps), it will already be evaluating to `true`; if you are starting from [the PostScript output by the C](../postscript_pdf/Penrose_Rh_09.ps), it might be &ldquo;`TilingId -9 eq`&rdquo;, which needs to be changed to be true, either as &ldquo;`TilingId 9 eq`&rdquo; or just &ldquo;`true`&rdquo;. 

How big are your tiles? 
That is, how long are their edges? 
Set `mmTileEdge`.

The room shape needs to be specified, in `KitchenRoomPath`. 
This can be detailed, accurately tracing around every boxed&#8209;in pipe and other minor feature. 
At the start, keep it simple: make it the broad outline of the room, so likely 4 to 8 lines. 
Also configure the esentials of `KitchenFurniturePath`, perhaps the major cabinetry that will hide parts of the floor.

There is a command resembling `TileMatrix setmatrix  -11.3 -21.8 translate`: change this to position your room on the tiling. 
Aesthetic suggestion: include within the room a long path, either part of a very long path, or the whole of a closed path, so that its length can contrast with the many shorter closed paths that will be present. 

For the moment, keep `ToPaint_XMin` etc very wide, perhaps at their original values.

Then, several laps of amendments:
* change `ToPaint_XMin` etc to zoom in, proceeding gradually;
* tinker with the _x_ and _y_ values of the `translate`;
* more precisely specify `KitchenRoomPath` and `KitchenFurniturePath`.

Repeatedly re-distill, to check that code is working and the design aesthetics are improving.

There is comment in the code, worth heeding:

> Going to do a translation, in `TileSpace`, in which a tile is of length `EdgeLength` = 1. Almost certainly, you want to translate to a point within `ToPaint_XMin` etc. This point is the origin, the zero point, for the drawing of the room. Suggestion: make it the point from which physical tiling will start, so the point of certain tile alignment, so allowing (small) changes in mmTileEdge to correctly adjust for mis-judgements of grouting thickness.

Do re-tinker. 
Do come back the following day to review. 
Do show to the spouse, for approval. 
When it&rsquo;s final good, return to the string &ldquo;`Boolean: paint gridlines?`&rdquo;, and revert to `false`. 

It is all somewhat fiddly. 
But it can be done, even by a non-programmer, and is likely to take only a few hours. 

Finally: do check your room measurements! Do not get these wrong.
