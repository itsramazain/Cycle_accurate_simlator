#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>

using namespace std;

struct {            
    bitset<32> Instruction;
    bitset<6>pcplus4;
} FDreg;

struct {            
    bitset<32> Instruction;
    bitset<6>pcplus4;
    bitset<32>RSData;
    bitset<32>RTData;
    bitset<32> SEImm;
    bitset<5> RDadd;
    bitset<1>RegDist;
    bitset<1>ALUSrc;
    bitset<3>ALUOP;

} DExreg;
