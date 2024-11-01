#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>

using namespace std;

class RF
{
    private:
        array<bitset<32>, 32> mem;


    public:

    RF(){
        for(int i=0;i<32;i++){
            mem[i]=0;
        } 
    }

    void printRF() {
        ofstream outputFile("RFcontent.txt",ios::app);

        for (int i = 0; i < 32; i++) {
            outputFile <<  i << ": " << mem[i] << endl;
        }

        outputFile.close();
        cout << "Memory content has been written to RFcontent" << endl;
    }

    bitset<32> readRS(bitset<5> address){
         int addr = static_cast<int>(address.to_ulong());
        if (addr >= 0 && addr < 32) {
            return mem[addr];
        } else {
            cerr << "Error: Address out of bounds" << endl;
            return bitset<32>();
        }
    }


    bitset<32> readRT(bitset<5> address){
         int addr = static_cast<int>(address.to_ulong());
        if (addr >= 0 && addr < 32) {
            return mem[addr];
        } else {
            cerr << "Error: Address out of bounds" << endl;
            return bitset<32>();
        }
    }

    void write(bitset<5> address, bitset<32> data) {
        int addr = static_cast<int>(address.to_ulong());
        if (addr >= 0 && addr < 32) {
            mem[addr] = data;
        } else {
            cerr << "Error: Address out of bounds" << endl;
        }
    }








};
