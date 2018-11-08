#ifndef __NOARG_H__
#define __NOARG_H__

#include "instruction.h"

class Noarg: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths;
    static unordered_set<addressing_mode> allowed_modes;
public:
    Noarg(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
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

class TXS: public Noarg {
public:
	TXS(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg xreg = Reg::X;
		Reg sreg = Reg::S;
		
		*emu->quick_map[sreg] = *emu->quick_map[xreg];	
	}

};

class TSX: public Noarg {
public:
	TSX(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg xreg = Reg::X;
		Reg sreg = Reg::S;
		
		*emu->quick_map[xreg] = *emu->quick_map[sreg];	
	}

};


class PHA: public Noarg {
public:
	PHA(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg areg = Reg::A;
		Reg sreg = Reg::S;
		
		//stack overflow detection has not been enabled, since some of the programs may need this as a trick
		
		emu->mem->write((*emu->mem).STACK_LOW_ADDR + *emu->quick_map[sreg], *emu->quick_map[areg]);
		*emu->quick_map[sreg] = *emu->quick_map[sreg] - 0x1;
	}
};

class PLA: public Noarg {
public:
	PLA(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg areg = Reg::A;
		Reg sreg = Reg::S;

		*emu->quick_map[sreg] = *emu->quick_map[sreg] + 0x1;
		*emu->quick_map[areg] = emu->mem->read((*emu->mem).STACK_LOW_ADDR + *emu->quick_map[sreg]);
	}
};

class PHP: public Noarg {
public:
	PHP(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg preg = Reg::P;
		Reg sreg = Reg::S;

		emu->mem->write((*emu->mem).STACK_LOW_ADDR + *emu->quick_map[sreg], *emu->quick_map[preg] | 0x110000);
		*emu->quick_map[sreg] = *emu->quick_map[sreg] - 0x1;
	}
};

class PLP: public Noarg {
public:
	PLP(Emulator *e, string l) : Noarg(e, l) {};
	void run() {
		Reg sreg = Reg::S;
		Reg preg = Reg::P;

		*emu->quick_map[sreg] = *emu->quick_map[sreg] - 0x1;
		word p_reg_new_val = emu->mem->read((*emu->mem).STACK_LOW_ADDR + *emu->quick_map[sreg]);
		word p_reg_old_val = *emu->quick_map[preg];
		p_reg_new_val &= 0x11001111;
		p_reg_new_val |= (p_reg_old_val & 0x00110000);
		*emu->quick_map[preg] = p_reg_new_val;	
	}


};

template <Reg R, int N>
class INCREMENT: public Noarg {
public:
    INCREMENT(Emulator *e, string l): Noarg(e, l) {}
    void run() {
	*emu->quick_map[R] = *emu->quick_map[R] + N;
    }
};


template <Reg R, Reg S>
class TRANSFER: public Noarg {
public:
    TRANSFER(Emulator *e, string l): Noarg(e, l) {}
    void run() {
	*emu->quick_map[S] = *emu->quick_map[R];
    }
};

typedef TRANSFER<Reg::A, Reg::X> TAX;
typedef TRANSFER<Reg::X, Reg::A> TXA;
typedef TRANSFER<Reg::A, Reg::Y> TAY;
typedef TRANSFER<Reg::Y, Reg::A> TYA;
typedef INCREMENT<Reg::X, 1> INX;
typedef INCREMENT<Reg::X, -1> DEX;
typedef INCREMENT<Reg::Y, 1> INY;
typedef INCREMENT<Reg::Y, -1> DEY;



#endif
