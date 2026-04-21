#include <cstdint>
#include <string>
#include <map>

class Registers{
    registerFile[32]; // holds 32 register
    unordered_map<string, int> registerMap; // maps each register to index of registerFile
};


class CPU{
    Registers registers;
    int memory[1024]; // dont know what this will do yet
    int PC = 0; // need for beq / jump --- but dont really know why 


    void ADD(string rDest, string r1, string r2);
    void ADDI(string rDest, string r1, int num);
    void SUB(string rDest, string r1, string r2);
    void MUL(string rDest, string r1, string r2);
    void AND(string rDest, string r1, string r2);
    void OR(string rDest, string r1, string r2);
    void SLL(string rDest, string r1, int num);
    void SLR(string rDest, string r1, int num);
    void LW(string rDest, int offset, int r1);
    void SW(string int r1, int offest, string rDest);
    void BEQ(string rDest, string r1, string label);
    void JUMP(string label);
    void NOP();
};