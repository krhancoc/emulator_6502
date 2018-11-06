#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <regex>

#include "emulator.h"

enum addressing_mode {
	ADDR_IMM,
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

	cout << "INPUT " << argument << endl;

	if (std::regex_match(argument, immediate)) {
		cout << "IMM" << endl << endl;
		return ADDR_IMM;
	} else if (std::regex_match(argument, absa)) {
		cout << "ABSA" << endl << endl;
		return ADDR_ABSA;
	} else if (std::regex_match(argument, absx)) {
		cout << "ABSX" << endl << endl;
		return ADDR_ABSX;
	} else if (std::regex_match(argument, absy)) {
		cout << "ABSY" << endl << endl;
		return ADDR_ABSY;
	} else {
		//throw("Invalid instruction");
	}

	cout << "WOOPS" << endl;
	return ADDR_IMM;
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
		throw("Invalid addressing mode");
	}

	string result = m[0].str();
	cout << "FIRST  " << argument << endl;
	cout << "ARG    " << arg << endl;
	cout << "RESULT " << result << endl;
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
		throw("Invalid addressing mode");
	}

}


class Instruction {
protected:
    Emulator * emu;
    int CYCLES = 0;
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
    void dec_cycle()
    {
        current_cycle--;
    };
    void reset()
    {
        current_cycle = CYCLES;
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
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        *emu->quick_map[target] = *emu->quick_map[source];
    }
};

class ADC: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    ADC(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] 
		+ get_value(emu, line, mode);
    }
};

// 2 or 3 byte instruction
class INC : public Instruction {
private:
    Reg target_reg;
public:
    INC(Reg target, Emulator * e, string l) : target_reg(target) {
        emu = e;
        line = l;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        (*emu->quick_map[target_reg])++;
        reset();
    };
};

class JMP: public Instruction {
private:
    string label;
public:
    JMP(string label, Emulator * e, string l) : label(label) {
        line = l;
        emu = e;
        CYCLES = 1;
        current_cycle = CYCLES;
        byte_length = 2;
    };
    void run()
    {
        emu->jump_to(label);
    }
};


class AND: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    AND(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] 
		& get_value(emu, line, mode);
    }
};


class CMP: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    CMP(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
	//Needs flags register
    }
};


class EOR: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    EOR(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] 
		^  get_value(emu, line, mode);
    }
};


class LDA: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    LDA(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = get_value(emu, line, mode);
    }
};


class ORA: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    ORA(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] 
		| get_value(emu, line, mode);
    }
};


class SBC: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    SBC(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_IMM:
        	CYCLES = 2;
		byte_length = 2;
		break;
		
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator] 
		- get_value(emu, line, mode);
    }
};


class STA: public Instruction {
private:
    // NOTE: Value may represent both literals and addresses
    Value * value;
    address addr;
    addressing_mode mode;   
public:
    STA(Emulator * e, string l) : Instruction(e, l) {
	
	mode = parse_addr_mode(l.substr(3));
	switch (mode) {
	case ADDR_ABSA:
        	CYCLES = 4;
		byte_length = 3;
		break;
		
	// XXX Will be 5 if the page boundary is crossed 
	// while fetching the instruction.
	case ADDR_ABSX:
        	CYCLES = 4;
		byte_length = 3;
		break;

	case ADDR_ABSY:
        	CYCLES = 4;
		byte_length = 3;
		break;

	default:
		throw("Invalid addressing mode");
	}

        current_cycle = CYCLES;
    };
    void run()
    {
	
    	Reg accumulator = Reg::A;
        *emu->quick_map[accumulator] = *emu->quick_map[accumulator]; 
    }
};

#endif
