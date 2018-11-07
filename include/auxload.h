#ifndef __AUXLOAD_H__
#define __AUXLOAD_H__

#include "instruction.h"

class Auxload: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths;
    static unordered_set<addressing_mode> allowed_modes;
public:
    Auxload(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};

class AuxloadX: public Auxload {
public:
    AuxloadX(Emulator *e, string l) 
	    : Auxload(e, l) {
	if (mode == ADDR_ABSX || mode == ADDR_ZERX)
		throw "Invalid instruction";

    }
};


class LDX: public AuxloadX {
public:
    LDX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        *emu->quick_map[xreg] = emu->mem->read(get_address(emu, line, mode)); 
    }
};


class STX: public AuxloadX{
public:
    STX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        emu->mem->write(get_address(emu, line, mode), *emu->quick_map[xreg]); 
    }
};

class AuxloadY: public Auxload {
public:
    AuxloadY(Emulator *e, string l) 
	    : Auxload(e, l) {
	if (mode == ADDR_ABSY || mode == ADDR_ZERY)
		throw "Invalid instruction";

    }
};


class LDY: public AuxloadY {
public:
    LDY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
    	Reg yreg = Reg::Y;
        emu->mem->write(get_address(emu, line, mode), *emu->quick_map[yreg]); 
    }
};


class STY: public AuxloadY {
public:
    STY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
    	Reg yreg = Reg::Y;
        *emu->quick_map[yreg] = emu->mem->read(get_address(emu, line, mode)); 
    }
};

#endif
