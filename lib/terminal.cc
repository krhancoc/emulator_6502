#include <ncurses.h>
#include <panel.h>
#include <sstream>
#include <functional>

#include "../include/terminal.h"
#include "../include/instruction.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

string choices[7] = {
    "Attach program",
    "Step",
    "Reset", 
    "Mem GOTO",
    "Run Program",
    "Exit",
    ""
};

void Terminal::init_menu() 
{
    ITEM * items[7];
    for (int i = 0; i < 7; i++) {
        items[i] = new_item(choices[i].c_str(), choices[i].c_str());
    }

    menu = new_menu(items);
    menu_opts_off(menu, O_SHOWDESC);

    menu_window = newwin(3, 100, 1, 1);
    keypad(menu_window, TRUE);

    set_menu_win(menu, menu_window);
    set_menu_sub(menu, derwin(menu_window, 1, 95, 1, 1));
    set_menu_format(menu, 1, 6);
    set_menu_mark(menu, "*");
    box(menu_window, 0, 0);
    refresh();

    post_menu(menu);
    wrefresh(menu_window);
}

Terminal::Terminal() 
{
        start();
}

void Terminal::start()
{
    initscr();        
    init_menu();
    reg_border = newwin(12, 40, 4, 1);
    reg = newwin(10, 35, 5, 2);
    code_border = newwin(12, 59, 4, 42);
    code = newwin(10, 40, 5, 44);
    memory_border = newwin(12,100, 16, 1);
    memory = newwin(10, 80, 17, 2);

    box(reg_border, 0, 0);
    box(code_border, 0, 0);
    box(memory_border, 0, 0);

    reg_panel = new_panel(reg);
    code_panel = new_panel(code);
    memory_panel = new_panel(memory);

    update_panels();
    doupdate();
    redraw();
    take_input();
    endwin();
}

string Terminal::take_line(string message) {
    int rows, cols; 
    getmaxyx(stdscr, rows,cols);
    keypad(menu_window, FALSE);
    move(rows - 1, 0);
    clrtoeol();
    stringstream ss;
    string filename;
    char inputbuffer[256];
    mvprintw(rows - 1, 1, message.c_str());
    refresh();
    mvgetstr(rows - 1, message.length() + 1, inputbuffer);
    ss << inputbuffer;
    ss >> filename;
    keypad(menu_window, TRUE);
    return filename;
}

void Terminal::take_input() 
{
    int c;
    while(( c = wgetch(menu_window)) != KEY_F(1)) {


        switch(c)
        {
            case KEY_LEFT:
                menu_driver(menu, REQ_LEFT_ITEM);
                break;
            case KEY_RIGHT:
                menu_driver(menu, REQ_RIGHT_ITEM);
                break;
            case 10:
                ITEM * cur;
                cur = current_item(menu);
                string menu_item = cur->description.str;
                if (!menu_item.compare(choices[0])) {
                    string filename = take_line("File name: ");
                    emu->attach(filename);
                } else if (!menu_item.compare(choices[1])) {
                    emu->step();
                } else if (!menu_item.compare(choices[2])) {
                    emu->reset();
                } else if (!menu_item.compare(choices[3])) {
                    string val = take_line("Memory location as 16 bit hex: ");
                    stringstream ss;
                    uint16_t x;
                    ss << hex << val;
                    ss >> x;
                    memory_view = x;
                } else if (!menu_item.compare(choices[4])) {
                    emu->run();
                } else if (!menu_item.compare(choices[5])) {
                    return;
                }
                redraw();
                break;
        }
        wrefresh(menu_window);
    } 
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

    wprintw(memory, emu->mem->to_string(memory_view).c_str()); 
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



