#include <ncurses.h>
#include <panel.h>
#include <sstream>

#include "../include/terminal.h"
#include "../include/instruction.h"

void Terminal::start()
{
    stringstream ss;

    initscr();        
    printw("File name: ");
    refresh();
    char inputbuffer[256]; 
    getstr(inputbuffer);
    string filename;
    ss << inputbuffer;
    ss >> filename;
    clear();
    emu->attach(filename);
    printw("Press s to start...\n");
    char c = getch(); 
    if (c == 's') {
        run();
    }
    endwin();
}

void Terminal::redraw_reg() 
{
    wclear(reg);
    box(reg_border, 0, 0);

    wprintw(reg, emu->to_string().c_str());

    wnoutrefresh(reg_border);
    wnoutrefresh(reg);
}
void Terminal::redraw_code() 
{

    wclear(code);
    Instruction * inst = emu->get_current_inst();
    box(code_border, 0, 0);

    if (inst != nullptr) {
        vector<string> window = inst->get_window();
        for (const auto &w: window) {
            wprintw(code, w.c_str()); 
            wprintw(code, "\n"); 
        }
    }

    wnoutrefresh(code_border);
    wnoutrefresh(code);
}

void Terminal::redraw_memory()
{
    wclear(memory);
    box(memory_border, 0, 0);

    wprintw(memory, emu->mem->to_string().c_str());

    wnoutrefresh(memory_border);
    wnoutrefresh(memory);

}
void Terminal::redraw() 
{

    redraw_reg();
    redraw_code();
    redraw_memory();
    update_panels();
    doupdate();
}

void Terminal::run()
{
    clear();
    reg_border = newwin(12,42, 1, 1);
    reg = newwin(10, 40, 2, 2);
    code_border = newwin(12,42, 1, 43);
    code = newwin(10, 40, 2, 44);
    memory_border = newwin(12,84, 13, 1);
    memory = newwin(10, 80, 14, 2);

    box(reg_border, 0, 0);
    box(code_border, 0, 0);
    box(memory_border, 0, 0);

    reg_panel = new_panel(reg);
    code_panel = new_panel(code);
    memory_panel = new_panel(memory);
    update_panels();
    doupdate();
    
    char input;
    int stopped = 0;
    while(!stopped) {

        redraw();
        input = mvgetch(26, 1);
        if (input == 'q') {
                break;
            } 
            switch (input) {
                case 's':
                    stopped = emu->step();
                    break;
                case 'r':
                    emu->run();
                    stopped = 1;
                    break;
            }
    }
    clear();
    printw("Program complete\n");
    printw("Hit any character to quit\n");
    refresh();
    getch();
}



