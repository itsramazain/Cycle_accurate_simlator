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
} IFIDreg;



struct {            
    bitset<32> Instruction;
    
    bitset<1>Branch;
    bitset<1>Zero;
    bitset<1>MemRead;
    bitset<1>MemWrite;

    bitset<1> Regwwrite;
    bitset<1> MemToreg;

    
    bitset<32>RSData;
    bitset<32>RTData;
    bitset<32> SEImm;
    bitset<5> RDadd;
    
    bitset<1>RegDist;
    bitset<1>ALUSrc;
    bitset<3>ALUOP;

} IDExreg;


struct {            
    bitset<32> Instruction;
    bitset<32> ALUresult;

    bitset<1>Branch;
    bitset<1>Zero;
    bitset<1>MemRead;
    bitset<1>MemWrite;

    bitset<1> Regwwrite;
    bitset<1> MemToreg;

} EXMEMreg;

struct {            
    bitset<32> Instruction;

    bitset<32> ALUresult;
    bitset<32> Data_Mem;

    bitset<1> Regwwrite;
    bitset<1> MemToreg;
    

} MEMRBreg;





