#include <fstream>

#include "../include/translator.h"
#include "../include/instruction_set.h"

bool check_register_t(string val) 
{
    return !val.compare("X") || !val.compare("A") || !val.compare("Y");
}

Reg get_register_t(string val)
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

Value * evaluate_t(string value, Emulator * emu) 
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
            if (check_register_t(val)) {
                return new Register(get_register_t(val), emu);
            }

            if (val[0] == '(' && val[val.length() - 1] == ')') {
                v = evaluate_t(val.substr(1, val.length() - 1), emu);
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
            v = evaluate_t(arguments[0], emu);
            Value * o = evaluate_t(arguments[1], emu);
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
        v = evaluate_t(arguments[1], nullptr);
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
    {"BNE", CreateInstruction<BNE>},
    // compare.h
    {"CMP", CreateInstruction<Compare>},
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
            labels[p->label] = program.size() + 1;        
            program.push_back(inst);
        } else {
            program.push_back(inst);
        }
    };

}
string check_jmp(Instruction * i) 
{
    JMP * p = dynamic_cast<JMP *>(i);
    if (p != nullptr) {
        return p->v->label;
    }
    return "";
}

string check_bne(Instruction * i) 
{
    BNE * p = dynamic_cast<BNE *>(i);
    if (p != nullptr) {
        return p->v->label;
    }
    return "";
}

string check_label(Instruction * i) 
{
    Label * p = dynamic_cast<Label *>(i);
    if (p != nullptr) {
        return p->label;
    }
    return "";
}


void Translator::start()
{
    string dingdong = "start";
    Translator::TranslationEntry * prev = nullptr;
    while (dingdong != "") {
        prev = do_chunk(dingdong, prev);
        dingdong = prev->go_to;
        int x;
        cin >> x;
    }
}

Translator::TranslationEntry * Translator::do_chunk(string label, TranslationEntry * prev)
{
    register uint64_t code asm("r9");

    TranslationEntry * e;
    Instruction * last_i = nullptr;
    bool appended = false;
    if(TC.find(label) == TC.end()) {
        vector<Instruction *> chunk;

        string go_func;
        for (size_t i = labels[label]; i < program.size(); i++) {
            Instruction * inst = program[i];
            go_func = check_label(inst);
            if (go_func != "") {
                last_i = inst;
                break;
            }
            go_func = check_jmp(inst);
            if (go_func != "") {
                last_i = inst;
                break;
            }

            if (check_bne(inst) != "") {
                appended = true; 
            }

            chunk.push_back(inst);
        }
        e = new Translator::TranslationEntry(chunk, go_func, this);
        e->last_inst = last_i;
        e->appended = appended;
        TC[label] = e;
    } else {
        e = TC[label];
    }
    
    if (prev != nullptr && !prev->appended)
    {
        TranslationSnippet * f = JMP::gen_jump(prev->code_section);
        e->reappend(f->translation, f->bytes);
        prev->appended = true;
        cout << "APPENDED" << endl;
    }
    cout << "A IS " << (int) A << endl;
    cout << "X IS " << (int) X << endl;
    cout << "Y IS " << (int) Y << endl;

    for (int i = 0; i < 30; i++)
        printf("%x ",((uint8_t *) e->code_section)[i]);
    printf("\n");

    code = (uint64_t) e->code_section;
    asm volatile (
        "pushq %%rax\n"
        "pushq %%rbx\n"
        "pushq %%rcx\n"
        "pushq %%rdx\n"
        "pushq %%r10\n"
        "xor %%rax, %%rax\n"
        "xor %%rbx, %%rbx\n"
        "xor %%rcx, %%rcx\n"
        "xor %%rdx, %%rdx\n"
        "xor %%r10, %%r10\n"
        "mov %[X], %%bl\n"
        "mov %[Y], %%cl\n"
        "mov %[A], %%dl\n"
        "callq *%[code]\n"
        "mov %%bl, %[X]\n"
        "mov %%cl, %[Y]\n"
        "mov %%dl, %[A]\n"
        "popq %%r10\n"
        "popq %%rdx\n"
        "popq %%rcx\n"
        "popq %%rbx\n"
        "popq %%rax\n"
        : [A] "+m" (A) , [X] "+m" (X), [Y] "+m" (Y) 
        : [code] "r" (code)
    );
 
    cout << "X IS " << (int) X << endl;
    cout << "Y IS " << (int) Y << endl;
    return e;
}



