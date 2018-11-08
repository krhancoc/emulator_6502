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
	ADDR_NONE,
	ADDR_ACC,
	ADDR_ABSA,
	ADDR_ABSX,
	ADDR_ABSY,
	ADDR_ZERA,
	ADDR_ZERX,
	ADDR_ZERY,
	ADDR_LABEL,
}; 


// XXX Here temporarily
static string prefix = "\\$";
static string word_str = "([0-9a-fA-F]{2})";
static string address_str = "([0-9a-fA-F]{4})";
static string endline_str = "\\s*";
static string label_str("[a-zA-Z][a-zA-Z0-9]*");

static addressing_mode parse_addr_mode(string argument)
{

	std::regex word_regex(word_str);
	std::regex address_regex(address_str);
	std::regex immediate("\\s*#" + prefix + word_str + endline_str);
	std::regex acc("\\s*A\\s*");
	std::regex none("\\s*");
	std::regex absa("\\s*" + prefix + address_str + endline_str);
	std::regex absx("\\s*" + prefix + address_str + "\\s*,\\s*X" + "\\s*");
	std::regex absy("\\s*" + prefix + address_str + "\\s*,\\s*Y" + "\\s*");
	std::regex zera("\\s*" + prefix + word_str + endline_str);
	std::regex zerx("\\s*" + prefix + word_str + "\\s*,\\s*X" + "\\s*");
	std::regex label("\\s*" + label_str + "\\s*");


	if (std::regex_match(argument, immediate)) {
		return ADDR_IMM;
	} else if (std::regex_match(argument, none)) {
		return ADDR_NONE;
	} else if (std::regex_match(argument, acc)) {
		return ADDR_ACC;
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
	} else if (std::regex_match(argument, label)) {
		return ADDR_LABEL;
	} else {
		throw invalid_argument("Invalid instruction");
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
		throw invalid_argument("Invalid addressing mode");
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
		throw invalid_argument("Invalid addressing mode");
	}

}

static int get_address(Emulator *emu, string argument, addressing_mode mode)
{

    std::smatch m;
    std::regex word_regex(word_str);
    std::regex address_regex(address_str);

    //Remove the opcode, because it may contain a hex value (e.g. ADC)
    auto arg = argument.substr(3);

    switch(mode) {
    case ADDR_ACC:
    case ADDR_IMM:
            throw invalid_argument("Tried to get address from invalid mode");
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
            throw invalid_argument("Invalid addressing mode");
    }

    string result = m[0].str();
    int address;
    stringstream ss;
    ss << hex << result;
    ss >> address;
    return address;
}

class Instruction {
protected:
    Emulator * emu;
    int current_cycle = 0;
    uint16_t byte_length = 0;
    string line = "";
    vector<string> code_window;

    void sign_flag_check(ssize_t result) {
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
    Instruction() {}
    Instruction(Emulator *e, string l): emu(e), line(l) {
    }
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
    string label;

    Label(string value, string l) : label(value)
    {
        line = l;
    };
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
		throw invalid_argument("Invalid addressing mode");

	byte_length = instruction_lengths[mode];
    };
};

#endif
