#include <ncurses.h>
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

void Terminal::redraw() {
    clear();
    printw(emu->to_string().c_str());
    Instruction * inst = emu->get_current_inst();
    if (inst != nullptr) {
        vector<string> window = inst->get_window();
        for (const auto &w: window) {
            printw(w.c_str()); 
            printw("\n"); 
        }
    }
    refresh();
}

void Terminal::run()
{
    char input;
    int stopped = 0;
    while(!stopped) {

        redraw();
        input = getch();
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
    redraw();
    printw("Program complete\n");
    printw("Hit any character to quit\n");
    refresh();
    getch();

}



