A virtual machine - bbmachine

This directory contains a small fictitious emulated
machine simply called "bbmachine". As of this writing,
bbmachine has a memory-mapped console output (no input)
which the helloworld program writes to.

bbmachine source files:
* main.cpp
* console.h

There are a number of assembler files in this directory.
It's a mess. The one that works is helloworld.vasm, which
is a one-file source formatted for the VASM 6052 assembler.
It's trivially built with
 $ vasm6502 -wdc02 -Fbin -dotdir -L helloworld.lst \
   -o helloworld.bin helloworld.vasm

The other files will be the eventual cleaned-up code that I would
like to have separate files for macros, bbmachine system
definitions, and of course the corresponding helloworld source,
to be built with the CC65/ca65 toolset, not vasm.
These eventual files include
* bbmac.inc
* bbmachine.cfg    Memory configuration file for CC65 tools
* build.sh         Script to build w/ca65
* helloworld.s     Assembler source for CC65
* rom.s            High-memory ROM functions for bbmachine
