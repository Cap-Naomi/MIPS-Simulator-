#include <iostream>
#include <fstream>
#include "mips_simulator.hpp"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        cerr << "Usage: ./mips_sim input.asm [debug]\n";
        return 1;
    }
    bool debug = false;
    if (argc == 3) {
        string flag = argv[2];
        if (flag == "debug" || flag == "-d") debug = true;
        else { cerr << "Usage: ./mips_sim input.asm [debug]\n"; return 1; }
    }
    try {
        CPU mips_sim(debug);
        mips_sim.loadProgram(argv[1]);
        mips_sim.run();
        mips_sim.printFinalState();
    } catch (const exception &ex) {
        cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}