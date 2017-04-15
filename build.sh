g++ -c Chip8Emulator.cpp  Chip8.c Chip8Disassembler.c Chip8Assembler.c
g++ Chip8.o Chip8Emulator.o Chip8Disassembler.o Chip8Assembler.o -o Chip8Emu -lsfml-graphics -lsfml-window -lsfml-system