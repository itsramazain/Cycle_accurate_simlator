#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>

using namespace std;

class Instruction_Memory {
private:
    array<bitset<32>, 64> mem;
    int Num_of_inst;

public:
    Instruction_Memory() {
        ifstream inputFile("Instruction_memory.txt");
        if (!inputFile) {
            cerr << " Instruction_memory.txt doesnt exisit" << endl;
            return;
        }

        int IMEMcount = 0;
        string line;
        while (getline(inputFile, line) && IMEMcount < 64) {
            bitset<32> inst(line);
            mem[IMEMcount] = inst;
            IMEMcount++;
        }

        if (line.find("num_of_inst=") != string::npos) {
            // Extract the number after "num_of_inst="
            size_t pos = line.find("=");
            if (pos != string::npos) {
                Num_of_inst = stoi(line.substr(pos + 1));
            }
        }

        inputFile.close();
    }
    int num(){
        return Num_of_inst;
    }
     void printMemory() {
        ofstream outputFile("instmemory_output.txt");

        for (int i = 0; i < 64; i++) {
            outputFile <<i << ": " << mem[i] << endl;
        }

        outputFile.close();
        cout << "Memory content has been written to instmemory_output.txt" << endl;
    }

    bitset<32> read(bitset<6> address) {
        int addr = static_cast<int>(address.to_ulong());
        if (addr >= 0 && addr < 64) {
            return mem[addr];
        } else {
            cerr << "Error: Address out of bounds" << endl;
            return bitset<32>();
        }
    }

    bool stop(){
         for (int i = 0; i < 63; i++) { 
        bitset<32> zero(0); 

       
        if (mem[i] == zero && mem[i + 1] == zero) {
            return 0; 
        }
    }
    }
};


