#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <array>

using namespace std;

// ---------------- REGISTER FILE ----------------
class RegisterFile {
public:
    int registers[32] = {0};
    unordered_map<string, int> regMap{
        {"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3},
        {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7},
        {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11},
        {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15},
        {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19},
        {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
        {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27},
        {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}
    };

    int get_val(const string &regName) const;
    void set_val(const string &regName, int val);
    void dump() const;

    std::array<std::string,32> regNames{
    "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
    "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
    "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
    "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"
    };

};



// ---------------- MEMORY ----------------
class CPU_Memory {
public:
    static const int MEM_SIZE = 100; // 1024
    int32_t data_memory[MEM_SIZE] = {0};
    RegisterFile &regFile;

    CPU_Memory(RegisterFile &_regFile);
    int get_addr(int addr) const;
    void set_addr(int addr, int val);
    void LW(const string &rDest, int offset, const string &r1);
    void SW(const string &r1, int offset, const string &rDest);
    void dump() const;
};

// ---------------- CONTROL SIGNALS / OPCODE ----------------
struct ControlSignals {
    bool regDst = false; 
    bool aluSrc = false;
    bool memRead = false;
    bool memWrite = false;
    bool memToReg = false;
    bool regWrite = false;
    bool branch = false;
    bool jump = false;
    string aluOp = "NOP";
};

enum class Opcode {
    ADD, ADDI, SUB, MUL, AND_OP, OR_OP, SLL, SRL, LW, SW, BEQ, J, NOP
};

// ---------------- INSTRUCTION ----------------
class Instruction {
public:
    Opcode op = Opcode::NOP;
    int rs = 0;
    int rt = 0;
    int rd = 0; 
    int shamt = 0; // shift amount for SLL/SRL
    int32_t imm = 0;
    int target = -1;
    string label;
    string raw = "NOP";
    ControlSignals cs;

    static string trim(const string &s);
    vector<string> tokenize_instr(const string &rawLine) const;
    static Opcode parseOpcode(const string &opText);
};

// ---------------- CONTROL UNIT ----------------
class Control_Unit {
private:
    RegisterFile &regFile;
    CPU_Memory &memory;

public:
    Instruction instrReg;
    Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory);
    vector<string> fetch(const string &rawLine);
    Instruction decode(const string &rawLine, vector<string> tokens, const unordered_map<string, int> &labels);
    void execute();
    void mem();
    void write_back();
};

// ---------------- STATE REGISTERS ----------------
struct IF_ID {
    bool valid = false; 
    int pc = -1;
    Instruction instr;
};

struct ID_EX {
    bool valid = false; 
    int pc = -1;
    Instruction instr;
    int32_t rsVal = 0;
    int32_t rtVal = 0;
    ControlSignals control;
};

struct EX_MEM {
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

struct MEM_WB {
    bool valid = false;
    int pc = -1;
    Instruction instr;
    int32_t aluResult = 0;
    int32_t memData = 0;
    int destReg = 0;
    ControlSignals control;
};

// ---------------- CPU ----------------
class CPU {
public:
    RegisterFile regFile;
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

    CPU(bool debug = false);

    // pipeline stage functions
    IF_ID fetchStage();
    ID_EX decodeStage(const IF_ID &current);
    EX_MEM executeStage(const ID_EX &current);
    MEM_WB memoryStage(const EX_MEM &current);
    void writeBackStage(const MEM_WB &current);
    void printPipelineState() const;

    // cpu control functions
    void run();
    void loadProgram(const string &filename);
    void printDebugState() const;
    void printFinalState() const;

    // forwarding & hazard helpers 
    void handleForwarding(const ID_EX &current, int32_t &opA, int32_t &opB) const;
    bool handleHazards(const IF_ID &ifid, const ID_EX &idex) const;
};
