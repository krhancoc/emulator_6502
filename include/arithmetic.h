#include "instruction.h"


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
        *emu->register_map[target] = *emu->register_map[source];
    }
};

typedef Transfer<Reg::A, Reg::X> TAX;
typedef Transfer<Reg::A, Reg::Y> TAY;
typedef Transfer<Reg::S, Reg::X> TSX;
typedef Transfer<Reg::X, Reg::A> TXA;
typedef Transfer<Reg::X, Reg::S> TXS;
typedef Transfer<Reg::Y, Reg::A> TYA;

class Increment : public Instruction {
private:
    Value * v;
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
            default:
                v = val;
        }
        byte_length = 1;
        current_cycle = 2;
    }
    void run()
    {
        word val = v->get_value();
        if (val + 1 > 255) {}
        v->set_value(val + 1);
    }
};



