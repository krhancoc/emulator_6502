#include "../include/emulator.h"
#include <iostream>
#include <assert.h>

using namespace std;
int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/store_load_test.asm");
    emu->run();
    cout << emu->to_string() << endl;
    cout << emu->mem->to_string(0x0100) << endl;
    return 0;
};
