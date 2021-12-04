#include <stdio.h>
#include "six5c02.h"

void printRegisters(six5c02::CPU &cpu)
{
    printf("PC   SP A  X  Y  P: NV1BDIZC  Cycles\n");
    printf("%04x %02x %02x %02x %02x %02x %u%u1%u%u%u%u%u  %u\n",
           cpu.PC(), cpu.SP(), cpu.A(), cpu.X(), cpu.Y(), cpu.P(),
           cpu.N(), cpu.V(), cpu.B(), cpu.D(), cpu.I(), cpu.Z(), cpu.C(),
           cpu.getClockticks());
}

void dumpMem(six5c02::IMemoryController &memory, uint16_t start, uint16_t len)
{
    printf("+---- 0x%04x - 0x%04x\n", start, start+len-1);
    uint16_t skip = start % 16;
    uint16_t addr = start;
    uint16_t end = start+len;
    int bytes = 0;                          // number of bytes printed on this line
    for(;;)                                 // for each line printed...
    {
        printf("%06x:", addr-skip);
        if (skip) {                         // skip bytes on first line only
            for (;skip > 0; skip--)
            {
                printf(" --");
                bytes++;
            }
            skip = 0;
        }
        for ( ; bytes < 16 && addr < end; bytes++,addr++)
        {
            printf(" %02x", memory.readMem(addr));
        }
        printf("\n");
        if (addr >= end)
            break;                          // no more bytes to print

        bytes = 0;
    }
    printf("+----\n");
}

