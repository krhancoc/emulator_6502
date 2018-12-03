#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <string>
#include <unordered_map>
#include <vector>
#include <sys/mman.h>

#include "instruction_set.h"


class Translator {
public:
    word A;
    word X;
    word Y;
    word F;

    struct TranslationEntry {

        void * code_section;
        string go_to = "";
        bool appended = false;
        off_t offset_replace = 0;
        Instruction * last_inst = nullptr;
        const size_t SIZE = 4 * 1024;
        off_t offset = 0;
        char retcode[1] = { '\xC3' };    
        TranslationSnippet  ret =  TranslationSnippet(static_cast<void *>(retcode), 1);
        TranslationEntry() {};
        TranslationEntry(vector<Instruction *> &instructions, string go, Translator * trans) : go_to(go)
        {

            code_section = mmap(NULL, SIZE, 
                PROT_READ | PROT_WRITE | PROT_EXEC, 
                MAP_PRIVATE | MAP_ANONYMOUS, 
                -1, 0);
            if (code_section == MAP_FAILED) {
                cout << "DANGER: MMAP FAILED\n" << endl;
            }

            size_t total_bytes = 0;
            for(size_t t = 0 ; t < instructions.size(); t++) {
                Instruction * inst = instructions[t];
                BNE * p = dynamic_cast<BNE *>(inst);
                if (p != nullptr) {
                    TranslationEntry * entry = trans->TC[p->v->label];
                    TranslationSnippet * snippet = (BNE::gen_jump(entry->code_section));
                    te_write(snippet->translation, snippet->bytes);
                    total_bytes += snippet->bytes;
                } else {
                    TranslationSnippet snippet = instructions[t]->translate();
                    te_write(snippet.translation, snippet.bytes);
                    total_bytes += snippet.bytes;
                }
            }

            /* Add the return at the end */
            te_write(ret.translation, ret.bytes);
            total_bytes += ret.bytes;


        };

        void te_write(void * buffer, size_t bytes)
        {
            memcpy(((char *)code_section + offset), buffer, bytes);
            offset += bytes;
        }

        void reappend(void * buffer, size_t bytes)
        {
            offset -= ret.bytes;
            memcpy(((char *)code_section + offset), buffer, bytes);
            offset += bytes;
        }
    };
    typedef std::unordered_map<string, TranslationEntry *> TranslationCache;

    Translator() : A(0), X(0), Y(0), F(0) {};
    void mount(std::string filename);
    TranslationEntry * do_chunk(string label, TranslationEntry * prev);
    size_t translate(TranslationEntry * e);
    void start();
    unordered_map<string, address> labels;
    vector<Instruction *> program;
    TranslationCache TC;
};

#endif
