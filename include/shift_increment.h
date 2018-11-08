#ifndef __SHIFT_INCREMENT_H__
#define __SHIFT_INCREMENT_H__

#include "instruction.h"

class Shift: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths;
    static unordered_set<addressing_mode> allowed_modes;
protected:
    void carry_flag_check(word value) {
	Reg preg = Reg::P;

	if (value)
		*emu->quick_map[preg] |= (emu->p_bit)[0];
	else
		*emu->quick_map[preg] &= ~(emu->p_bit)[0];
    }
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
	word result = current_value >> 1;
	emu->mem->write(opaddress, result);
	
	sign_flag_check(result);
	carry_flag_check(current_value & 0x01);
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
	word result = current_value << 1;
	emu->mem->write(opaddress, result);

	sign_flag_check(result);
	carry_flag_check(current_value & 0x80);
    }
};

class ROL: public Shift {
public:
    ROL(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	word result; 
	word bit7;
	
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		// Brittle if we change word size, but we won't
		bit7 = (current_value & 0x80) >> 7;
		result = (current_value << 1) | bit7;
        	*emu->quick_map[accumulator] = result; 
		return;
	} else {
		address opaddress = get_address(emu, line, mode);
		word current_value = get_value(emu, line, mode);
		bit7 = (current_value & 0x80) >> 7;
		result = (current_value << 1) | bit7;
		emu->mem->write(opaddress, result);
	}

	sign_flag_check(result);
	carry_flag_check(bit7);
    }
};


class ROR: public Shift {
public:
    ROR(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	word result;
	word bit0;

	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		bit0 = (current_value & 0x01);
		result = (current_value >> 1) | (bit0 << 7);
        	*emu->quick_map[accumulator] = result; 
		return;
	} else {
		address opaddress = get_address(emu, line, mode);
		word current_value = get_value(emu, line, mode);
		bit0 = current_value & 0x01;
		result = (current_value >> 1) | (bit0 << 7);
		emu->mem->write(opaddress, result);
	}

	sign_flag_check(result);
	carry_flag_check(bit0);
    }
};

class INC: public Increment {
public:
    INC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word result = current_value + 1;
	emu->mem->write(opaddress, result);
	sign_flag_check(result);
    }
};


class DEC: public Increment {
public:
    DEC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word result = current_value - 1;
	emu->mem->write(opaddress, result);
	sign_flag_check(result);
    }
};

#endif
