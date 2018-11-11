#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "instruction.h"

template<Reg target>
class LD : public Instruction {
    Value * val;
public:
    LD(Value * v, Emulator * e, string l) : Instruction(e, l), val(v) 
    {
    }
    void run()
    {
    }
};

template<Reg target>
class ST : public Instruction {
    Value * val;
public:
    ST(Value * v, Emulator * e, string l) : Instruction(e, l), val(v) 
    {
    }

    void run() 
    {
    }
    
};



typedef LD<Reg::X> LDX;
typedef LD<Reg::Y> LDY;
typedef LD<Reg::A> LDA;

typedef ST<Reg::X> STX;
typedef ST<Reg::Y> STY;
typedef ST<Reg::A> STA;



#endif
