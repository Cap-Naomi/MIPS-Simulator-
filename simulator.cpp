// simulator.cpp
#include "mips_simulator.hpp"
#include <regex>
#include <fstream>
#include <iomanip>
#include <algorithm>

// ---------------- RegisterFile ----------------

int RegisterFile::get_val(const string &regName) const {
    auto index = regMap.find(regName); // if iterator reaches end, register name is invalid
    if (index == regMap.end()) throw runtime_error("Unknown register: " + regName); 
    return registers[index->second];
}

void RegisterFile::set_val(const string &regName, int val) {
    auto index = regMap.find(regName); 
    if (index == regMap.end()) throw runtime_error("Unknown register: " + regName); 
    if (index->second == 0) return; // $zero immutable
    registers[index->second] = val;
}

void RegisterFile::dump() const {
    cout << "\n*Register File*\n";
    static const string names[32] = {
        "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
        "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
        "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
        "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"
    };
 
    for (int i = 0; i < 32; ++i) { // output with 4 registers per line
        cout << left << setw(6) << names[i] << " = " << right << setw(9) << registers[i];
        if ((i + 1) % 4 == 0 || i == 31) cout << '\n'; else cout << "    ";
    }
}


// ---------------- CPU_Memory ----------------

CPU_Memory::CPU_Memory(RegisterFile &_regFile) : regFile(_regFile) {}

int CPU_Memory::get_addr(int addr) const {
    return data_memory[addr];
}

void CPU_Memory::set_addr(int addr, int val) {
    data_memory[addr] = val;
}

void CPU_Memory::LW(const string &rDest, int offset, const string &r1) {
    int addr = regFile.get_val(r1) + offset;
    int val = get_addr(addr);
    regFile.set_val(rDest, val);
}

void CPU_Memory::SW(const string &r1, int offset, const string &rDest) {
    int addr = regFile.get_val(rDest) + offset;
    int val = regFile.get_val(r1);
    set_addr(addr, val);
}

void CPU_Memory::dump() const {
    bool empty = true;
    cout << "\n*Data Memory*\n";
    for (int i = 0; i < MEM_SIZE; ++i) {
        if (data_memory[i] != 0){
            cout << "mem[" << i << "] = " << data_memory[i] << '\n';
            empty = false;
        }
    }
    if (empty) {
        cout << "\n<Memory Empty>\n";
        return;
    }

}

// ---------------- Instruction helpers ----------------

