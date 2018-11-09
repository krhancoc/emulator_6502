#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <regex>
#include <unordered_map>
#include <unordered_set>

#include "emulator.h"
#include <math.h>

typedef uint8_t word;

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
    int execute() {
        run();
        emu->increment_pc(byte_length);
        return current_cycle;
    };
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
    virtual word get_value()=0;
};

class Constant : public Value {
    word data;
public:
    Constant(word data) : data(data) {};
    word get_value() 
    {
        return data;
    };
};

class Register : public Value {
private:
    Reg target;
    Emulator * emu;
public:
    Register(Reg target, Emulator *e) : target(target), emu(e) {};
    word get_value() { return *emu->quick_map[target]; }
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

class T: public Instruction {
private:
    Reg source;
    Reg target;
public:
    T(Reg target, Reg source, Emulator * e, string l) : source(source), target(target) 
    {
        line = l;
        emu = e;
        byte_length = 2;
    };
    void run()
    {
        *emu->quick_map[target] = *emu->quick_map[source];
    }
};


class JMP: public Instruction {
private:
    string label;
public:
    JMP(string label, Emulator * e, string l) : label(label) {
        line = l;
        emu = e;
        byte_length = 2;
    };
    void run()
    {
        emu->jump_to(label);
    }
};
#endif
