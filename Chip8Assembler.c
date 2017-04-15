#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "Chip8Assembler.h"

//address lables and pointer
Chip8AssAddressPointer addressPointers[256];
int addressPointersCount = 0;

//working memory
unsigned char memory[4096];
int pc = 0x200;

//possable opcides and count
const int opcodeCount = 28;
const char *opcodes[] = {
    "CLS",
    "RET",
    "JP",
    "CALL",
    "SE",
    "SNE",
    "LD",
    "ADD",
    "OR",
    "AND",
    "XOR",
    "SUBN",
    "SHR",
    "SUB",
    "SHL",
    "RND",
    "DRW",
    "SKP",
    "SKNP",
    "SCD",
    "SCR",
    "SCL",
    "EXIT",
    "LOW",
    "HIGH",
    "DA",       //data
    "DW",       //data
    "DB"        //data
};

//possable opcode params and count
const int opcodeParamsCount = 25;
const char *opcodePerams[] = {
    "V0",
    "V1",
    "V2",
    "V3",
    "V4",
    "V5",
    "V6",
    "V7",
    "V8",
    "V9",
    "VA",
    "VB",
    "VC",
    "VD",
    "VE",
    "VF",
    "I",
    "DT",
    "DS",
    "B",
    "[I]",
    "HF",
    "R",
    "K",
    "F"
};

/**
* Process a file
*
* @param filenamein file to read and assemble
* @param filenameout file to save the assembled code to
* @return false if the file could not be opened
*/
bool Chip8AssProcessFile (char* filenamein, char* filenameout)
{
    FILE *fp;

    //buffer for reading in file lines
    size_t len = 256;
    char line[256];
    
    //current line in the file
    int lineNumber = 0;

    fp = fopen(filenamein, "r");

    //error opening file
    if (fp == NULL)
        return false;

    //proncess the file line by line
    while (fgets(line, len, fp) != NULL) 
    {
        lineNumber++;   
        if (!Chip8AssProcessLine(line, true))
            printf("Error line %i: \'%s\'\n", lineNumber, line);
    }

    //this does not seem like a very good way to do this but it works for now
    //rewind and run it again so the correct lable address are used
    rewind(fp);
    pc = 0x200;

    //process the file line by line again to resolve address lables(no need to print errors twice)
    while (fgets(line, len, fp) != NULL) 
    {
        lineNumber++;   
        Chip8AssProcessLine(line, false);
    }

    fclose(fp);

    //write the FILE
    fp = fopen(filenameout, "wb");

    if (fp == NULL)
        return false;

    //write memory to file
    fwrite(&memory[0x200], sizeof(char) * (pc - 0x200), 1, fp);
    
    //close the file
    fclose(fp);

    printf("Complete, program size: %i bytes\n", pc - 0x200);

    return true;
    
}

/**
* Process a line from the assembly file
*
* @param line pointer to the string
* @param lables, weather or not to process lables, this sould be false on secodn run
* @return false if the line is not parsed
*/
bool Chip8AssProcessLine(char *line, bool lables)
{
    //convert line to Chip8AssUppercase we will work all in Chip8AssUppercase
    Chip8AssUppercase(line);
    
    //remove any white spaces
    while (*line != '\0' && isspace(*line))
        ++line;
    
    //if this line is a comment or empty line return ok
    if (*line == '\0' || *line == ';')
        return true;

    //check for and add a lable
    bool lbl = 0;
    for(int i = 0; i < 25 && lbl == false && line[i] != '\0' && !isspace(line[i]) ; i++)
        lbl = (line[i] == ':');
        
    //check if this line hase a lbl
    if (lbl)
    {                 
        for (int i = 0; i < 25 && *line != ':'; i++)
        {
            if (lables)
                addressPointers[addressPointersCount].name[i] = *line;
            ++line;
        }
        //move past the ':'
        ++line;
        if (lables)
        {
            addressPointers[addressPointersCount].address = pc;
            addressPointersCount++;
        }
    }

    ///remove any white spaces after the lable
    while (*line != '\0' && isspace(*line))
        ++line;
    
    ///if this line is a comment or empty line after the lable return ok
    if (*line == '\0' || *line == ';')
        return true;
        
    //store opcide that may be found
    int opcode = -1;

    //loop for a matching opcode
    for (int i = 0; i < opcodeCount && opcode == -1; i++)
    {
        if (strncmp(line, opcodes[i], strlen(opcodes[i])) == 0)
            opcode = i;
    }
    
    //a matching opcode was not found so return error
    if (opcode == -1)
        return false;

    //movepast the opcode
    while (*line != '\0' && !isspace(*line))
        ++line;
    
    //remove any white spaces after the opcode
    while (*line != '\0' && isspace(*line))
        ++line;

    //if it is a data type write the data
    if (opcode == OPCODE_DA)
    {
        int p = 1;
        while (line[p] != '\'')
        {
            memory[pc++] = (unsigned char)line[p++];
        }
        if (!(p % 2))
            memory[pc++] == 0x00;
        
        return true;
    }
    //if it is a data type write the data
    else if (opcode == OPCODE_DW)
    {
        int value = Chip8AssParseInt(line);
        memory[pc++] = (unsigned char)((0xFF00 & value) >> 8);
        memory[pc++] = (unsigned char)(0x00FF & value);
        return true;
    }
    //if it is a data type write the data
    else if (opcode == OPCODE_DB)
    {
        memory[pc++] = (unsigned char)Chip8AssParseInt(line);
        return true;
    }
    //process other opcodes
    else
    {
        int builtOpcode = Chip8AssBuildCode(opcode, line);
        memory[pc++] = (unsigned char)((0xFF00 & builtOpcode) >> 8);
        memory[pc++] = (unsigned char)(0x00FF & builtOpcode);
        return true;
    }

    return false;
}

