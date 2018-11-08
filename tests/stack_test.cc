#include "../include/emulator.h"
#include <assert.h>

using namespace std;

void ass(state check, uint16_t pc, uint8_t a, uint8_t x, uint8_t y, uint8_t p) {
    assert(check.p == p);
}

int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/stack_test.asm");
    state check = emu->run();   
    cout << emu->to_string() << endl;
    ass(check, 12, 5, 0xff, 0, 0x20);
    return 0;
};
