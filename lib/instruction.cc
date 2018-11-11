#include "../include/instruction.h"

int Instruction::execute()
{
    run();
    emu->increment_pc(byte_length);
    return current_cycle;
}


Mem::Mem(string hex_string, Emulator * emu) : emu(emu)
{
    switch (hex_string.length()) {
        case 2:
            mode = Mode::ZERO_PAGE;
            break;
        case 4:
            mode = Mode::ABSOLUTE;
            break;
        default:
            throw "BAD MEMORY LOCATION";
    }
    address = string_to_int(hex_string);
};

word Mem::get_value()
{
    word o = offset != nullptr ? offset->get_value() : 0;
    return emu->mem->read(address + o);
}
void Mem::set_value(word n)
{
    word o = offset != nullptr ? offset->get_value() : 0;
    emu->mem->write(address + o, n);
}

Constant::Constant(string hex_string) 
{
    mode = Mode::IMMEDIATE;
    data = string_to_int(hex_string);
};
word Register::get_value() 
{ 
    return *emu->register_map[target]; 
}
void Register::set_value(word data) {
    *emu->register_map[target] = data;
}






