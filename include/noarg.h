#ifndef __NOARG_H__
#define __NOARG_H__

class Noarg: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths;
    static unordered_set<addressing_mode> allowed_modes;
public:
    Noarg(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};

class INX: public Noarg {
public:
    INX(Emulator * e, string l) : Noarg(e, l) {};
    void run()
    {
	Reg xreg = Reg::X;
	// Weird way to write it, but the compiler was complaining,
	// will look into it later
	*emu->quick_map[xreg] = *emu->quick_map[xreg] + 1;
    }

};

#endif
