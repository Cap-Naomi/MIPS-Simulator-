#include <cstdint>
#include <string>
#include <unordered_map>
using namespace std;


class Registers{
    public:
        int registerFile[32]; // holds 32 register
        unordered_map<string, int> regMap; // maps each register to index of registerFile
};

class Control_Unit{
// Control unit signals:
    bool RegDest = false;
    bool ALUOp = false;
    bool ALUSrc = false;
    bool MemRead = false;
    bool MemWrite = false;
    bool MemtoReg = false;
    bool RegWrite = false;
    bool Branch = false; // jump / beq

    void fetch(); // fetch instruction, update pc

    void decode(); // decode instruction, read from register file

    void execute(); // execute arithmetic instruction, calculate mem addr

    void memory(); // read/write data from/to memory

    void write_back(); // write data back to regFile
};

class CPU_ALU
{
    void ADD(string rDest, string r1, string r2);
    void ADDI(string rDest, string r1, int num);
    void SUB(string rDest, string r1, string r2);
    void MUL(string rDest, string r1, string r2);
    void AND(string rDest, string r1, string r2);
    void OR(string rDest, string r1, string r2);
    void SLL(string rDest, string r1, int num);
    void SLR(string rDest, string r1, int num);
    void BEQ(string rDest, string r1, string label);
    void JUMP(string label);
    void NOP();
};

class CPU_Memory
{
    int data_memory[1024];
    void LW(string rDest, int offset, int r1);
    void SW(string r1, int offest, string rDest);
};

class MIPS_CPU
{
    Registers registers;
    CPU_ALU ALU;
    CPU_Memory Memory;
    Control_Unit control_unit;
    int PC;
    string IR;
    bool debugMode;

    MIPS_CPU();
};
