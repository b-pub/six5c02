/*
 * six5c02 -  emulated 6502 chip framework
 *
 * 6502 instruction implementations
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */
#include "six5c02.h"
#include "support.h"

namespace six5c02 {

/* A note about 65C02 changes:
 *  BRK      now clears D
 *  ADC/SBC  set N and Z in decimal mode. They also set V, but this is
 *           essentially meaningless so this has not been implemented.
 */
void CPU::adc()
{
    penaltyop = 1;

    if (status & FLAG_DECIMAL)
    {
        uint16_t tmp, tmp2;
        value = getvalue();
        tmp = ((uint16_t)a & 0x0F) + (value & 0x0F) + (uint16_t)(status & FLAG_CARRY);
        tmp2 = ((uint16_t)a & 0xF0) + (value & 0xF0);
        if (tmp > 0x09)
        {
            tmp2 += 0x10;
            tmp += 0x06;
        }
        if (tmp2 > 0x90)
        {
            tmp2 += 0x60;
        }
        if (tmp2 & 0xFF00)
        {
            setcarry();
        } else {
            clearcarry();
        }
        result = (tmp & 0x0F) | (tmp2 & 0xF0);

        zerocalc(result);                /* 65C02 change, Decimal Arithmetic sets NZV */
        signcalc(result);

        clockticks++;
    }
    else
    {
        value = getvalue();
        result = (uint16_t)a + value + (uint16_t)(status & FLAG_CARRY);

        carrycalc(result);
        zerocalc(result);
        overflowcalc(result, a, value);
        signcalc(result);
    }

    saveaccum(result);
}

void CPU::And()
{
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a & value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

void CPU::asl()
{
    value = getvalue();
    result = value << 1;

    carrycalc(result);
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::bcc()
{
    if ((status & FLAG_CARRY) == 0)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::bcs()
{
    if ((status & FLAG_CARRY) == FLAG_CARRY)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::beq()
{
    if ((status & FLAG_ZERO) == FLAG_ZERO)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::bit()
{
    value = getvalue();
    result = (uint16_t)a & value;

    zerocalc(result);
    status = (status & 0x3F) | (uint8_t)(value & 0xC0);
}

void CPU::bmi()
{
    if ((status & FLAG_SIGN) == FLAG_SIGN)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::bne()
{
    if ((status & FLAG_ZERO) == 0)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::bpl()
{
    if ((status & FLAG_SIGN) == 0)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::brk()
{
    pc++;


    push16(pc);                    // push next instruction address onto stack
    push8(status | FLAG_BREAK);    // push CPU status to stack
    setinterrupt();                // set interrupt flag
    cleardecimal();                // clear decimal flag (65C02 change)
    pc = (uint16_t)read6502(0xFFFE) | ((uint16_t)read6502(0xFFFF) << 8);
}

void CPU::bvc()
{
    if ((status & FLAG_OVERFLOW) == 0)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::bvs()
{
    if ((status & FLAG_OVERFLOW) == FLAG_OVERFLOW)
    {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00))
            clockticks += 2; //check if jump crossed a page boundary
        else
            clockticks++;
    }
}

void CPU::clc()
{
    clearcarry();
}

void CPU::cld()
{
    cleardecimal();
}

void CPU::cli()
{
    clearinterrupt();
}

void CPU::clv()
{
    clearoverflow();
}

void CPU::cmp()
{
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a - value;

    if (a >= (uint8_t)(value & 0x00FF))
        setcarry();
    else
        clearcarry();

    if (a == (uint8_t)(value & 0x00FF))
        setzero();
    else
        clearzero();

    signcalc(result);
}

void CPU::cpx()
{
    value = getvalue();
    result = (uint16_t)x - value;

    if (x >= (uint8_t)(value & 0x00FF))
        setcarry();
    else
        clearcarry();

    if (x == (uint8_t)(value & 0x00FF))
        setzero();
    else
        clearzero();

    signcalc(result);
}

void CPU::cpy()
{
    value = getvalue();
    result = (uint16_t)y - value;

    if (y >= (uint8_t)(value & 0x00FF))
        setcarry();
    else
        clearcarry();

    if (y == (uint8_t)(value & 0x00FF))
        setzero();
    else
        clearzero();

    signcalc(result);
}

void CPU::dec()
{
    value = getvalue();
    result = value - 1;

    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::dex()
{
    x--;

    zerocalc(x);
    signcalc(x);
}

void CPU::dey()
{
    y--;

    zerocalc(y);
    signcalc(y);
}

void CPU::eor()
{
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a ^ value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

void CPU::ill()
{
    /* This method is a placeholder to detect illegal opcodes and
     * is never executed.
     */
}

void CPU::inc()
{
    value = getvalue();
    result = value + 1;

    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::inx()
{
    x++;

    zerocalc(x);
    signcalc(x);
}

void CPU::iny()
{
    y++;

    zerocalc(y);
    signcalc(y);
}

void CPU::jmp()
{
    pc = ea;
}

void CPU::jsr()
{
    push16(pc - 1);
    pc = ea;
}

void CPU::lda()
{
    penaltyop = 1;
    value = getvalue();
    a = (uint8_t)(value & 0x00FF);

    zerocalc(a);
    signcalc(a);
}

void CPU::ldx()
{
    penaltyop = 1;
    value = getvalue();
    x = (uint8_t)(value & 0x00FF);

    zerocalc(x);
    signcalc(x);
}

void CPU::ldy()
{
    penaltyop = 1;
    value = getvalue();
    y = (uint8_t)(value & 0x00FF);

    zerocalc(y);
    signcalc(y);
}

void CPU::lsr()
{
    value = getvalue();
    result = value >> 1;

    if (value & 1)
        setcarry();
    else
        clearcarry();
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::nop()
{
    switch (opcode)
    {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            penaltyop = 1;
            break;
    }
}

void CPU::ora()
{
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a | value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

void CPU::pha()
{
    push8(a);
}

void CPU::php()
{
    push8(status | FLAG_BREAK);
}

void CPU::pla()
{
    a = pull8();

    zerocalc(a);
    signcalc(a);
}

void CPU::plp()
{
    status = pull8() | FLAG_CONSTANT;
}

void CPU::rol()
{
    value = getvalue();
    result = (value << 1) | (status & FLAG_CARRY);

    carrycalc(result);
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::ror()
{
    value = getvalue();
    result = (value >> 1) | ((status & FLAG_CARRY) << 7);

    if (value & 1) setcarry();
        else clearcarry();
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

void CPU::rti()
{
    status = pull8();
    value = pull16();
    pc = value;
}

void CPU::rts()
{
    value = pull16();
    pc = value + 1;
}

void CPU::sbc()
{
    penaltyop = 1;

    if (status & FLAG_DECIMAL)
    {
        value = getvalue();
        result = (uint16_t)a - (value & 0x0f) + (status & FLAG_CARRY) - 1;
        if ((result & 0x0f) > (a & 0x0f))
        {
            result -= 6;
        }
        result -= (value & 0xf0);
        if ((result & 0xfff0) > ((uint16_t)a & 0xf0))
        {
            result -= 0x60;
        }
        if (result <= (uint16_t)a)
        {
            setcarry();
        } else {
            clearcarry();
        }

        zerocalc(result);                /* 65C02 change, Decimal Arithmetic sets NZV */
        signcalc(result);

        clockticks++;
    } else {
        value = getvalue() ^ 0x00FF;
        result = (uint16_t)a + value + (uint16_t)(status & FLAG_CARRY);

        carrycalc(result);
        zerocalc(result);
        overflowcalc(result, a, value);
        signcalc(result);
    }

    saveaccum(result);
}

void CPU::sec()
{
    setcarry();
}

void CPU::sed()
{
    setdecimal();
}

void CPU::sei()
{
    setinterrupt();
}

void CPU::sta()
{
    putvalue(a);
}

void CPU::stx()
{
    putvalue(x);
}

void CPU::sty()
{
    putvalue(y);
}

void CPU::tax()
{
    x = a;

    zerocalc(x);
    signcalc(x);
}

void CPU::tay()
{
    y = a;

    zerocalc(y);
    signcalc(y);
}

void CPU::tsx()
{
    x = sp;

    zerocalc(x);
    signcalc(x);
}

void CPU::txa()
{
    a = x;

    zerocalc(a);
    signcalc(a);
}

void CPU::txs()
{
    sp = x;
}

void CPU::tya()
{
    a = y;

    zerocalc(a);
    signcalc(a);
}


} // ::six5c02
