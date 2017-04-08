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

#ifndef CHIP8_DIS_H
#define CHIP8_DIS_H

/**
* Disassemble a given opcode and put it in the buffer
*
* @param opcodeInt Interger value of the op opcode
* @param the buffer to store the Disassembled string
* @return none
*/
void Chip8Disassemble(int opcodeInt, char *buffer);

/**
* Generate a string based on opcode and values
*
* @param opcodeformat the format of the opcode hex value
* @param opcode to generate a string for
* @param buffer buffer to store string in
* @return none
*/
void genString(const char *opcodeformat, char *opcode, char *buffer);

/**
* replace the given char with the supplied car, only the first one found
*
* @param str string to repace value in
* @param replace char to replace
* @param with char to replace with
* @return none
*/
void replaceChar(char *str, char replace, char with);

#endif