#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "instruction_set.h"


class Translator {
public:
    struct TranslationEntry {
        void * jump_point = nullptr;
        x86op * opcodes;
        TranslationEntry(vector<Instruction *> instructions) {
            opcodes = new x86op[instructions.size()];
            for(size_t t = 0 ; t < instructions.size(); t++) {
               opcodes[t] = instructions[t]->translate();
            }
        };
    };
    typedef std::unordered_map<int, TranslationEntry> TranslationCache;

    Translator() {};
    void mount(std::string filename);
    int do_chunk(int current_position);
    size_t translate(TranslationEntry * e);
private:
    vector<Instruction *> program;
    TranslationCache TC;
};
#endif
