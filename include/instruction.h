#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <regex>
#include <unordered_map>
#include <unordered_set>

#include "emulator.h"

// XXX Used for masks, probably belongs in a class 
#define MASK(x) ((word) ((1L << x) - 1)

enum addressing_mode {
	ADDR_IMM,
	ADDR_ACC,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ABSY,
	ADDR_ZERA,
	ADDR_ZERX,
	ADDR_ZERY,
}; 


// XXX Here temporarily
static string prefix = "\\$";
static string word_str = "([0-9a-fA-F]{2})";
static string address_str = "([0-9a-fA-F]{4})";
static string endline_str = "\\s*";

static addressing_mode parse_addr_mode(string argument)
{

	std::regex word_regex(word_str);
	std::regex address_regex(address_str);
	std::regex immediate("\\s*#" + prefix + word_str + endline_str);
	std::regex absa("\\s*" + prefix + address_str + endline_str);
	std::regex absx("\\s*" + prefix + address_str + "\\s*,\\s*X" + "\\s*");
	std::regex absy("\\s*" + prefix + address_str + "\\s*,\\s*Y" + "\\s*");
	std::regex zera("\\s*" + prefix + word_str + endline_str);
	std::regex zerx("\\s*" + prefix + word_str + "\\s*,\\s*X" + "\\s*");


	if (std::regex_match(argument, immediate)) {
		return ADDR_IMM;
	} else if (std::regex_match(argument, absa)) {
		return ADDR_ABSA;
	} else if (std::regex_match(argument, absx)) {
		return ADDR_ABSX;
	} else if (std::regex_match(argument, absy)) {
		return ADDR_ABSY;
	} else if (std::regex_match(argument, zera)) {
		return ADDR_ZERA;
	} else if (std::regex_match(argument, zerx)) {
		return ADDR_ZERX;
	} else {
		throw "Invalid instruction";
	}

}

// We have already checked if the regex checks will succeed, when
// we parsed the instruction the first time.
static word get_value(Emulator *emu, string argument, addressing_mode mode)
{
	std::smatch m;
	std::regex word_regex(word_str);
	std::regex address_regex(address_str);

	//Remove the opcode, because it may contain a hex value (e.g. ADC)
	auto arg = argument.substr(3);

	// XXX Very ugly, can be made prettier with defines
	switch(mode) {
	case ADDR_IMM:
	case ADDR_ZERA:
	case ADDR_ZERX:
	case ADDR_ZERY:
		std::regex_search(arg, m, word_regex);
		break;
	case ADDR_ABSA:
	case ADDR_ABSX:
	case ADDR_ABSY:
		std::regex_search(arg, m, address_regex);
		break;
	
	default:
		throw "Invalid addressing mode";
	}

	string result = m[0].str();
	auto num = stoi(result, nullptr, 16);

	switch(mode) {
	case ADDR_IMM:
		return num;
	case ADDR_ZERA:
	case ADDR_ABSA:
		return emu->mem->read(num);
	case ADDR_ZERX:
	case ADDR_ABSX:
		return emu->mem->read(num + emu->get_x());
	case ADDR_ZERY:
	case ADDR_ABSY:
		return emu->mem->read(num + emu->get_y());
	default:
		throw "Invalid addressing mode";
	}

}

static word get_address(Emulator *emu, string argument, addressing_mode mode)
{
	std::smatch m;
	std::regex word_regex(word_str);
	std::regex address_regex(address_str);

	//Remove the opcode, because it may contain a hex value (e.g. ADC)
	auto arg = argument.substr(3);

	switch(mode) {
	case ADDR_ACC:
	case ADDR_IMM:
		throw "Tried to get address from invalid mode";
	case ADDR_ZERA:
	case ADDR_ZERX:
	case ADDR_ZERY:
		std::regex_search(arg, m, word_regex);
		break;
	case ADDR_ABSA:
	case ADDR_ABSX:
	case ADDR_ABSY:
		std::regex_search(arg, m, address_regex);
		break;
	
	default:
		throw "Invalid addressing mode";
	}

	string result = m[0].str();
	return stoi(result, nullptr, 16);
}

class Instruction {
protected:
    Emulator * emu;
    int current_cycle = 0;
    uint16_t byte_length = 0;
    string line = "";
    vector<string> code_window;
public:
    Instruction() {}
    Instruction(Emulator *e, string l): emu(e), line(l) {}
    virtual ~Instruction() = default;
    virtual void run()=0;
    int execute() {
        run();
        emu->increment_pc(byte_length);
        return current_cycle;
    };
    int get_cycle() 
    {
        return current_cycle;
    };
    string to_string() 
    {
        return line;
    }
    uint16_t get_byte_length() {
        return byte_length;
    }
    vector<string> get_window() {
        return code_window;
    }
    void set_window(vector<string> w) {
        code_window = w;
    }
};

class Value {
protected:
    word value;
public:
    Value() : value(0) {}
    Value(word data) : value(data) {}
    virtual word get_value() { return value; } 
    virtual void set_value(word data) { value = data; }
    // We need to only overload set_value once if all versions end up
    // calling the one above.
    virtual void set_value(Value *val) { set_value(val->get_value()); }
};

class Constant : public Value {
public:
    Constant(word data) : Value(data) {};
    void set_value(word data) {};
};

class Register : public Value {
private:
    Reg target;
    Emulator * emu;
public:
    Register(Reg target, Emulator *e) : target(target), emu(e) {};
    word get_value() { return *emu->quick_map[target]; }
    void set_value(word data) { *emu->quick_map[target] = data; }
};


class Label : public Instruction {
private:
public:
    Label(string value, string l) : label(value)
    {
        line = l;
    };
    string label;
    void run(){};
};

