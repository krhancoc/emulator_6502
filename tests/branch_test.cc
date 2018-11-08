#include "../include/emulator.h"
#include <iostream>
#include <assert.h>

void ass(state check, uint8_t a) {
    assert(check.a == a);
}

using namespace std;
int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/branch_test.asm");
    state check = emu->run();   
    ass(check,  0xBB);
    cout << emu->to_string() << endl;
    cout << emu->quick_map[Reg::A] << endl;
    return 0;
};
