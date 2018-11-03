#ifndef __TERMINAL_H_
#define __TERMINAL_H_
#include <ncurses.h>
#include <panel.h>

#include "emulator.h"
class Terminal {
private:
    WINDOW * reg_border;
    WINDOW * reg;
    WINDOW * code;
    WINDOW * code_border;
    WINDOW * memory; 
    WINDOW * memory_border; 
    PANEL * reg_panel;
    PANEL * code_panel;
    PANEL * memory_panel;
    uint16_t memory_view = 0x0000;
    int stopped = 0;
    Emulator * emu = new Emulator();
public:
    Terminal(){
    };
    void start();
    void run();
    void redraw();
    void redraw_reg();
    void redraw_code();
    void redraw_memory();
};

#endif
