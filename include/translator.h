#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <unordered_map>
#include <vector>

#include "instruction_set.h"


class Translator {
public:
    Translator() {};
    void mount(std::string filename);
    void do_chunk();
private:
    vector<Instruction *> program;

    struct TranslationEntry {
        vector<Instruction *> inst;
        void * jump_point;
    };
    typedef std::unordered_map<int, TranslationEntry> TranslationCache;
};
#endif
