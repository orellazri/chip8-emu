#include <stdio.h>
#include "chip8.h"

using namespace std;

int main()
{
	Chip8 emu;
	emu.init();
	emu.loadFile("test.ch8");

	while (1)
    {
	    if (!emu.cycle())
	        break;
    }

	return 0;
}