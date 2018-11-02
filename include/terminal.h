#ifndef __TERMINAL_H_
#define __TERMINAL_H_

#include "emulator.h"
class Terminal {
private:
    int stopped = 0;
    Emulator * emu = new Emulator();
public:
    Terminal(){};
    void start();
    void run();
    void redraw();
};

#endif
