#include "types.h"
#include "gdt.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"

void printf(const char* str)
{
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    static uint8_t x = 0, y = 0;
    for (int i = 0; str[i] != '\0'; ++i) {
        switch(str[i]) {
            case '\n' : 
                y ++;
                x = 0;
                break;
            default :
                VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
                x ++;
                break;
        }
        if (x >= 80) {
            x = 0;
            y ++;
        }
        if (y >= 25) {
            for (int y = 0; y < 25; y++) {
                for (int x = 0 ; x < 80; x++) {
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xff00 | ' ');
                }
            }
            x = 0, y = 0;
        }
    }
}

typedef void(*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; i++) {
        (*i)();
    }
}

extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber)
{
    printf((char*)"yaochen yyds!\n");
    printf((char*)"yaochen yyds!");

    GlobalDescriptorTable gdt;
    InterruptManager interrupts(0x20, &gdt);

    KeyBoardDriver keyboard(&interrupts);
    MouseDriver mouse(&interrupts);
    interrupts.Activate();

    while (1);
}