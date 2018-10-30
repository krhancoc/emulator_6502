#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "instruction.h"

using namespace std;

Instruction * parse_one_arg(vector<string> arguments, Emulator * emu) 
{
    switch (arguments[0][2]) {
        case 'X':
            return new INC(Reg::X, emu);
        case 'Y':
            return new INC(Reg::Y, emu);
        case 'A':
            return new INC(Reg::A, emu);
    }
    throw invalid_argument("Invalid instruction");
};

Instruction * parse(string line, Emulator * emu)
{
    vector<string> arguments;
    string token;
    istringstream args(line);
    while(getline(args, token, ' ')){
        arguments.push_back(token);
    }
    
    switch (arguments.size()) {
        case 1:
            return parse_one_arg(arguments, emu);
            break;

    }
    return 0;
}

#endif

