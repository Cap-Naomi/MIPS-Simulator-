#include <iostream>
#include <fstream>
using namespace std;

/* Main Notes:
 * two-pass/parse approach, iterate through instructions twice (bc of jumps)
    - store jump labels in hashmap ["label name"] : number in program counter
    - on second pass, use label hashmap to jump to place in program 

* on second pass: need to extract [operation] [reg] -- [depends on instr] -- [dep on instr]

* need to simulate 5 stage pipeline process (implementing control unit)

*/

int main(){

    ifstream file("mips_input.asm");
    string line;

    if(!file.is_open()){
        cout << "Error opening file.";
        return 1;
    }


    while(getline(file, line)){
    }


    file.close();

    return 0;
}