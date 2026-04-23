#include <cstdint>
#include <string>
#include <unordered_map>
#include "mips_simulator.hpp"
#include <vector>
#include <regex>
#include <iostream>
using namespace std;

/* Simulator Notes: 
- State registers: show state at point between stages in pipeline (pipeline latches)
- check ???, use find 

*/




// --- CPU --- //

CPU :: CPU() : ALU(regFile), control_unit(regFile, memory), memory(regFile){ // initialize ALU with reference to regFile
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

void RegisterFile :: dump(){
    for (const auto& pair : regMap) {
        cout << pair.first << ": " << registers[pair.second];
        if (pair.second % 4 == 3) { // print 4 registers per line
            cout << '\n';
        } else {
            cout << '\t';
        }
    }
}


// --- ALU --- //

CPU_ALU :: CPU_ALU(RegisterFile &_regFile) : regFile(_regFile) {} // references regFile in CPU class


void CPU_ALU :: ADD(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) + regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

void CPU_ALU :: ADDI(string rDest, string r1, int num){
        int result = regFile.get_val(r1) + num;
        regFile.set_val(rDest, result);
    }

void CPU_ALU :: SUB(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) - regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

void CPU_ALU :: MUL(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) * regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

void CPU_ALU :: AND(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) & regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

void CPU_ALU :: OR(string rDest, string r1, string r2){
    int result = regFile.get_val(r1) || regFile.get_val(r2);
    regFile.set_val(rDest, result);
}

void CPU_ALU :: SLL(string rDest, string r1, int num){
    int result = regFile.get_val(r1) << num;
    regFile.set_val(rDest, result);
}

void CPU_ALU :: SLR(string rDest, string r1, int num){
    int result = regFile.get_val(r1) >> num;
    regFile.set_val(rDest, result);
}

    // BRANCH Intructions 
void CPU_ALU :: BEQ(string rDest, string r1, string label){
    if (regFile.get_val(rDest) == regFile.get_val(r1)){
        // update PC to label address
    }
}

void CPU_ALU :: JUMP(string label){
    // update PC to label address
}

    // do nothing 
void CPU_ALU :: NOP(){
    return;
}



// --- MEMORY --- //
CPU_Memory :: CPU_Memory(RegisterFile &_regFile) : regFile(_regFile){} // for read/write to/from regFile

void CPU_Memory :: set_addr(int addr, int val){
    data_memory[addr] = val;
}
int CPU_Memory :: get_addr(int addr){
    return data_memory[addr];
}

void CPU_Memory :: LW(string rDest, int offset, string r1){
    int addr = regFile.get_val(r1) + offset;
    int val = get_addr(addr);
    regFile.set_val(rDest, val);
}

void CPU_Memory :: SW(string r1, int offset, string rDest){
    int addr = regFile.get_val(rDest) + offset;
    int val = regFile.get_val(r1);
    set_addr(addr, val);
}

void CPU_Memory :: dump(){
    for (int i = 0; i < 100; i++) {
        cout << "Address " << i << ": " << data_memory[i] << '\n';
    }
}



// --- INSTRUCTION HELPER FUNCTIONS --- //

vector<string> Instruction :: tokenize_instr(const string &rawLine){ //  split string into individual components (opcode, operands)
    string cleaned = rawLine;
    vector<string> tokens;
    regex pattern("[,()]+");   // remove commas spaces and parentheses
    
    cleaned = regex_replace(cleaned, pattern, " ");
    stringstream ss(cleaned); // treats string as stream to split into tokens
    while (ss >> cleaned) {
        tokens.push_back(cleaned);
    }

    return tokens;
}

Opcode parseOpcode(const string& opText) {
    static const unordered_map<string, Opcode> opcodeMap = {
        {"add", Opcode::ADD},   {"addi", Opcode::ADDI}, {"sub", Opcode::SUB},
        {"mul", Opcode::MUL},   {"and", Opcode::AND_OP}, {"or", Opcode::OR_OP},
        {"sll", Opcode::SLL},   {"srl", Opcode::SRL},   {"lw", Opcode::LW},
        {"sw", Opcode::SW},     {"beq", Opcode::BEQ},   {"j", Opcode::J},
        {"nop", Opcode::NOP}
    };

    auto validOp = opcodeMap.find(opText);
    if (validOp == opcodeMap.end()) { // if opcode not found in map, throw error
        throw runtime_error("Unsupported opcode: " + opText);
    }
    return validOp->second; // return corresponding enum value
}

int32_t parseNumber(const string& token) { // convert string to int
    return static_cast<int32_t>(stol(token, nullptr, 0)); 
}

int handleLabel(const string& token, const unordered_map<string, int>& labels) {
    auto validLabel = labels.find(token);
    if (validLabel != labels.end()) {
        return validLabel->second; // return address if token is a label
    }
    return static_cast<int>(stol(token, nullptr, 0)); // otherwise, convert token to number (for J instruction)
}


// --- CONTROL UNIT --- //

Control_Unit :: Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory) : regFile(_regFile), memory(_memory) {}

