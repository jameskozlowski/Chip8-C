/**
* Chip-8 
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

#ifndef Chip8_H
#define Chip8_H

#include <stdbool.h>

typedef struct
{
    //The currently running opcode
    unsigned short opcode;

    /*
     Memory Map:
     +---------------+= 0xFFF (4095) End of Chip-8 RAM
     |               |
     |               |
     | 0x200 to 0xFFF|
     |     Chip-8    |
     | Program / Data|
     |     Space     |
     |               |
     |               |
     +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
     |               |
     |               |
     +---------------+= 0x200 (512) Start of most Chip-8 programs
     | 0x000 to 0x1FF|
     | Reserved for  |
     |  interpreter  |
     +---------------+= 0x000 (0) Start of Chip-8 RAM
    */
    unsigned char memory[4096];

    //16 general purpose 8-bit registers.
    //The VF register should not be used by any program, as it is used as a flag by some instructions.
    unsigned char V[16];

    //This register is generally used to store memory addresses.
    unsigned short I;

    //points the the location in memory of the next Opcode
    unsigned short pc;

    /*
     The display memmory
     *********************
     *(0,0)        (63,0)*
     *                   *
     *(0,32)      (63,31)*
     *********************
    */
    unsigned char videoMemory[64 * 32];

    //The delay timer is active whenever the delay timer register (DT) is non-zero. 
    //This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz. When DT reaches 0, it deactivates.
    unsigned char delayTimer;

    //The sound timer is active whenever the sound timer register (ST) is non-zero. This timer also decrements at a rate of 60Hz, 
    //however, as long as ST's value is greater than zero, the Chip-8 buzzer will sound. When ST reaches zero, the sound timer deactivates.
    unsigned char soundTimer;

    //LIFO Call Stack
    unsigned short stack[16];

    //Call Stack ointer
    unsigned short sp;

    /*
     The computers which originally used the Chip-8 Language had a 16-key hexadecimal keypad with the following layout:
     1 2 3 4
     4 5 6 D
     7 8 9 E
     A 0 B F
    */
    unsigned char key[16];

    //Set to true when the screen needs to be redrawn
    //Should be set to false once the screen has been redrawn
    bool refreshScreen;

    //Set to true if a beep needs to be played
    bool playBeep;
} Chip8CPU;

/**
* Resets the Chip8CPU to power on defaults
* Loads the default font set into memory
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8Reset(Chip8CPU *Chip8);

/**
* Loads the Chip8 ROM into memory starting at 0x200
*
* @param Chip8 Address of the Chip8CPU object
* @param filename filename to load
* @return false if the file could not be loaded.
*/
bool Chip8LoadRom(Chip8CPU *Chip8, char *filename);

/**
* Fetches and runs a opcode
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8EmulateCycle(Chip8CPU *Chip8);


/**********************************************************************************************
 * CHIP-8 has 35 opcodes, which are all two bytes long and stored big-endian. 
 * The opcodes are listed below, in hexadecimal and with the following symbols:
 *
 * OPCODE 	DISC
 * --------------------------------------------------------------------------------------------
 * 0NNN    	RCA 1802 program at address NNN. Not necessary for most ROMs.
 * 00E0 	Clears the screen.
 * 00EE 	Returns from a subroutine.
 * 1NNN 	Jumps to address NNN.
 * 2NNN 	Calls subroutine at NNN.
 * 3XNN 	Skips the next instruction if VX equals NN. 
 * 4XNN 	Skips the next instruction if VX doesn't equal NN. 
 * 5XY0 	Skips the next instruction if VX equals VY. 
 * 6XNN 	Sets VX to NN.
 * 7XNN 	Adds NN to VX.
 * 8XY0 	Sets VX to the value of VY.
 * 8XY1 	Sets VX to VX or VY. (Bitwise OR operation) VF is reset to 0.
 * 8XY2 	Sets VX to VX and VY. (Bitwise AND operation) VF is reset to 0.
 * 8XY3 	Sets VX to VX xor VY. VF is reset to 0.
 * 8XY4 	Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
 * 8XY5 	VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
 * 8XY6 	Shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift.
 * 8XY7 	Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
 * 8XYE 	Shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift.
 * 9XY0 	Skips the next instruction if VX doesn't equal VY. 
 * ANNN 	Sets I to the address NNN.
 * BNNN 	Jumps to the address NNN plus V0.
 * CXNN 	Sets VX to the result of a bitwise and operation on a random number and NN.
 * DXYN 	Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
 * EX9E 	Skips the next instruction if the key stored in VX is pressed. 
 * EXA1 	Skips the next instruction if the key stored in VX isn't pressed.
 * FX07 	Sets VX to the value of the delay timer.
 * FX0A 	A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
 * FX15 	Sets the delay timer to VX.
 * FX18 	Sets the sound timer to VX.
 * FX1E 	Adds VX to I.[3]
 * FX29 	sets I to the location of the sprite for the character in VX. Characters 0-F  are represented by a 4x5 font.
 * FX33 	Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I.
 * FX55 	Stores V0 to VX (including VX) in memory starting at address I.[4]
 * FX65 	Fills V0 to VX (including VX) with values from memory starting at address I.
 **********************************************************************************************/

