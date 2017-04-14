/**
* Chip-8 Disassembler 
* 
* Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems 
* in the late 1970s and early 1980s. The COSMAC VIP, DREAM 6800, and ETI 660 computers are a few examples. 
* These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. 
* The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller.
*
* In the early 1990s, the Chip-8 language was revived by a man named Andreas Gustafsson. He created a Chip-8 interpreter for the HP48 graphing calculator, 
* called Chip-48. The HP48 was lacking a way to easily make fast games at the time, and Chip-8 was the answer. Chip-48 later begat Super Chip-48, 
* a modification of Chip-48 which allowed higher resolution graphics, as well as other graphical enhancements.
*
* Chip-48 inspired a whole new crop of Chip-8 interpreters for various platforms, including MS-DOS, Windows 3.1, Amiga, HP48, MSX, Adam, and ColecoVision.
*
* Technical Reference(used for this emulator): http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
*
* More information: https://en.wikipedia.org/wiki/CHIP-8
* 
* @author  James Kozlowski
* @version April 2, 2017
*/

#include <string.h>
#include <stdio.h>

#include "Chip8Disassembler.h"

//opcodes for the Chip-8
int opcodeCount = 44;
const char * opCodes[][2] = {
    { "00E0", "CLS" },
    { "00EE", "RET" },
    { "0NNN", "SYS NNN" },
    { "1NNN", "JP NNN" },
    { "2NNN", "CALL NNN" },
    { "3XKK", "SE VX, KK" },
    { "4XKK", "SNE VX, KK" },
    { "5XY0", "SE VX, VY" },
    { "6XKK", "LD VX, KK" },
    { "7XKK", "ADD VX, KK" },
    { "8XY0", "LD VX, VY" },
    { "8XY1", "OR VX, VY" },
    { "8XY2", "AND VX, VY" },
    { "8XY3", "XOR VX, VY" },
    { "8XY4", "ADD VX, VY" },
    { "8XY5", "SUB VX, VY" },
    { "8XY6", "SHR VX" },
    { "8XY7", "SUBN VX, VY" },
    { "8XYE", "SHL VX" },
    { "9XY0", "SNE VX, VY" },
    { "ANNN", "LD I, NNN" },
    { "BNNN", "JP V0, NNN" },
    { "CXKK", "RND VX, KK" },
    { "DXYN", "DRW VX, VY, N" },
    { "EX9E", "SKP VX" },
    { "EXA1", "SKNP VX" },
    { "FX07", "LD VX, DT" },
    { "FX0A", "LD VX, K" },
    { "FX15", "LD DT, VX" },
    { "FX18", "LD ST, VX" },
    { "FX1E", "ADD I, VX" },
    { "FX29", "LD F, VX" },
    { "FX33", "LD B, VX" },
    { "FX55", "LD [I], VX" },
    { "FX65", "LD VX, [I]" },
    { "00CN", "SCD N" },
    { "00FB", "SCR" },
    { "00FC", "SCL" },
    { "00FD", "EXIT" },
    { "00FE", "LOW" },
    { "00FF", "HIGH" },
    { "FX30", "LD HF, VX" },
    { "FX75", "LD R, VX" },
    { "FX85", "LD VX, R" }
};

/**
* Disassemble a given opcode and put it in the buffer
*
* @param opcodeInt Interger value of the op opcode
* @param the buffer to store the Disassembled string
* @return none
*/
void Chip8Disassemble(int opcodeInt, char *buffer)
{
    char opcode[5];
    sprintf (opcode, "%04X", opcodeInt);
    for (int opCodesPtr = 0; opCodesPtr < opcodeCount; opCodesPtr++)
    {
        for (int i = 0; i < 4; i++)
        {
            char c = opCodes[opCodesPtr][0][i];
            if (c != '\0' && (c == opcode[i] || c == 'X' || c == 'Y' || c == 'K' || c == 'N'))
            {
                if (i == 3)
                {
                    strcpy(buffer, opCodes[opCodesPtr][1]);
                    genString(opCodes[opCodesPtr][0], opcode, buffer);
                    return;
                }
            }
            else
                i = 4;
        }
    }
    for (int i = 0; i < 4; i++)
	buffer[i] = opcode[i];
    buffer[4] = '\0';
}

/**
* Generate a string based on opcode and values
*
* @param opcodeformat the format of the opcode hex value
* @param opcode to generate a string for
* @param buffer buffer to store string in
* @return none
*/
void genString(const char *opcodeformat, char *opcode, char *buffer)
{

    for (int i = 0; i < 4; i++)
    {
        char c = opcodeformat[i];
        if (c == 'X' || c == 'Y' || c == 'K' || c == 'N')
            replaceChar(buffer, c, opcode[i]);
    }
}


/**
* replace the given char with the supplied car, only the first one found
*
* @param str string to repace value in
* @param replace char to replace
* @param with char to replace with
* @return none
*/
void replaceChar(char *str, char replace, char with)
{
    char *c = str;

    while (*c != '\0')
    {
        if (*c == replace)
        {
            *c = with;
            return;
        }
        ++c;  
    }
}
