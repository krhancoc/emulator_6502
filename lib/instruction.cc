#include "../include/instruction.h"

int Instruction::execute()
{
    run();
    emu->increment_pc(byte_length);
    return current_cycle;
}


Mem::Mem(string hex_string, Emulator * emu) : emu(emu)
{
    address = string_to_word(hex_string);
};

word Mem::get_value()
{
    return emu->mem->read(address);
}
void Mem::set_value(word n)
{
    emu->mem->write(address, n);
}

Constant::Constant(string hex_string) 
{
    mode = Mode::IMMEDIATE;
    data = string_to_word(hex_string);
};
word Register::get_value() 
{ 
    return *emu->register_map[target]; 
}
void Register::set_value(word data) {
    *emu->register_map[target] = data;
}






