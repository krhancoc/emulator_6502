#ifndef __EMULATOR_H__
#define __EMULATOR_H__

#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <vector>
#include <time.h>
#include <sstream>
#include <cstring>


#define MAX_MEMORY 65536
#define MAX_PER_LINE 16

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
                cout << "TICK" << endl;
                break;
            }
        } 
    }

};

class Memory {
public:
    address internal_memory[MAX_MEMORY];
    Memory() {
        memset(internal_memory, 0, MAX_MEMORY);
    }
    void write(address a){};
    uint8_t read(address a)
    {
        return 0;
    };
    string to_string(){
        // 2 * 16 , 2 bytes per word, 16 words per line;
        stringstream ss;
        uint16_t lines = (MAX_MEMORY / (MAX_PER_LINE)); 

        for(uint16_t i = 0; i < lines; i++) {
            ss << "0x" << setfill('0') << setw(4) << hex << (i * MAX_PER_LINE) << ": ";
            for(int t = 0; t < MAX_PER_LINE; t++) {
                ss << setw(2);
                address c = internal_memory[(i * MAX_PER_LINE) + t];
                ss << hex << c;
                ss << " ";
            }
            ss << "\n";
        }
        return ss.str();
    };
};

struct state {
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    address * internal_memory;
};

class Emulator {
    vector<Instruction *> program;
    uint16_t current_inst  = 0;
    unordered_map<string, uint16_t> labels;
    uint16_t pc = 0;
    uint8_t a = 0; 
    uint8_t x = 0; 
    uint8_t y = 0; 
public:
    Memory * mem = new Memory();
    Clock * clock = new Clock(1);
    unordered_map<Reg, uint8_t *> quick_map {
        {Reg::X, &x}, {Reg::A, &a}, {Reg::Y, &y}
    };

    void increment_pc(uint16_t val)
    {
        pc += val;
    }
    Instruction * get_current_inst() {
        if (current_inst >= program.size()) {
            return nullptr;         
        }
        return program[current_inst];
    }

    state run()
    {

        while(current_inst < program.size()) {
            step();
        }
        return state {
            .pc = pc,
            .a = a,
            .x = x,
            .y = y,
            .internal_memory = mem->internal_memory
        };
    };

    int step();
    void attach(string filename);
    void jump_to(string label);
    void reset();
        
    string to_string()
    {
       stringstream ss; 
       ss << "PC : " << pc << endl;
       ss << "A : " << static_cast<unsigned>((unsigned char) a) << endl;
       ss << "X : " << static_cast<unsigned>((unsigned char) x) << endl;
       ss << "Y : " << static_cast<unsigned>((unsigned char) y) << endl;
       return ss.str();
    };

};
#endif

