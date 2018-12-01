#include "../include/translator.h"

int main() 
{
    Translator * t = new Translator();
    t->mount("test_files/adc_test.asm");
    t->do_chunk(0);
    return 0;
}