vector<string> Control_Unit :: fetch(const string& rawLine) {
    vector<string> tokens = instrReg.tokenize_instr(rawLine);
    if (tokens.empty()) {
        return {};
    }

    return tokens;
}


Instruction Control_Unit :: decode(const string& rawLine, vector<string> tokens,
                                         const unordered_map<string, int>& labels){ // decode instruction, read from register file

    Instruction instr;
    ControlSignals cs;
    string opText = tokens[0];
    instr.op = parseOpcode(opText);

    switch (instr.op) {
        case Opcode::ADD:
            if (tokens.size() != 4) {
                throw runtime_error("Bad ADD format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::SUB:
            if (tokens.size() != 4) {
                throw runtime_error("Bad SUB format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);
            
            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::MUL:
                if (tokens.size() != 4) {
                throw runtime_error("Bad MUL format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::AND_OP:
            if (tokens.size() != 4) {
                throw runtime_error("Bad AND format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::OR_OP:
            if (tokens.size() != 4) {
                throw runtime_error("Bad R-type format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = regFile.regMap[tokens[3]];

            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::ADDI:
            if (tokens.size() != 4) {
                throw runtime_error("Bad ADDI format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.imm = parseNumber(tokens[3]);

            cs.aluSrc = true;
            cs.regWrite = true;
            break;

        case Opcode::SLL:
        case Opcode::SRL:
            if (tokens.size() != 4) {
                throw runtime_error("Bad shift format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rt = regFile.regMap[tokens[2]];
            instr.shamt = static_cast<int>(parseNumber(tokens[3]));

            cs.regDst = true;
            cs.regWrite = true;
            break;

        case Opcode::LW:
            if (tokens.size() != 4) {
                throw runtime_error("Bad LW format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.imm = parseNumber(tokens[2]);
            instr.rs = regFile.regMap[tokens[3]];

            cs.aluSrc = true;
            cs.memRead = true;
            cs.memToReg = true;
            cs.regWrite = true;
            break;

        case Opcode::SW:
            if (tokens.size() != 4) {
                throw runtime_error("Bad SW format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.imm = parseNumber(tokens[2]);
            instr.rs = regFile.regMap[tokens[3]];

            cs.aluSrc = true;
            cs.memWrite = true;
            break;

        case Opcode::BEQ:
            if (tokens.size() != 4) {
                throw runtime_error("Bad BEQ format: " + rawLine);
            }
            instr.rs = regFile.regMap[tokens[1]];
            instr.rt = regFile.regMap[tokens[2]];
            instr.label = tokens[3];
            instr.target = handleLabel(tokens[3], labels);

            cs.branch = true;
            break;

        case Opcode::J:
            if (tokens.size() != 2) {
                throw runtime_error("Bad J format: " + rawLine);
            }
            instr.label = tokens[1];
            instr.target = handleLabel(tokens[1], labels);

            cs.jump = true;
            break;

        case Opcode::NOP:
            if (tokens.size() != 1) {
                throw runtime_error("Bad NOP format: " + rawLine);
            }
            break;
    }

    return instr;

}


void Control_Unit :: execute(){ // execute arithmetic instruction, calculate mem addr
}

void Control_Unit :: mem(){ // read/write data from/to memory
}

void Control_Unit :: write_back(){ // write data back to regFile
}







