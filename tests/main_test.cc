#include "../include/emulator.h"

using namespace std;

int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("in.asm");
    emu->run();
};
