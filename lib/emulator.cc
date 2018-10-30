#include "../include/emulator.h"

#include "../include/instruction.h"

void Emulator::step(){
    if (current_inst->get_cycle()) {
        current_inst->dec_cycle();
    } else {
        current_inst->execute();
        current_inst = inst[int(pc / 2)];
        pc += 2;
    }
};

