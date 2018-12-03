#include "instruction.h"

class Compare : public Instruction {
public:
    Value * v;
    Compare(Value * val, Emulator * e, string l) : Instruction(e,l)
    {
        v = val;
    }    
    void run() {};
    TranslationSnippet translate() 
    {
	const size_t size = 4;
        uint8_t binary[size] = { 
            0x48, 0x83, 0xfb, 0x64
        };

        return TranslationSnippet(static_cast<void *>(&binary), size);
    };
};
