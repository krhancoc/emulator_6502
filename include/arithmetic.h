#include "instruction.h"


/*
 *
 TAX  Transfer Accumulator to Index X

     A -> X                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TAX           AA    1     2


TAY  Transfer Accumulator to Index Y

     A -> Y                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TAY           A8    1     2


TSX  Transfer Stack Pointer to Index X

     SP -> X                          N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TSX           BA    1     2


TXA  Transfer Index X to Accumulator

     X -> A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TXA           8A    1     2


TXS  Transfer Index X to Stack Register

     X -> SP                          N Z C I D V
                                      - - - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TXS           9A    1     2


TYA  Transfer Index Y to Accumulator

     Y -> A                           N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       TYA           98    1     2
*/
template<Reg source, Reg target>
class Transfer: public Instruction {
public:
    Transfer(Value *, Emulator * e, string l) : 
        Instruction(e, l)
    {
        byte_length = 1;
        current_cycle = 2;
    };
    void run()
    {
        word prev = *emu->register_map[target];
        word next = *emu->register_map[source];
        *emu->register_map[target] = *emu->register_map[source];
        set_flags(prev, next);
    }
};

typedef Transfer<Reg::A, Reg::X> TAX;
typedef Transfer<Reg::A, Reg::Y> TAY;
typedef Transfer<Reg::S, Reg::X> TSX;
typedef Transfer<Reg::X, Reg::A> TXA;
typedef Transfer<Reg::X, Reg::S> TXS;
typedef Transfer<Reg::Y, Reg::A> TYA;

/*
 INC  Increment Memory by One

     M + 1 -> M                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     zeropage      INC oper      E6    2     5
     zeropage,X    INC oper,X    F6    2     6
     absolute      INC oper      EE    3     6
     absolute,X    INC oper,X    FE    3     7


INX  Increment Index X by One

     X + 1 -> X                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INX           E8    1     2


INY  Increment Index Y by One

     Y + 1 -> Y                       N Z C I D V
                                      + + - - - -

     addressing    assembler    opc  bytes  cyles
     --------------------------------------------
     implied       INY           C8    1     2

 */
class Increment : public Instruction {
private:
    Value * v;
    unordered_map<Mode, BCPair> byte_map {
        {Mode::ZERO_PAGE, {2, 5}},
        {Mode::ZERO_PAGE_INDEXED, {2, 6}},
        {Mode::ABSOLUTE, {3, 6}},
        {Mode::ABSOLUTE_INDEXED, {3, 7}},
        {Mode::REGISTER, {1, 2}}
    };
public:
    Increment(Value * val, Emulator * e, string l) :
        Instruction(e, l)
    {
        switch (l[2]) {
            case 'X':
                v = new Register(Reg::X, emu);
                break;
            case 'Y':
                v = new Register(Reg::X, emu);
                break;
            case 'A':
                throw "Bad Instruction";
            default:
                v = val;
        }

        if (byte_map.find(v->mode) == byte_map.end()) {
            throw "Bad addressing mode";
        }

        BCPair p = byte_map[v->mode];
        byte_length = p.bytes;
        current_cycle = p.cycle;
    }
    void run()
    {
        word val = v->get_value();
        v->set_value(val + 1);
        set_flags(val, val + 1);
    }
    
};



