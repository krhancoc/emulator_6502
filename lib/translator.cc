#include <fstream>

#include "../include/translator.h"
#include "../include/instruction_set.h"

using namespace std;

bool check_register(string val) 
{
    return !val.compare("X") || !val.compare("A") || !val.compare("Y");
}

Reg get_register(string val)
{
    switch (val[0]) {
        case 'X':
            return Reg::X;
        case 'Y':
            return Reg::Y;
        case 'A':
            return Reg::A;
        case 'S':
            return Reg::S;
    }
    throw "HELLO";
};

Value * evaluate(string value, Emulator * emu) 
{
    Value * v;
    vector<string> arguments;
    string token;
    istringstream args(value); 
    while(getline(args, token, ',')){
        arguments.push_back(token);
    }
    
    switch (arguments.size()) {
        case 0: 
        {
            v = nullptr;
            break;
        }
        case 1: 
        {
            string val = arguments[0];
            if (check_register(val)) {
                return new Register(get_register(val), emu);
            }

            if (val[0] == '(' && val[val.length() - 1] == ')') {
                v = evaluate(val.substr(1, val.length() - 1), emu);
                if (v->mode == Mode::ZERO_PAGE_INDEXED) {
                    v->mode = Mode::INDEXED_INDIRECT;
                } else {
                    v->mode = Mode::INDIRECT;
                }
                return v;
            } else if (!val.substr(0, 2).compare("#$")) {
                return new Constant(val.substr(2, val.length()));
            } else if (val[0] == '#') {
                return new Mem(val.substr(1, val.length()), emu);
            } else {
                return new Lab(val.substr(0, val.length()));
            }
            return nullptr;
        }
        case 2:
        {
            v = evaluate(arguments[0], emu);
            Value * o = evaluate(arguments[1], emu);
            v->indexed(o);
            return v;
        }
    } 
    return v;
};


template <class F>
Instruction * CreateInstruction(string line) 
{
    vector<string> arguments;
    string token;
    istringstream args(line); 
    while(getline(args, token, ' ')){
        arguments.push_back(token);
    }

    Value * v = nullptr;
    if (arguments.size() == 2) {
        v = evaluate(arguments[1], nullptr);
    };
    return new F(v, nullptr, line);
}



unordered_map<string, Instruction*(*)(string)>instruction_map = {
    // Arithmetic.h
    {"INX", CreateInstruction<Increment>},
    {"INY", CreateInstruction<Increment>},
    {"INC", CreateInstruction<Increment>},
    {"TXA", CreateInstruction<TXA>},
    {"TAX", CreateInstruction<TAX>},
    {"TAY", CreateInstruction<TAY>},
    {"TSX", CreateInstruction<TSX>},
    {"TXS", CreateInstruction<TXS>},
    {"TYA", CreateInstruction<TYA>},
    // Branch.h
    {"JMP", CreateInstruction<JMP>},
};

Instruction * parse(string line)
{
    vector<string> arguments;
    string token;
    istringstream args(line); 
    while(getline(args, token, ' ')){
        arguments.push_back(token);
    }
    string val = arguments[0];
    if (val[val.length() - 1] == ':') {
        return new Label(val.substr(0, val.length() -1), line);
    } 
    return instruction_map[arguments[0]](line);
}

void Translator::mount(string filename) 
{
    unordered_map<string, address> labels;
    string line;
    ifstream in;

    in.open(filename);    
    vector<Instruction *> instructions;
	if (!in) {
		perror(string("Cannot open input file: " + filename).c_str());
	}
	
	if (in.is_open()) {
        while(getline(in, line)) {
            if (line.length() != 0 && line[0] != ';') {
                try {
                    instructions.push_back(parse(line));
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
bool check_jmp(Instruction * i) 
{
    JMP * p = dynamic_cast<JMP *>(i);
    if (p != nullptr) {
        return true;
    }
    return false;
}

int Translator::do_chunk(int current_index)
{
    vector<Instruction *> chunk;
    for (size_t i = current_index; i < program.size(); i++) {
        Instruction * inst = program[i];
        if (check_jmp(inst)) {
            chunk.push_back(inst);
            break;
        }
        chunk.push_back(inst);
    }

    TC[current_index] = Translator::TranslationEntry(chunk);

    current_index += translate(&TC[current_index]);
    return current_index;
}

size_t Translator::translate(TranslationEntry * e) 
{
         
    return e->inst.size();
}


