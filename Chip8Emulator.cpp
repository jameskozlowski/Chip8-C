/**
* Chip-8 emulator 
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

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <string.h>

#include "Chip8.h"

using namespace std;

//the chip-8 object
Chip8CPU mychip8;

//factor each pixel by this value
const int modifier = 10;

//set screen width and height
const int width = 64 * modifier + 550;
const int height = 32 * modifier + 300;

//if true emulator is running, if false emulator is paused
bool run = true;

//used for displaying the memory in the debugger
int displayMemLocation;

//holds the breakpoint
int breakpoint = -1;

int main(int argc, char **argv)
{
    //reset the CPU
    Chip8Reset(&mychip8);

    //try to load the rom file
    if (argc != 2 || Chip8LoadRom(&mychip8, argv[1]) == false)
    {
        cout << "Error loading file";
        return -1;
    }

    //setup and open a window
    sf::ContextSettings settings;
    settings.depthBits = 0;
    settings.stencilBits = 0;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 3;
    settings.minorVersion = 0;
    
    sf::RenderWindow window(sf::VideoMode(width, height), "Chip8 Emu", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(false);
    //window.setFramerateLimit(60);

    //load a font
    sf::Font font;
    if (!font.loadFromFile("DroidSansMono.ttf"))
    {
        cout << "Error loading Font (DroidSansMono.ttf)";
    }
    
    //main loop
    while (window.isOpen())
    {
        sf::Event event;
        
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            
            //check for key presses
            else if (event.type == sf::Event::KeyPressed)
            {
                //program/debugger keys
                if (event.key.code == sf::Keyboard::Escape)                
                        exit(0);
                else if (event.key.code == sf::Keyboard::Space)                
                        run = !run;
                else if (!run && event.key.code == sf::Keyboard::N)                
                        Chip8EmulateCycle(&mychip8);
                else if (!run && event.key.code == sf::Keyboard::Down)                
                        displayMemLocation += 2;
                else if (!run && event.key.code == sf::Keyboard::Up)                
                        displayMemLocation -= 2;
                else if (!run && event.key.code == sf::Keyboard::PageDown)                
                        displayMemLocation += 16;
                else if (!run && event.key.code == sf::Keyboard::PageUp)                
                        displayMemLocation -= 16;       
                else if (!run && event.key.code == sf::Keyboard::B)
                {
                    if (breakpoint == displayMemLocation)
                        breakpoint = -1;
                    else                
                        breakpoint = displayMemLocation;
                }
                else if (!run && event.key.code == sf::Keyboard::M)
                { 
                    mychip8.pc = displayMemLocation;
                    run = true;               
                }

                //load state keys
                else if (event.key.code == sf::Keyboard::F1)
                    Chip8SaveState(&mychip8, (char*)"state.c8");
                else if (event.key.code == sf::Keyboard::F2)
                    Chip8LoadState(&mychip8, (char*)"state.c8");

                //gamepad keys
                else if (event.key.code == sf::Keyboard::Num1)
                    mychip8.key[0x1] = 1;
                else if (event.key.code == sf::Keyboard::Num2)
                    mychip8.key[0x2] = 1;
                else if (event.key.code == sf::Keyboard::Num3)
                    mychip8.key[0x3] = 1;
                else if (event.key.code == sf::Keyboard::Num4)        
                    mychip8.key[0xC] = 1;
                else if (event.key.code == sf::Keyboard::Q)        
                    mychip8.key[0x4] = 1;
                else if (event.key.code == sf::Keyboard::W)
                    mychip8.key[0x5] = 1;
                else if (event.key.code == sf::Keyboard::E)
                    mychip8.key[0x6] = 1;
                else if (event.key.code == sf::Keyboard::R)
                    mychip8.key[0xD] = 1;
                else if (event.key.code == sf::Keyboard::A)
                    mychip8.key[0x7] = 1;
                else if (event.key.code == sf::Keyboard::S)
                    mychip8.key[0x8] = 1;
                else if (event.key.code == sf::Keyboard::D)
                    mychip8.key[0x9] = 1;
                else if (event.key.code == sf::Keyboard::F)
                    mychip8.key[0xE] = 1;
                else if (event.key.code == sf::Keyboard::Z)
                    mychip8.key[0xA] = 1;
                else if (event.key.code == sf::Keyboard::X)
                    mychip8.key[0x0] = 1;
                else if (event.key.code == sf::Keyboard::C)
                    mychip8.key[0xB] = 1;
                else if (event.key.code == sf::Keyboard::V)
                    mychip8.key[0xF] = 1;
            }

            //key up events to clear gamepad keys
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Num1)
                    mychip8.key[0x1] = 0;
                else if (event.key.code == sf::Keyboard::Num2)
                    mychip8.key[0x2] = 0;
                else if (event.key.code == sf::Keyboard::Num3)
                    mychip8.key[0x3] = 0;
                else if (event.key.code == sf::Keyboard::Num4)        
                    mychip8.key[0xC] = 0;
                else if (event.key.code == sf::Keyboard::Q)        
                    mychip8.key[0x4] = 0;
                else if (event.key.code == sf::Keyboard::W)
                    mychip8.key[0x5] = 0;
                else if (event.key.code == sf::Keyboard::E)
                    mychip8.key[0x6] = 0;
                else if (event.key.code == sf::Keyboard::R)
                    mychip8.key[0xD] = 0;
                else if (event.key.code == sf::Keyboard::A)
                    mychip8.key[0x7] = 0;
                else if (event.key.code == sf::Keyboard::S)
                    mychip8.key[0x8] = 0;
                else if (event.key.code == sf::Keyboard::D)
                    mychip8.key[0x9] = 0;
                else if (event.key.code == sf::Keyboard::F)
                    mychip8.key[0xE] = 0;
                else if (event.key.code == sf::Keyboard::Z)
                    mychip8.key[0xA] = 0;
                else if (event.key.code == sf::Keyboard::X)
                    mychip8.key[0x0] = 0;
                else if (event.key.code == sf::Keyboard::C)
                    mychip8.key[0xB] = 0;
                else if (event.key.code == sf::Keyboard::V)
                    mychip8.key[0xF] = 0;
            }
        }

        //if the emulator is not paused
        if(run)
        {
            //if we are about to process the breakpoint line
            if (mychip8.pc == breakpoint - 2)
                run = false;
            
            Chip8EmulateCycle(&mychip8);
            displayMemLocation = mychip8.pc;
        }

        //display the UI and game screen
        window.clear();
        DrawUI(&window, &font);
        DrawGameScreen(&window);
        window.display();

        //play a beep if needed (not done yet)
        if (mychip8.playBeep)
        {
            mychip8.playBeep = false;
        }
    }
    return 0;
}

/**
* Draws the UI
*
* @param window the SF::RenderWindow
* @return none
*/
void DrawUI(sf::RenderWindow *window, sf::Font *font)
{
    //print out box around screen
    sf::RectangleShape rectangle(sf::Vector2f(648, 330));
    rectangle.setFillColor(sf::Color::Black);
    rectangle.setOutlineThickness(2);
    rectangle.setOutlineColor(sf::Color::Red);
    rectangle.setPosition(2, 2);

    //print out registers
    stringstream os;
    os << "REGISTERS" << endl;
    os << "Reg  " << "Value " << endl << "------------"<< endl;
    
    os << "PC:  " << hex << (int)mychip8.pc << endl << endl;
    os << "I:   " << hex << (int)mychip8.I << endl;
    
    for (int i = 0; i < 16; i++)
        os << "V" << hex << (int) i << ":  " << hex << (int)mychip8.V[i] << endl;
    
    os << endl;
    os << "DT:  " << hex << (int)mychip8.delayTimer << endl;
    os << "ST:  " << hex << (int)mychip8.soundTimer << endl;
    
    sf::Text text;
    text.setFont(*font); 
    text.setString(os.str());
    text.setCharacterSize(20);
    text.setColor(sf::Color(173, 173, 173));
    text.setPosition(690, 20);

    //print out memory
    stringstream mem;
    mem << "MEMORY" << endl;
    mem << "Loc     " << "Value " << endl << "----------------------"<< endl;
    

    for(int i = displayMemLocation - 20; i <= displayMemLocation + 20; i += 2)
        if (i == breakpoint)
            mem << "* " << hex << (int)i << ":\t" << hex << (int)mychip8.memory[i] << hex << (int)mychip8.memory[i + 1] << endl;
        else
            mem << "  " << hex << (int)i << ":\t" << hex << (int)mychip8.memory[i] << hex << (int)mychip8.memory[i + 1] << endl;
    
    sf::Text memText;
    memText.setFont(*font); 
    memText.setString(mem.str());
    memText.setCharacterSize(20);
    memText.setColor(sf::Color(173, 173, 173));
    memText.setPosition(870, 20);

    //current memory marker
    sf::RectangleShape memrectangle(sf::Vector2f(300, 20));
    memrectangle.setFillColor(sf::Color(1, 112, 10));
    memrectangle.setPosition(860, 323);

    //printout keypad
    stringstream keypadstream;
    keypadstream << "KEYPAD INPUT" << endl;
    keypadstream << "-----------------" << endl;
    keypadstream << "| 1 | 2 | 3 | 4 |" << endl;
    keypadstream << "| Q | W | E | R |" << endl;
    keypadstream << "| A | S | D | F |" << endl;
    keypadstream << "| Z | X | C | V |" << endl;
    keypadstream << "-----------------" << endl;

    sf::Text keypadText;
    keypadText.setFont(*font); 
    keypadText.setString(keypadstream.str());
    keypadText.setCharacterSize(14);
    keypadText.setColor(sf::Color(173, 173, 173));
    keypadText.setPosition(20, 350);

    //print out instructions
    stringstream instructions;
    instructions << "Instruction" << endl;
    instructions << "---------------------" << endl;
    instructions << "ESC: Quit" << endl;
    instructions << "F1: Save State" << endl;
    instructions << "F2: Load State" << endl << endl;
    instructions << "SPACE: Pause/Run Emulation" << endl;
    instructions << "N: Setp forward" << endl << endl;
    instructions << "While paused:" << endl;
    instructions << "Up/Down: Move memory location" << endl;
    instructions << "PgUp/PgDn: Move memory location page " << endl;
    instructions << "B: Set break point" << endl;
    instructions << "M: Run from here (Not recomended)" << endl;

    sf::Text instructionsText;
    instructionsText.setFont(*font); 
    instructionsText.setString(instructions.str());
    instructionsText.setCharacterSize(14);
    instructionsText.setColor(sf::Color(173, 173, 173));
    instructionsText.setPosition(250, 350);

    //draw eash object to the screen
    window->draw(keypadText);
    window->draw(instructionsText);
    window->draw(memrectangle);
    window->draw(memText);
    window->draw(text);
    window->draw(rectangle);
}

