#include "../include/emulator.h"
#include <iostream>
#include <assert.h>

using namespace std;

void ass(state check, uint16_t pc, uint8_t a, uint8_t x, uint8_t y) {
//    assert(check.pc == pc);
    assert(check.a == a);
    assert(check.x == x);
    assert(check.y == y);
}

int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/store_load_test.asm");
    state check = emu->run();   
    cout << emu->to_string() << endl;
    cout << emu->mem->to_string(0x0100) << endl;
    return 0;
};
