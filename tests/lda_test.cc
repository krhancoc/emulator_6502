#include "../include/emulator.h"
#include <iostream>
#include <assert.h>

using namespace std;
int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/lda_test.asm");
    emu->run();
    cout << emu->to_string() << endl;
    return 0;
};
