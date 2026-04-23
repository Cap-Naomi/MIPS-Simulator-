#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
using namespace std;

class RegisterFile{
    public:
        int registers[32] = {0}; // 32 register initialized to 0
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
        void dump() const;
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
        int32_t data_memory[100] = {0}; // 100 memory addresses initialized to 0
        RegisterFile &regFile;

        CPU_Memory(RegisterFile &_regFile);
        int get_addr(int addr);
        void set_addr(int addr, int val);
        void LW(string rDest, int offset, string r1); 
        void SW(string r1, int offset, string rDest);
        void dump() const; 
};

struct ControlSignals {
    bool regDst = false;
    bool aluSrc = false; // use immediate value instead of reg value for 2nd ALU operand
    bool memRead = false;
    bool memWrite = false;
    bool memToReg = false;
    bool regWrite = false;
    bool branch = false; // for beq
    bool jump = false;
    string aluOp = "NOP";
    
};

enum class Opcode {
    ADD,
    ADDI,
    SUB,
    MUL,
    AND_OP,
    OR_OP,
    SLL,
    SRL,
    LW,
    SW,
    BEQ,
    J,
    NOP
};


class Instruction { // dont think op and raw should be initialized with nop ???
    public:
        Opcode op = Opcode::NOP;
        int rs = 0; // reg source
        int rt = 0; // reg target
        int rd = 0;
        int shamt = 0; // shift amount for shift instructions
        int32_t imm = 0; // immediate value for I-type instructions (ADDI, LW, SW)
        int target = -1; // target address for branch/jump instructions (BEQ, J)
        string label;
        string raw = "NOP";
        ControlSignals cs; // control signals determined during decode stage
        
        vector<string> tokenize_instr(const string &rawLine); // remove commas, extra spaces, etc
        Opcode parseOpcode(const string& opText); // convert opcode text to enum

};

class Control_Unit{ // needs access to regFile and memory 
    private:
        RegisterFile &regFile; 
        CPU_Memory &memory;

    public:
        Instruction instrReg; // holds current instruction being processed
        Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory);
        vector<string> fetch(const string& rawLine); // fetch instruction, update pc
        Instruction decode(const string& rawLine, vector<string> tokens, const unordered_map<string, int>& labels); // decode instruction tokens, read from register file
        void execute(); // execute arithmetic instruction, calculate mem addr
        void mem(); // read/write data from/to memory
        void write_back(); // write data back to regFile
};




// --- STATE REGISTERS --- //
struct IF_ID { // fetch/decode pipeline register
    bool valid = false;
    int pc = -1;
    Instruction instr;
};

struct ID_EX { // decode/execute 
    bool valid = false;
    int pc = -1;
    Instruction instr;
    int32_t rsVal = 0;
    int32_t rtVal = 0;
    ControlSignals control;
};

struct EX_MEM { // execute/memory
    bool valid = false;
    int pc = -1;
    Instruction instr;
    int32_t aluResult = 0;
    int32_t rtVal = 0; 
    int destReg = 0;
    bool zero = false;
    bool takeBranch = false;
    int branchTarget = -1;
    ControlSignals control;
};

struct MEM_WB { // memory/writeback
    bool valid = false;
    int pc = -1;
    Instruction instr;
    int32_t aluResult = 0;
    int32_t memData = 0;
    int destReg = 0;
    ControlSignals control;
};

// --- MIPS SIMULATOR CPU --- //

class CPU
{
    public:
        RegisterFile regFile;
        CPU_ALU ALU;
        CPU_Memory memory;
        Control_Unit control_unit;
        vector<Instruction> program;
        
        int pc = 0;
        int cycle = 0;
        bool debugMode = false;
        
        IF_ID if_id;
        ID_EX id_ex;
        EX_MEM ex_mem;
        MEM_WB mem_wb;


        
        // pipeline functions
        IF_ID fetchStage();
        ID_EX decodeStage(const IF_ID& current);
        EX_MEM executeStage(const ID_EX& current);
        MEM_WB memoryStage(const EX_MEM& current);
        void writeBackStage(const MEM_WB& current);
        void printPipelineState() const;

        // cpu control functions
        CPU(bool debug = false);
        void run();
        void loadProgram(const string& filename);
        void printDebugState() const;
        void printFinalState() const;

};