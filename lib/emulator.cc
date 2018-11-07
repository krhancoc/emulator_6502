#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/emulator.h"
#include "../include/instruction.h"
#include "../include/instruction_set.h"

using namespace std;

address evaluate(Emulator *e 
        )
{
	return 0;
}


Reg get_target(string arg) 
{
    switch(arg[2]) {
        case 'X':
            return Reg::X;
        case 'Y':
            return Reg::Y;
        case 'A':
            return Reg::A;
    }
    throw invalid_argument("Invalid instruction");
}
bool check_prefix(string prefix, string arg) 
{
   return !arg.compare(0, prefix.size(), prefix);
}

Value * determine_value(string arg) {
    if (check_prefix("#$", arg)) {
        stringstream ss;
        int x;
        string h = arg.substr(2, arg.size());
        ss << hex << h;
        ss >> x;
        cout << x << endl;
        return new Constant(x);
    }
    throw invalid_argument("Invalid instruction");
}

Instruction * parse_two_arg(vector<string> arguments, Emulator * emu, string line) 
{
    string inst = arguments[0];
    if (check_prefix("JMP", inst)) {
        return new JMP(arguments[1], emu, line);
    }
    throw invalid_argument("Invalid instruction");
};

Instruction * parse(string line, Emulator * emu)
{
    /* Check if it's a label*/
    std::regex label_regex("\\s*[a-zA-Z]+\\s*:\\s*");
    std::regex name_regex("[a-zA-Z]+");

    if (regex_match(line, label_regex)) {
	    std::smatch m;
	    std::regex_search(line, m, name_regex);
	    return new Label(m[0].str(), line);
    }

    // NOTE: Parsing only works if there is no whitespace at the beginning
    // of the line. It can be solved by trimming, but C++ has
    // no standard function for that. We can use Boost for that.
    string command = line.substr(0, 3);

    
    if (!command.compare("ADC")) {
	    return new ADC(emu, line);
    } else if (!command.compare("AND")) {
	    return new AND(emu, line);
    } else if (!command.compare("CMP")) {
	    return new CMP(emu, line);
    } else if (!command.compare("EOR")) {
	    return new EOR(emu, line);
    } else if (!command.compare("LDA")) {
	    return new LDA(emu, line);
    } else if (!command.compare("ORA")) {
	    return new ORA(emu, line);
    } else if (!command.compare("SBC")) {
	    return new SBC(emu, line);
    } else if (!command.compare("STA")) {
	    return new STA(emu, line);
    } else if (!command.compare("LSR")) {
	    return new LSR(emu, line);
    } else if (!command.compare("ASL")) {
	    return new ASL(emu, line);
    } else if (!command.compare("ROL")) {
	    return new ROL(emu, line);
    } else if (!command.compare("ROR")) {
	    return new ROR(emu, line);
    } else if (!command.compare("INC")) {
	    return new INC(emu, line);
    } else if (!command.compare("DEC")) {
	    return new DEC(emu, line);
    } else if (!command.compare("LDX")) {
	    return new LDX(emu, line);
    } else if (!command.compare("STX")) {
	    return new STX(emu, line);
    } else if (!command.compare("LDX")) {
	    return new LDX(emu, line);
    } else if (!command.compare("LDY")) {
	    return new LDY(emu, line);
    } else if (!command.compare("INX")) {
	    return new INX(emu, line);
    } else if (!command.compare("JMP")) {
	    vector<string> arguments;
	    string token;
	    istringstream args(line); 
	    while(getline(args, token, ' ')){
	    	arguments.push_back(token);
	    }
            return parse_two_arg(arguments, emu, line);
    } else {
	throw "Invalid opcode";
    }

}



int Emulator::step(){
    if (current_inst >= program.size()) {
        return 1;
    }
    Instruction * inst = program[current_inst];
    int execute_cost = inst->execute();
    for (int i = 0; i < execute_cost; i++) {
        this->clock->tick();
    }
    current_inst++;
    return 0;
};
void Emulator::attach(string filename) 
{
    string line;
    ifstream in;

    for (auto i: program) {
        delete i;
    }
    program.clear();

    reset();
    in.open(filename);    
    vector<Instruction *> instructions;
	if (!in) {
		perror(string("Cannot open input file: " + filename).c_str());
	}
	if (in.is_open()) {
        while(getline(in, line)) {
            if (line.length() != 0 && line[0] != ';') {
                try {
                    instructions.push_back(parse(line, this));
                } catch (invalid_argument){
                    throw invalid_argument(line);
                }
            }
        }
    } else { 
        throw invalid_argument("File not found"); 
    }
    in.close();
    
	for (size_t i = 0; i < instructions.size(); i++) {
        vector<string> code_window;
        Instruction * inst = instructions[i];
        size_t start = int (i - 2) > 0  ? i - 2: 0;
        for(size_t t = start; t < instructions.size() && t < start + 5; t++) {
            Instruction * inst = instructions[t];
            if (t == i) {
                code_window.push_back(">>" + inst->to_string());
            } else {
                code_window.push_back(inst->to_string());
            }
        }
        inst->set_window(code_window);
        Label * p = dynamic_cast<Label *>(inst);
        if (p != nullptr) {
            labels[p->label] = program.size();        
        } else {
            program.push_back(inst);
        }
    };
}

void Emulator::jump_to(string label) 
{
    current_inst = labels[label];
    // WE WILL INCREMENT THIS AFTER WE EXECUTE AND IT PUSHES IT FORWARD ONE
    pc = 0;
    current_inst--;
    for (int i = 0; i < current_inst; i++) {
        Instruction * inst = program[i];
        uint16_t byte_length = inst->get_byte_length();
        increment_pc(byte_length);
    }
}

void Emulator::reset(){
    pc = a = x = y = current_inst = 0;
};

