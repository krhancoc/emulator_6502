#include <sstream>

#include "../include/terminal.h"
#include "../include/instruction.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

string choices[7] = {
    "Attach program",
    "Step",
    "Reset", "Mem GOTO",
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
}

enum COLOR {
    BLACK = 0, WHITE = 1, RED = 2, 
    MAGENTA = 3, YELLOW = 4, GREEN = 5,
    CYAN = 6, BLUE = 7
};

void Terminal::start()
{
    initscr();        
    init_menu();


    reg = new Panel(12, 50, 4, 1);
    code = new Panel(12, 50, 4, 51);
    memory = new Panel(22,100, 4 + 12, 1);
    start_color();
    init_pair(COLOR::BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR::WHITE, COLOR_WHITE, COLOR_WHITE);
    init_pair(COLOR::RED, COLOR_RED, COLOR_RED);
    init_pair(COLOR::MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(COLOR::YELLOW, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(COLOR::GREEN, COLOR_GREEN, COLOR_GREEN);
    init_pair(COLOR::CYAN, COLOR_CYAN, COLOR_CYAN);
    init_pair(COLOR::BLUE, COLOR_BLUE, COLOR_BLUE);
    display = new Panel(22, 43, 4 + 12 + 22, 25);

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
                break; case KEY_RIGHT:
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

void Terminal::redraw_code() 
{

    Instruction * inst = emu->get_current_inst();
    code->clear();
    if (inst != nullptr) {
        vector<string> window = inst->get_window();
        for (const auto &w: window) {
            code->write(w.c_str()); 
            code->write("\n"); 
        }
    }
    code->refresh();
}

void Terminal::redraw() 
{
    reg->redraw(emu->to_string().c_str());
    redraw_code();
    memory->redraw(emu->mem->to_string(memory_view).c_str());
    redraw_display();
    update_panels();
    doupdate();
}

void Terminal::redraw_display() 
{
    if (display == nullptr) return;
    display->clear();
    int start = 0xc000;
    for (int i = 0; i < 20; i++) {
        for (int  t = 0; t < 40; t++) {
            uint16_t color = emu->mem->read(start + (i * 16) + t);
            if (color < 7) {
                COLOR c = (COLOR) color;
                wattron(display->win, COLOR_PAIR(c));
                waddch(display->win, ' ');
                wattroff(display->win, COLOR_PAIR(c));
            } else {
                waddch(display->win, ' ');
            }
        }
        waddch(display->win, '\n');
    }
    display->refresh();
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



