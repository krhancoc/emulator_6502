#ifndef __SHIFT_INCREMENT_H__
#define __SHIFT_INCREMENT_H__

#include "instruction.h"

class Shift: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths;
    static unordered_set<addressing_mode> allowed_modes;
public:
    Shift(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};

class Increment: public Shift {
public:
    Increment(Emulator *e, string l) : Shift(e, l) {
	if (mode == ADDR_ACC)
		throw "Invalid instruction";
    }

};

class LSR: public Shift {
public:
    LSR(Emulator * e, string l) : Shift(e, l) {};
    void run()
    {
    	Reg accumulator = Reg::A;

	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
        	*emu->quick_map[accumulator] = (current_value >> 1); 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value >> 1);
    }
};


class ASL: public Shift {
public:
    ASL(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
        	*emu->quick_map[accumulator] = (current_value << 1); 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value << 1);
    }
};

class ROL: public Shift {
public:
    ROL(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		// Brittle if we change word size, but we won't
		word bit7 = (current_value & 0x80) >> 7;
        	*emu->quick_map[accumulator] = (current_value << 1) | bit7; 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word bit7 = (current_value & 0x80) >> 7;
	emu->mem->write(opaddress, (current_value << 1) | bit7);
    }
};


class ROR: public Shift {
public:
    ROR(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		word bit0 = (current_value & 0x01) << 7;
        	*emu->quick_map[accumulator] = (current_value >> 1) | bit0; 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word bit0 = (current_value & 0x01) << 7;
	emu->mem->write(opaddress, (current_value >> 1) | bit0);
    }
};

class INC: public Increment {
public:
    INC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value + 1);
    }
};


class DEC: public Increment {
public:
    DEC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value - 1);
    }
};

#endif
