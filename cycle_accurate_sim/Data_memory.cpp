#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>

using namespace std;

class Data_memory {
private:
    array<bitset<32>, 64> mem;

public:
    Data_memory() {
        ifstream inputFile("Data_memory.txt");
        if (!inputFile) {
            cerr << " Data_memory.txt doesnt exisit" << endl;
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
        ofstream outputFile("Dnstmemory_output.txt",ios::app);

        for (int i = 0; i < 64; i++) {
            outputFile << i << ": " << mem[i] << endl;
        }

        outputFile.close();
        cout << "Memory content has been written to .Dnstmemory_output" << endl;
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
    void write(bitset<6> address, bitset<32> data) {
        int addr = static_cast<int>(address.to_ulong());
        if (addr >= 0 && addr < 64) {
            mem[addr] = data;
        } else {
            cerr << "Error: Address out of bounds" << endl;
        }
    }
};


