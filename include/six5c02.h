#ifndef SIX5C02_H
#define SIX5C02_H

#include <stdint.h>

namespace six5c02 {

    class CPU;

    /**
     * The IMemoryController interface defines an interface that a
     * CPU instance uses to access memory. The implementation
     * of IMemoryController is provided by the user, and models the
     * RAM, mapped I/O, and ROM. It is fixed at 64KB (65536 bytes).
     */
    class IMemoryController
    {
      public:

        /**
         * readMem() attempts to read a byte from memory
         * location \c address. Address is any value in the
         * 64KB memory space.
         * 
         * Whether the address value corresponds to RAM, ROM,
         * or mapped I/O is up to the implementation of the
         * called IMemoryController.
         * 
         * @retval The byte value at location \c address
         */
        virtual uint8_t readMem(uint16_t address) const = 0;

        /**
         * writeMem() attempts to write \c value to memory
         * location \c address, which must be writeable.
         * 
         * @retval True The memory was a valid writeable location.
         * @retval False The memory write failed (e.g., to ROM).
         */
        virtual bool writeMem(uint16_t address, uint8_t value) = 0;


      protected:

        uint8_t m_mem[65536];

        IMemoryController() {}
        virtual ~IMemoryController() {}

      private:
        IMemoryController(IMemoryController const& o) = delete; // No copy ctor
        IMemoryController& operator=(IMemoryController const& o) = delete; // No copy

    };

    /**
     * This interface class provides a callback API for
     * an emulated machine to receive callbacks during
     * the emulation main loop. Frequency of callbacks
     * depends entirely on how the client's main loop is
     * structured to call exec() or step().
     */
    class ILoopCallback
    {
      public:
        /**
         * The callback method. Clients provide an implementation
         * of this method to perform actions every period.
         */
        virtual void callback(CPU &cpu) = 0;

      protected:
        ILoopCallback() = default;
        virtual ~ILoopCallback() {}

      private:
        ILoopCallback(ILoopCallback const& o) = delete; // No copy ctor
        ILoopCallback& operator=(ILoopCallback const& o) = delete; // No copy
    };

    /**
     * The CPU class is the main 65c02 emulation core.
     *
     * After construction, caller should provide a IMemoryController
     * implementation that contains the memory model, as well as a
     * ILoopCallback implementation if periodic callbacks are necessary.
     *
     * Once initialized, call reset(), then either step() to single-step
     * the emulator one instruction at a time, or call exec() to run
     * many instructions up to some cycle limit.
     *
     * Interrupt requests are initiated via irq(). Poke the memory as
     * required, then call irq(). Similarly, non-maskable interrupts
     * can be triggered via nmi().
     */
    class CPU
    {
      public:

        CPU();
        virtual ~CPU() { }

        void setMemoryController(IMemoryController *memctrl);
        void setLoopCallback(ILoopCallback *callback);

        bool reset(); // True means it's in a working state; False means no-go

        bool isStopped() const { return stopped; } // true = STP called;
        bool step();
        bool exec(uint32_t tickcount);

        void irq();
        void nmi();

        // Accessors for emulator state
        uint32_t getClockticks() const { return clockticks; }
        uint16_t PC() const { return pc; }
        uint8_t  SP() const { return sp; }
        uint8_t  A() const { return a; }
        uint8_t  X() const { return x; }
        uint8_t  Y() const { return y; }

        void setPC(uint16_t addr) { pc = addr; }

        // Status flags, used in emulator and client
#define FLAG_CARRY     0x01
#define FLAG_ZERO      0x02
#define FLAG_INTERRUPT 0x04
#define FLAG_DECIMAL   0x08
#define FLAG_BREAK     0x10
#define FLAG_CONSTANT  0x20                 // always 1
#define FLAG_OVERFLOW  0x40
#define FLAG_SIGN      0x80
        uint16_t P() const { return status; }
        uint16_t C() const { return (status & FLAG_CARRY) ? 1 : 0; }
        uint16_t Z() const { return (status & FLAG_ZERO) ? 1 : 0; }
        uint16_t I() const { return (status & FLAG_INTERRUPT) ? 1 : 0; }
        uint16_t D() const { return (status & FLAG_DECIMAL) ? 1 : 0; }
        uint16_t B() const { return (status & FLAG_BREAK) ? 1 : 0; }
        uint16_t V() const { return (status & FLAG_OVERFLOW) ? 1 : 0; }
        uint16_t N() const { return (status & FLAG_SIGN) ? 1 : 0; }

      protected:

