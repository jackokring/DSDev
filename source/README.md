DSDev (Technical)
=================

A bit of a hack. Music from Aminet by Neurodancer. Graphics from various homebrew examples from devkitPro plus some own design. Code from examples with a refactor and some own homebrew. Built to compile on Linux (OK), and maybe Windows.

Basic Design Things
-------------------

* Bottom display has console and keyboard
* Top display uses 2D and 3D GL. Allocated 2 * 64kB (256 colour) textures plus 2 3D fonts
* Font resources use a modified ASCII (upper case)
  * Sword - output kenetic, fast (A)
  * Shield - input potential, slow (B)
  * Chest - output potential, slow (Y)
  * Binoculars - input kinetic, fast (X)
  * Heart - replaces backslash (life)
  * End - replaces caret for easy name select end on high score
  * Various other glyphs - box art, arrows, angled quotes ...
* Automated music scheduling
* Distinct texture palettes (VRAM_E)
* Digit containment in prints

An Arcade IDE
-------------

A simple text based IDE for building on device. Using the console as a code entry device for compiling a domain specific language. All navigation and data entry should be possible with the pad and buttons. Various screens (windows) should show different views on the IDE and should be interlinked for switching view, as well as browsing via SELECT.

START should of course run the active entry item, and also stop or pause code running. The other buttons could roughly be defined as A for add selected or insert, B for delete last or backspace, X for building complex views based on current item such as references, and finally Y should change between the insert type for numbers, strings, lists and words.

Some sort of simple programming could be part of in game play. To this end keys can be intercepted by the console on the sub display.

VRAM
----

Main
* BG0/3D - A, B (texture gfx, using F texture palette)
* BG2, BG3 - E (shared gfx and seperate maps using G extended palette)

Sub
* BG0 - C (ANSI minimal console) 
* BG1 - C (shared gfx and seperate maps, using H extended palette)
* BG2 - (as above)
* BG3 - C (inbuilt keyboard)
* Sprites - D, (using I extended palette)

