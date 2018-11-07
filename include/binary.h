#ifndef __BINARY_H__
#define __BINARY_H__

#include "instruction.h"

class Binary: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths; 
    static unordered_set<addressing_mode> allowed_modes;
public:
    Binary(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};

class ADC: public Binary {
public:
    ADC(Emulator * e, string l) : Binary(e, l) {}
	
    void run()
    {
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = 
		*emu->quick_map[accumulator] + get_value(emu, line, mode);
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
    }
};


class CMP: public Binary {
public:
    CMP(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
	//Needs flags register
    }
};


class CPX: public Binary {
public:
	CPX(Emulator *e, string l): Binary(e, l) {}
	void run() {

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
    }
};


class LDA: public Binary {
public:
    LDA(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = get_value(emu, line, mode);
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
    }
};


class SBC: public Binary {
public:
    SBC(Emulator * e, string l) : Binary(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = 
		*emu->quick_map[accumulator] - get_value(emu, line, mode);
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
