#include "SFML/Graphics.hpp"
#include "chip8.h"

#define PIXEL_SCALE 13

void draw(Chip8& emu, sf::RenderWindow& window);
void updateKeys(Chip8& emu);

int main()
{
    sf::RenderWindow window(sf::VideoMode(GFX_COLS * PIXEL_SCALE, GFX_ROWS * PIXEL_SCALE), "CHIP-8 Emulator");
    window.setFramerateLimit(300);
    sf::Clock clock;
    float refreshSpeed = 1.f / 300.f;

    Chip8 emu;
    emu.init();
    emu.loadFile((char *) "spaceinvaders.ch8");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Refresh
        if (clock.getElapsedTime().asSeconds() >= refreshSpeed)
        {
            // Update key states
            updateKeys(emu);

            // Cycle
            if (!emu.cycle())
                window.close();

            // Draw
            if (emu.drawFlag)
            {
                window.clear(sf::Color::Black);

                draw(emu, window);

                window.display();
                emu.drawFlag = false;
            }

            clock.restart();
        }
    }

    return 0;
};

void draw(Chip8& emu, sf::RenderWindow& window)
{
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
}

void updateKeys(Chip8& emu)
{
    emu.key[0x1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1);
    emu.key[0x2] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num2);
    emu.key[0x3] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num3);
    emu.key[0xC] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num4);

    emu.key[0x4] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q);
    emu.key[0x5] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
    emu.key[0x6] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E);
    emu.key[0xD] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);

    emu.key[0x7] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
    emu.key[0x8] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
    emu.key[0x9] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
    emu.key[0xE] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F);

    emu.key[0xA] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z);
    emu.key[0x0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X);
    emu.key[0xB] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C);
    emu.key[0xF] = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::V);
}