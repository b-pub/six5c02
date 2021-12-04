#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "six5c02.h"
#include "console.h"

#define KB64 0x10000

/*
 * This emulated machine looks like this:
 *  * 48K of RAM, from 0x0000-0xBFFF
 *  * 16K of ROM, from 0xC000-0xFFFF
 *
 * RAM:
 *  * 0x00-0xff: normal zero page
 *  * 0x100-0x1ff: normal stack space
 *  * 0x200-0x3ff: program's use
 *  * 0x400: main entry point
 *  * 0x0400-0xBEFF: program's use
 *  * 0xBF00-BFFF: 256 addresses for I/O
 *
 * ROM:
 *  * 0xC000: start of ROM routines, TBD.
 *  * 0xFFFA: IRQ, RESET, and BRK vectors
 *
 * Programs must have an entry point at 0x0400. They are free to
 * use the zero page / direct page as needed, and the stack as
 * normal (0x100-0x1ff), plus the RAM between 0x200-3ff. The
 * reset vector starts executing at 0x0400.
 *
 * Memory-mapped I/O devices are 0xBF00-0xBFFF.
 *  * 0xBF00 - console control port
 *  * 0xBF01 - console data 1
 *  * 0xBF02 - console data 2
 *  * 0xBF03 - console data 3
 */

class Memory4816: public six5c02::IMemoryController
{
  protected:
    uint16_t ramStart, ramEnd, ramSize;
    uint16_t romStart, romEnd, romSize;

    Console console;

  public:
    Memory4816()
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
    virtual ~Memory4816() {}

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
        // Check for I/O, then RAM, then ROM addresses.

        // Console Device
        if (CONS_BEGIN <= address && address <= CONS_END)
        {
            m_mem[address] = value;
            console.write(*this, address);
            return true;
        }

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


static bool loadProgramBin(Memory4816 &mem, char const *filename)
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

void runBinary(char const *filename)
{
    six5c02::CPU cpu;
    Looper callback;
    Memory4816 memory;
    const uint16_t startAddr = 0x0400;

    memory.init(0xEA); // fill with NOPs
    if (!loadProgramBin(memory, filename))
    {
        return;
    }

    cpu.setMemoryController(&memory);
    //cpu.setLoopCallback(&callback);
    if (cpu.reset())
    {
        printf("Program binary %s loaded OK\n", filename);
        printf("CPU reset OK\n");
        printf("Setting PC to $%04x\n", startAddr);
        cpu.setPC(startAddr);                  // start of this test program

        // Run program until STP is issued.
        while (true)
        {
            uint16_t lastPC = cpu.PC();

            //cpu.exec(1000);
            cpu.step();
            if (cpu.isStopped())
            {
                printRegisters(cpu);
                printf("STOPPED - program called STP ($db)\n");
                return;
            }
        }
    }
    else
    {
        printf("CPU reset FAILED\n");
        return;
    }
}

static void usage(const char *name)
{
    fprintf(stderr, "usage: bbmachine prog6502.bin\n");
    fprintf(stderr, "No other help available.\n");
    exit(1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        usage(argv[0]);

    const char *filename = argv[1];

#define MM 1000000u
    runBinary(filename);
    return 0;
}
