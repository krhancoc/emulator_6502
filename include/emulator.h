#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include <unordered_map>
#include <iostream>
#include <vector>
#include <time.h>
#include <sstream>


#define MAX_MEMORY 65536

using namespace std;

enum class Reg { X, Y, A};
typedef uint16_t address;


//FORWARD DEC
class Instruction;

class Clock {
private:
    int speed;
public:
    Clock(int speed) : speed(speed) {};
    void tick()
    {
        clock_t t = clock();
        while(true) {
            if((clock() - t) >= speed) {
                cout << "Tick\n" << endl;
                break;
            }
        } 
    }

};

class Memory {
private:
    char * internal_memory[MAX_MEMORY];
public:
    void write(address a){};
    uint8_t read(address a)
    {
        return 0;
    };
};

class Emulator {
    vector<Instruction *> inst;
    Instruction * current_inst;
    Memory * mem = new Memory();
    uint16_t pc = 0;
    uint8_t a = 0; 
    uint8_t x = 0; 
    uint8_t y = 0; 
public:
    unordered_map<Reg, uint8_t *> quick_map {
        {Reg::X, &x}, {Reg::A, &a}, {Reg::Y, &y}
    };
    void attach(vector<Instruction *> i) {
        inst = i;
        current_inst = inst[0];
    }
    void run(){
        Clock * clock = new Clock(100000);
        cout << to_string() << endl;
        while(pc != inst.size() * 2) {
            step();
            clock->tick();
        }
        cout << to_string() << endl;
    };

    void step();

    string to_string()
    {
       stringstream ss; 
       ss << "PC : " << pc << endl;
       ss << "A : " << static_cast<unsigned>((unsigned char) a) << endl;
       ss << "X : " << static_cast<unsigned>((unsigned char) x) << endl;
       ss << "Y : " << static_cast<unsigned>((unsigned char) y) << endl;
       return ss.str();
    };
    void reset(){
        pc = a = x = y = 0;
    };

};

#endif

