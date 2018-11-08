#include "../include/emulator.h"
#include <iostream>
#include <assert.h>


using namespace std;
int main() 
{
    Emulator * emu = new Emulator();
    emu->reset();
    emu->attach("test_files/bubble_sort.asm");
    state check = emu->run();   
    cout << emu->to_string() << endl;
    cout << emu->quick_map[Reg::A] << endl;
    return 0;
};
