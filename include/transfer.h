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
    TranslationSnippet translate() 
    {
        return TranslationSnippet(nullptr, 0);
    }

};

typedef Transfer<Reg::A, Reg::X> TAX;
typedef Transfer<Reg::A, Reg::Y> TAY;
typedef Transfer<Reg::S, Reg::X> TSX;
typedef Transfer<Reg::X, Reg::A> TXA;
typedef Transfer<Reg::X, Reg::S> TXS;
typedef Transfer<Reg::Y, Reg::A> TYA;

