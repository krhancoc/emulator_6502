#ifndef __PARSE_H_
#define __PARSE_H_

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "instruction.h"

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
    return new INC(get_target(arguments[0]), emu);
};

Instruction * parse_two_arg(vector<string> arguments, Emulator * emu) 
{
    string inst = arguments[0];
    if (check_prefix("ADC", inst)) {
        Value * val = determine_value(arguments[1]);
        return new ADC(val, emu);
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
        case 2:
            return parse_two_arg(arguments, emu);
            break;
    }
    return 0;
}

#endif

