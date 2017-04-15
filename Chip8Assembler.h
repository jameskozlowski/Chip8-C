/**
* Chip-8 Assembler 
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

#ifndef CHIP8_ASS_H
#define CHIP8_ASS_H



#include <ctype.h>
#include <stdbool.h>

//opcode possitions in the array
#define OPCODE_CLS          0
#define OPCODE_RET          1
#define OPCODE_JP           2
#define OPCODE_CALL         3
#define OPCODE_SE           4
#define OPCODE_SNE          5
#define OPCODE_LD           6
#define OPCODE_ADD          7
#define OPCODE_OR           8
#define OPCODE_AND          9
#define OPCODE_XOR          10
#define OPCODE_SUBN         11
#define OPCODE_SHR          12
#define OPCODE_SUB          13
#define OPCODE_SHL          14
#define OPCODE_RND          15
#define OPCODE_DRW          16
#define OPCODE_SKP          17
#define OPCODE_SKNP         18
#define OPCODE_SCD          19
#define OPCODE_SCR          20
#define OPCODE_SCL          21
#define OPCODE_EXIT         22
#define OPCODE_LOW          23
#define OPCODE_HIGH         24
#define OPCODE_DA           25
#define OPCODE_DW           26
#define OPCODE_DB           27

//opcode param posistion in the array
#define OPCODE_PARAM_NULL   99
#define OPCODE_PARAM_V0     0
#define OPCODE_PARAM_V1     1
#define OPCODE_PARAM_V2     2
#define OPCODE_PARAM_V3     3
#define OPCODE_PARAM_V4     4
#define OPCODE_PARAM_V5     5
#define OPCODE_PARAM_V6     6
#define OPCODE_PARAM_V7     7
#define OPCODE_PARAM_V8     8
#define OPCODE_PARAM_V9     9
#define OPCODE_PARAM_VA     10
#define OPCODE_PARAM_VB     11
#define OPCODE_PARAM_VC     12
#define OPCODE_PARAM_VD     13
#define OPCODE_PARAM_VE     14
#define OPCODE_PARAM_VF     15
#define OPCODE_PARAM_I      16
#define OPCODE_PARAM_DT     17
#define OPCODE_PARAM_DS     18
#define OPCODE_PARAM_B      19
#define OPCODE_PARAM__I_    20
#define OPCODE_PARAM_HF     21
#define OPCODE_PARAM_R      22
#define OPCODE_PARAM_K      23
#define OPCODE_PARAM_F      24

typedef struct
{
    //the lable's naem'
    char name[25];

    //the lable's location
    int address;
} Chip8AssAddressPointer;

/**
* Process a file
*
* @param filenamein file to read and assemble
* @param filenameout file to save the assembled code to
* @return false if the file could not be opened
*/
bool Chip8AssProcessFile (char* filenamein, char* filenameout);

/**
* Process a line from the assembly file
*
* @param line pointer to the string
* @param lables, weather or not to process lables, this sould be false on secodn run
* @return false if the line is not parsed
*/
bool Chip8AssProcessLine(char *line, bool lables);

/**
* Converts a string to the correct hex value based on the opcode
*
* @param opcode index of the current opcide
* @param line the currnet line in the file
* @return None
*/
int Chip8AssBuildCode(int opcode, char *line);

/**
* tries to match a opcode param,
* If one is found moves the pointer past the param
*
* @param line a pointer to the line pointer 
* @return the index of the param found
*/
int Chip8AssGetV(char **line);


/**
* attempts to find a matching lable
* if a lable is not found, it assumes it is a number and converts it
*
* @param line the current string of the line
* @return the parsed address
*/
int Chip8AssGetAddress(char *line);

/**
* using some basic rules formats a string as a int
*
* Strings starting with a $ are parsed as a binary string
* in a binary string . = 0
*
* Strings starting with a # are pased as a hex
*
* Strings that are only 1 char long are parsed as a hex
*
* All others are parsed as a int 
*
* @param sPtr pointer to the string
* @return a int of the strings value
*/
int Chip8AssParseInt(char *sPtr);

/**
* Converts a string to upper case
*
* @param sPtr pointer to the string
* @return None
*/
void Chip8AssUppercase( char *sPtr );

#endif