/**
* Converts a string to the correct hex value based on the opcode
*
* @param opcode index of the current opcide
* @param line the currnet line in the file
* @return None
*/
int Chip8AssBuildCode(int opcode, char *line)
{
    //used for param checking below
    int paramCheck1 = OPCODE_PARAM_NULL;
    int paramCheck2 = OPCODE_PARAM_NULL;
    int address = -1;

    switch(opcode)
    {
        case OPCODE_CLS: //CLS 00ed
            return 0x00E0;
            break;

        case OPCODE_RET: //RET 00ee
            return 0x00EE;
            break;

        case OPCODE_JP: //JP 1nnn
            //using paramCheck2 for address only for this opcode
            address = Chip8AssGetAddress(line);
            paramCheck1 = Chip8AssGetV(&line);
            if (paramCheck1 == OPCODE_PARAM_V0)
                return 0xB000 | Chip8AssGetAddress(line);
            else
                return 0x1000 | address;
            break;

        case OPCODE_CALL: //CALL 2nnn
            return 0x2000 | Chip8AssGetAddress(line);
            break;

        case OPCODE_SE: //SE 3xkk
            paramCheck1 = Chip8AssGetV(&line);
            paramCheck2 = Chip8AssGetV(&line);
            if (paramCheck2 <= OPCODE_PARAM_VF)
                return 0x5000 | (paramCheck1 << 8) | (paramCheck2 << 4);
            else
                return 0x3000 | (paramCheck1 << 8) | Chip8AssParseInt(line);
            break;

        case OPCODE_SNE: //SNE 4xkk 9xy0
            paramCheck1 = Chip8AssGetV(&line);
            paramCheck2 = Chip8AssGetV(&line);
            //4xkk
            if (paramCheck2 == OPCODE_PARAM_NULL)
                return 0x4000 | (paramCheck1 << 8) | Chip8AssParseInt(line);
            //9xy0
            else
                return 0x9000 | (paramCheck1 << 8) | (paramCheck2 << 4);
            break;
        
        case OPCODE_LD: //LD
            paramCheck1 = Chip8AssGetV(&line);
            address = Chip8AssGetAddress(line);
            paramCheck2 = Chip8AssGetV(&line);

            //LD vx vy 8xy0
            if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 <= OPCODE_PARAM_VF)
                return 0x8000 | (paramCheck1 << 8) | (paramCheck2 << 4);
            //LD xv dt Fx07
            else if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 == OPCODE_PARAM_DT)
                return 0xF007 | (paramCheck1 << 8);
            //LD vx K Fx0A
            else if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 == OPCODE_PARAM_K)
                return 0xF00A | (paramCheck1 << 8);
            //LD vx [I] Fx65
            else if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 == OPCODE_PARAM__I_)
                return 0xF065 | (paramCheck1 << 8);  
            //LD vx R Fx85  
            else if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 == OPCODE_PARAM_R)
                return 0xF085 | (paramCheck1 << 8);
            //LD vx byte
            else if(paramCheck1 <= OPCODE_PARAM_VF && paramCheck2 == OPCODE_PARAM_NULL)
                return 0x6000 | (paramCheck1 << 8) | Chip8AssParseInt(line);

            //LD I addr Annn  
            else if(paramCheck1 == OPCODE_PARAM_I)// && paramCheck2 == OPCODE_PARAM_NULL)
                return 0xA000 | address;
            //LD [I] vx Fx55  
            else if(paramCheck1 == OPCODE_PARAM__I_ && paramCheck2 <= OPCODE_PARAM_VF)
                return 0xF055 | (paramCheck2 << 8); 
            //LD F vx Fx29  
            else if(paramCheck1 == OPCODE_PARAM_F && paramCheck2 <= OPCODE_PARAM_VF)
                return 0xF029 | (paramCheck2 << 8); 
            //LD B vx Fx33  
            else if(paramCheck1 == OPCODE_PARAM_B && paramCheck2 <= OPCODE_PARAM_VF)
                return 0xF033 | (paramCheck2 << 8);
            //LD HF vx Fx30  
            else if(paramCheck1 == OPCODE_PARAM_HF && paramCheck2 <= OPCODE_PARAM_VF)
                return 0xF030 | (paramCheck2 << 8);
            //LD R vx Fx75
            else if(paramCheck1 == OPCODE_PARAM_R && paramCheck2 <= OPCODE_PARAM_VF)
                return 0xF075 | (paramCheck2 << 8); 
            else
                return -1;
            break;

        case OPCODE_ADD: //ADD 7xkk or 8xy4
            paramCheck1 = Chip8AssGetV(&line);
            paramCheck2 = Chip8AssGetV(&line);
            //7xkk
            if (paramCheck2 == OPCODE_PARAM_NULL)
                return 0x7000 | (paramCheck1 << 8) | Chip8AssParseInt(line);
            //Fx1E
            else  if (paramCheck1 == OPCODE_PARAM_I)
                return 0xF000 | (paramCheck1 << 8) | 0x001E;
            //8xy4
            else
                return 0x8004 | (paramCheck1 << 8) | (paramCheck2 << 4);
            break;

        case OPCODE_OR: //OR 8xy1
            return 0x8001 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line)<< 4);
            break;
        
        case OPCODE_AND: //AND 8xy2
            return 0x8002 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line) << 4);
            break;
        
        case OPCODE_XOR: //XOR 8xy3
            return 0x8003 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line) << 4);
            break;

        case OPCODE_SUB: //SUB 8xy5
            return 0x8005 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line) << 4);
            break;
        
        case OPCODE_SHR: //SHR  8xy6
            return 0x8000 | (Chip8AssGetV(&line) << 8) | 0x0006;
            break;
        
        case OPCODE_SUBN: //SUBN 8xy7
            return 0x8007 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line) << 4);
            break;
        
        case OPCODE_SHL: //SHL 8xyE
            return 0x8000 | (Chip8AssGetV(&line) << 8) | 0x000E;
            break;
        
        case OPCODE_RND: //RND Vx, byte
            return 0xC000 | (Chip8AssGetV(&line) << 8) | Chip8AssParseInt(line);
            break;
        
        case OPCODE_DRW://DRW Dxyn
            return 0xD000 | (Chip8AssGetV(&line) << 8) | (Chip8AssGetV(&line) << 4) | Chip8AssParseInt(line);
            break;
        
        case OPCODE_SKP: //SKP Ex9E
            return 0xE000 | (Chip8AssGetV(&line) << 8) | 0x009E;
            break;

        case OPCODE_SKNP: //SKNP ExA1
             return 0xE000 | (Chip8AssGetV(&line) << 8) | 0x00A1;
             break;

        case OPCODE_SCD: //SCD 00Cn
            return 0x00C0 | Chip8AssParseInt(line);
            break;
        
        case OPCODE_SCR: //SCR 00FB
            return 0x00FB;
            break;

        case OPCODE_SCL: //SCL 00FC
            return 0x00FC;
            break;
        
        case OPCODE_EXIT: //EXIT 00FD
            return 0x00FD;
            break;

        case OPCODE_LOW: //LOW 00FE
            return 0x00FE;
            break;

        case OPCODE_HIGH: //HIGH 00FF
            return 0x00FF;
            break;

        default:
            return -1;
    }
}

