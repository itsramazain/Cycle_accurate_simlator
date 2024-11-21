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
    bitset<6>pcplus=0;
    bool PCwrite;
    
    bool Branch=0;
    bool Zero=0;
    bool MemRead=0;
    bool MemWrite=0;
    bool PCsrc=0;
    bitset<5>rd;
    bitset<5>rt;
    bitset<5>rs;



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
    bitset<6>pcplus=0;
    bitset<5>rd;
    
    bitset<5>rt;
    bitset<5>rs;
    bool Branch=0;
    bool Zero=0;
    bool MemRead=0;
    bool MemWrite=0;
    

    bool Regwwrite=0;
    bool MemToreg=0;

} ;

struct MEMRBreg{       
    bitset<6>pcplus=0;     
    bitset<32> Instruction=0;
    bitset<5>rd;
    bitset<32> ALUresult=0;
    bitset<32> Data_Mem=0;
    bool Regwwrite=0;
    bool MemToreg=0;
    bitset<5>rt;
    bitset<5>rs;
    

};




bitset<32> signExtendImmediate(std::bitset<32> instruction) {
    int16_t immediate = instruction.to_ulong() & 0xFFFF;
    int32_t extendedValue;
    if (immediate & 0x8000) { 
        
        extendedValue = static_cast<int32_t>(immediate | 0xFFFF0000);
    } else {
        
        extendedValue = static_cast<int32_t>(immediate);
    }

    
    return std::bitset<32>(static_cast<uint32_t>(extendedValue));
}

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
        bitset<6> pcpuls1=bitset<6>((pc.PCVlue.to_ulong() + 1) % 64);
        bitset<6> offset = bitset<6>(IFID.Instruction.to_ulong() & 0b111111);
        if (IDEx.PCwrite==1 ){
            if (IDEx.Zero==1 && IDEx.Branch==1){
                    next_pc.PCVlue=bitset<6>((IFID.pcplus.to_ulong() + offset.to_ulong() ) % 64);
            } else {
                next_pc.PCVlue=pcpuls1;

            }


        }
        
        next_IFID.Instruction=INSTRUCTION_MEM.read(pc.PCVlue);
        next_IFID.pcplus=pcpuls1;

        //decode

        bitset<6> opcode= ( IFID.Instruction>> 26).to_ulong();
        bitset<5> rs= (IFID.Instruction >> 21).to_ulong() & 0x1F;
        bitset<5> rt=   (IFID.Instruction  >> 16).to_ulong() & 0x1F;
        bitset<5> rd = (IFID.Instruction >> 11).to_ulong() & 0x1F;
        bitset<6> funct = IFID.Instruction.to_ulong() & 0x3F;
        bitset<32>SEImm=signExtendImmediate(IFID.Instruction);

        bitset<32>rsdata=Rfile.readRS(rs);
        bitset<32>rddata=Rfile.readRS(rd);

        bool zero=0;
        if (rsdata==rddata){
            zero==1;
        }
        //missprediction logic goes here if zero==0 then pcwitre==0 and all the signals in the if id signal should be set to zros
        int aluop;
        //long switch case

        if (opcode == 0x00000000) {
        if (funct == 0x20) {
            cout << "Instruction is add" << std::endl;
        } else if (funct == 0x22) {
            cout << "Instruction is sub" << std::endl;
        } else if (funct == 0x24) {
            cout << "Instruction is and" << std::endl;
        } else if (funct == 0x25) {
            cout << "Instruction is or" << std::endl;
        } else if (funct == 0x2A) {
            cout << "Instruction is slt" << std::endl;
        } else {
            cout << "Unknown R-type instruction" << std::endl;
        }
        } else if (opcode == 0x23) {
            cout << "Instruction is lw (load word)" << std::endl;
        } else if (opcode == 0x2B) {
            cout << "Instruction is sw (store word)" << std::endl;
        } else if (opcode == 0x04) {
            cout << "Instruction is beq (branch on equal)" << std::endl;
        } else {
            cout << "Unknown instruction" << std::endl;
        }


        //leave it for bader to do it



        //excute stage
        bitset<32>rsdata=IDEx.RSData;
        bitset<32>rtdata=IDEx.RTData;

        if (IDEx.ALUSrc==1){
            rtdata=IDEx.SEImm;
        }

        if (IDEx.ALUOP=0x0){
            next_EXMEM.ALUresult=bitset<32>((rsdata.to_ulong() + rtdata.to_ulong()) % 64);
        }else if(IDEx.ALUOP=0x1){
              next_EXMEM.ALUresult=bitset<32>((rsdata.to_ulong() - rtdata.to_ulong()) % 64);

        }else if(IDEx.ALUOP=0x2){
              next_EXMEM.ALUresult=bitset<32>((rsdata.to_ulong() & rtdata.to_ulong()) % 64);

        }else if(IDEx.ALUOP=0x3){
               if (rsdata.to_ulong() < rtdata.to_ulong()){
                next_EXMEM.ALUresult=1;
               }else{
                next_EXMEM.ALUresult=0;
               }

        }
        //there should be some contro, signals passed to the exme register

        next_EXMEM.Instruction=IDEx.Instruction;
        next_EXMEM.rd=IDEx.rd;
        next_EXMEM.rs=IDEx.rs;
        next_EXMEM.rt=IDEx.rt;

        //there has to be more stuff that i forgot about!!

        //memory stage

        



















        
        pc=next_pc;
        INSTRUCTION_MEM=next_INSTRUCTION_MEM;
        DATA_MEM=next_DATA_MEM;
        IFID=next_IFID;
        IDEx=next_IDEx;
        EXMEM=next_EXMEM;
        MEMRB=next_MEMRB;
        //print to a log file
        cycles++;

}



        



        



        




        














        



    



          
return 0;
}






