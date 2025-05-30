#pragma once
#include "Arduino.h"
// #include <cstddef>

typedef unsigned char byte;

class Chip8
{
public:
    Chip8();
    static constexpr unsigned short SCREEN_WIDTH = 64;
    static constexpr unsigned short SCREEN_HEIGHT = 32;
    bool beeping;
    bool *get_display();
    void keypress(byte idx, bool pressed);
    void keypressReset();
    void load_byte(byte data, size_t size);
    void reset();
    void tick();
    void tick_timers();

private:
    void push(unsigned short item);
    unsigned short pop();
    unsigned short fetch();
    void execute(unsigned short op);

    static constexpr short RAM_SIZE = 4096;
    static constexpr short NUM_REGS = 16;
    static constexpr short STACK_SIZE = 16;
    static constexpr short NUM_KEYS = 16;
    static constexpr unsigned short START_ADDR = 0x200;

    static const short FONTSET_SIZE = 80;
    const byte FONTSET[FONTSET_SIZE] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };

    unsigned short pc; // program counter
    byte ram[RAM_SIZE] = {};
    bool screen[SCREEN_WIDTH * SCREEN_HEIGHT] = {};
    byte v_reg[NUM_REGS] = {};
    unsigned short i_reg;
    unsigned short sp; // stack pointer
    unsigned short stack[STACK_SIZE] = {};
    bool keys[NUM_KEYS] = {};
    byte dt; // delay timer
    byte st; // sound timer
};
