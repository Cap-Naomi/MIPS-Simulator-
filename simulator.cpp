#include <cstdint>
#include <string>
#include <unordered_map>
#include "mips_simulator.hpp"
#include <iostream>
using namespace std;

/* Notes: 
- State registers: show state at point between stages in pipeline (pipeline latches)
*/




// --- CPU --- //

CPU :: CPU() : ALU(regFile), control_unit(regFile, memory){ // initialize ALU with reference to regFile
    int PC = 0;
    string IR = "";
    bool debugMode = false;
}


// --- REGISTER FILE --- //

int RegisterFile :: get_val(string regName){
    return registers[regMap[regName]];
}

void RegisterFile :: set_val(string regName, int val){
    registers[regMap[regName]] = val;
}



// --- ALU --- //

CPU_ALU :: CPU_ALU(RegisterFile &_regFile) : regFile(_regFile) {} // references regFile in CPU class


void CPU_ALU :: ADD(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) + regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

//     void ADDI(string rDest, string r1, int num){ cout << "addi"; }

//     void SUB(string rDest, string r1, string r2){}

//     void MUL(string rDest, string r1, string r2){}

//     void AND(string rDest, string r1, string r2){}

//     void OR(string rDest, string r1, string r2){}

//     void SLL(string rDest, string r1, int num){}

//     void SLR(string rDest, string r1, int num){}

//     // BRANCH Intructions 
//     void BEQ(string rDest, string r1, string label){}

//     void JUMP(string label){}

//     // do nothing 
//     void NOP(){}


// --- CONTROL UNIT --- //

Control_Unit :: Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory) : regFile(_regFile), memory(_memory) {}

// void fetch(){} // fetch instruction, update pc

// void decode(){} // decode instruction, read from register file

// void execute(){} // execute arithmetic instruction, calculate mem addr

// void memory(){} // read/write data from/to memory

// void write_back(){} // write data back to regFile



// --- MEMORY --- //
CPU_Memory :: CPU_Memory(RegisterFile &_regFile) : regFile(_regFile){} // for read/write to/from regFile

//     void LW(string rDest, int offset, int r1){}

//     void SW(string r1, int offest, string rDest){}