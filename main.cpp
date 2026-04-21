#include <iostream>
#include <fstream>
using namespace std;

int main(){

    ifstream file("mips_input.asm");
    string line;

    if(!file.is_open()){
        cout << "Error opening file.";
        return 1;
    }

    // operation -- regDestination -- [reg or int or line] -- [reg or int or nothing]


    while(getline(file, line)){
    }


    file.close();

    return 0;
}