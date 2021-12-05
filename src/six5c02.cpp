/*
 * six5c02 -  emulated 6502 chip framework
 *
 * CPU framework class implementation
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */

#include <stdio.h>
#include <stdint.h>

#include "six5c02.h"
#include "support.h"

#define BASE_STACK     0x100

namespace six5c02 {

CPU::CPU()
    : m_memCtrl(nullptr)
    , m_loopCallback(nullptr)
{
    init();
    setInstructions();
}

void CPU::init()
{
    // Init registers
    pc = 0;
    sp = a = x = y = status = 0;

    // Init emulation state:
    instructions = clockticks = clockgoal = 0;
    oldpc = ea = reladdr = value = result = 0;
    opcode = oldstatus = 0;
    penaltyop = penaltyaddr = waiting = false;

    stopped = false;
}

uint16_t CPU::getvalue()
{
    if (m_insn[opcode].mode == &CPU::acc)
        return((uint16_t)a);
    else
        return((uint16_t)read6502(ea));
}

uint16_t CPU::getvalue16()
{
    return ((uint16_t)read6502(ea) |
            ((uint16_t)read6502(ea+1) << 8));
}

void CPU::putvalue(uint16_t saveval)
{
    if (m_insn[opcode].mode == &CPU::acc)
        a = (uint8_t)(saveval & 0x00FF);
    else
        write6502(ea, (saveval & 0x00FF));
}

void CPU::nmi() {
    push16(pc);
    push8(status);
    status |= FLAG_INTERRUPT;
    pc = (uint16_t)read6502(0xFFFA) | ((uint16_t)read6502(0xFFFB) << 8);
	waiting = 0;
}

void CPU::irq() {
    push16(pc);
    push8(status & ~FLAG_BREAK);
    status |= FLAG_INTERRUPT;
    pc = (uint16_t)read6502(0xFFFE) | ((uint16_t)read6502(0xFFFF) << 8);
	waiting = 0;
}

/**
 * stepInstruction() fetches and executes one instruction.
 * This method is called by both exec() and step(), and is
 * an internal method.
 *
 * Should an invalid opcode be encountered ("ill"), the
 * emulator is halted.
 *
 * @retval True if a valid insn/opcode was run
 * @retval False if an illegal instruction is at PC
 */
bool CPU::stepInstruction()
{
    opcode = read6502(pc++);
    status |= FLAG_CONSTANT;

    penaltyop = 0;
    penaltyaddr = 0;

    /* Illegal opcode: Not a documented 65c02 instruction
     * so stop.
     */
    if (m_insn[opcode].insn == &CPU::ill)
    {
        // rewind the pc to point at the illegal byte
        pc--;
        stopped = true;
        return false;
    }

    (this->*m_insn[opcode].mode)();
    (this->*m_insn[opcode].insn)();
    clockticks += m_insn[opcode].ticks;
    if (penaltyop && penaltyaddr)
        clockticks++;

    instructions++;
    return true;
}

/*
 * exec() runs many instructions.
 * This method runs as many instructions as will fit within
 * the number of clocks specified by \c tickcount. Once
 * the clock budget is depleted, exec() returns.
 *
 * @retval True if a valid insn/opcode was run
 * @retval False if an illegal instruction is at PC
 */
bool CPU::exec(uint32_t tickcount)
{
	if (waiting)
    {
		clockticks += tickcount;
		clockgoal = clockticks;
		return true;
    }

    clockgoal += tickcount;
   
    bool isValid = true;
    while (isValid && !stopped && clockticks < clockgoal)
    {
        isValid = stepInstruction();

        if (isValid && m_loopCallback)
            m_loopCallback->callback(*this);
    }
    return isValid;
}

/*
 * step() runs one instruction.
 * This method runs the next instruction at PC.
 *
 * @retval True if a valid insn/opcode was run
 * @retval False if an illegal instruction is at PC
 */
bool CPU::step()
{
	if (waiting)
    {
		++clockticks;
		clockgoal = clockticks;
		return true;
	}

    bool isValid = stepInstruction();
    clockgoal = clockticks;

    if (m_loopCallback)
        m_loopCallback->callback(*this);

    return isValid;
}

uint8_t CPU::read6502(uint16_t address)
{
    if (!m_memCtrl)
        return 0xFF;

    return m_memCtrl->readMem(address);
}

void CPU::write6502(uint16_t address, uint8_t value)
{
    if (m_memCtrl)
        m_memCtrl->writeMem(address, value);
}

void CPU::setMemoryController(IMemoryController *memctrl)
{
    m_memCtrl = memctrl;
}

void CPU::setLoopCallback(ILoopCallback *callback)
{
    m_loopCallback = callback;
}

//  Fixes from http://6502.org/tutorials/65c02opcodes.html
//
//  65C02 Cycle Count differences.
//        ADC/SBC work differently in decimal mode.
//        The wraparound fixes may not be required.

void CPU::push8(uint8_t pushval)
{
    write6502(BASE_STACK + sp--, pushval);
}

uint8_t CPU::pull8()
{
    return (read6502(BASE_STACK + ++sp));
}

void CPU::push16(uint16_t pushval)
{
    write6502(BASE_STACK + sp, (pushval >> 8) & 0xFF);
    write6502(BASE_STACK + ((sp - 1) & 0xFF), pushval & 0xFF);
    sp -= 2;
}

uint16_t CPU::pull16()
{
    uint16_t temp16;
    temp16 = (read6502(BASE_STACK + ((sp + 1) & 0xFF)) |
              ((uint16_t)read6502(BASE_STACK + ((sp + 2) & 0xFF)) << 8));
    sp += 2;
    return(temp16);
}

bool CPU::reset()
{
    if (!m_memCtrl)
        return false;

    pc = (uint16_t)read6502(0xFFFC) | ((uint16_t)read6502(0xFFFD) << 8);
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFD;
    status |= FLAG_CONSTANT;

    stopped = false;

    return true;
}

} // ns six5c02
