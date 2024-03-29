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

enum class Reg { X, Y, A, P, S};
typedef uint16_t address;
typedef uint8_t word;


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
	const address STACK_LOW_ADDR = 0x100;
	const address STACK_HIGH_ADDR = 0x1ff;

    word internal_memory[MAX_MEMORY];
    Memory() {
        memset(internal_memory, 0, MAX_MEMORY);
    }
    void write(address a, word data)
    {
        internal_memory[a] = data; 
    };
    word read(address a) { return internal_memory[a]; }

    string to_string(uint16_t start){

        stringstream gethex;
        int x;
        x = start / 16;

        uint16_t lines = (MAX_MEMORY / (MAX_PER_LINE)); 
        stringstream ss;

        for(uint16_t i = x; i < lines && i < x + 20; i++) {
            ss << "0x" << setfill('0') << setw(4) << hex << (i * MAX_PER_LINE) << ": ";
            for(int t = 0; t < MAX_PER_LINE; t++) {
                ss << setw(2);
                uint8_t c = internal_memory[(i * MAX_PER_LINE) + t];
                char h[3];
                sprintf(h, "%02x", c);
                ss << h;
                ss << " ";
            }
            ss << "\n";
        }
        return ss.str();
    };
};

struct state {
    address pc;
    word a;
    word x;
    word y;
    word p;
    word s;
    word *internal_memory;
};

class Emulator { 
    vector<Instruction *> program;
    address current_inst  = 0;
    unordered_map<string, address> labels;
    address pc = 0;
    word a = 0; 
    word x = 0; 
    word y = 0;
    // only keep the bit 5 always be 1 (bit order 7->0)
    word p = 0x20;
    // start point of the stack 
    word s = 0xff;
public:
    // to get flag bit, do AND for corrsponding p_bit and P, and test if larger than 0(true)
    // to set flag bit, do OR or (P & ~p_bit[])
    const word p_bit[8] = {
	    0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80
    };

    Memory * mem = new Memory();
    Clock * clock = new Clock(1);
    unordered_map<Reg, word *> quick_map {
        {Reg::X, &x}, {Reg::A, &a}, {Reg::Y, &y}, 
		{Reg::P, &p}, {Reg::S, &s}
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
    word get_a() { return a; }
    word get_x() { return x; }
    word get_y() { return y; }
    word get_p() { return p; }
	word get_s() { return s; }

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
			.p = p,
			.s = s,
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
		ss << "S : " << static_cast<unsigned>((unsigned char) s) << endl;
        ss << endl;
        ss << "Flags" << endl;
        int p = this->p;
        ss << "C: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "Z: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "I: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "D: " << static_cast<unsigned>((unsigned char) (p & 1)) << endl;
        p = p >> 1;
        ss << "B: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "N/A: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "V: " << static_cast<unsigned>((unsigned char) (p & 1)) << ", ";
        p = p >> 1;
        ss << "S: " << static_cast<unsigned>((unsigned char) (p & 1)) << endl;;
        p = p >> 1;
       return ss.str();
    };

};
#endif

