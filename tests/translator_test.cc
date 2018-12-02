#include "../include/translator.h"

int main() 
{
    Translator * t = new Translator();
    t->mount("test_files/adc_test.asm");
    t->do_chunk(0);
    std::cout << "X: " << (int) t->X << endl;
    std::cout << "Y: " << (int) t->Y << endl;
    return 0;
}
