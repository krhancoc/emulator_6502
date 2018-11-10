#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <math.h>

#include "emulator.h"

using namespace std; 
typedef uint8_t word;
class Emulator;

class Instruction {
protected:
    Emulator * emu;
    int current_cycle = 0;
    uint16_t byte_length = 0;
    string line = "";
    vector<string> code_window;
public:
    Instruction() {}
    Instruction(Emulator *e, string l): emu(e), line(l) {
    }
    virtual ~Instruction() = default;
    virtual void run()=0;
    int execute();
    int get_cycle() 
    {
        return current_cycle;
    };
    string to_string() 
    {
        return line;
    }
    uint16_t get_byte_length() {
        return byte_length;
    }
    vector<string> get_window() {
        return code_window;
    }
    void set_window(vector<string> w) {
        code_window = w;
    }
};

class Value {
public:
    Mode mode;
    virtual word get_value()=0;
    virtual void set_value(word)=0;
    void indexed() 
    {
        switch (mode) {
            case Mode::ABSOLUTE:
                mode = Mode::ABSOLUTE_INDEXED; 
            case Mode::ZERO_PAGE:
                mode = Mode::ZERO_PAGE_INDEXED;
            case Mode::INDIRECT:
                mode = Mode::INDIRECT_INDEXED;
            default:
                throw "Problem in parsing";
        } 
    }
};

class Mem: public Value {
    word address;
    Emulator * emu;
public:
    Reg offset = Reg::Null;

    Mem(string hex_string, Emulator * emu);

    word get_value();
    void set_value(word n);
};

class Constant : public Value {
    word data;
public:
    Constant(string hex_string);
    Constant(word data) : data(data) 
    {
        mode = Mode::IMMEDIATE;
    };
    word get_value() 
    {
        return data;
    };
    void set_value(word) {};
};

class Register : public Value {
private:
    Reg target;
    Emulator * emu;
public:
    Register(Reg target, Emulator *e) : target(target), emu(e) {
        mode = Mode::REGISTER;
    };
    word get_value();
    void set_value(word data);
};

class Lab : public Value {
public:
    string label;
    Lab(string value) : label(value) {};
    word get_value() { return 0; };
    void set_value(word) {};
};



class Label : public Instruction {
private:
public:
    string label;

    Label(string value, string l) : label(value)
    {
        line = l;
    };
    void run(){};
};



#endif
