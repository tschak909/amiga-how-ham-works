# amiga-how-ham-works

This is demo code, intended to be compiled with AMIGA Lattice C 3.03, showing how the HAM mode works.

!(Screenshot)[screenshot.png]

It does the following:

* Draws the 16 base colors
* Draws the three modify states, 16 steps each
* re-Draws the 16 base colors at the beginning of each step to show how color palette reacts to changes of the base colors at different positions.
* Changes the color palette of the first few base colors to show how changes in the color palette also affect how the held colors react dynamically.

It can be built using the MakeSimple script that is a part of Lattice C 3.03.

## Author

Thom Cherryhomes <thom dot cherryhomes at gmail dot com>