        IMemoryController *m_memCtrl;   ///< the memory model
        ILoopCallback *m_loopCallback; ///< Callback during loop

        // 6502 CPU registers
        uint16_t pc;
        uint8_t sp, a, x, y, status;


        // Emulation state variables
        uint32_t instructions; // total number of instructions executed
        uint32_t clockticks;   // total number of clock ticks emulated
        uint32_t clockgoal;    // 
        uint16_t oldpc, ea, reladdr, value, result;
        uint8_t opcode, oldstatus;

        bool penaltyop, penaltyaddr;
        bool waiting;
        bool stopped;                       // default false; stp ($db) sets to true.

        // Wrappers to access IMemoryController safely
        uint8_t read6502(uint16_t address);
        void write6502(uint16_t address, uint8_t value);

        // Other utility functions:
        void     init(); // Zero-init of internal state. reset() must be called afterwards.

        uint16_t getvalue();
        uint16_t getvalue16();
        void     putvalue(uint16_t saveval);

        void     push8(uint8_t pushval);
        uint8_t  pull8();
        void     push16(uint16_t pushval);
        uint16_t pull16();

        // execute a single instruction at PC. Called by exec() and step().
        bool     stepInstruction();

        // Addressing mode methods
        void imp(); // implied
        void acc(); // accumulator
        void imm(); // immediate
        void zp();  // zero-page
        void zpx(); // zero-page, X
        void zpy(); // zero-page, Y
        void rel(); // relative for branch ops (8b immediate sign-extended value)
        void abso(); // absolute
        void absx(); // absolute, X
        void absy(); // absolute, Y
        void ind(); // indirect
        void indx(); // indirect, X
        void indy(); // indirect, Y
        void zprel(); // zero-page, relative for branch ops (8b imm val, sign-extended)

        // Instruction methods

        // Fake instructions
        void ill();                         // illegal instruction placeholder

        // 6502 instructions
        void adc();
        void And();  // capitalized due to C++'s new keyword
        void asl();
        void bcc();
        void bcs();
        void beq();
        void bit();
        void bmi();
        void bne();
        void bpl();
        void brk();
        void bvc();
        void bvs();
        void clc();
        void cld();
        void cli();
        void clv();
        void cmp();
        void cpx();
        void cpy();
        void dec();
        void dex();
        void dey();
        void eor();
        void inc();
        void inx();
        void iny();
        void jmp();
        void jsr();
        void lda();
        void ldx();
        void ldy();
        void lsr();
        void nop();
        void ora();
        void pha();
        void php();
        void pla();
        void plp();
        void rol();
        void ror();
        void rti();
        void rts();
        void sbc();
        void sec();
        void sed();
        void sei();
        void sta();
        void stx();
        void sty();
        void tax();
        void tay();
        void tsx();
        void txa();
        void txs();
        void tya();

        // 65c02 addressing modes
        void ainx();
        void ind0();

        // 65c02 instructions
        void bra();
        void phx();
        void phy();
        void plx();
        void ply();
        void stp();
        void stz();
        void trb();
        void tsb();
        void wai();
        void bbr(uint16_t bitmask);         // not an insn, a helper
        void bbr0();
        void bbr1();
        void bbr2();
        void bbr3();
        void bbr4();
        void bbr5();
        void bbr6();
        void bbr7();
        void bbs(uint16_t bitmask);         // not an insn, a helper
        void bbs0();
        void bbs1();
        void bbs2();
        void bbs3();
        void bbs4();
        void bbs5();
        void bbs6();
        void bbs7();
        void rmb0();
        void rmb1();
        void rmb2();
        void rmb3();
        void rmb4();
        void rmb5();
        void rmb6();
        void rmb7();
        void smb0();
        void smb1();
        void smb2();
        void smb3();
        void smb4();
        void smb5();
        void smb6();
        void smb7();

        // Collect instruction and addr mode methods, plus cycle time.
        // Pointers to non-static methods can be questionable,
        // but in this case they are set and referenced
        // internally so the vtable is guaranteed to be valid for
        // *this at dispatch.
        struct Insn
        {
            void (CPU::*insn)();            // ptr to instruction method
            void (CPU::*mode)();            // ptr to addressing mode method
            uint32_t ticks;                 // number of clock ticks of insn
        };

        Insn m_insn[256];

        void setInstructions();             // Part of initialization, fills m_insn[].
    };

} // ns six5c02

/**
 * Public utility functions for clients
 */
void printRegisters(six5c02::CPU &cpu);
void dumpMem(six5c02::IMemoryController &memory, uint16_t start, uint16_t len);

#endif
