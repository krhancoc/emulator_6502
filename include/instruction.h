#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "emulator.h"


class Instruction {
private:
    int CYCLES = 10;
    int current_cycle = CYCLES;
public:
    virtual void tick()=0;
    virtual void execute()=0;
    int get_cycle() {
        return current_cycle;
    };
    void dec_cycle() {
        current_cycle--;
    };
    void reset() {
        current_cycle = CYCLES;
    };
};


class INC : public Instruction {
private:
    Emulator * emu;
    int CYCLES = 1;
    int current_cycle = CYCLES;
    Reg target_reg;
public:
    INC(Reg target, Emulator * emu) : emu(emu), target_reg(target) {};
    void tick()
    {
    };
    void execute(){
        cout << "EXECUTE" << endl;
        (*emu->quick_map[target_reg])++;
        reset();
    };
};

#endif
