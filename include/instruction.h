#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "emulator.h"


class Instruction {
protected:
    Emulator * emu;
    int CYCLES = 0;
    int current_cycle = 0;
    uint16_t byte_length = 0;
public:
    virtual ~Instruction() = default;
    virtual void run()=0;
    void execute() {
        run();
        emu->increment_pc(byte_length);
        reset();
    };
    int get_cycle() 
    {
        return current_cycle;
    };
    void dec_cycle()
    {
        current_cycle--;
    };
    void reset()
    {
        current_cycle = CYCLES;
    };
    uint16_t get_byte_length() {
        return byte_length;
    }
};

class Value {
public:
    virtual uint8_t get_value()=0;
};

class Constant : public Value {
    uint8_t value;
public:
    Constant(uint8_t val) : value(val) {};
    uint8_t get_value() 
    {
        return value;
    }
    
};

class Register : public Value {
};

class JMP: public Instruction {
private:
    string label;
public:
    JMP(string label, Emulator * e) : label(label) {
        emu = e;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        emu->jump_to(label);
    }
};

// 2 or 3 byte instruction
class INC : public Instruction {
private:
    Reg target_reg;
public:
    INC(Reg target, Emulator * e) : target_reg(target) {
        emu = e;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        (*emu->quick_map[target_reg])++;
        reset();
    };
};

class Label : public Instruction {
private:
public:
    Label(string value) : label(value) {};
    string label;
    void run(){};
};

class T: public Instruction {
private:
    Reg source;
    Reg target;
public:
    T(Reg target, Reg source, Emulator * e) : source(source), target(target) 
    {
        emu = e;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        *emu->quick_map[target] = *emu->quick_map[source];
    }
};


class ADC: public Instruction {
private:
    Value * value;
    Reg accumulator = Reg::A;
public:
    ADC(Value * v, Emulator * e) : value(v) {
        emu = e;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] + value->get_value();
    }
};
#endif
