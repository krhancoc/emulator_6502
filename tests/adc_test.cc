#include "../include/emulator.h"
#include <assert.h>

using namespace std;

void ass(state check, uint16_t pc, uint8_t a, uint8_t x, uint8_t y) {
    assert(check.a == a);
    assert(check.x == x);
    assert(check.y == y);
}

int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/adc_test.asm");
    emu->mem->write(0x10, 0x05);
    emu->mem->write(0x11, 0x01);
    state check = emu->run();   
    ass(check, 12, 5, 1, 0);
    cout << emu->to_string() << endl;
    cout << emu->mem->to_string(0x0100) << endl;
    return 0;
};
