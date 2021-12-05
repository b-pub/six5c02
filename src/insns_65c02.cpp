/*
 * six5c02 -  emulated 6502 chip framework
 *
 * 65c02 instruction implementations
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */
#include "six5c02.h"
#include "support.h"

namespace six5c02 {

/*
 * 65c02 additional functions.
 *
 * Author: Paul Robson (paul@robson.org.uk)
 */

void CPU::stz()
{
    putvalue(0);
}

void CPU::bra()
{
    oldpc = pc;
    pc += reladdr;
    if ((oldpc & 0xFF00) != (pc & 0xFF00))
        clockticks += 2; //check if jump crossed a page boundary
    else
        clockticks++;
}


void CPU::phx()
{
    push8(x);
}

void CPU::plx()
{
    x = pull8();
   
    zerocalc(x);
    signcalc(x);
}

void CPU::phy()
{
    push8(y);
}

void CPU::ply()
{
    y = pull8();
  
    zerocalc(y);
    signcalc(y);
}

void CPU::tsb()
{
    value = getvalue();                  // Read memory
    result = (uint16_t)a & value;        // calculate A & memory
    zerocalc(result);                    // Set Z flag from this.
    result = value | a;                  // Write back value read, A bits are set.
    putvalue(result);
}

void CPU::trb()
{
    value = getvalue();                  // Read memory
    result = (uint16_t)a & value;        // calculate A & memory
    zerocalc(result);                    // Set Z flag from this.
    result = value & (a ^ 0xFF); 	     // Write back value read, A bits are clear.
    putvalue(result);
}

void CPU::stp() // formerly dbg()
{
    // Invoke debugger. Or stop.
    stopped = true;
}

void CPU::wai()                             // wait for interrupt
{
	if (~status & FLAG_INTERRUPT) waiting = 1;
}

void CPU::bbr(uint16_t bitmask)
{
	if ((getvalue() & bitmask) == 0)
    {
		oldpc = pc;
		pc += reladdr;
		if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
		else
            clockticks++;
	}
}

void CPU::bbr0() { bbr(0x01); }
void CPU::bbr1() { bbr(0x02); }
void CPU::bbr2() { bbr(0x04); }
void CPU::bbr3() { bbr(0x08); }
void CPU::bbr4() { bbr(0x10); }
void CPU::bbr5() { bbr(0x20); }
void CPU::bbr6() { bbr(0x40); }
void CPU::bbr7() { bbr(0x80); }

void CPU::bbs(uint16_t bitmask)
{
	if ((getvalue() & bitmask) != 0)
    {
		oldpc = pc;
		pc += reladdr;
		if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
		else
            clockticks++;
	}
}

void CPU::bbs0() { bbs(0x01); }
void CPU::bbs1() { bbs(0x02); }
void CPU::bbs2() { bbs(0x04); }
void CPU::bbs3() { bbs(0x08); }
void CPU::bbs4() { bbs(0x10); }
void CPU::bbs5() { bbs(0x20); }
void CPU::bbs6() { bbs(0x40); }
void CPU::bbs7() { bbs(0x80); }

void CPU::smb0() { putvalue(getvalue() | 0x01); }
void CPU::smb1() { putvalue(getvalue() | 0x02); }
void CPU::smb2() { putvalue(getvalue() | 0x04); }
void CPU::smb3() { putvalue(getvalue() | 0x08); }
void CPU::smb4() { putvalue(getvalue() | 0x10); }
void CPU::smb5() { putvalue(getvalue() | 0x20); }
void CPU::smb6() { putvalue(getvalue() | 0x40); }
void CPU::smb7() { putvalue(getvalue() | 0x80); }

void CPU::rmb0() { putvalue(getvalue() & ~0x01); }
void CPU::rmb1() { putvalue(getvalue() & ~0x02); }
void CPU::rmb2() { putvalue(getvalue() & ~0x04); }
void CPU::rmb3() { putvalue(getvalue() & ~0x08); }
void CPU::rmb4() { putvalue(getvalue() & ~0x10); }
void CPU::rmb5() { putvalue(getvalue() & ~0x20); }
void CPU::rmb6() { putvalue(getvalue() & ~0x40); }
void CPU::rmb7() { putvalue(getvalue() & ~0x80); }

} // ::six5c02
