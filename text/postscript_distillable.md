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
	<td><a href="../postscript_pdf/Penrose_Rh_10.ps">10</a></td>
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
	<td><a href="../postscript_pdf/Penrose_Arcs_09.ps">09</a></td>
	<td><a href="../postscript_pdf/Penrose_Arcs_10.ps">10</a></td>
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
</tr>
</table>

Endpapers, being a riff on an old-school [marbling](https://en.wikipedia.org/wiki/Paper_marbling): 
[Endpapers_015.pdf](../postscript_pdf/Endpapers_015.pdf) (similar to that used in [Port&nbsp;Vintages](https://academieduvinlibrary.com/products/port-vintages)); 
[Endpapers_215.pdf](../postscript_pdf/Endpapers_215.pdf); and the PostScript from which both generated (&plusmn;&nbsp;tiny variation) [Endpapers_215.ps](../postscript_pdf/Endpapers_215.ps).

Also files making the map of my kitchen:
[Kitchen_Rh_08.**ps**](../postscript_pdf/Kitchen_Rh_08.ps); and 
[Kitchen_Rh_08.**pdf**](../postscript_pdf/Kitchen_Rh_08.pdf); and also
[20230923_kitchen_tiles.**png**](../images/20230923_kitchen_tiles.png). 

## The PostScript Language ##

PostScript ([wikipedia](http://en.wikipedia.org/wiki/PostScript), [Adobe&rsquo;s&nbsp;PostScript Language Reference 3<small><sup>rd</sup></small>&nbsp;edition](https://www.adobe.com/jp/print/postscript/pdfs/PLRM.pdf)) is an an early-1980s reverse-Polish stack-based weakly-typed printer-control language. 
It is a delightful simple clean text-based user-alterable [Turing-complete](https://en.wikipedia.org/wiki/Turing_completeness) means of making [PDF](https://en.wikipedia.org/wiki/PDF)s. 
But, even in the most flattering light, PostScript shows its age.

[This author](https://jdawiseman.com/author.html) is fond of PostScript, and has done much work in it. 
(Indeed, I maintain and use a [splendid PostScript program](https://github.com/jdaw1/placemat/blob/main/README.md#readme) of a mere 15&frac23;k lines &mdash; perhaps it would be safest not to look.) 

PostScript is a natural and relatively simple way to generate PDFs from a low-level language such as&nbsp;C: it writes ASCII-text PostScript, which is then converted to PDF. 
That conversion, that &lsquo;distillation&rsquo;, requires an application, likely to be either [Adobe&nbsp;Distiller](https://en.wikipedia.org/wiki/Adobe_Distiller) or [Ghostscript](https://en.wikipedia.org/wiki/Ghostscript) or an online conversion service such as [ps2pdf.org](https://www.ps2pdf.org/convert.htm). 
For these purposes, any would be satisfactory (but Adobe is expensive).


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

The `360 mul 127 div` converts millimetres to the units of PostScript, DTP points. 
A2 is 594mm&#8239;&times;&#8239;420mm; A3 is 420mm&#8239;&times;&#8239;297mm; A4 is 297mm&#8239;&times;&#8239;210mm. 
For US&nbsp;8&frac12;&Prime;&#8239;&times;&#8239;11&Prime; use <code>/PageWidth&nbsp;8.5&nbsp;72&nbsp;mul&nbsp;def</code> and <code>/PageHeight&nbsp;11&nbsp;72&nbsp;mul&nbsp;def</code> (&because;&nbsp;72pt&nbsp;&equiv;&nbsp;1&Prime;), and likewise for other inch-specified page sizes.
(*NB*: [1&nbsp;inch is defined](https://en.wikipedia.org/wiki/International_yard_and_pound) to be exactly 25.4mm; [1&nbsp;DTP&nbsp;point is defined](https://en.wikipedia.org/wiki/Point_(typography)) to be 1&frasl;72 of an inch; &DoubleLongRightArrow; 360pt&nbsp;=&nbsp;127mm &Longleftrightarrow; 1mm&nbsp;=&nbsp;360&frasl;127pt.)


The `/Margin 18 def` says 18pt =&nbsp;18&frasl;72&nbsp;inch =&nbsp;&frac13;&Prime; &asymp;&nbsp;8.5mm. 
If not liked, change.

In the arcs file, `/LongestPathToBeFilled 215 def` is as explained in the [SVG&nbsp;documentation](svg.md).

The `ScaleFactor` is derived from values computed from the `ToPaint_XMin` etc, but can be overidden by the user.

The rhombus output specifies the routine `PaintByRhombus`, which takes parameters off the stack, and then paints the parameter-specified rhombus. 
It encodes conditions making the choice of which colour, and then a `setrgbcolor` or a `setgray`. 
The [author](https://jdawiseman.com/author.html) has strong shape perception but very weak colour perception, so you might well want to change the colours. 
Really, don&rsquo;t trust my colours: be not shy about recolouration.

The rhombus code also has multiple sections of commented-out code that were used in debugging, with nearby explanatory comments. 
If useful to you, play and experiment. 
(In PostScript comments begin with a `%`.) 

Is the `stroke` to be a single line, as in the second diagram on the [SVG&nbsp;page](svg.md), or outer-and-inner lines, as in that page&rsquo;s top diagram? 
To allow variations, `stroke`ing is done by the user-alterable routine `StrokeMulti`, which has a commented-out example.

Users might also want to find and change instances of `setlinewidth`. 
When making such changes, it might be simplest to replace the current &ldquo;`setlinewidth`&rdquo; with something of the form &ldquo;<code>2&nbsp;mul&nbsp;setlinewidth</code>&rdquo;.


## Kitchen ##

The files 
[Kitchen_Rh_08.**ps**](../postscript_pdf/Kitchen_Rh_08.ps) and 
[Kitchen_Rh_08.**pdf**](../postscript_pdf/Kitchen_Rh_08.pdf) show the map of my kitchen.

Obviously, for the particulars of your project, changes are needed.

Start by possibly changing `mmTileEdge`, which defaults to `152.5`, being 150mm of tile + 2.5mm grout. 

Next, in the PostScript file are two instances of the string &ldquo;Where on the map&rdquo;. 
Go to second instance, which is almost at the end of the file. 
Just before this is are routines `KitchenRoomPath` and `KitchenFurniturePath`, specifying their shape in millimetres. 
Adjust this template to your details. 
But where on the map is your kitchen to be: which part of the tile pattern do you want? 
Change the parameters of the `translate` near &ldquo;Where on the map&rdquo;. 

Back to the first instance of the string, at about line 47 of the file. 
This chooses what is to be shown on the page: the `translate` choosing where, and the `scale` filling the page. 
Simplest is to understate the scale (maybe `2`), by trial-and-error set the `translate` approximately correct, then increase the zoom and re-improve the `translate`. 

If the PDF is to be published online, perhaps lessen the file size by shrinking the bounding box controlled by `ToPaint_XMin`, `ToPaint_XMax`, `ToPaint_YMin`, and `ToPaint_YMax`.

Indeed, for those tiling a non-huge space, this PostScript or its SVG equivalents, appropriately amended, might be all that is needed from this repo.
