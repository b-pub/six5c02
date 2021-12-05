/*
 * A Console memory-mapped output device.
 *
 * Programs can write text to output one line at a time. They do
 * this by sending characters singly, then flushing the
 * line. This writes the buffer contents out to stdout with a
 * newline (programs do not need to write newlines).
 *
 * This device supports a few basic operations:
 *  * Send a character (ASCII) out (appends to buffer).
 *  * Flush the line. This prints it to stdout, and clears the buffer
 *
 * Copyright 2021, Brent Burton
 * See LICENSE file for BSD 2-clause license.
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdio.h>

// I/O addresses. Control plus 2 data registers.
#define CONS_CTRL  0xBF00
#define CONS_DATA1 0xBF01

#define CONS_BEGIN CONS_CTRL  // Total memory range, inclusive.
#define CONS_END   CONS_DATA1

// Commands.
#define CONS_CMD_PRINT 0x00   // reads DATA1, adds to print buffer.
#define CONS_CMD_FLUSH 0x01   // flushes output buffer; clears buffer.
#define CONS_CMD_CLEAR 0x02   // no data needed/used

class Console
{
  public:

    Console()
        : m_buf("")
        , m_idx(0)
    { }

    ~Console()
    { }

    bool write(six5c02::IMemoryController const &mem, uint16_t const address)
    {
        if (CONS_CTRL <= address && address <= CONS_DATA1)
        {
            if (CONS_CTRL == address)
            {
                uint8_t const cmd = mem.readMem(CONS_CTRL);
                switch (cmd)
                {
                  case CONS_CMD_PRINT:
                    m_buf[m_idx++] = mem.readMem(CONS_DATA1);
                    m_buf[m_idx] = 0;

                    if (m_idx == 255)       // Keep overwriting last char.
                        m_idx--;            // This won't overrun buffer.
                    break;

                  case CONS_CMD_FLUSH:      // Print contents.
                    printf("%s\n", m_buf);
                    fflush(stdout);
                    // fall-through

                  case CONS_CMD_CLEAR:      // Clear manually or auto after flush
                    m_idx = 0;
                    m_buf[0] = 0;
                    break;

                  default:
                    printf("CONS: Invalid command %02x\n", (uint16_t)cmd);
                }
            }
            // ignore writes to data reg; those values are kept in mem
        }
        else
        {
            printf("Invalid address dispatched to Console device\n");
            return false;
        }
        return true;
    }

  protected:
    uint8_t  m_buf[256];
    uint16_t m_idx;                             // write index into m_buf
};

#endif // CONSOLE_H