string Instruction::trim(const string &str) { // removes leading/trailing whitespace
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

vector<string> Instruction::tokenize_instr(const string &rawLine) const { // splits instruction into tokens
    vector<string> tokens;
    string cleaned = rawLine;
    regex pattern("[,()]+"); // replace commas and parentheses with spaces
    cleaned = regex_replace(cleaned, pattern, " ");
    regex multi("\\s+");
    cleaned = regex_replace(cleaned, multi, " ");
    cleaned = trim(cleaned);

    stringstream ss(cleaned);
    string token; 
    while (ss >> token){tokens.push_back(token);}

    return tokens;
}

Opcode Instruction::parseOpcode(const string &opText) {
    static const unordered_map<string, Opcode> opcodeMap = {
        {"add", Opcode::ADD}, {"addi", Opcode::ADDI}, {"sub", Opcode::SUB},
        {"mul", Opcode::MUL}, {"and", Opcode::AND_OP}, {"or", Opcode::OR_OP},
        {"sll", Opcode::SLL}, {"srl", Opcode::SRL}, {"lw", Opcode::LW},
        {"sw", Opcode::SW}, {"beq", Opcode::BEQ}, {"j", Opcode::J},
        {"nop", Opcode::NOP}
    };
    auto validOp = opcodeMap.find(opText);
    if (validOp == opcodeMap.end()){
        throw runtime_error("Unsupported opcode: " + opText);
    }

    return validOp->second;
}

// ---------------- Control_Unit (minimal helpers) ----------------

Control_Unit::Control_Unit(RegisterFile &_regFile, CPU_Memory &_memory) : regFile(_regFile), memory(_memory) {}

vector<string> Control_Unit::fetch(const string &rawLine) {
    Instruction instr;
    return instr.tokenize_instr(rawLine);
}

Instruction Control_Unit::decode(const string &rawLine, vector<string> tokens, const unordered_map<string, int> &labels) {
    Instruction instr;
    if (tokens.empty()) return instr;
    instr.raw = rawLine;
    instr.op = Instruction::parseOpcode(tokens[0]); 

    switch (instr.op) { // extract operands based on opcode, set control signals
        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::MUL:
        case Opcode::AND_OP:
        case Opcode::OR_OP:
            if (tokens.size() != 4) throw runtime_error("Bad R-type format: " + rawLine);
            instr.rd = regFile.regMap.at(tokens[1]);
            instr.rs = regFile.regMap.at(tokens[2]);
            instr.rt = regFile.regMap.at(tokens[3]);
            instr.cs.regDst = true; instr.cs.regWrite = true;
            break;

        case Opcode::ADDI:
            if (tokens.size() != 4) throw runtime_error("Bad ADDI format: " + rawLine);
            instr.rt = regFile.regMap.at(tokens[1]);
            instr.rs = regFile.regMap.at(tokens[2]);
            instr.imm = static_cast<int32_t>(stol(tokens[3], nullptr, 0));
            instr.cs.aluSrc = true; instr.cs.regWrite = true; 
            break;

        case Opcode::SLL:
        case Opcode::SRL:
            if (tokens.size() != 4) throw runtime_error("Bad shift format: " + rawLine);
            instr.rd = regFile.regMap.at(tokens[1]);
            instr.rt = regFile.regMap.at(tokens[2]);
            instr.shamt = static_cast<int>(stol(tokens[3], nullptr, 0));
            instr.cs.regDst = true; instr.cs.regWrite = true;
            break;

        case Opcode::LW:
            if (tokens.size() != 4) throw runtime_error("Bad LW format: " + rawLine);
            instr.rt = regFile.regMap.at(tokens[1]);
            instr.imm = static_cast<int32_t>(stol(tokens[2], nullptr, 0));
            instr.rs = regFile.regMap.at(tokens[3]);
            instr.cs.aluSrc = true; instr.cs.memRead = true; instr.cs.memToReg = true; instr.cs.regWrite = true;
            break;

        case Opcode::SW:
            if (tokens.size() != 4) throw runtime_error("Bad SW format: " + rawLine);
            instr.rt = regFile.regMap.at(tokens[1]);
            instr.imm = static_cast<int32_t>(stol(tokens[2], nullptr, 0));
            instr.rs = regFile.regMap.at(tokens[3]);
            instr.cs.aluSrc = true; instr.cs.memWrite = true;
            break;

        case Opcode::BEQ:
            if (tokens.size() != 4) throw runtime_error("Bad BEQ format: " + rawLine);
            instr.rs = regFile.regMap.at(tokens[1]);
            instr.rt = regFile.regMap.at(tokens[2]);
            instr.label = tokens[3];

            if (labels.count(instr.label)) {instr.target = labels.at(instr.label);}
            else {instr.target = static_cast<int>(stol(instr.label, nullptr, 0));}
            instr.cs.branch = true;
            break;

        case Opcode::J:
            if (tokens.size() != 2) throw runtime_error("Bad J format: " + rawLine);
            instr.label = tokens[1];

            if (labels.count(instr.label)) {instr.target = labels.at(instr.label);}
            else {instr.target = static_cast<int>(stol(instr.label, nullptr, 0));}
            instr.cs.jump = true;
            break;

        case Opcode::NOP:
            break;
    }
    return instr;
}

// ---------------- CPU implementation ----------------

CPU::CPU(bool debug) : memory(regFile), control_unit(regFile, memory), debugMode(debug) {}

// fetch stage
IF_ID CPU::fetchStage() {
    IF_ID next; 
    if (pc >= 0 && pc < static_cast<int>(program.size())) {
        next.valid = true; 
        next.pc = pc;
        next.instr = program[pc];
        ++pc;
    }
    return next;
}

// decode stage
ID_EX CPU::decodeStage(const IF_ID &current) {
    ID_EX next;
    if (!current.valid) return next;
    next.valid = true;
    next.pc = current.pc;
    next.instr = current.instr;
    next.rsVal = regFile.registers[current.instr.rs]; // read register values early for hazard detection and forwarding
    next.rtVal = regFile.registers[current.instr.rt];
    next.control = current.instr.cs;
    return next;
}

// choose opA/opB for EX stage using forwarding from EX/MEM and MEM/WB
void CPU::handleForwarding(const ID_EX &current, int32_t &opA, int32_t &opB) const { 
    // default values from ID/EX
    opA = current.rsVal;
    opB = current.rtVal;

    // only forward if EX/MEM is writing to a register (and not $zero)
    if (ex_mem.valid && ex_mem.control.regWrite && ex_mem.destReg != 0) { 
        if (ex_mem.destReg == current.instr.rs) {
            opA = ex_mem.aluResult;
        }
        if (ex_mem.destReg == current.instr.rt) {
            opB = ex_mem.aluResult;
        }
    }

    // MEM/WB forwarding 
    if (mem_wb.valid && mem_wb.control.regWrite && mem_wb.destReg != 0) { 
        int32_t wbValue = mem_wb.control.memToReg ? mem_wb.memData : mem_wb.aluResult;
        if (mem_wb.destReg == current.instr.rs) {
            // only forward if EX/MEM didn't already forward (EX/MEM checked first)
            if (!(ex_mem.valid && ex_mem.control.regWrite && ex_mem.destReg == current.instr.rs)) { 
                opA = wbValue;
            }
        }
        if (mem_wb.destReg == current.instr.rt) {
            if (!(ex_mem.valid && ex_mem.control.regWrite && ex_mem.destReg == current.instr.rt)) {
                opB = wbValue;
            }
        }
    }
}

// RAW hazard: ID/EX is a LW and IF/ID instruction uses the loaded register
// only need to check for RAW hazard, other hazards are resolved by forwarding
bool CPU::handleHazards(const IF_ID &if_id, const ID_EX &id_ex) const {
    if (!id_ex.valid) return false;
    if (!id_ex.control.memRead) return false; // not a load in EX stage
    if (!if_id.valid) return false;
    int loadDest = id_ex.instr.rt; // LW writes to rt
    if (loadDest == 0) return false;
    // if next instruction uses rs or rt as source
    int if_rs = if_id.instr.rs;
    int if_rt = if_id.instr.rt;
    if (loadDest == if_rs || loadDest == if_rt) return true; 
    return false;
}

// execute stage
EX_MEM CPU::executeStage(const ID_EX &current) {
    EX_MEM next;
    if (!current.valid) return next;
    next.valid = true;
    next.pc = current.pc;
    next.instr = current.instr;
    next.control = current.control;
    next.destReg = current.control.regDst ? current.instr.rd : current.instr.rt; 

    // Resolve operands with forwarding
    int32_t opA = 0, opB = 0; 
    handleForwarding(current, opA, opB); 
    next.rtVal = opB; // save real rt value for SW, BEFORE override 
    if (current.control.aluSrc) { opB = current.instr.imm; }

    switch (current.instr.op) {
        case Opcode::ADD: next.aluResult = opA + opB; break;
        case Opcode::ADDI: next.aluResult = opA + current.instr.imm; break;
        case Opcode::SUB: next.aluResult = opA - opB; break;
        case Opcode::MUL: next.aluResult = opA * opB; break;
        case Opcode::AND_OP: next.aluResult = opA & opB; break;
        case Opcode::OR_OP: next.aluResult = opA | opB; break;
        case Opcode::SLL: next.aluResult = static_cast<int32_t>(static_cast<uint32_t>(opB) << current.instr.shamt); break;
        case Opcode::SRL: next.aluResult = static_cast<int32_t>(static_cast<uint32_t>(opB) >> current.instr.shamt); break;
        case Opcode::LW:
        case Opcode::SW:
            next.aluResult = opA + current.instr.imm;
            break;
        case Opcode::BEQ:
            next.zero = (opA == opB);
            next.takeBranch = next.zero;
            next.branchTarget = current.instr.target;
            next.aluResult = opA - opB;
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

// memory stage
MEM_WB CPU::memoryStage(const EX_MEM &current) {
    MEM_WB next;
    if (!current.valid) return next;
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

// writeback stage
void CPU::writeBackStage(const MEM_WB &current) {
    if (!current.valid) return;
    if (current.control.regWrite) {
        int writeData = current.control.memToReg ? current.memData : current.aluResult;
        if (current.destReg < 0 || current.destReg >= 32) throw runtime_error("Invalid destReg index");
        regFile.set_val(regFile.regNames[current.destReg], writeData);
        // string destRegName;
        // for (const auto &reg : regFile.regMap) {
        //     if (reg.second == current.destReg) { destRegName = reg.first; break; }
        // }
        // if (destRegName.empty()) throw runtime_error("Invalid destination register index: " + to_string(current.destReg));
        // regFile.set_val(destRegName, writeData);
    }
}

//  print control signals
static string printControl(const ControlSignals &c) {
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

void CPU::printPipelineState() const {
    cout << "IF/ID  : ";
    if (if_id.valid) {
        cout << "PC= " << if_id.pc << " Instr: {" << if_id.instr.raw << "}";
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "ID/EX  : ";
    if (id_ex.valid) {
        cout << "PC=" << id_ex.pc
             << " Instr: {" << id_ex.instr.raw << "} "
             << "rsVal: " << id_ex.rsVal
             << " rtVal: " << id_ex.rtVal << '\n'
             << "         Control: " << printControl(id_ex.control);
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "EX/MEM : ";
    if (ex_mem.valid) {
        cout << "PC=" << ex_mem.pc
             << " Instr: {" << ex_mem.instr.raw << "} "
             << "ALUResult: " << ex_mem.aluResult
             << " rtVal: " << ex_mem.rtVal
             << " destReg: " << ex_mem.destReg
             << " takeBranch: " << ex_mem.takeBranch;
        if (ex_mem.takeBranch) cout << " target: " << ex_mem.branchTarget;
        cout << '\n'
             << "         Control: " << printControl(ex_mem.control);
    } else {
        cout << "empty";
    }
    cout << '\n';

    cout << "MEM/WB : ";
    if (mem_wb.valid) {
        cout << "PC=" << mem_wb.pc
             << " Instr: {" << mem_wb.instr.raw << "} "
             << "ALUResult: " << mem_wb.aluResult
             << " memData: " << mem_wb.memData
             << " destReg: " << mem_wb.destReg << '\n'
             << "         Control: " << printControl(mem_wb.control);
    } else {
        cout << "empty";
    }
    cout << '\n';
}

// run: pipeline loop with hazard detection and forwarding
void CPU::run() {
    auto pipelineEmpty = [&]() { // helper to check if pipeline is fully empty (all stages invalid)
        return !(if_id.valid || id_ex.valid || ex_mem.valid || mem_wb.valid);
    };

    while (pc < static_cast<int>(program.size()) || !pipelineEmpty()) {
        ++cycle;

        // 1. Write back
        writeBackStage(mem_wb);

        // 2. Memory stage (from EX/MEM)
        MEM_WB next_mem_wb = memoryStage(ex_mem);

        // 3. Execute stage (from ID/EX) - uses forwarding helper which reads ex_mem and mem_wb
        EX_MEM next_ex_mem = executeStage(id_ex);

        // 4. Decode stage (from IF/ID)
        ID_EX next_id_ex = decodeStage(if_id);

        // 5. Fetch stage
        bool raw_hazard = handleHazards(if_id, id_ex); 
        IF_ID next_if_id = raw_hazard ? if_id : fetchStage();
        if (raw_hazard && !next_ex_mem.takeBranch) {
            next_id_ex = ID_EX{};
            if (pc > 0) --pc;
        }
        if (next_ex_mem.takeBranch) {
            next_id_ex = ID_EX{};
            next_if_id = IF_ID{};
            pc = (next_ex_mem.branchTarget == next_ex_mem.pc)
                ? static_cast<int>(program.size())
                : next_ex_mem.branchTarget;
        }

        mem_wb = next_mem_wb;
        ex_mem = next_ex_mem;
        id_ex = next_id_ex;
        if_id = next_if_id;

        if (debugMode) printDebugState();
    }
}

//  two-pass to parse labels
void CPU::loadProgram(const string &filename) {
    ifstream fin(filename); 
    if (!fin) throw runtime_error("Cannot open file: " + filename);

    vector<string> lines;
    unordered_map<string,int> labels;
    string line;
    int addr = 0;

    while (getline(fin, line)) { // first pass to extract labels and clean lines
        string str = Instruction::trim(line);
        if (str.empty()) continue;

        size_t colon = str.find(':');
        if (colon != string::npos) {
            string label = Instruction::trim(str.substr(0, colon));
            labels[label] = addr;
            str = Instruction::trim(str.substr(colon + 1));
        }

        if (!str.empty()) {
            lines.push_back(str);
            ++addr;
        }
    }

    for (string &line : lines) { // second pass to decode instructions
        auto tokens = control_unit.fetch(line);
        if (tokens.empty()) continue;
        Instruction instr = control_unit.decode(line, tokens, labels);
        instr.raw = line;
        program.push_back(instr);
    }
}

void CPU::printDebugState() const {
    cout << "\n========================================\n";
    cout << "Cycle: " << cycle << '\n';
    cout << "PC: " << pc << '\n';
    printPipelineState();
    cout << '\n';
    regFile.dump();
    cout << '\n';
    memory.dump();
    cout << "========================================\n";
}

void CPU::printFinalState() const {
    cout << "\n*Final Machine State*\n";
    cout << "===================\n";
    regFile.dump();
    cout << '\n';
    memory.dump();
}
