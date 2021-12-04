#!/usr/bin/python
#
# File:     writeInstructions.py
# Date:     December 2021
# Purpose:  Converts *.opcodes files into instructions.cpp
# Author:   Paul Robson, Brent Burton
#
# Note: Based on buildtables.py, this version is reduced and
#       simplified for readability. -bpb

import re

# Constants
TOTAL_NUMBER_OPCODES = 256

# Filenames
OPCODES_6502_FNAME = "6502.opcodes"
OPCODES_65c02_FNAME = "65c02.opcodes"

# Regex Constants
OPCODE_REGEX_STR = "^(?P<insn>\\w+)\\s+(?P<mode>\\w+)\\s+(?P<cycles>\\d)\\s+\\$(?P<opcode>[0-9a-fA-F]+)$"

def loadSource(srcFile):
    '''Load in a source file with opcode descriptors'''
    # Read file line by line and omit committed
    fileLinesObject = open(srcFile).readlines()
    strippedOpcodeLines = [opcodeLine.strip() for opcodeLine in fileLinesObject if
                           opcodeLine.strip() != "" and not opcodeLine.startswith(";")]

    # Import Opcodes
    for opcodeLine in strippedOpcodeLines:
        matchLine = re.match(OPCODE_REGEX_STR, opcodeLine)
        assert matchLine is not None, "Format {}".format(opcodeLine)

        opcode = int(matchLine.group('opcode'), 16)
        assert opcodesList[opcode] is None, "Duplicate {0:02x}".format(opcode)

        opcodesList[opcode] = {
            'mode': matchLine.group('mode'),
            'insn': matchLine.group('insn'),
            'cycles': matchLine.group('cycles'),
            'opcode': opcode
        }

def fillNop():
    '''Fill instruction slots with ILLs'''
    for i in range(0, TOTAL_NUMBER_OPCODES):
        if opcodesList[i] is None:
            opcodesList[i] = {
                'mode': 'imp',
                'insn': 'ill',
                'cycles': '2',
                'opcode': i
            }

def generateInstructionArray(outfile):
    '''Generate C++ source to init instruction array'''
    outfile.write('/* Do not edit. Created by writeInstructions.py */\n')
    outfile.write('#include "six5c02.h"\n\n')
    outfile.write('namespace six5c02 {\n\n')
    outfile.write('void CPU::setInstructions()\n{\n')
    for i in range(0, TOTAL_NUMBER_OPCODES):
        item = opcodesList[i]
        outfile.write("    m_insn[0x%02x] = { .insn = &CPU::%s, .mode = &CPU::%s, .ticks = %s };\n" %
                      (i, item['insn'], item['mode'], item['cycles']) )
    outfile.write('}\n\n')
    outfile.write('\n} // ::six5c02\n')

if __name__ == '__main__':
    opcodesList = [None] * TOTAL_NUMBER_OPCODES

    loadSource(OPCODES_6502_FNAME)
    loadSource(OPCODES_65c02_FNAME)
    fillNop()

    with open('instructions.cpp', 'w') as outputFile:
        generateInstructionArray(outputFile)
