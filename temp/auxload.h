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
	if (mode == ADDR_ZERX)
		throw "Invalid instruction";

    }
};


class LDX: public AuxloadX {
public:
    LDX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        *emu->quick_map[xreg] = get_value(emu, line, mode); 
	sign_flag_check(*emu->quick_map[xreg]);
    }
};


class STX: public AuxloadX{
public:
    STX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        vector<string> arguments;
        string token;
        istringstream args(line); 
        while(getline(args, token, ',')){
	    arguments.push_back(token);
	}
        address a = get_address(emu, line, mode);
        if (arguments.size() == 2) {
            if (!arguments[1].compare("X")) {
                a += emu->get_x();
            } else if (!arguments[1].compare("Y")) {
                a += emu->get_y();
            } else if (!arguments[1].compare("A")) {
                a += emu->get_a();
            }
        }
        emu->mem->write(a, *emu->quick_map[xreg]); 
    }
};

class AuxloadY: public Auxload {
public:
    AuxloadY(Emulator *e, string l) 
	    : Auxload(e, l) {
	if (mode == ADDR_ZERY)
		throw "Invalid instruction";

    }
};


class LDY: public AuxloadY {
public:
    LDY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
    	Reg xreg = Reg::Y;
        *emu->quick_map[xreg] = get_value(emu, line, mode); 
	sign_flag_check(*emu->quick_map[xreg]);
    }
};


class STY: public AuxloadY {
public:
    STY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
        Reg reg = Reg::Y;
        vector<string> arguments;
        string token;
        istringstream args(line); 
        while(getline(args, token, ',')){
	    arguments.push_back(token);
	}
        address a = get_address(emu, line, mode);
        if (arguments.size() == 2) {
            if (!arguments[1].compare("X")) {
                a += emu->get_x();
            } else if (!arguments[1].compare("Y")) {
                a += emu->get_y();
            } else if (!arguments[1].compare("A")) {
                a += emu->get_a();
            }
        }
        emu->mem->write(a, *emu->quick_map[reg]); 

    }
};

#endif