class T: public Instruction {
private:
    Reg source;
    Reg target;
public:
    T(Reg target, Reg source, Emulator * e, string l) : source(source), target(target) 
    {
        line = l;
        emu = e;
        byte_length = 2;
    };
    void run()
    {
        *emu->quick_map[target] = *emu->quick_map[source];
    }
};


class JMP: public Instruction {
private:
    string label;
public:
    JMP(string label, Emulator * e, string l) : label(label) {
        line = l;
        emu = e;
        byte_length = 2;
    };
    void run()
    {
        emu->jump_to(label);
    }
};


class InstructionGroup : public Instruction {
protected:
    address addr;
    addressing_mode mode;   
public:
    InstructionGroup(Emulator *e, string l, 
		    unordered_map<addressing_mode, size_t> instruction_lengths, 
		    unordered_set<addressing_mode> allowed_modes) : Instruction(e, l) {
	mode = parse_addr_mode(l.substr(3));
	if (allowed_modes.find(mode) == allowed_modes.end())
		throw "Invalid addressing mode";

	byte_length = instruction_lengths[mode];
    };
};

class Binary: public InstructionGroup {
    unordered_map<addressing_mode, size_t> instruction_lengths {
	{ADDR_IMM, 2},
	{ADDR_ABSA, 4},
	{ADDR_ABSX, 4},
	{ADDR_ABSY, 4},
	{ADDR_ZERA, 3},
	{ADDR_ZERX, 4},
    };

    unordered_set<addressing_mode> allowed_modes {
	ADDR_IMM,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ABSY,
	ADDR_ZERA,
	ADDR_ZERX,
    };
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

class Shift: public InstructionGroup {
    unordered_map<addressing_mode, size_t> instruction_lengths {
	{ADDR_ACC, 2},
	{ADDR_ABSA, 5},
	{ADDR_ABSX, 6},
	{ADDR_ZERA, 6},
	{ADDR_ZERX, 7},
    };

    unordered_set<addressing_mode> allowed_modes {
	ADDR_ACC,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ZERA,
	ADDR_ZERX,
    };
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
	emu->mem->write(opaddress, current_value >> 1);
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
	emu->mem->write(opaddress, current_value << 1);
    }
};

class ROL: public Shift {
public:
    ROL(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		// Brittle if we change word size, but we won't
		word bit7 = (current_value & 0x80) >> 7;
        	*emu->quick_map[accumulator] = (current_value << 1) | bit7; 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word bit7 = (current_value & 0x80) >> 7;
	emu->mem->write(opaddress, (current_value << 1) | bit7);
    }
};


class ROR: public Shift {
public:
    ROR(Emulator * e, string l) : Shift(e, l) {}
    void run()
    {
    	Reg accumulator = Reg::A;
	if (mode == ADDR_ACC) {
		word current_value = *emu->quick_map[accumulator];
		word bit0 = (current_value & 0x01) << 7;
        	*emu->quick_map[accumulator] = (current_value >> 1) | bit0; 
		return;
	}

	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	word bit0 = (current_value & 0x01) << 7;
	emu->mem->write(opaddress, (current_value >> 1) | bit0);
    }
};

class INC: public Increment {
public:
    INC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value + 1);
    }
};


class DEC: public Increment {
public:
    DEC(Emulator * e, string l) : Increment(e, l) {}
    void run()
    {
	address opaddress = get_address(emu, line, mode);
	word current_value = get_value(emu, line, mode);
	emu->mem->write(opaddress, current_value - 1);
    }
};

class Auxload: public InstructionGroup {
    unordered_map<addressing_mode, size_t> instruction_lengths {
	{ADDR_IMM, 2},
	{ADDR_ABSA, 3},
	{ADDR_ABSX, 4},
	{ADDR_ABSY, 4},
	{ADDR_ZERA, 4},
	{ADDR_ZERX, 4},
	{ADDR_ZERY, 4},
    };

    unordered_set<addressing_mode> allowed_modes {
	ADDR_IMM,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ABSY,
	ADDR_ZERA,
	ADDR_ZERX,
	ADDR_ZERY,
    };
public:
    Auxload(Emulator *e, string l) 
	    : InstructionGroup(e, l, instruction_lengths, allowed_modes) {}
};

class AuxloadX: public Auxload {
public:
    AuxloadX(Emulator *e, string l) 
	    : Auxload(e, l) {
	if (mode == ADDR_ABSX || mode == ADDR_ZERX)
		throw "Invalid instruction";

    }
};


class LDX: public AuxloadX {
public:
    LDX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        *emu->quick_map[xreg] = emu->mem->read(get_address(emu, line, mode)); 
    }
};


class STX: public AuxloadX{
public:
    STX(Emulator * e, string l) : AuxloadX(e, l) {}
    void run()
    {
    	Reg xreg = Reg::X;
        emu->mem->write(get_address(emu, line, mode), *emu->quick_map[xreg]); 
    }
};

class AuxloadY: public Auxload {
public:
    AuxloadY(Emulator *e, string l) 
	    : Auxload(e, l) {
	if (mode == ADDR_ABSY || mode == ADDR_ZERY)
		throw "Invalid instruction";

    }
};


class LDY: public AuxloadY {
public:
    LDY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
    	Reg yreg = Reg::Y;
        emu->mem->write(get_address(emu, line, mode), *emu->quick_map[yreg]); 
    }
};


class STY: public AuxloadY {
public:
    STY(Emulator * e, string l) : AuxloadY(e, l) {}
    void run()
    {
    	Reg yreg = Reg::Y;
        *emu->quick_map[yreg] = emu->mem->read(get_address(emu, line, mode)); 
    }
};

#endif
