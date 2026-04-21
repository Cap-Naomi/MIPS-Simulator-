#include <cstdint>
#include <string>
#include <map>

/* Questions: 
- which emitted control signals should be shown (RegWrite, MemWrite, MemRead, ALUOp...)
- State registers: program counter, stack pointer, instruction register ? any others

*/


class Registers{
    registerFile[32];
    unordered_map<string, int> registerMap{
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

class ControlSignals{


};


class CPU{
    Registers registers;
    int memory[1024]; // for sw and lw ?
    int PC = 0; // need for beq / jump --- but dont really know why
    string IR; // instruction register - current instruction
    bool debugMode = false;

    void ADD(string rDest, string r1, string r2){
        registers
    }

    void ADDI(string rDest, string r1, int num){}

    void SUB(string rDest, string r1, string r2){}

    void MUL(string rDest, string r1, string r2){}

    void AND(string rDest, string r1, string r2){}

    void OR(string rDest, string r1, string r2){}

    void SLL(string rDest, string r1, int num){}

    void SLR(string rDest, string r1, int num){}

    void LW(string rDest, int offset, int r1){}

    void SW(string int r1, int offest, string rDest){}

    void BEQ(string rDest, string r1, string label){}

    void JUMP(string label){}

    void NOP(){}

};