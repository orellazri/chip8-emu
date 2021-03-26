#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

void Chip8::init()
{
    PC = 0x200;
    opcode = 0;
    I = 0;
    SP = 0;

    // Set all array cells to zero
    memset(memory, 0, sizeof(byte) * MEMORY_SIZE);
    memset(stack, 0, sizeof(word) * STACK_SIZE);
    memset(V, 0, sizeof(byte) * V_SIZE);
    memset(gfx, 0, sizeof(byte) * GFX_SIZE);
    memset(key, 0, sizeof(byte) * KEY_SIZE);

    // Load fontset
    for (int i = 0; i < FONT_SIZE; i++)
        memory[i] = fontset[i];

    drawFlag = true;

    srand(time(NULL));
}

void Chip8::loadFile(char *fileName)
{
    byte buffer[10];
    FILE *file = fopen(fileName, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open file");
        exit(1);
    }

    fread(&memory[0x200], 1, MAX_GAME_SIZE, file);

    fclose(file);
}

bool Chip8::cycle()
{
    // Fetch opcode. Merge two bytes (PC and PC+1) into the opcode word variable
    opcode = memory[PC] << 8 | memory[PC + 1];

    if (!decodeOpcode())
        return false;

    // Update timers
    if (delay_timer > 0)
        delay_timer--;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
        {
            printf("BEEP\n");
        }
        sound_timer--;
    }

    return true;
}

bool Chip8::decodeOpcode()
{
    byte x, y, n, nn;
    word nnn;
    x = (opcode >> 8) & 0x000F; // Lower 4 bits of the high byte
    y = (opcode >> 4) & 0x000F; // Lower 4 bits of the high byte
    n = opcode & 0x000F; // Lowest 4 bits
    nn = opcode & 0x00FF; // Lowest 8 bits
    nnn = opcode & 0x0FFF; // Lowest 12 bits

    // Check the first 4 bits
    switch (opcode & 0xF000)
    {
        case 0x0000:
            switch (nn)
            {
                case 0x00E0: // 00E0
                    memset(gfx, 0, sizeof(byte) * GFX_SIZE);
                    drawFlag = true;
                    PC += 2;
                    break;
                case 0x00EE: // 00EE
                    PC = stack[--SP];
                    //PC += 2; // TODO: Check if this is needed
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
                    return false;
            }
            break;
        case 0x1000: // 1NNN
            PC = nnn;
            break;
        case 0x2000: // 2NNN
            stack[SP++] = PC + 2;
            PC = nnn;
            break;
        case 0x3000: // 3NNN
            PC += (V[x] == nn) ? 4 : 2;
            break;
        case 0x4000: // 4NNN
            PC += (V[x] != nn) ? 4 : 2;
            break;
        case 0x5000: // 5XY0
            PC += (V[x] == V[y]) ? 4 : 2;
            break;
        case 0x6000: // 6XNN
            V[x] = nn;
            PC += 2;
            break;
        case 0x7000: // 7XNN
            V[x] += nn;
            PC += 2;
            break;
        case 0x8000: // 8XYN
            switch (n)
            {
                case 0x0: // 8XY0
                    V[x] = V[y];
                    break;
                case 0x01: // 8XY1
                    V[x] = V[x] | V[y];
                    break;
                case 0x02: // 8XY2
                    V[x] = V[x] & V[y];
                    break;
                case 0x03: // 8XY3
                    V[x] = V[x] ^ V[y];
                    break;
                case 0x04: // 8XY4
                    V[0xF] = (((int) V[x] + (int) V[y]) > 255) ? 1 : 0; // Carry
                    V[x] += V[y];
                    break;
                case 0x05: // 8XY5
                    V[0xF] = (V[x] > V[y]) ? 1 : 0; // Borrow
                    V[x] -= V[y];
                    break;
                case 0x06: // 8XY6
                    V[0xF] = V[x] & 0x1;
                    V[x] = (V[x] >> 1);
                    break;
                case 0x07: // 8XY7
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    break;
                case 0x0E: // 8XYE
                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] = (V[x] << 1);
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
                    return false;
            }

            PC += 2;
            break;
        case 0x9000: // 9XY0
            if (n != 0x0)
            {
                fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
                return false;
            }
            PC += (V[x] != V[y]) ? 4 : 2;
            break;
        case 0xA000: // ANNN
            I = nnn;
            PC += 2;
            break;
        case 0xB000: // BNNN
            PC = V[0] + nnn;
            break;
        case 0xC000: // CXNN
            V[x] = (rand() % 256) & nn;
            PC += 2;
            break;
        case 0xD000: // DXYN
        {
            x = V[(opcode & 0x0F00) >> 8];
            y = V[(opcode & 0x00F0) >> 4];

            V[0xF] = 0;
            byte pixel;

            for (byte yline = 0; yline < n; yline++)
            {
                pixel = memory[I + yline];
                for (byte xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        if (gfx[(x + xline + ((y + yline) * GFX_COLS))] == 1)
                            V[0xF] = 1;
                        gfx[x + xline + ((y + yline) * GFX_COLS)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            PC += 2;
            break;
        }
        case 0xE000:
            switch (nn)
            {
                case 0x9E: // EX9E
                    PC += (key[V[x]]) ? 4 : 2;
                    break;
                case 0xA1: // EXA1
                    PC += (!key[V[x]]) ? 4 : 2;
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
                    return false;
            }
            break;
        case 0xF000:
            switch (nn)
            {
                case 0x07: // FX07
                    V[x] = delay_timer;
                    PC += 2;
                    break;
                case 0x0A: // FX0A
                    log("Waiting for key instruction...");
                    while (true)
                    {
                        for (int i = 0; i < KEY_SIZE; i++)
                        {
                            if (key[i])
                            {
                                V[x] = i;
                                goto got_key_press;
                            }
                        }
                    }
                    got_key_press:
                    PC += 2;
                    break;
                case 0x15: // FX15
                    delay_timer = V[x];
                    PC += 2;
                    break;
                case 0x18: // FX18
                    sound_timer = V[x];
                    PC += 2;
                    break;
                case 0x1E: // FX1E
                    V[0xF] = (I + V[x] > 0xfff) ? 1 : 0; // TODO: Might not need this. Wiki says VF not affected
                    I += V[x];
                    PC += 2;
                    break;
                case 0x29: // FX29
                    I = FONTSET_BYTES_PER_CHAR * V[x];
                    PC += 2;
                    break;
                case 0x33: // FX33
                    memory[I]     = (V[x] % 1000) / 100;
                    memory[I + 1] = (V[x] % 100) / 10;
                    memory[I + 2] = (V[x] % 10);
                    PC += 2;
                    break;
                case 0x55: // FX55
                    for (int i = 0; i <= x; i++)
                        memory[I + i] = V[i];
                    I += x + 1;
                    PC += 2;
                    break;
                case 0x65: // FX65
                    for (int i = 0; i <= x; i++)
                        V[i] = memory[I + i];
                    I += x + 1;
                    PC += 2;
                    break;
                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
                    return false;
            }
            break;
        default:
            fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            return false;
    }

    return true;
}