/**
* tries to match a opcode param,
* If one is found moves the pointer past the param
*
* @param line a pointer to the line pointer 
* @return the index of the param found
*/
int Chip8AssGetV(char **line)
{
    for (int i = 0; i < opcodeParamsCount; i++)
    {
        if (strncmp(*line, opcodePerams[i], strlen(opcodePerams[i])) == 0)
        {
            for(int x = 0; x < strlen(opcodePerams[i]); x++)
                ++*line;
            //movepast the opcode
            ++*line;
    
            //remove any white spaces after the V
            while (**line != '\0' && isspace(**line))
                ++*line;

            return i;
        }
    }

    return OPCODE_PARAM_NULL;
}

/**
* attempts to find a matching lable
* if a lable is not found, it assumes it is a number and converts it
*
* @param line the current string of the line
* @return the parsed address
*/
int Chip8AssGetAddress(char *line)
{
    //check if it is a lable
    while (*line != '\0' && isspace(*line))
        ++line;

    for (int i = 0; i < addressPointersCount; i++)
    {
        if (strncmp(addressPointers[i].name, line, strlen(addressPointers[i].name)) == 0)
            return addressPointers[i].address;
    }

    //not in the lable list so we assume it is a number
    return Chip8AssParseInt(line);
}

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
int Chip8AssParseInt(char *s)
{
    while (*s != '\0' && isspace(*s))
        ++s;
    //number is a hex
    if (s[0] == '#')
        return (int)strtol(&s[1], NULL, 16);
    //number is a binary
    else if (s[0] == '$')
    {
        int binvalue = 0;
        for(int i = 0; i < 8; i++)
        {
            if(s[i+1] == '.' || s[i+1] == '0')
                binvalue = (binvalue << 1);
            else 
                binvalue = (binvalue << 1) + 1;
        }
        return binvalue;
    }

    //number is decimal
    else
        return atoi(s);
}

/**
* Converts a string to upper case
*
* @param sPtr pointer to the string
* @return None
*/
void Chip8AssUppercase( char *sPtr )
{
    while( *sPtr != '\0' )
      {
         *sPtr = toupper( ( unsigned char ) *sPtr );
         ++sPtr;
      }
}