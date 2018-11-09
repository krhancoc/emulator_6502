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
        vector<string> arguments;
        string token;
        istringstream args(line); 
        while(getline(args, token, ' ')){
	    arguments.push_back(token);
	}
        label = arguments[1].substr(0, arguments[1].size());

    }
};


template <unsigned int N>
class BRC: public Branch {
public:
    BRC(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    if (!(emu->get_p() & (1 << N)))
		emu->jump_to(label);
    }


};

template <unsigned int N>
class BRS: public Branch {
public:
    BRS(Emulator *e, string l): Branch(e, l) {} 
    void run() {
	    if ((emu->get_p() & (1 << N)))
		emu->jump_to(label);
    }
};

typedef BRC<7> BPL;
typedef BRS<7> BMI;

typedef BRC<6> BVC;
typedef BRS<6> BVS;

typedef BRC<1> BNE;
typedef BRS<1> BEQ;

typedef BRC<0> BCC;
typedef BRS<0> BCS;

#endif
