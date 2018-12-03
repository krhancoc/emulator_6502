#ifndef __BRANCH_H__
#define __BRANCH_H__

#include "instruction.h"

class JMP: public Instruction {
public:
    Lab * v;

    JMP(Value * v, Emulator * e, string l) : Instruction(e, l), v((Lab *)v) {
        byte_length = 2;
        current_cycle = 3;
    };
    void run()
    {
        emu->jump_to(v->label);
    }
    TranslationSnippet translate() 
    {
        return TranslationSnippet();
    }

    static TranslationSnippet * gen_jump(void * address)
    {
	const size_t size = 12;
        uint8_t binary[size] = { 
            // MOV rdx, address
            0x48, 0xB8, 
            0x00, 0x00, 0x00,  0x00,
            0x00, 0x00, 0x00,  0x00,
            // JUMP INSTRUCTION
            0xFF, 0xe0 };

        memcpy((void *)(binary + 2), &address, sizeof(address));

        return new TranslationSnippet(static_cast<void *>(&binary), size);
    }


};


class BNE : public Instruction {
public:
    Lab * v;
    BNE(Value * v, Emulator * e, string l) : Instruction(e, l), v((Lab *)v) {
        byte_length = 2;
        current_cycle = 3;
    };
    void run() {};

    TranslationSnippet translate() {
        cout << "WTF" <<endl;
	const size_t size = 15;
        uint8_t binary[size] = { 
            0x75, 0x01, 
            0xC3, 
            0xC3, 0x90, 0x90, 0x90,
            0x90, 0x90, 0x90, 0x90,
            0x90, 0x90, 0x90, 0x90,
        };
        
        return TranslationSnippet(static_cast<void *>(&binary), size);

    };

    static TranslationSnippet * gen_jump(void * address)
    {
	const size_t size = 15;
        uint8_t binary[size] = { 
            0x75, 0x01, 
            /* First return */
            0xC3, 
            /* Second return */
            0xC3, 0x90, 0x90, 0x90,
            0x90, 0x90, 0x90, 0x90,
            0x90, 0x90, 0x90, 0x90,
        
        };
        memcpy((void *)(binary + 3), &address, sizeof(address));

        return new TranslationSnippet(static_cast<void *>(&binary), size);
    }
};

#endif

