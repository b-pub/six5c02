A virtual machine - bbmachine

This directory contains a small fictitious emulated machine
simply called "bbmachine". As of this writing, bbmachine has a
memory-mapped console output (no input) which the helloworld
program writes to. It shows how writing to special RAM addresses
can be caught and redirected to I/O devices.

The bbmachine sources are built as part of the toplevel CMake
build process.

bbmachine source files:
* main.cpp
* console.h

There are also a number of assembler files in this directory that
define a sample helloworld program, but build a full 64KB binary
that includes the fictitious ROM containing the "OS" (print
functions).  The code is built with the CC65/ca65 toolset via
"make" in this directory. The assembly code in this directory is
not included as part of the top-level CMake build structure.

These files include:
* Makefile         GNU Makefile to build the .bin image
* bbmac.inc        General "bbmachine" definitions
* bbmachine.cfg    CC65 Linker configuration file
* helloworld.s     Assembly source for sample program
* zeropage.s       Definition of the ZP area
* zeropage.inc     exports of ZP
* rom.s            High-memory ROM functions for bbmachine
* rom.inc          exports of ROM

Enjoy the tinkering.

Brent Burton
Dec 2021
