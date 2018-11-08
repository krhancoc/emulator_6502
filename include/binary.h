#ifndef __BINARY_H__
#define __BINARY_H__

#include "instruction.h"

class Binary: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths; 
    static unordered_set<addressing_mode> allowed_modes;

protected:
    void binary_flag_checks(ssize_t result) {

	Reg preg = Reg::P;
	// Check for zero value
	if (result)
		*emu->quick_map[preg] |= (emu->p_bit)[1];
	else 
		*emu->quick_map[preg] &= ~(emu->p_bit)[1];

	// Check for negative value
	if (result & 0x80)
		*emu->quick_map[preg] |= (emu->p_bit)[7];
	else
		*emu->quick_map[preg] &= ~(emu->p_bit)[7];
    }
public:
    Binary(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};


class Addition: public Binary {
protected:
	void addition_flag_checks(ssize_t result) { 
	    Reg preg = Reg::P;

		binary_flag_checks(result);

		// Check for carry 
		if (result > 0xFF)
			*emu->quick_map[preg] |= (emu->p_bit)[0];
		else 
			*emu->quick_map[preg] &= ~(emu->p_bit)[0];

		// Check for overflow
		if (result > 127 || result < -128)
			*emu->quick_map[preg] |= (emu->p_bit)[6];
		else 
			*emu->quick_map[preg] &= ~(emu->p_bit)[6];
	}
public:
    Addition(Emulator *e, string l) : Binary(e, l) {}
};

class ADC: public Addition {
public:
    ADC(Emulator * e, string l) : Addition(e, l) {}
	
    void run()
    {
    	Reg accumulator = Reg::A;
    	Reg preg = Reg::P;

        ssize_t result = *emu->quick_map[accumulator] + get_value(emu, line, mode) 
                        + (*emu->quick_map[preg] & 0x01);

        addition_flag_checks(result);

        *emu->quick_map[accumulator] = result & 0xFF;
    }
};

class AND: public Binary {
public:
    AND(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = 
		*emu->quick_map[accumulator] & get_value(emu, line, mode);

	    binary_flag_checks(*emu->quick_map[accumulator]);
    }
};


class CMP: public Addition {
public:
    CMP(Emulator * e, string l) : Addition(e, l) {}
    void run()
    {
        // Do a subtraction without saving the result, just for the flags.
	    Reg preg = Reg::P;
    	Reg accumulator = Reg::A;

        ssize_t result = *emu->quick_map[accumulator] - get_value(emu, line, mode) 
                        - (1 - (*emu->quick_map[preg] & 0x01));
        addition_flag_checks(result);
    }
};


class EOR: public Binary {
public:
    EOR(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = 
		*emu->quick_map[accumulator] ^  get_value(emu, line, mode);

        binary_flag_checks(*emu->quick_map[accumulator]);
    }
};


class LDA: public Binary {
public:
    LDA(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;

        word result = get_value(emu, line, mode);
        *emu->quick_map[accumulator] = result;

        binary_flag_checks(result);
    }
};


class ORA: public Binary {
public:
    ORA(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = 
		*emu->quick_map[accumulator] | get_value(emu, line, mode);

        binary_flag_checks(*emu->quick_map[accumulator]);
    }
};


class SBC: public Addition {
public:
    SBC(Emulator * e, string l) : Addition(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
    	Reg preg = Reg::P;

        ssize_t result = *emu->quick_map[accumulator] - get_value(emu, line, mode) 
                        - (1 - (*emu->quick_map[preg] & 0x01));

        addition_flag_checks(result);
        *emu->quick_map[accumulator] = result & 0xFF;
    }
};


class STA: public Binary {
public:
    STA(Emulator * e, string l) : Binary(e, l) {};
    void run()
    {
    	Reg accumulator = Reg::A;
        emu->mem->write(get_address(emu, line, mode), *emu->quick_map[accumulator]); 
    }
};

#endif
