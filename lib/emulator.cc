#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <queue>

#include "../include/emulator.h"
#include "../include/instruction.h"

using namespace std;

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

Instruction * parse_one_arg(vector<string> arguments, Emulator * emu, string line) 
{
    string arg = arguments[0];
    char back = arg.back();
    if (check_prefix("IN", arg)) {
        return new INC(get_target(arg), emu, line);
    } else if (back == ':'){
        return new Label(arg.substr(0, arg.length() - 1), line);
    }
    throw invalid_argument("Invalid instruction");
};

Instruction * parse_two_arg(vector<string> arguments, Emulator * emu, string line) 
{
    string inst = arguments[0];
    if (check_prefix("ADC", inst)) {
        Value * val = determine_value(arguments[1]);
        return new ADC(val, emu, line);
    } else if (check_prefix("JMP", inst)) {
        return new JMP(arguments[1], emu, line);
    }
    throw invalid_argument("Invalid instruction");
};

Instruction * parse(string line, Emulator * emu)
{
    vector<string> arguments;
    string token;
    istringstream args(line); while(getline(args, token, ' ')){
        arguments.push_back(token);
    }
    
    switch (arguments.size()) {
        case 1:
            return parse_one_arg(arguments, emu, line);
            break;
        case 2:
            return parse_two_arg(arguments, emu, line);
            break;
    }
    return 0;
}



int Emulator::step(){
    if (current_inst >= program.size()) {
        return 1;
    }
    Instruction * inst = program[current_inst];
    while(inst->get_cycle()) {
        inst->dec_cycle();
        this->clock->tick();
    }
    inst->execute();
    current_inst++;
    return 0;
};
void Emulator::attach(string filename) 
{
    string line;
    ifstream in;

    reset();

    in.open(filename);    
    vector<Instruction *> instructions;
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
    for (auto i: program) {
        delete i;
    }
    program.clear();
};

