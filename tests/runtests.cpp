/*
 * six5c02 -  emulated 6502 chip framework
 *
 * A simple machine to run test code against.
 *
 * The memory map include 48K RAM, 16K ROM, and there are no
 * memory mapped addresses.
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */
#include <stdio.h>
#include <string.h>
#include "six5c02.h"

#define KB64 0x10000


class Memory_48_16: public six5c02::IMemoryController
{
  protected:
    uint16_t ramStart, ramEnd, ramSize;
    uint16_t romStart, romEnd, romSize;

  public:
    Memory_48_16()
        : ramStart(0x0000)
        , ramEnd(0xBFFF)
        , ramSize(ramEnd - ramStart + 1)
        , romStart(0xC000)
        , romEnd(0xFFFF)
        , romSize(romEnd - romStart + 1)
    {}

    /**
     * Destroy this memory object.
     */
    virtual ~Memory_48_16() {}

    /**
     * Initialize the entire 64KB memory space with
     * a known value.
     */
    void init(uint8_t val)
    {
        ::memset(m_mem, val, sizeof(m_mem));
    }

    /**
     * Fill the memory space starting at \c start, for \c len
     * bytes, using \c buf as the source.
     */
    bool fillMem(uint32_t start, uint32_t len, uint8_t const* buf)
    {
        if ((start + len) > KB64)
        {
            printf("Invalid memory fill - too large!\n");
            return false;
        }
        if (!buf)
        {
            printf("Invalid memory fill - NULL source buffer\n");
            return false;
        }

        ::memcpy(&m_mem[start], buf, len);
        return true;
    }

    // ---- IMemoryController API:

    virtual uint8_t readMem(uint16_t address) const
    {
        // Check for specific address accesses, as needed.
        //printf("mr: %04x %02x\n", address, m_mem[address]);
        return m_mem[address];
    }

    virtual bool writeMem(uint16_t address, uint8_t value)
    {
        // Check for RAM access, then ROM access, then
        // specific address, such as for I/O.

        if (ramStart <= address && address <= ramEnd)
        {
            //printf("mW: %04x %02x\n", address, value);
            m_mem[address] = value;
            return true;
        }

        if (romStart <= address && address <= romEnd)
        {
            // Can't write to ROM. Error msg?
            //printf("ROM write: %04x\n", address);
            return false;
        }

        // No I/O memory map yet

        return false;
    }

};


class Looper: public six5c02::ILoopCallback
{
  public:
    Looper() {}
    virtual ~Looper() {}

    void callback(six5c02::CPU &cpu)
    {
        printRegisters(cpu);
    }
};


static bool loadTestBin(Memory_48_16 &mem, char const *filename)
{
    FILE *binf = fopen(filename, "rb");
    if (!binf)
    {
        printf("Couldn't open %s\n", filename);
        return false;
    }

    fseek(binf, 0,SEEK_END);                // goto EOF
    size_t filesize = ftell(binf);          // get size
    fseek(binf, 0,SEEK_SET);                // rewind

    if (filesize > KB64)
    {
        printf("BIN file is larger than 65536: %s %lu\n", filename, filesize);
        return false;
    }

    uint8_t buf[KB64];                     // max size is 64K
    size_t numread = fread(buf, 1, filesize, binf);
    fclose(binf);

    if (numread != filesize)
    {
        printf("Couldn't read entire file %s\n", filename);
        return false;
    }

    // Got all the data, now load it into the VM
    if (!mem.fillMem(0, filesize, buf))
    {
        printf("VM memory could not be set\n");
        return false;
    }
    return true;
}

void runTestBinary(char const *filename, uint16_t startAddr,
                   uint32_t clockLimit, uint32_t clockStep, uint16_t successAddr)
{
    six5c02::CPU cpu;
    Looper callback;
    Memory_48_16 memory;

    printf("\nTest: %s\n", filename);

    memory.init(0xEA); // fill with NOPs
    if (!loadTestBin(memory, filename))
    {
        return;
    }

    cpu.setMemoryController(&memory);
    if (cpu.reset())
    {
        printf("Test binary %s loaded OK\n", filename);
        printf("CPU reset OK\n");
        printf("Setting PC to $%04x\n", startAddr);
        cpu.setPC(startAddr);                  // start of this test program
        printRegisters(cpu);
        printf("ind1: 0x0024 = %02x %02x  should be abs1:, 0x0210\n",
               memory.readMem(0x0024), memory.readMem(0x0025));
        printf("abs1: 0x0210 = %02x %02x %02x %02x  should be: c3 82 41 0\n",
               memory.readMem(0x0210), memory.readMem(0x0211),
               memory.readMem(0x0212), memory.readMem(0x0213));

        // Run long enough for test to work (clockLimit)
        // and check PC for success address.
        while (cpu.getClockticks() < clockLimit)
        {
            uint16_t lastPC = cpu.PC();

            cpu.exec(clockStep);
            if (cpu.isStopped())
            {
                printRegisters(cpu);
                printf("STOPPED - code called STP ($db)\n");
                return;
            }

            if (cpu.PC() == successAddr)
            {
                printRegisters(cpu);
                printf("SUCCESS - %s test passed\n", filename);
                return;
            }

            // Errors in some tests are "trapped" in an infinite loop.
            //   E.g.:     errtrap: jmp errtrap
            // The PC remains the same in this situation.
            if (lastPC == cpu.PC())
            {
                printRegisters(cpu);
                printf("TRAPPED - test condition failure detected\n");
                return;
            }
        }
        printRegisters(cpu);
        printf("FAILED - %s reached clock limit\n", filename);
    }
    else
    {
        printf("CPU reset FAILED\n");
        return;
    }
}

int main()
{
#define MM 1000000u
    runTestBinary("functional_test.bin",     0x0400, 100*MM, 1000, 0x3469);
    runTestBinary("decimal_test.bin",        0x0400, 1*MM, 1000, 0x044b);
    runTestBinary("extended_65c02_test.bin", 0x0400, 100u*MM, 1000, 0x18f8);
    return 0;
}
