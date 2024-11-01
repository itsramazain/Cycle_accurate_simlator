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

    bitset<32> readRS(bitset<8>){
        
    }









};