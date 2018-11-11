#ifndef __BRANCH_H__
#define __BRANCH_H__

#include "instruction.h"

class JMP: public Instruction {
private:
    Lab * v;
public:

    JMP(Value * v, Emulator * e, string l) : Instruction(e, l), v((Lab *)v) {
        byte_length = 2;
        current_cycle = 3;
    };
    void run()
    {
        emu->jump_to(v->label);
    }

};

#endif

