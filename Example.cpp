#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>

#include "Chip8.h"

using namespace std;

Chip8CPU mychip8;

void DrawScreen(sf::RenderWindow * window);


int modifier = 10;
int width = 64 * modifier;
int height = 32 * modifier;


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

				//else if (event.key.code == sf::Keyboard::F1)
				//	mychip8.SaveState("state.c8");
				//else if (event.key.code == sf::Keyboard::F2)
				//	mychip8.LoadState("state.c8");
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

        Chip8EmulateCycle(&mychip8);

		if (mychip8.refreshScreen == true)
		{
			//window.clear();
			DrawScreen(&window);
			window.display();
			mychip8.refreshScreen = false;
		}
		if (mychip8.playBeep)
		{
			mychip8.playBeep = false;
		}
    }
	return 0;
}



void DrawScreen(sf::RenderWindow * window)
{
	int screeny = 32;
	int screenx = 64;
	modifier = 10;

	if (mychip8.extendedGraphicsMode == true)
	{
		screeny = 64;
		screenx = 128;
		modifier = 5;
	}
	// Draw
	for(int y = 0; y < screeny; ++y)		
		for(int x = 0; x < screenx; ++x)
		{
			if(mychip8.videoMemory[(y*screenx) + x] == 0) 
			{
				sf::RectangleShape rectangle(sf::Vector2f(modifier, modifier));
				rectangle.setFillColor(sf::Color::Black);
				rectangle.setPosition(x * modifier, y * modifier);
				window->draw(rectangle);
			}
			else			
			{
				sf::RectangleShape rectangle(sf::Vector2f(modifier, modifier));
				rectangle.setFillColor(sf::Color::White);
				rectangle.setPosition(x * modifier, y * modifier);
				window->draw(rectangle);
			}

		}
}



