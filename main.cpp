#include <stdio.h>

#include "SFML/Graphics.hpp"
#include "chip8.h"

#define PIXEL_SCALE 13

using namespace std;

int main()
{
    sf::RenderWindow window(sf::VideoMode(GFX_COLS * PIXEL_SCALE, GFX_ROWS * PIXEL_SCALE), "CHIP-8 Emulator");
    window.setFramerateLimit(60);
    sf::Clock clock;
    float refreshSpeed = 1.f/10.f;

	Chip8 emu;
	emu.init();
	emu.loadFile((char*) "test2.ch8");

	while (window.isOpen())
    {
	    sf::Event event;
	    while (window.pollEvent(event))
        {
	        if (event.type == sf::Event::Closed)
	            window.close();
        }

	    if (clock.getElapsedTime().asSeconds() >= refreshSpeed)
        {
            if (!emu.cycle())
                window.close();

            if (emu.drawFlag) {
                window.clear(sf::Color::Black);
                sf::RectangleShape rectangle(sf::Vector2f(PIXEL_SCALE, PIXEL_SCALE));

                for (int y = 0; y < GFX_ROWS; y++)
                {
                    for (int x = 0; x < GFX_COLS; x++)
                    {
                        if (emu.gfx[x + (GFX_COLS * y)] > 0)
                        {
                            rectangle.setPosition(sf::Vector2f((float) x * PIXEL_SCALE, (float) y * PIXEL_SCALE));
                            window.draw(rectangle);
                        }
                    }
                }

                window.display();
                emu.drawFlag = false;
            }

            clock.restart();
        }
    }

	return 0;
};