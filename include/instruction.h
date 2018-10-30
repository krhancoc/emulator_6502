#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "emulator.h"


class Instruction {
protected:
    int CYCLES = 1;
    int current_cycle = CYCLES;
public:
    virtual void execute()=0;
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
};


class INC : public Instruction {
private:
    Emulator * emu;
    Reg target_reg;
public:
    INC(Reg target, Emulator * emu) : emu(emu), target_reg(target) {
        CYCLES = 1;
        current_cycle = CYCLES;
    };
    void execute()
    {
        cout << "EXECUTE" << endl;
        (*emu->quick_map[target_reg])++;
        reset();
    };
};

class T: public Instruction {
private:
    Emulator * emu;
    int CYCLES = 1;
    int current_cycle = CYCLES;
    Reg source;
    Reg target;
public:
    T(Reg target, Reg source, Emulator * emu) : emu(emu), 
       source(source), target(target) {};
    void execute()
    {
        *emu->quick_map[target] = *emu->quick_map[source];
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

class ADC: public Instruction {
private:
    Emulator * emu;
        Value * value;
    Reg accumulator = Reg::A;
public:
    ADC(Value * v, Emulator * emu) : emu(emu), value(v) {
        CYCLES = 1;
        current_cycle = CYCLES;
    };
    void execute()
    {
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] + value->get_value();
    }
};



#endif
