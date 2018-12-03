#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <math.h>

#include "emulator.h"

using namespace std; 
typedef uint8_t word;
typedef uint64_t x86op;

struct BCPair {
    int bytes;
    int cycle;
};

struct TranslationSnippet {
    char *translation;
    size_t bytes;
    TranslationSnippet() {};
    TranslationSnippet(void *val, size_t size) : bytes(size) {
	    translation = new char[size];
	    memcpy(translation, val, size);
    }

    ~TranslationSnippet() {
	delete translation;
    }
};

class Instruction {
protected:
    Emulator * emu;
    int current_cycle = 0;
    uint16_t byte_length = 0;
    string line = "";
    vector<string> code_window;
public:
    Instruction() {}
    Instruction(Emulator *e, string l): emu(e), line(l) { }
    virtual ~Instruction() = default;
    virtual void run()=0;
    virtual TranslationSnippet translate()=0;
    int execute();
    int get_cycle() 
    {
        return current_cycle;
    };
    string to_string() 
    {
        return line;
    }
    uint16_t get_byte_length() 
    {
        return byte_length;
    }
    vector<string> get_window() 
    {
        return code_window;
    }
    void set_window(vector<string> w) 
    {
        code_window = w;
    }
    void set_flags(word prev, word next)
    {
        // Check Z flag - If value is zero
        if (next) {
            emu->clr_flag(Flag::Z);
        } else {
            emu->set_flag(Flag::Z);
        }
        
        // Check N flag - if value is Negative
        if ((1 << 7) & next) {
                emu->set_flag(Flag::N);
        } else {
                emu->clr_flag(Flag::N);
        }
    }
};

class Value {
public:
    Value * offset = nullptr;
    Mode mode;
    virtual word get_value()=0;
    virtual void set_value(word)=0;
    void indexed(Value * o) 
    {
        switch (mode) {
            case Mode::ABSOLUTE:
                mode = Mode::ABSOLUTE_INDEXED; 
                break;
            case Mode::ZERO_PAGE:
                mode = Mode::ZERO_PAGE_INDEXED;
                break;
            case Mode::INDIRECT:
                mode = Mode::INDIRECT_INDEXED;
                break;
            default:
                throw "Problem in parsing";
        } 
        offset = o;
    }
};

class Register : public Value {
private:
    Emulator * emu;
public:
    Reg target;
    Register(Reg target, Emulator *e) : target(target), emu(e) {
        mode = Mode::REGISTER;
    };
    word get_value();
    void set_value(word data);
};


class Mem: public Value {
    address addr;
    Emulator * emu;
public:
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
    TranslationSnippet translate() 
    {
        return TranslationSnippet(nullptr, 0);
    }

};



#endif