//Array of function pointers to the OpCodes
void (*Chip8OpcodeTable[16])(Chip8CPU *Chip8);

//Array of Function pointers to the 8???? OpCodes
void (*Chip8ArithmeticOpcodeTable[16])(Chip8CPU *Chip8);

/**
* If this OPCODE is called then something went wrong
* or the opcode was not finished
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8CPUNULL(Chip8CPU *Chip8);

/**
* 00E0 - Clear the display.
* 00EE - Return from a subroutine. 
* The interpreter sets the program counter to the address at the top of the stack,
* then subtracts 1 from the stack pointer.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode00EX(Chip8CPU *Chip8);

/**
* Jump to location nnn.
* The interpreter sets the program counter to nnn.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode1NNN(Chip8CPU *Chip8);

/**
* Call subroutine at nnn.
* The interpreter increments the stack pointer, 
* then puts the current PC on the top of the stack. 
* The PC is then set to nnn.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode2NNN(Chip8CPU *Chip8);

/**
* Skip next instruction if Vx = kk.
* The interpreter compares register Vx to kk, 
* and if they are equal, increments the program counter by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode3XNN(Chip8CPU *Chip8);

/**
* Skip next instruction if Vx != kk.
* The interpreter compares register Vx to kk, 
* and if they are not equal, increments the program counter by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode4XNN(Chip8CPU *Chip8);

/**
* Skip next instruction if Vx != Vy.
* The interpreter compares register Vx to Vy, 
* and if they are equal, increments the program counter by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode5XY0(Chip8CPU *Chip8);

/**
* Set Vx = kk.
* The interpreter puts the value kk into register Vx.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode6XN0(Chip8CPU *Chip8);

/**
* Set Vx = Vx + kk.
* Adds the value kk to the value of register Vx, 
* then stores the result in Vx. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode7XNN(Chip8CPU *Chip8);

/**
* Calls the 8??? op codes
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8ARITHMETIC(Chip8CPU *Chip8);

/**
* Set Vx = Vy.
* Stores the value of register Vy in register Vx. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY0(Chip8CPU *Chip8);

/**
* Set Vx = Vx OR Vy.
* Performs a bitwise OR on the values of Vx and Vy, 
* then stores the result in Vx. 
* A bitwise OR compares the corrseponding bits from two values, 
* and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY1(Chip8CPU *Chip8);

/**
* Set Vx = Vx AND Vy.
* Performs a bitwise AND on the values of Vx and Vy, 
* then stores the result in Vx.
* A bitwise AND compares the corrseponding bits from two values,
* and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY2(Chip8CPU *Chip8);

/**
* Set Vx = Vx XOR Vy.
* Performs a bitwise exclusive OR on the values of Vx and Vy, 
* then stores the result in Vx. 
* An exclusive OR compares the corrseponding bits from two values, 
* and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY3(Chip8CPU *Chip8);

/**
* Set Vx = Vx + Vy, set VF = carry.
* The values of Vx and Vy are added together. 
* If the result is greater than 8 bits (i.e., > 255,) VF is set to 1,
* otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY4(Chip8CPU *Chip8);

/**
* Set Vx = Vx - Vy, set VF = NOT borrow.
* If Vx > Vy, then VF is set to 1, otherwise 0. 
* Then Vy is subtracted from Vx, and the results stored in Vx.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY5(Chip8CPU *Chip8);

/**
* Set Vx = Vx SHR 1.
* If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. 
* Then Vx is divided by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY6(Chip8CPU *Chip8);

/**
* Set Vx = Vy - Vx, set VF = NOT borrow.
* If Vy > Vx, then VF is set to 1, otherwise 0. 
* Then Vx is subtracted from Vy, and the results stored in Vx.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XY7(Chip8CPU *Chip8);

/**
* Set Vx = Vx SHL 1.
* If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
* Then Vx is multiplied by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode8XYE(Chip8CPU *Chip8);

/**
* Skip next instruction if Vx != Vy.
* The values of Vx and Vy are compared, 
* and if they are not equal, the program counter is increased by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCode9XY0(Chip8CPU *Chip8);

/**
* Set I = nnn.
* The value of register I is set to nnn.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCodeANNN(Chip8CPU *Chip8);

/**
* Jump to location nnn + V0.
* The program counter is set to nnn plus the value of V0.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCodeBNNN(Chip8CPU *Chip8);

/**
* Set Vx = random byte AND kk.
* The interpreter generates a random number from 0 to 255, 
* which is then ANDed with the value kk. 
* The results are stored in Vx. 
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCodeCXKK(Chip8CPU *Chip8);

/**
* Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
* The interpreter reads n bytes from memory, starting at the address stored in I. 
* These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). 
* Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, 
* otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, 
* it wraps around to the opposite side of the screen.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCodeDXYN(Chip8CPU *Chip8);

/**
* Skip next instruction if key with the value of Vx is pressed.
* Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, 
* PC is increased by 2.
* OR
* Skip next instruction if key with the value of Vx is not pressed.
* Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, 
* PC is increased by 2.
*
* @param Chip8 Address of the Chip8CPU object
* @return Nothing.
*/
void Chip8OpCodeEXXX(Chip8CPU *Chip8);

#endif