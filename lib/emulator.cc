#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

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

Instruction * parse_one_arg(vector<string> arguments, Emulator * emu) 
{
    string arg = arguments[0];
    char back = arg.back();
    if (check_prefix("IN", arg)) {
        return new INC(get_target(arg), emu);
    } else if (back == ':'){
        return new Label(arg.substr(0, arg.length() - 1));
    }
    throw invalid_argument("Invalid instruction");
};

Instruction * parse_two_arg(vector<string> arguments, Emulator * emu) 
{
    string inst = arguments[0];
    if (check_prefix("ADC", inst)) {
        Value * val = determine_value(arguments[1]);
        return new ADC(val, emu);
    } else if (check_prefix("JMP", inst)) {
        return new JMP(arguments[1], emu);
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
            return parse_one_arg(arguments, emu);
            break;
        case 2:
            return parse_two_arg(arguments, emu);
            break;
    }
    return 0;
}



void Emulator::step(){
    Instruction * inst = program[current_inst];
    while(inst->get_cycle()) {
        inst->dec_cycle();
        this->clock->tick();
    }
    inst->execute();
    current_inst++;
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
    }
    in.close();

    for (size_t i = 0; i < instructions.size(); i++) {
        Instruction * inst = instructions[i];

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

