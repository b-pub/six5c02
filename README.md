# six5c02
A 65c02-based virtual machine framework in C++

## Description

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

On a Raspberry Pi 4 at 1.5GHz, the emulated 65c02 core runs at
about 65MHz. Just for theoretical comparison, if the Pi ran at
3GHz like the mac, the emulator would be at about 130MHz, so the
Core i5 has a good speed advantage.

## Building it

This project uses CMake, at least version 3.10. It's easy to build,
and I build it on Linux and MacOS with:

	$ <git clone the repo, cd into repo>
	$ mkdir build
	$ cd build
	$ cmake ..
	$ make -j

This builds in a couple seconds, and produces two executable virtual
machines, named "runtests" and "bbmachine".

These next steps are manual for the time being. Sorry.

Runtests needs test bins to run. Do this to create them:

	$ <still in ..../build from above>
	$ pushd ../tests
	$ ./build.sh
	$ cp *.bin ../build
	$ popd

Similarly, the bbmachine bin needs to be created:

	$ <still in ..../build from above>
	$ pushd ../bbmachine
	$ make
	$ cp *.bin ../build
	$ popd

Back in the build directory, each can be run like:

	$ ./runtests
	$ ./bbmachine helloworld.bin

Runtests looks for and loads a few test .bins that it knows
about, and runs each. Successful test runs end with "STOPPED"
since those tests currently don't have any other way to signal
success or failure short of looping. The "extended_65c02" test is
not ready and so that test run will fail.

The bbmachine runs a hello world program, using an internal print
function to a memory-mapped output console, displaying in the
current shell. It also has STOPPED at the end.


# Due credit

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

