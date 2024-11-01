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

        inputFile.close();
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
};


