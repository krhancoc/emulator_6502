#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <sys/mman.h>

#include "instruction_set.h"


class Translator {
public:
    struct TranslationEntry {
        void * code_section;
        const size_t SIZE = 1024 * 4;
        off_t offset = 0;

        TranslationEntry(vector<Instruction *> &instructions) 
        {

            code_section = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);

            size_t total_bytes = 0;
            for(size_t t = 0 ; t < instructions.size(); t++) {
                TranslationSnippet snippet = instructions[t]->translate();
                te_write(snippet.translation, snippet.bytes);
                total_bytes += snippet.bytes;
            }

            mprotect(code_section, total_bytes, PROT_READ | PROT_WRITE | PROT_EXEC);
        };

        void te_write(void * buffer, size_t bytes)
        {
            memcpy(((char *)code_section + offset), buffer, bytes);
            offset += bytes;
        }
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
