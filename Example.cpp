#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <string.h>

#include "Chip8.h"

using namespace std;

Chip8CPU mychip8;

void DrawScreen(sf::RenderWindow *window);
void DrawUI(sf::RenderWindow *window, sf::Font *font);

int modifier = 10;
int width = 64 * modifier + 550;
int height = 32 * modifier + 300;

bool run = true;

int main(int argc, char **argv)
{
    Chip8Reset(&mychip8);


    if (argc != 2 || Chip8LoadRom(&mychip8, argv[1]) == false)
        cout << "Error loading file";

    
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
        // error...
    }
    

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)                
                        exit(0);
                else if (event.key.code == sf::Keyboard::Space)                
                        run = !run;
                else if (!run && event.key.code == sf::Keyboard::N)                
                        Chip8EmulateCycle(&mychip8);

                else if (event.key.code == sf::Keyboard::F1)
                    Chip8SaveState(&mychip8, (char*)"state.c8");
                else if (event.key.code == sf::Keyboard::F2)
                    Chip8LoadState(&mychip8, (char*)"state.c8");


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

        if(run)
            Chip8EmulateCycle(&mychip8);

        //if (mychip8.refreshScreen == true)
        //{
            window.clear();
            DrawUI(&window, &font);
            DrawScreen(&window);
            window.display();
            //mychip8.refreshScreen = false;
        //}
        if (mychip8.playBeep)
        {
            mychip8.playBeep = false;
        }
    }
    return 0;
}

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
    
    for(int i = mychip8.pc - 20; i <= mychip8.pc + 20; i += 2)
        mem << hex << (int)i << ":\t" << hex << (int)mychip8.memory[i] << hex << (int)mychip8.memory[i + 1] << endl;
    
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

    //print out instructions
    stringstream instructions;
    instructions << "Instruction" << endl;
    instructions << "---------------------" << endl;
    instructions << "F1: Save State" << endl;
    instructions << "F2: Load State" << endl << endl << endl;
    instructions << "SPACE: Pause Emulation" << endl;
    instructions << "N:     Step to Next Line" << endl;

    sf::Text instructionsText;
    instructionsText.setFont(*font); 
    instructionsText.setString(instructions.str());
    instructionsText.setCharacterSize(20);
    instructionsText.setColor(sf::Color(173, 173, 173));
    instructionsText.setPosition(20, 350);

    //draw eash object to the screen
    window->draw(instructionsText);
    window->draw(memrectangle);
    window->draw(memText);
    window->draw(text);
    window->draw(rectangle);
}

void DrawScreen(sf::RenderWindow *window)
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
    //image.loadFromMemory(screen, 32 * modifier * 64 * modifier);

    sf::Texture texture;
    texture.create(screenx, screeny);
    texture.update(image);
    //
    //texture.loadFromImage(image, sf::IntRect(0,0,32 * modifier, 64 * modifier));

    sf::Sprite sprite;
    //image.loadFromMemory(screen, 32 * 64 * modifier * 4);
    sprite.setTexture(texture);
    sprite.setScale(modifierFactor,modifierFactor);
    //sprite.SetImage(screen);
    sprite.setPosition(4,4);
    window->draw(sprite);

}




