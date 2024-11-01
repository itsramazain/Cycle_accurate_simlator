#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>
#include "Data_memory.cpp"
#include "registerfile.cpp"
#include "Instruction_memory.cpp"


using namespace std;

struct {            
    bitset<32> currentPC = 0;
    bitset<32> nextPC = 0;
} PC;

struct {            
    bitset<32> currentInstruction = 0;
    bitset<32> nextInstruction = 0;

    bitset<6> currentPcPlus4 = 0;
    bitset<6> nextPcPlus4 = 0;
} IFIDreg;

struct {            
    bitset<32> currentInstruction = 0;
    bitset<32> nextInstruction = 0;

    bitset<1> currentBranch = 0;
    bitset<1> nextBranch = 0;

    bitset<1> currentZero = 0;
    bitset<1> nextZero = 0;

    bitset<1> currentMemRead = 0;
    bitset<1> nextMemRead = 0;

    bitset<1> currentMemWrite = 0;
    bitset<1> nextMemWrite = 0;

    bitset<1> currentRegwwrite = 0;
    bitset<1> nextRegwwrite = 0;

    bitset<1> currentMemToreg = 0;
    bitset<1> nextMemToreg = 0;

    bitset<32> currentRSData = 0;
    bitset<32> nextRSData = 0;

    bitset<32> currentRTData = 0;
    bitset<32> nextRTData = 0;

    bitset<32> currentSEImm = 0;
    bitset<32> nextSEImm = 0;

    bitset<5> currentRDadd = 0;
    bitset<5> nextRDadd = 0;

    bitset<1> currentRegDist = 0;
    bitset<1> nextRegDist = 0;

    bitset<1> currentALUSrc = 0;
    bitset<1> nextALUSrc = 0;

    bitset<3> currentALUOP = 0;
    bitset<3> nextALUOP = 0;
} IDExreg;

struct {            
    bitset<32> currentInstruction = 0;
    bitset<32> nextInstruction = 0;

    bitset<32> currentALUresult = 0;
    bitset<32> nextALUresult = 0;

    bitset<1> currentBranch = 0;
    bitset<1> nextBranch = 0;

    bitset<1> currentZero = 0;
    bitset<1> nextZero = 0;

    bitset<1> currentMemRead = 0;
    bitset<1> nextMemRead = 0;

    bitset<1> currentMemWrite = 0;
    bitset<1> nextMemWrite = 0;

    bitset<1> currentRegwwrite = 0;
    bitset<1> nextRegwwrite = 0;

    bitset<1> currentMemToreg = 0;
    bitset<1> nextMemToreg = 0;
} EXMEMreg;

struct {            
    bitset<32> currentInstruction = 0;
    bitset<32> nextInstruction = 0;

    bitset<32> currentALUresult = 0;
    bitset<32> nextALUresult = 0;

    bitset<32> currentDataMem = 0;
    bitset<32> nextDataMem = 0;

    bitset<1> currentRegwwrite = 0;
    bitset<1> nextRegwwrite = 0;

    bitset<1> currentMemToreg = 0;
    bitset<1> nextMemToreg = 0;
} MEMRBreg;

int main() {
    while (true) {
        // Simulation logic goes here
    }   
    return 0;
}
