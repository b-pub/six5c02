#include "six5c02.h"

namespace six5c02 {


void CPU::imp() { // implied
}

void CPU::acc() { // accumulator
}

void CPU::imm() { // immediate
    ea = pc++;
}

void CPU::zp() { // zero-page
    ea = (uint16_t)read6502((uint16_t)pc++);
}

void CPU::zpx() { // zero-page,X
    // with zero-page wraparound
    ea = ((uint16_t)read6502((uint16_t)pc++) + (uint16_t)x) & 0xFF;
}

void CPU::zpy() { // zero-page,Y
    // with zero-page wraparound
    ea = ((uint16_t)read6502((uint16_t)pc++) + (uint16_t)y) & 0xFF;
}

void CPU::rel() { // relative for branch ops (8-bit immediate value, sign-extended)
    reladdr = (uint16_t)read6502(pc++);
    if (reladdr & 0x80) reladdr |= 0xFF00;
}

void CPU::abso() { // absolute
    ea = (uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8);
    pc += 2;
}

void CPU::absx() { // absolute,X
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

void CPU::absy() { //absolute,Y
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

void CPU::ind() { // indirect
    uint16_t eahelp, eahelp2;
    eahelp = (uint16_t)read6502(pc) | (uint16_t)((uint16_t)read6502(pc+1) << 8);

#if 0  // Set 1 to replicate 6502 page-boundary wraparound bug:
    eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF);
#else
    eahelp2 = (eahelp+1) & 0xFFFF;          // Correct 65c02 math
#endif

    ea = (uint16_t)read6502(eahelp) | ((uint16_t)read6502(eahelp2) << 8);
    pc += 2;
}

void CPU::indx() { // (indirect,X)
    uint16_t eahelp;
    // zero-page wraparound for table pointer
    eahelp = (uint16_t)(((uint16_t)read6502(pc++) + (uint16_t)x) & 0xFF);
    ea = (uint16_t)read6502(eahelp & 0x00FF) | ((uint16_t)read6502((eahelp+1) & 0x00FF) << 8);
}

void CPU::indy() { // (indirect),Y
    uint16_t eahelp, eahelp2, startpage;
    eahelp = (uint16_t)read6502(pc++);
    eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); // zero-page wraparound
    ea = (uint16_t)read6502(eahelp) | ((uint16_t)read6502(eahelp2) << 8);
    startpage = ea & 0xFF00;
    ea += (uint16_t)y;

    if (startpage != (ea & 0xFF00))
    {
        // one cycle penalty for page-crossing on some opcodes
        penaltyaddr = 1;
    }
}

void CPU::zprel() { // zero-page, relative for branch ops (8-bit immediate value, sign-extended)
	ea = (uint16_t)read6502(pc);
	reladdr = (uint16_t)read6502(pc+1);
	if (reladdr & 0x80) reladdr |= 0xFF00;

	pc += 2;
}

} // ns six5c02
