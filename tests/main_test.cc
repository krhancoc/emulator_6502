#include "../include/parse.h"

using namespace std;

int main() 
{
    string line;
    ifstream in;
    in.open("in.asm");    
    vector<Instruction *> instructions;
    Emulator * emu = new Emulator();
    if (in.is_open()) {
        while(getline(in, line)) {
            if (line.length() != 0 && line[0] != ';') {
                instructions.push_back(parse(line, emu));
            }
        }
    }
    emu->reset();
    emu->attach(instructions);
    emu->run();
    in.close();
};
