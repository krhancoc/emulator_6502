#ifndef __BIT_H__
#define __BIT_H__

#include "instruction.h"

class BIT: public InstructionGroup {
    static unordered_map<addressing_mode, size_t> instruction_lengths; 
    static unordered_set<addressing_mode> allowed_modes;
public:
    BIT(Emulator *e, string l)
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
    void run() {
	word value = get_value(emu, line, mode);

	// Set zero value as if we ANDed with the accumulator,
	// copy the two upper bits of the value to the S, V flags
	Reg preg = Reg::P;
	sign_flag_check(value & emu->get_a());

	*emu->quick_map[preg] &= 0x3F;
	*emu->quick_map[preg] |= (value & 0xC0);

    }
};

#endif
