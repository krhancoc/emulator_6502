#ifndef __TERMINAL_H_
#define __TERMINAL_H_
#include <ncurses.h>
#include <panel.h>

#include "emulator.h"
class Terminal {
private:
    WINDOW * reg;
    WINDOW * code;
    WINDOW * memory; 
    PANEL * reg_panel;
    PANEL * code_panel;
    PANEL * memory_panel;
    int stopped = 0;
    Emulator * emu = new Emulator();
public:
    Terminal(){
    };
    void start();
    void run();
    void redraw();
};

#endif
