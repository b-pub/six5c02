/*
 * six5c02 -  emulated 6502 chip framework
 *
 * addressing mode methods
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */
#include "six5c02.h"

namespace six5c02 {


void CPU::abso() // 4.1 absolute: a
{
    ea = (uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8);
    pc += 2;
}

// 65c02:
void CPU::ainx() // 4.2 Absolute indexed indirect for JMP: (a,x)
{
    uint16_t ia;                            // temp indirect address
    ia = (uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8);
    ia += (uint16_t)x;
    ea = (uint16_t)read6502(ia) | ((uint16_t)read6502(ia+1) << 8);
    pc += 2;
}

void CPU::absx() // 4.3 absolute indexed with X: a,x
{
    uint16_t startpage;
    ea = ((uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8));
    startpage = ea & 0xFF00;
    ea += (uint16_t)x;

    if (startpage != (ea & 0xFF00))
    {
        // one cycle penalty for page-crossing on some opcodes
        penaltyaddr = 1;
    }

    pc += 2;
}

void CPU::absy() // 4.4 absolute indexed with Y: a,y
{
    uint16_t startpage;
    ea = ((uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8));
    startpage = ea & 0xFF00;
    ea += (uint16_t)y;

    if (startpage != (ea & 0xFF00))
    {
        // one cycle penalty for page-crossing on some opcodes
        penaltyaddr = 1;
    }

    pc += 2;
}

void CPU::ind() // 4.5 Absolute indirect for JMP: (a)
{
    uint16_t ia;                            // temp indirect address
    // 6502 only: if low byte of 'a' is $FF, result is incorrect.
    ia = (uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8);
    ea = (uint16_t)read6502(ia) | ((uint16_t)read6502(ia+1) << 8);
    pc += 2;
}

void CPU::acc() // 4.6 accumulator
{
    // nothing to do for this mode
}

void CPU::imm() // 4.7 immediate: #
{
    ea = pc++;
}

void CPU::imp() // 4.8 implied
{
    // nothing to do for this mode
}

void CPU::rel() // 4.9 PC relative: r
{
    // Used by branch ops (8-bit immediate value, sign-extended)
    reladdr = (uint16_t)read6502(pc++);
    if (reladdr & 0x80) reladdr |= 0xFF00;
}

// 4.10 Stack S - stack ops are implied (ph*, pl*, rti, rts)

void CPU::zp() // 4.11 zero page: zp
{
    ea = read6502(pc++);
}

void CPU::indx() // 4.12 zero page indexed indirect: (zp,X)
{
    uint16_t ia = (read6502(pc++) + x) & 0xFF;
    ea = (uint16_t)read6502(ia) | ((uint16_t)read6502(ia+1) << 8);
}

void CPU::zpx() // 4.13 zero page indexed with X: zp,x
{
    // Limit to 8b address of zero page
    ea = (read6502(pc++) + x) & 0xFF;
}

void CPU::zpy() // 4.14 zero page indexed with Y: zp,y
{
    // Limit to 8b address of zero page
    ea = (read6502(pc++) + y) & 0xFF;
}

// 65c02:
void CPU::ind0() // 4.15 zero page indirect: (zp)
{
    uint16_t ia;
    ia = (uint16_t)read6502(pc++);
    // handle zero-page wraparound for second byte:  (ia+1) & 0xFF
    ea = (uint16_t)read6502(ia) | ((uint16_t)read6502((ia+1) & 0xFF) << 8);
}

void CPU::indy() // 4.16 zero page indirect indexed with Y: (zp),Y
{
    uint16_t ia;
    ia = read6502(pc++);
    // handle zero-page wraparound for second byte:  (ia+1) & 0xFF
    ea = (uint16_t)read6502(ia) | ((uint16_t)read6502((ia+1) & 0xFF) << 8);
    uint16_t startpage = ea & 0xFF00;
    ea += (uint16_t)y;

    if (startpage != (ea & 0xFF00))
    {
        // one cycle penalty for page-crossing on some opcodes
        penaltyaddr = 1;
    }
}

/*
 * zprel() implements the combination of the zp,rel addressing
 * modes that was introduced with the BBR* and BBS* Rockwell
 * instructions. This mode is not in the WDC W65C02S datasheet,
 * but the BBR,BBS instructions are.
 */
void CPU::zprel() // zero page relative; zp,rel
{
	ea = (uint16_t)read6502(pc);            // zp
	reladdr = (uint16_t)read6502(pc+1);     // r = sign-extended 8-bit immediate value
	if (reladdr & 0x80) reladdr |= 0xFF00;
	pc += 2;
}

} // ::six5c02
