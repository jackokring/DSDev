DSDev (Technical)
=================

A bit of a hack. Music from Aminet by Neurodancer. Graphics from various homebrew examples from devkitPro plus some own design. Code from examples with a refactor and some own homebrew. Built to compile on Linux (OK), and maybe Windows.

Basic Design Things
-------------------

* Bottom display has console and keyboard
* Top display uses 2D and 3D GL. Allocated 2 * 64kB (256 colour) textures
* Font resources use a modified ASCII (upper case)
  * Sword - output producer, fast (A)
  * Shield - input consumer, slow (B)
  * Chest - output producer, slow (Y)
  * Binoculars - input consumer, fast (X)
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

Some sort of simple programming could be part of in game play.

