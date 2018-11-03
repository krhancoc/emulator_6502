#ifndef __TERMINAL_H_
#define __TERMINAL_H_
#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "emulator.h"
class Terminal {
private:
    WINDOW * reg;
    WINDOW * code;
    WINDOW * memory; 
    WINDOW * reg_border;
    WINDOW * code_border;
    WINDOW * memory_border; 
    WINDOW * menu_window;
    PANEL * reg_panel;
    PANEL * code_panel;
    PANEL * memory_panel;
    PANEL * file_input_panel;
    MENU * menu;
    uint16_t memory_view = 0x0000;
    int stopped = 0;
    Emulator * emu = new Emulator();
public:
    Terminal();
    void start();
    void run();
    void take_input();
    void init_menu();
    void redraw_menu();
    void redraw();
    void redraw_reg();
    void redraw_code();
    void redraw_memory();
    void attach();

    string take_line(string message);
};

#endif
