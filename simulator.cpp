#include <cstdint>
#include <string>
#include <map>
#include "mips_simulator.hpp"

/* Notes: 
- State registers: show state at point between stages in pipeline (pipeline latches)
*/


class Registers : public MIPS_CPU {
    public:
        int registerFile[32];
        unordered_map<string, int> regMap{
        {"$zero", 0},
        {"$at", 1},
        {"$v0", 2},
        {"$v1", 3},
        {"$a0", 4},
        {"$a1", 5},
        {"$a2", 6},
        {"$a3", 7},
        {"$t0", 8},
        {"$t1", 9},
        {"$t2", 10},
        {"$t3", 11},
        {"$t4", 12},
        {"$t5", 13},
        {"$t6", 14},
        {"$t7", 15},
        {"$s0", 16},
        {"$s1", 17},
        {"$s2", 18},
        {"$s3", 19},
        {"$s4", 20},
        {"$s4", 21},
        {"$s6", 22},
        {"$s7", 23},
        {"$t8", 24},
        {"$t9", 25},
        {"$k0", 26},
        {"$k1", 27},
        {"$gp", 28},
        {"$sp", 29},
        {"$fp", 30},
        {"$ra", 31}, 
        }

};

class Control_Unit{ // allows simulation of 5-stage pipeline
// Control unit signals:
    bool RegDest = false;
    bool ALUOp = false;
    bool ALUSrc = false;
    bool MemRead = false;
    bool MemWrite = false;
    bool MemtoReg = false;
    bool RegWrite = false;
    bool Branch = false; // jump / beq 

    void fetch(){} // fetch instruction, update pc

    void decode(){} // decode instruction, read from register file

    void execute(){} // execute arithmetic instruction, calculate mem addr

    void memory(){} // read/write data from/to memory

    void write_back(){} // write data back to regFile
};

class CPU_ALU{ // handles instructions 

    void ADD(string rDest, string r1, string r2){ // scope ???
        register.registerFile[regMap[rDest]] = register.registerFile[regMap[r1]] + register.registerFile[regMap[rDest]];
    }

    void ADDI(string rDest, string r1, int num){}

    void SUB(string rDest, string r1, string r2){}

    void MUL(string rDest, string r1, string r2){}

    void AND(string rDest, string r1, string r2){}

    void OR(string rDest, string r1, string r2){}

    void SLL(string rDest, string r1, int num){}

    void SLR(string rDest, string r1, int num){}

    // BRANCH Intructions 
    void BEQ(string rDest, string r1, string label){}

    void JUMP(string label){}

    // do nothing 
    void NOP(){}
};

class CPU_Memory{ // holds cpu memory and handles lw and sw instructions 
    int data_memory[1024]; // for sw and lw

    void LW(string rDest, int offset, int r1){}

    void SW(string r1, int offest, string rDest){}
};

class MIPS_CPU{
    public:
        Registers registers;
        CPU_ALU ALU;
        CPU_Memory Memory;
        Control_Unit control_unit;
        int PC; // need for beq / jump
        string IR; // instruction register - current instruction
        bool debugMode;

        MIPS_CPU() {
            int PC = 0;
            string IR = "";
            bool debugMode = false;
        }
    
};
