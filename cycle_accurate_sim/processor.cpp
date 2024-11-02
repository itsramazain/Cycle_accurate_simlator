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
    bitset<6> PCVlue=0;
    
} ;


struct IFIDreg{            
    bitset<32> Instruction=0;
    bitset<6>pcplus=0;
} ;



struct IDExreg {            
    bitset<32> Instruction=0;
    
    bool Branch=0;
    bool Zero=0;
    bool MemRead=0;
    bool MemWrite=0;
    bool PCsrc=0;
    bitset<5>rd;

    bool Regwwrite=0;
    bool MemToreg=0;

    
    bitset<32>RSData=0;
    bitset<32>RTData=0;
    bitset<32> SEImm=0;
    bitset<5> RDadd=0;
    
    bool RegDist=0;
    bool ALUSrc=0;
    int ALUOP=0;

} ;


struct EXMEMreg {            
    bitset<32> Instruction=0;
    bitset<32> ALUresult=0;
    bitset<5>rd;
    bool Branch=0;
    bool Zero=0;
    bool MemRead=0;
    bool MemWrite=0;
    

    bool Regwwrite=0;
    bool MemToreg=0;

} ;

struct MEMRBreg{            
    bitset<32> Instruction=0;
    bitset<5>rd;
    bitset<32> ALUresult=0;
    bitset<32> Data_Mem=0;
    bool Regwwrite=0;
    bool MemToreg=0;
    

};





int main(){
    bool exucte=1;
    int cycles=0;
    PC pc,next_pc;
    IFIDreg IFID,next_IFID;
    IDExreg IDEx,next_IDEx;
    MEMRBreg MEMRB,next_MEMRB;
    EXMEMreg EXMEM,next_EXMEM;

    while(exucte){
        exucte=INSTRUCTION_MEM.stop();

        //fitch state
        bitset<6> pcp=bitset<6>((pc.PCVlue.to_ulong() + 1) % 64);
        if (EXMEM.Branch & EXMEM.Zero){
            next_pc.PCVlue = pcp;

        } else{
            next_pc.PCVlue = EXMEM.ALUresult.to_ulong() & 0x3F;
        }

        next_IFID.Instruction=INSTRUCTION_MEM.read(pc.PCVlue);
        next_IFID.pcplus=pcp;

        //decode

        bitset<6> opcode= ( IFID.Instruction>> 26).to_ulong();
        

        bitset<5> rs= (IFID.Instruction >> 21).to_ulong() & 0x1F;
        bitset<5> rt=   (IFID.Instruction  >> 16).to_ulong() & 0x1F;

        next_IDEx.Instruction=IFID.Instruction;
        next_IDEx.RSData=Rfile.readRS(rs);
        next_IDEx.RTData=Rfile.readRS(rt);
        bitset<32> writedata;
        if(MEMRB.MemToreg==1){
            writedata=MEMRB.Data_Mem;

        }else{
            writedata=MEMRB.ALUresult;
        }
        next_Rfile.write(MEMRB.rd,writedata);
        


        














        



    }



          
return 0;
}






