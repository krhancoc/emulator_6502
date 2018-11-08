#ifndef __TERMINAL_H_
#define __TERMINAL_H_
#include <ncurses.h>
#include <panel.h>
#include <menu.h>

#include "emulator.h"
class Panel {
public:
    WINDOW * border;
    WINDOW * win;
    PANEL * panel;

    Panel(int h, int w, int y, int x) {
        border = newwin(h, w, y, x);
        win = newwin(h-2, w-2,y+1, x+1);
        panel = new_panel(win);
    }
    void clear() {
        wclear(border);
        wclear(win);
    }

    void write(string str) {
        wprintw(win, str.c_str());
    }
    void refresh() {
        box(border, 0, 0); 
        wnoutrefresh(border);        
        wnoutrefresh(win);
    }
    void redraw(string str) {
        clear();
        write(str);
        refresh();
    }
};
class Terminal {
private:

    Panel * reg;
    Panel * display = nullptr;
    Panel * code;
    Panel * memory;

    WINDOW * menu_window;

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
    void redraw_code();
    void redraw_display();
    void attach();

    string take_line(string message);
};

#endif
