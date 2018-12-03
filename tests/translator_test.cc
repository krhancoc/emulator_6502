#include "../include/translator.h"

int main() 
{
    Translator * t = new Translator();
    t->mount("test_files/adc_test.asm");
    t->start();
    std::cout << "X: " << (int) t->X << endl;
    std::cout << "Y: " << (int) t->Y << endl;
    return 0;
}
