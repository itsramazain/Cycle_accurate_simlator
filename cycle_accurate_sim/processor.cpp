#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>
#include"Data_memory.cpp"
#include"Instruction_memory.cpp"
#include"registerfile.cpp"

Data_memory DATA_MEM;
Data_memory next_DATA_MEM;
Instruction_Memory INSTRUCTION_MEM;
Instruction_Memory next_INSTRUCTION_MEM;
RF Rfile;
RF next_Rfile;

using namespace std;
struct PC{            
    bitset<32> PCVlue=0;
    
} ;


struct IFIDreg{            
    bitset<32> Instruction=0;
    bitset<6>pcplus=0;
} ;



struct IDExreg {            
    bitset<32> Instruction=0;
    
    bitset<1>Branch=0;
    bitset<1>Zero=0;
    bitset<1>MemRead=0;
    bitset<1>MemWrite=0;

    bitset<1> Regwwrite=0;
    bitset<1> MemToreg=0;

    
    bitset<32>RSData=0;
    bitset<32>RTData=0;
    bitset<32> SEImm=0;
    bitset<5> RDadd=0;
    
    bitset<1>RegDist=0;
    bitset<1>ALUSrc=0;
    bitset<3>ALUOP=0;

} ;


struct EXMEMreg {            
    bitset<32> Instruction=0;
    bitset<32> ALUresult=0;

    bitset<1>Branch=0;
    bitset<1>Zero=0;
    bitset<1>MemRead=0;
    bitset<1>MemWrite=0;

    bitset<1> Regwwrite=0;
    bitset<1> MemToreg=0;

} ;

struct MEMRBreg{            
    bitset<32> Instruction=0;

    bitset<32> ALUresult=0;
    bitset<32> Data_Mem=0;

    bitset<1> Regwwrite=0;
    bitset<1> MemToreg=0;
    

};





int main(){
    bool exucte=1;
    int cycles=0;
    PC pc,next_pc;
    IFIDreg IFID,next_IFID;
    IDExreg IDEx,next_IDEx;
    MEMRBreg MEMRB,next_MEMRB;

    while(exucte){
        exucte=INSTRUCTION_MEM.stop();

        //fitch state




        



    }



          
return 0;
}






