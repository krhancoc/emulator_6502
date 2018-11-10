#ifndef __BRANCH_H__
#define __BRANCH_H__

#include "instruction.h"

class JMP: public Instruction {
private:
    Lab * v;
public:

    JMP(Value * v, Emulator * e, string l) : v((Lab *)v) {
        line = l;
        emu = e;
        byte_length = 2;
    };
    void run()
    {
        emu->jump_to(v->label);
    }

};

#endif