/**
* Draws the game screen in the game screen area
*
* @param window the SF::RenderWindow
* @return none
*/
void DrawGameScreen(sf::RenderWindow *window)
{
    int screeny = 32;
    int screenx = 64;
    int modifierFactor = modifier;

    if (mychip8.extendedGraphicsMode == true)
    {
        screeny = 64;
        screenx = 128;
        modifierFactor = modifierFactor / 2;
    }

    sf::Uint8 screen[screeny * screenx * 4];
    memset (screen, 0, screeny * screenx * 4);
     for(int y = 0; y < screeny; y++)      
        for(int x = 0; x < screenx; x++)
            if(mychip8.videoMemory[(y*screenx) + x] == 1)
            {
                screen[(x * 4) + (y * screenx * 4) + 0] = 255;
                screen[(x * 4) + (y * screenx * 4) + 1] = 255;
                screen[(x * 4) + (y * screenx * 4) + 2] = 255;
                screen[(x * 4) + (y * screenx * 4) + 3] = 255;
            }
            
    sf::Image image;
    image.create(screenx, screeny, screen);

    sf::Texture texture;
    texture.create(screenx, screeny);
    texture.update(image);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sprite.setScale(modifierFactor,modifierFactor);
    sprite.setPosition(4,4);

    window->draw(sprite);
}