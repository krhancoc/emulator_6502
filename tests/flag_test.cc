#include "../include/emulator.h"
#include <assert.h>

using namespace std;

void ass(state check, uint8_t p) {
    assert(check.p == p);
}


int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/flag_test.asm");
    cout << emu->to_string() << endl;
    state check = emu->run(); 
    ass(check, 0x2d);
    cout << emu->to_string() << endl;
    return 0;
};
