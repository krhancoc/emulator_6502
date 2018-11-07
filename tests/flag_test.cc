#include "../include/emulator.h"
#include <assert.h>
#include <stdio.h>
using namespace std;

void ass(state check, uint8_t p) {
	printf("%d\n", p);
    assert(check.p == p);
}


int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/flag_test.asm");
    cout << emu->to_string() << endl;
    state check = emu->run(); 
    cout << emu->to_string() << endl;
    ass(check, 0x2d);
    return 0;
};
