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



// --- REGISTER FILE --- //

int RegisterFile :: get_val(string regName){
    return registers[regMap[regName]];
}

void RegisterFile :: set_val(string regName, int val){
    registers[regMap[regName]] = val;
}

void RegisterFile :: dump() const{
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

void CPU_Memory :: dump() const{
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
    string opText = tokens[0];
    instr.op = parseOpcode(opText);
    instr.raw = rawLine; // ???

    switch (instr.op) {
        case Opcode::ADD:
            if (tokens.size() != 4) {
                throw runtime_error("Bad ADD format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::SUB:
            if (tokens.size() != 4) {
                throw runtime_error("Bad SUB format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);
            
            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::MUL:
                if (tokens.size() != 4) {
                throw runtime_error("Bad MUL format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::AND_OP:
            if (tokens.size() != 4) {
                throw runtime_error("Bad AND format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = parseNumber(tokens[3]);

            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::OR_OP:
            if (tokens.size() != 4) {
                throw runtime_error("Bad R-type format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.rt = regFile.regMap[tokens[3]];

            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::ADDI:
            if (tokens.size() != 4) {
                throw runtime_error("Bad ADDI format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.rs = regFile.regMap[tokens[2]];
            instr.imm = parseNumber(tokens[3]);

            instr.cs.aluSrc = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::SLL:
        case Opcode::SRL:
            if (tokens.size() != 4) {
                throw runtime_error("Bad shift format: " + rawLine);
            }
            instr.rd = regFile.regMap[tokens[1]];
            instr.rt = regFile.regMap[tokens[2]];
            instr.shamt = static_cast<int>(parseNumber(tokens[3]));

            instr.cs.regDst = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::LW:
            if (tokens.size() != 4) {
                throw runtime_error("Bad LW format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.imm = parseNumber(tokens[2]);
            instr.rs = regFile.regMap[tokens[3]];

            instr.cs.aluSrc = true;
            instr.cs.memRead = true;
            instr.cs.memToReg = true;
            instr.cs.regWrite = true;
            break;

        case Opcode::SW:
            if (tokens.size() != 4) {
                throw runtime_error("Bad SW format: " + rawLine);
            }
            instr.rt = regFile.regMap[tokens[1]];
            instr.imm = parseNumber(tokens[2]);
            instr.rs = regFile.regMap[tokens[3]];

            instr.cs.aluSrc = true;
            instr.cs.memWrite = true;
            break;

        case Opcode::BEQ:
            if (tokens.size() != 4) {
                throw runtime_error("Bad BEQ format: " + rawLine);
            }
            instr.rs = regFile.regMap[tokens[1]];
            instr.rt = regFile.regMap[tokens[2]];
            instr.label = tokens[3];
            instr.target = handleLabel(tokens[3], labels);

            instr.cs.branch = true;
            break;

        case Opcode::J:
            if (tokens.size() != 2) {
                throw runtime_error("Bad J format: " + rawLine);
            }
            instr.label = tokens[1];
            instr.target = handleLabel(tokens[1], labels);

            instr.cs.jump = true;
            break;

        case Opcode::NOP:
            if (tokens.size() != 1) {
                throw runtime_error("Bad NOP format: " + rawLine);
            }
            break;
    }

    return instr;

}

// --- CPU --- //

CPU ::CPU(bool debug = false) : ALU(regFile), control_unit(regFile, memory), memory(regFile), debugMode(debug){}

// --- PIPELINE STAGES --- //

IF_ID CPU :: fetchStage() {
    IF_ID next; 
    if (pc >= 0 && pc < static_cast<int>(program.size())) {
        next.valid = true;
        next.pc = pc;
        next.instr = program[pc];
        ++pc;
    }
    return next;
}

ID_EX CPU :: decodeStage(const IF_ID& current) {
    ID_EX next;
    if (!current.valid) {
        return next;
    }

    next.valid = true;
    next.pc = current.pc;
    next.instr = current.instr;
    next.rsVal = regFile.registers[current.instr.rs];
    next.rtVal = regFile.registers[current.instr.rt];
    next.control = current.instr.cs;
}

EX_MEM CPU :: executeStage(const ID_EX& current) {
    EX_MEM next;
    if (!current.valid) {
        return next;
    }

    next.valid = true;
    next.pc = current.pc;
    next.instr = current.instr;
    next.rtVal = current.rtVal;
    next.control = current.control;
    next.destReg = current.control.regDst ? current.instr.rd : current.instr.rt; // for R-type, dest reg is rd; for I-type, dest reg is rt

    switch (current.instr.op) {
        case Opcode::ADD:
            next.aluResult = current.rsVal + current.rtVal;
            break;
        case Opcode::ADDI:
            next.aluResult = current.rsVal + current.instr.imm;
            break;
        case Opcode::SUB:
            next.aluResult = current.rsVal - current.rtVal;
            break;
        case Opcode::MUL:
            next.aluResult = current.rsVal * current.rtVal;
            break;
        case Opcode::AND_OP:
            next.aluResult = current.rsVal & current.rtVal;
            break;
        case Opcode::OR_OP:
            next.aluResult = current.rsVal | current.rtVal;
            break;
        case Opcode::SLL:
            next.aluResult = static_cast<int32_t>(static_cast<uint32_t>(current.rtVal) << current.instr.shamt);
            break;
        case Opcode::SRL:
            next.aluResult = static_cast<int32_t>(static_cast<uint32_t>(current.rtVal) >> current.instr.shamt);
            break;
        case Opcode::LW:
        case Opcode::SW:
            next.aluResult = current.rsVal + current.instr.imm;
            break;
        case Opcode::BEQ:
            next.zero = (current.rsVal == current.rtVal);
            next.takeBranch = next.zero;
            next.branchTarget = current.instr.target;
            next.aluResult = current.rsVal - current.rtVal;
            break;
        case Opcode::J:
            next.takeBranch = true;
            next.branchTarget = current.instr.target;
            break;
        case Opcode::NOP:
            break;
    }

    return next;
}

MEM_WB CPU :: memoryStage(const EX_MEM& current) {
    MEM_WB next;
    if (!current.valid) {
        return next;
    }

    next.valid = true;
    next.pc = current.pc;
    next.instr = current.instr;
    next.aluResult = current.aluResult;
    next.destReg = current.destReg;
    next.control = current.control;

    if (current.control.memRead) {
        next.memData = memory.get_addr(current.aluResult);
    }
    if (current.control.memWrite) {
        memory.set_addr(current.aluResult, current.rtVal);
    }

    return next;
}

void CPU :: writeBackStage(const MEM_WB& current) {
    if (!current.valid) {
        return;
    }

    if (current.control.regWrite) { // if instruction writes to register, write either memData (for LW) or aluResult (for R-type and ADDI) back to regFile
        // registers.write(current.destReg, current.control.memToReg ? current.memData : current.aluResult);
        int writeData = current.control.memToReg ? current.memData : current.aluResult;
        string destRegName;
        for (const auto& pair : regFile.regMap) {
            if (pair.second == current.destReg) {
                destRegName = pair.first;
                break;
            }
        }
        regFile.set_val(destRegName, writeData);
    }
}

static string controlText(const ControlSignals& c) {
    ostringstream out;
    out << "RegDst=" << c.regDst
        << " ALUSrc=" << c.aluSrc
        << " MemRead=" << c.memRead
        << " MemWrite=" << c.memWrite
        << " MemToReg=" << c.memToReg
        << " RegWrite=" << c.regWrite
        << " Branch=" << c.branch
        << " Jump=" << c.jump
        << " ALUOp=" << c.aluOp;
    return out.str();
}

void CPU :: printPipelineState() const {
    cout << "IF/ID  : ";
    if (if_id.valid) {
        cout << "PC=" << if_id.pc << " Instr={" << if_id.instr.raw << "}";
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "ID/EX  : ";
    if (id_ex.valid) {
        cout << "PC=" << id_ex.pc
                    << " Instr={" << id_ex.instr.raw << "} "
                    << "rsVal=" << id_ex.rsVal
                    << " rtVal=" << id_ex.rtVal << '\n'
                    << "         Control: " << controlText(id_ex.control);
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "EX/MEM : ";
    if (ex_mem.valid) {
        cout << "PC=" << ex_mem.pc
                    << " Instr={" << ex_mem.instr.raw << "} "
                    << "ALUResult=" << ex_mem.aluResult
                    << " rtVal=" << ex_mem.rtVal
                    << " destReg=" << ex_mem.destReg
                    << " takeBranch=" << ex_mem.takeBranch;
        if (ex_mem.takeBranch) {
            cout << " target=" << ex_mem.branchTarget;
        }
        cout << '\n'
                    << "         Control: " << controlText(ex_mem.control);
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "MEM/WB : ";
    if (mem_wb.valid) {
        cout << "PC=" << mem_wb.pc
                    << " Instr={" << mem_wb.instr.raw << "} "
                    << "ALUResult=" << mem_wb.aluResult
                    << " memData=" << mem_wb.memData
                    << " destReg=" << mem_wb.destReg << '\n'
                    << "         Control: " << controlText(mem_wb.control);
    } else {
        cout << "empty";
    }
    cout << '\n';
}


// --- SIMULATION CONTROL --- //

void CPU :: run() {
    bool pipelineEmpty = !(if_id.valid || id_ex.valid || ex_mem.valid || mem_wb.valid);

    while (pc < static_cast<int>(program.size()) || !pipelineEmpty) {
        ++cycle;

        writeBackStage(mem_wb);

        MEM_WB next_mem_wb = memoryStage(ex_mem);
        EX_MEM next_ex_mem = executeStage(id_ex);
        ID_EX next_id_ex = decodeStage(if_id);
        IF_ID next_if_id = fetchStage();

        if (next_ex_mem.takeBranch) {
            next_id_ex = ID_EX{};
            next_if_id = IF_ID{};
            pc = next_ex_mem.branchTarget;
        }

        mem_wb = next_mem_wb;
        ex_mem = next_ex_mem;
        id_ex = next_id_ex;
        if_id = next_if_id;

        if (debugMode) {
            printDebugState();
        }
    }
}

void loadProgram(const string& filename) {
}


// --- OUTPUT/DEBUGGING --- //

void CPU :: printDebugState() const {
    cout << "\n========================================\n";
    cout << "Cycle: " << cycle << '\n';
    cout << "Next fetch PC: " << pc << '\n';
    printPipelineState();
    cout << '\n';
    regFile.dump();
    cout << '\n';
    memory.dump();
    cout << "========================================\n";
}

void CPU :: printFinalState() const {
    cout << "\nFinal Machine State\n";
    cout << "===================\n";
    regFile.dump();
    cout << '\n';
    memory.dump();
}


