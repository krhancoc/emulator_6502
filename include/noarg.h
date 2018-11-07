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

class CLC: public Noarg {
public:
	CLC(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] & ~((*emu).p_bit[0]);
	}
};

class SEC: public Noarg {
public:
	SEC(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] | ((*emu).p_bit[0]);
	}
};

class CLI: public Noarg {
public:
	CLI(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] & ~((*emu).p_bit[2]);
	}
};

class SEI: public Noarg {
public:
	SEI(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] | ((*emu).p_bit[2]);
	}
};

class CLV: public Noarg {
public:
	CLV(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] & ~((*emu).p_bit[6]);
	}
};

class CLD: public Noarg {
public:
	CLD(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] & ~((*emu).p_bit[3]);
	}
};

class SED: public Noarg {
public:
	SED(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		*emu->quick_map[preg] = *emu->quick_map[preg] | ((*emu).p_bit[3]);
	}
};

#endif
