# six5c02
A 65c02-based virtual machine framework in C++

## What is this?

This project is an environment to aid experimentation of computer
system design and learn how to do some basic things in computers,
like memory-mapped I/O. It is based on the classic 6502/65c02
microprocessor, which is small and simple enough for one to have
a complete understanding, as well as having broad support in
development tools.

A big part of this project was reworking old C code into a more
flexible C++ architecture, plus making virtual system components out
of a number of classes that are easily interchanged.

For example, a memory model of 16K RAM and 8K ROM can be easily
constructed by providing a new IMemoryController implementation.

## Performance notes

I have not tested performance extensively, and what I have tested
has been limited to various functional tests for the 6502 that
have been published across the net.

Running the default -O3 optimized build on a 3GHz Intel Core i5
(macOS 10.14), the emulator runs the 65c02 core at an effective
clock frequency of about 180MHz. That's /a bit/ faster than a
Commodore PET 2001.

## Due credit

This emulation playground would not be possible without a few
particular projects.

1. First of all is the fantastic Commander X16 project to create
a new old computer in the vein of 80's classics. Michael Steil
created the emulator, which utilizes the fake6502 core emulator.
Paul Robson added 65c02 instructions and split the fake6502 sources.
His code was the basis for my conversion to C++.

See their project repo and main site at:
* https://github.com/commanderx16
* https://www.commanderx16.com/

2. The fake6502 C emulator, written by Mike Chambers, available
at http://rubbermallet.org/fake6502.c .
This is a public-domain 6502 emulator.

3. The VASM assembler. This is a good assembler for making quick
full-64k memory images of absolute code. http://sun.hasenbraten.de/vasm/

4. The CC65 toolchain, especially ca65, the assembler. See
https://github.com/cc65/cc65
