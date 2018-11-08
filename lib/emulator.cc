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

Value * evaluate(string line) 
{
}

template <class F>
Instruction * CreateInstruction(Emulator * emu, string line) 
{
    vector<string> arguments;
    string token;
    istringstream args(line); 
    while(getline(args, token, ' ')){
        arguments.push_back(token);
    }

    Value * v = nullptr;
    if (arguments.size() == 2) {
        v = evaluate(arguments[1]);
    };
    return new F(emu, v, line);
}

unordered_map<string, Instruction*(*)(Emulator *,string)>instruction_map = {
    {"INX", CreateInstruction<CLI>}
};

Instruction * parse(string line, Emulator * emu)
{
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
    if (current_inst) {
        pc = 0; 
        current_inst--;
        for (int i = 0; i < current_inst; i++) {
            Instruction * inst = program[i];
            uint16_t byte_length = inst->get_byte_length();
            increment_pc(byte_length);
        }
    } else {
        current_inst--;
    }
}

void Emulator::reset(){
    pc = a = x = y = current_inst = 0;
	p = 0x20;
	s = 0xff;
    memset(mem->internal_memory, 0, MAX_MEMORY);
};

