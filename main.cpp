#include <iostream>
#include <fstream>
#include "mips_simulator.hpp"
using namespace std;

/* Main Notes:
 * two-pass/parse approach, iterate through instructions twice (bc of jumps)
    - store jump labels in hashmap ["label name"] : number in program counter
    - on second pass, use label hashmap to jump to place in program 

* on second pass: need to extract [operation] [reg] -- [depends on instr] -- [dep on instr]

* need to simulate 5 stage pipeline process (implementing control unit)

*/

int main(int argc, char* argv[]) {

    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./mips_sim input.asm [debug]\n";
        return 1;
    }

    bool debug = false;
    if (argc == 3) {
        string flag = argv[2];
        if (flag == "debug" || flag == "--debug" || flag == "-d") {
            debug = true;
        } else {
            cerr << "Usage: ./mips_sim input.asm [debug]\n";
            return 1;
        }
    }

    try {
        CPU mips_sim(debug);
        mips_sim.loadProgram(argv[1]);
        mips_sim.run();
        mips_sim.printFinalState();
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << '\n'; 
        return 1;
    }
}