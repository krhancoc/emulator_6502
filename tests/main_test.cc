#include "../include/emulator.h"
#include <assert.h>

using namespace std;

int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/inc_t_test.asm");
    state c = emu->run();   
    cout << emu->to_string() << endl;

    assert (c.x == 2);
    assert (c.a == 2);
    assert (c.y == 2);
    assert (c.y == 2);
    assert (c.pc == 19);
    assert (c.clock_ticks == 35);
    return 0;
};
