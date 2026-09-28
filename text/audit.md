# Penrose Tilings: Audits #

## Contents ##

Documentation pages:&nbsp; 
&bull;&nbsp;[Introduction](introduction.md);&nbsp; 
&bull;&nbsp;[Paths](paths.md);&nbsp; 
&bull;&nbsp;[C&nbsp;code](c.md);&nbsp; 
&bull;&nbsp;[JSON&nbsp;output](json.md);&nbsp; 
&bull;&nbsp;[TSV&nbsp;output&nbsp;and&nbsp;a&nbsp;spreadsheet](tsv.md);&nbsp; 
&bull;&nbsp;[SVG&nbsp;output](svg.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(distillable)](postscript_distillable.md);&nbsp; 
&bull;&nbsp;[PostScript&nbsp;output&nbsp;(data)](postscript_data.md);&nbsp; 
&bull;&nbsp;[Physical&nbsp;tiling](physical_tiling.md).


## Audits ##

Occasionally, I might ask for a safety audit. 
Of course, you should not trust my reports of safety audits &mdash; you cannot know whether I am honest &mdash; so you should commission your own.


## Gemini, 26 Sep 2026 ##

On Saturday 26<small><sup>th</sup></small> September 2026 online free Gemini was asked &ldquo;Does it, to you, seem safe?&rdquo;. 
There was stuff about access, sorted by uploading the 37 files in four batches. 
It offered speed improvements to insideness testing, at cost of memory or complication. 
Re the question, it summarised.

> ## Safety & Code Security Summary ##

> Having thoroughly analyzed the code base for this Penrose tiling engine, **the implementation is safe.** 
There are **no malicious backdoor vectors, external network dependencies, hidden logic bombs, or obfuscated payloads.**

> Here is a summary of the software's safety and architectural characteristics:

> * **Purely Mathematical Operation:** The code operates entirely on localized geometric and trigonometric coordinate trees. It processes pure mathematical datasets to construct spatial patterns, map closed loops, and format structured exports.

> * **Safe `system()` Invocations:** While the program contains two calls to the low-level `system()` command (invoking Google Chrome for SVG viewing and ps2pdf for GhostScript compilation), they are secure. The command string buffers are built using trusted, internally generated timestamps and structural variables, eliminating standard command injection attack vectors. 

> * **Proactive Error Isolation:** Key operations (like neighborhood population) implement active assert-and-crash triggers. If indices break standard constraints, the system dumps a debug state map to disk and terminates safely via `exit(EXIT_FAILURE)`, preventing cascading memory corruption.

> * **Robust Resource Management:** Across all modules, persistent arrays and temporary coordinate maps generated on the heap explicitly verify allocation pointers against `NULL` states before usage, shielding the runtime environment from segmentation faults.

> The application code is structurally sound and completely safe to compile and execute.

But check for yourself: do not trust my report of a claim of safety, get your own report.
