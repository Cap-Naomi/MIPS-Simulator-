#include <cstdint>
#include <string>
#include <unordered_map>
using namespace std;

class CPU
{
    public:
        RegisterFile regFile;
        CPU_ALU ALU;
        CPU_Memory memory;
        Control_Unit control_unit;
        int PC;
        string IR;
        bool debugMode;

        CPU();
};


class RegisterFile{
    public:
        int registers[32]; // holds 32 register
        unordered_map<string, int> regMap{ // maps each register to index of registerFile
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
            {"$s5", 21},
            {"$s6", 22},
            {"$s7", 23},
            {"$t8", 24},
            {"$t9", 25},
            {"$k0", 26},
            {"$k1", 27},
            {"$gp", 28},
            {"$sp", 29},
            {"$fp", 30},
            {"$ra", 31}};

        int get_val(string regName);
        void set_val(string regName, int val);
};


class CPU_ALU
{
    private:
        RegisterFile &regFile; // accessing address of cpu obj regFile

    public:
        CPU_ALU(RegisterFile &_regFile);

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
    public:
        int data_memory[1024];
        RegisterFile regFile;

        CPU_Memory(RegisterFile);

        int get_addr(int addr);
        void set_addr(int addr, int val);

        void LW(string rDest, int offset, int r1); 
        void SW(string r1, int offest, string rDest);
};


class Control_Unit{ // needs access to regFile and memory 
    private:
    // Control unit signals:
        bool RegDest = false;
        bool ALUOp = false;
        bool ALUSrc = false;
        bool MemRead = false;
        bool MemWrite = false;
        bool MemtoReg = false;
        bool RegWrite = false;
        bool Branch = false; // jump / beq

        RegisterFile &regFile; 
        CPU_Memory &memory;
    
    public:
        Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory);

        void fetch(); // fetch instruction, update pc

        void decode(); // decode instruction, read from register file

        void execute(); // execute arithmetic instruction, calculate mem addr

        void mem(); // read/write data from/to memory

        void write_back(); // write data back to regFile
};
