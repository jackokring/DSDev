DSDev (Technical)
=================

A bit of a hack. Music from Aminet by Neurodancer. Graphics from various homebrew examples from devkitPro plus some own design. Code from examples with a refactor and some own homebrew. Built to compile on Linux (OK), and maybe Windows.

Basic Design Things
-------------------

* Bottom display has console and keyboard.
* Top display uses 2D and 3D GL.
* Font resources use a modified ASCII (upper case) and:
  * Sword - output producer, fast (A)
  * Shield - input consumer, slow (B)
  * Chest - output producer, slow (Y)
  * Binoculars - input consumer, fast (X)
  * Heart - replaces backslash (life)
  * End - replaces caret for easy name select end on high score
  * Various other glyphs - box art, arrows, musical notes, sex, angled quotes ...
* Automated music scheduling
* Distinct texture palettes
* Digit containment in prints
