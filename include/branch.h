#ifndef __BRANCH_H__
#define __BRANCH_H__

#include <regex>
#include <string>

#include "instruction.h"

class Branch: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths; 
    static unordered_set<addressing_mode> allowed_modes;
protected:
    string label;

public:
    Branch(Emulator *e, string l)
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {
	std::regex label_regex("[a-z][A-Za-z0-9]*");

	// Get the label.
	std::smatch m;
	std::regex_search(l, m, label_regex);
	label = m[0].str();	
    }
};

class BMI: public Branch {
public:
    BMI(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (*emu->quick_map[preg] && emu->p_bit[7])
		emu->jump_to(label);

    }
};

class BPL: public Branch {
public:
    BPL(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (!(*emu->quick_map[preg] && emu->p_bit[7]))
		emu->jump_to(label);

    }
};

class BVC: public Branch {
public:
    BVC(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (!(*emu->quick_map[preg] && emu->p_bit[6]))
		emu->jump_to(label);

    }
};

class BVS: public Branch {
public:
    BVS(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (*emu->quick_map[preg] && emu->p_bit[6])
		emu->jump_to(label);

    }
};

class BCC: public Branch {
public:
    BCC(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (!(*emu->quick_map[preg] && emu->p_bit[0]))
		emu->jump_to(label);

    }
};

class BCS: public Branch {
public:
    BCS(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (*emu->quick_map[preg] && emu->p_bit[0])
		emu->jump_to(label);

    }
};

class BNE: public Branch {
public:
    BNE(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (!(*emu->quick_map[preg] && emu->p_bit[1]))
		emu->jump_to(label);

    }
};

class BEQ: public Branch {
public:
    BEQ(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    Reg preg = Reg::P;
	    if (*emu->quick_map[preg] & emu->p_bit[1])
		emu->jump_to(label);

    }
};

#endif
