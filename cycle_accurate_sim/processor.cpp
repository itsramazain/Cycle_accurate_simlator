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




// Operation parameters
enum ALUOperations {
    ADD  = 0b0000,
    SUB  = 0b0001,
    AND  = 0b0010,
    OR   = 0b0011,
    SLT  = 0b0100,
    XOR  = 0b0101,
    NOR  = 0b0110,
    SLL  = 0b0111,
    SRL  = 0b1000,
    SGT  = 0b1001
};

#include <bitset>
#include <cstdint>

const int DATA_WIDTH = 32;
const int SEL_WIDTH = 4;

// ALU function
std::bitset<DATA_WIDTH> alu(
     std::bitset<DATA_WIDTH> operand1,
     std::bitset<DATA_WIDTH> operand2,
     std::bitset<SEL_WIDTH> opSel,
    std::bitset<5> shamt
) {
    std::bitset<DATA_WIDTH> result;

    // Convert opSel to an integer value for easier comparison
    uint8_t opCode = static_cast<uint8_t>(opSel.to_ulong());
    uint8_t shiftAmount = static_cast<uint8_t>(shamt.to_ulong());
    switch (opCode) {
        case 0b0000: { // ADD
            uint32_t op1 = operand1.to_ulong();
            uint32_t op2 = operand2.to_ulong();
            result = op1 + op2;
            break;
        }
        case 0b0001: { // SUB
            uint32_t op1 = operand1.to_ulong();
            uint32_t op2 = operand2.to_ulong();
            result = op1 - op2;
            break;
        }
        case 0b0010: // AND
            result = operand1 & operand2;
            break;
        case 0b0011: // OR
            result = operand1 | operand2;
            break;
        case 0b0100: // SLT
            result = (operand1.to_ulong() < operand2.to_ulong()) ? 1 : 0;
            break;
        case 0b0101: // XOR
            result = operand1 ^ operand2;
            break;
        case 0b0110: // NOR
            result = ~(operand1 | operand2);
            break;
        case 0b0111: // SLL (Shift Left Logical)
            result = operand1 << shiftAmount;
            break;
        case 0b1000: // SRL (Shift Right Logical)
            result = operand1 >> shiftAmount;
            break;
        case 0b1001: { // SGT (Set Greater Than)
            int32_t op1 = static_cast<int32_t>(operand1.to_ulong());
            int32_t op2 = static_cast<int32_t>(operand2.to_ulong());
            result = (op1 > op2) ? 1 : 0;
            break;
        }
        default:
            result = 0xFFFFFFFF; // Undefined operation
            break;
    }

    // Restrict result to 5 bits
    result &= std::bitset<DATA_WIDTH>(0x1F); // Mask to keep only the lower 5 bits

    return result;
}


// Define constants for the opcode and funct codes
const uint8_t _RType = 0x0;
const uint8_t _addi = 0x8;
const uint8_t _lw = 0x23;
const uint8_t _sw = 0x2b;
const uint8_t _beq = 0x4;
const uint8_t _bne = 0x5;
const uint8_t _ori = 0xd;
const uint8_t _xori = 0xe;
const uint8_t _andi = 0xc;
const uint8_t _slti = 0xa;
const uint8_t _jump = 0x2;
const uint8_t _jumpandlink = 0x3;

// Function to generate control signals based on the opcode
void controlUnit(
    std::bitset<6> opCode,
    bool& RegDst,
    bool& BranchEqual,
    bool& MemReadEn,
    bool& MemtoReg,
    std::bitset<3>& ALUOp,
    bool& MemWriteEn,
    bool& RegWriteEn,
    bool& ALUSrc,
    bool& Jump,
    bool& BranchNotEqual
) {
    // Convert opcode to integer for easier comparison
    uint8_t opCodeInt = static_cast<uint8_t>(opCode.to_ulong());

    // Reset all control signals
    RegDst = false;
    BranchEqual = false;
    MemReadEn = false;
    MemtoReg = false;
    ALUOp = 0b000;
    MemWriteEn = false;
    RegWriteEn = false;
    ALUSrc = false;
    Jump = false;
    BranchNotEqual = false;

    // Generate control signals based on the opcode
    switch (opCodeInt) {
        case _RType:
            RegDst = true;
            RegWriteEn = true;
            ALUOp = 0b110;
            break;

        case _addi:
            ALUSrc = true;
            RegWriteEn = true;
            ALUOp = 0b000;
            break;

        case _andi:
            ALUSrc = true;
            RegWriteEn = true;
            ALUOp = 0b100;
            break;

        case _ori:
            ALUSrc = true;
            RegWriteEn = true;
            ALUOp = 0b011;
            break;

        case _xori:
            ALUSrc = true;
            RegWriteEn = true;
            ALUOp = 0b010;
            break;

        case _slti:
            ALUSrc = true;
            RegWriteEn = true;
            ALUOp = 0b101;
            break;

        case _lw:
            ALUSrc = true;
            RegWriteEn = true;
            MemReadEn = true;
            MemtoReg = true;
            ALUOp = 0b000;
            break;

        case _sw:
            ALUSrc = true;
            MemWriteEn = true;
            ALUOp = 0b000;
            break;

        case _beq:
            BranchEqual = true;
            ALUOp = 0b001;
            break;

        case _bne:
            BranchNotEqual = true;
            ALUOp = 0b001;
            break;

        case _jump:
            Jump = true;
            break;

        case _jumpandlink:
            Jump = true;
            RegWriteEn = true;
            break;

        default:
            // All signals are already reset at the start
            break;
    }
}




#include <bitset>
#include <cstdint>

// Define constants for ALU operations (opSel)
const uint8_t _ADD = 0b0000;
const uint8_t _SUB = 0b0001;
const uint8_t _AND = 0b0010;
const uint8_t _OR = 0b0011;
const uint8_t _SLT = 0b0100;
const uint8_t _XOR = 0b0101;
const uint8_t _NOR = 0b0110;
const uint8_t _SLL = 0b0111;
const uint8_t _SRL = 0b1000;
const uint8_t _SGT = 0b1001;
const uint8_t _DONTCARE = 0b11111;

// Define constants for function field
const uint8_t _add_ = 0x20;
const uint8_t _sub_ = 0x22;
const uint8_t _and_ = 0x24;
const uint8_t _or_ = 0x25;
const uint8_t _slt_ = 0x2A;
const uint8_t _nor_ = 0x27;
const uint8_t _sll_ = 0x00;
const uint8_t _srl_ = 0x02;
const uint8_t _jr_ = 0x08;
const uint8_t _xor_ = 0x26;
const uint8_t _sgt_ = 0x2B;

void ALUcontrol(
    std::bitset<3> ALUOp,   // 3-bit ALUOp input
    std::bitset<6> funct,   // 6-bit funct input
    bool& JumpReg,                 // Output: JumpReg signal
    std::bitset<4>& opSel          // Output: ALU operation selection
) {
    // Reset outputs
    JumpReg = false;
    opSel = _DONTCARE;

    uint8_t ALUOpInt = static_cast<uint8_t>(ALUOp.to_ulong());
    uint8_t functInt = static_cast<uint8_t>(funct.to_ulong());

    // Match ALUOp and funct combinations
    if (ALUOpInt == 0b000) {
        // lw or sw or addi
        opSel = _ADD;
    } else if (ALUOpInt == 0b001) {
        // beq
        opSel = _SUB;
    } else if (ALUOpInt == 0b010) {
        // xori
        opSel = _XOR;
    } else if (ALUOpInt == 0b011) {
        // ori
        opSel = _OR;
    } else if (ALUOpInt == 0b100) {
        // andi
        opSel = _AND;
    } else if (ALUOpInt == 0b101) {
        // slti
        opSel = _SLT;
    } else if (ALUOpInt == 0b110) {
        // R-type operations based on funct
        switch (functInt) {
            case _add_: opSel = _ADD; break;
            case _sub_: opSel = _SUB; break;
            case _and_: opSel = _AND; break;
            case _or_:  opSel = _OR; break;
            case _slt_: opSel = _SLT; break;
            case _xor_: opSel = _XOR; break;
            case _nor_: opSel = _NOR; break;
            case _sll_: opSel = _SLL; break;
            case _srl_: opSel = _SRL; break;
            case _sgt_: opSel = _SGT; break; // Set Greater Than
            case _jr_:  JumpReg = true; break; // Jump Register
            default:    opSel = _DONTCARE; break; // Undefined funct
        }
    } else {
        // Default case for undefined ALUOp
        opSel = _DONTCARE;
    }
}


#include <iostream>
#include <bitset>

// Function to simulate the hazard detection logic
void detectHazard(
    bool IDEX_MemRead,
    std::bitset<5> IDEX_RegisterRt,
    std::bitset<5> IFID_RegisterRs,
    std::bitset<5> IFID_RegisterRt,
    bool branch,
    bool branch_not,
    bool& PCwrite,
    bool& IFID_Write,
    bool& nopSel,
    bool& IDIF_reset
) {
    // Default output values
    PCwrite = true;
    IFID_Write = true;
    nopSel = true;
    IDIF_reset = true;

    // Load-use hazard detection
    if (IDEX_MemRead &&
        (IDEX_RegisterRt == IFID_RegisterRs || IDEX_RegisterRt == IFID_RegisterRt)) {
        PCwrite = false;
        IFID_Write = false;
        nopSel = false;
        IDIF_reset = true;
    }

    // Branch hazards
    if (branch || branch_not) {
        PCwrite = false;
        IFID_Write = false;
        nopSel = false;
        IDIF_reset = false;
    }
}








// Forwarding Unit Function
void forwardingUnit(
    std::bitset<5> ID_EX_rs,     // Source register 1
    std::bitset<5> ID_EX_rt,     // Source register 2
    std::bitset<5> EX_MEM_rd,    // Destination register from EX/MEM
    std::bitset<5> MEM_WB_rd,    // Destination register from MEM/WB
    bool EX_MEM_RegWrite,               // RegWrite signal from EX/MEM
    bool MEM_WB_RegWrite,               // RegWrite signal from MEM/WB
    std::bitset<2>& ForwardA,           // Forwarding signal for ALU input A
    std::bitset<2>& ForwardB            // Forwarding signal for ALU input B
) {
    // Reset forwarding signals
    ForwardA = 0b00;
    ForwardB = 0b00;

    // ForwardA Logic
    if (EX_MEM_RegWrite && EX_MEM_rd.to_ulong() != 0 &&
        EX_MEM_rd == ID_EX_rs) {
        ForwardA = 0b10; // Forward from EX/MEM stage
    } else if (MEM_WB_RegWrite && MEM_WB_rd.to_ulong() != 0 &&
               MEM_WB_rd == ID_EX_rs) {
        ForwardA = 0b01; // Forward from MEM/WB stage
    }

    // ForwardB Logic
    if (EX_MEM_RegWrite && EX_MEM_rd.to_ulong() != 0 &&
        EX_MEM_rd == ID_EX_rt) {
        ForwardB = 0b10; // Forward from EX/MEM stage
    } else if (MEM_WB_RegWrite && MEM_WB_rd.to_ulong() != 0 &&
               MEM_WB_rd == ID_EX_rt) {
        ForwardB = 0b01; // Forward from MEM/WB stage
    }
}








struct PC{
    bitset<6>pc=0b111111;            
    
    
} ;


struct IFIDreg{            
    bitset<6>pcplus1=000000;
    bitset<32>instruction=000000;

} ;



struct IDExreg {            
    bool branch=0;
    bitset<6>opCode=0;
    bool RegDst=0;
    bool BranchEqual=0; 
    bool MemReadEn=0;
    bool MemtoReg=0;
    bool MemWriteEn=0;
    bool RegWriteEn=0;
    bool ALUSrc=0; 
    bool Jump=0;
    bool BranchNotEqual=0;    
    bitset<3> ALUOp=0;
    bitset<5> rs=0;
    bitset<5> rt=0;
    bitset<5> rd=0;
    bitset<32> Eximm=0;
    bitset<6> funct=0;
    bitset<5> shamnt=0;
    bitset<32>ReadData1;
    bitset<32>ReadData2;

} ;


struct EXMEMreg {
    bool RegWrite=0;
    bool MemReadEn=0;
    bool MemWriteEn=0; 
    bitset<5>rd=0;
    bitset<5>rt=0;   
    bool MemtoReg;
    bitset<32>ALURESULT; 
    bitset<32>ReadData2;
    bool RegDst=0;
    

    

} ;

struct MEMRBreg{  
    
    bool RegWrite=0;
    bitset<5>rd=0;
    bitset<5>rt=0;
    bool MemtoReg;
    bitset<32>memData;
    bitset<32>ALUresult; 
    bool RegDst=0;

    
         
    
    

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

std::bitset<6> calculateTarget(const std::bitset<6> pcplus1, const std::bitset<6> imm) {
    // Add the two 6-bit values and mask the result to 6 bits
    unsigned long result = (pcplus1.to_ulong() + imm.to_ulong()) & 0b111111;

    // Convert the result back to a 6-bit bitset and return it
    return std::bitset<6>(result);
}




int i=1;
void writePipelineRegistersToFile(
    const PC& pc,
    const IFIDreg& IFID,
    const IDExreg& IDEx,
    const EXMEMreg& EXMEM,
    const MEMRBreg& MEMRB,
    RF& registerFile,
    Data_memory& dataMemory,
    const std::string filename,
    const int& i 
) {
    

    
    std::ofstream outFile(filename, std::ios::app); // Open file in append mode
    
    if (!outFile) {
        std::cerr << "Error: Unable to open file " << filename << " for writing." << std::endl;
        return;
    }

    outFile << "----------------------------" << std::endl;
    outFile << "Cycle number:";
    outFile << "#" << i << std::endl;
    outFile << "----------------------------" << std::endl;
    outFile << "----------------------------" << std::endl;
    outFile << "Pipeline Register Contents" << std::endl;
    outFile << "----------------------------" << std::endl;

    // Write PC
    outFile << "PC: " << pc.pc << std::endl;

    // Write IF/ID Register
    outFile << "IF/ID Register:" << std::endl;
    outFile << "  PC+1: " << IFID.pcplus1 << std::endl;
    outFile << "  Instruction: " << IFID.instruction << std::endl;

    // Write ID/EX Register
    outFile << "ID/EX Register:" << std::endl;
    outFile << "  Branch: " << IDEx.branch << std::endl;
    outFile << "  OpCode: " << IDEx.opCode << std::endl;
    outFile << "  RegDst: " << IDEx.RegDst << std::endl;
    outFile << "  BranchEqual: " << IDEx.BranchEqual << std::endl;
    outFile << "  MemReadEn: " << IDEx.MemReadEn << std::endl;
    outFile << "  MemtoReg: " << IDEx.MemtoReg << std::endl;
    outFile << "  MemWriteEn: " << IDEx.MemWriteEn << std::endl;
    outFile << "  RegWriteEn: " << IDEx.RegWriteEn << std::endl;
    outFile << "  ALUSrc: " << IDEx.ALUSrc << std::endl;
    outFile << "  Jump: " << IDEx.Jump << std::endl;
    outFile << "  BranchNotEqual: " << IDEx.BranchNotEqual << std::endl;
    outFile << "  ALUOp: " << IDEx.ALUOp << std::endl;
    outFile << "  RS: " << IDEx.rs << std::endl;
    outFile << "  RT: " << IDEx.rt << std::endl;
    outFile << "  RD: " << IDEx.rd << std::endl;
    outFile << "  Immediate (Eximm): " << IDEx.Eximm << std::endl;
    outFile << "  Function: " << IDEx.funct << std::endl;
    outFile << "  Shamt: " << IDEx.shamnt << std::endl;
    outFile << "  ReadData1: " << IDEx.ReadData1 << std::endl;
    outFile << "  ReadData2: " << IDEx.ReadData2 << std::endl;

    // Write EX/MEM Register
    outFile << "EX/MEM Register:" << std::endl;
    outFile << "  RegWrite: " << EXMEM.RegWrite << std::endl;
    outFile << "  MemReadEn: " << EXMEM.MemReadEn << std::endl;
    outFile << "  MemWriteEn: " << EXMEM.MemWriteEn << std::endl;
    outFile << "  RD: " << EXMEM.rd << std::endl;
    outFile << "  RT: " << EXMEM.rt << std::endl;
    outFile << "  MemtoReg: " << EXMEM.MemtoReg << std::endl;
    outFile << "  ALUResult: " << EXMEM.ALURESULT << std::endl;
    outFile << "  ReadData2: " << EXMEM.ReadData2 << std::endl;
    outFile << "  RegDst: " << EXMEM.RegDst << std::endl;

    // Write MEM/WB Register
    outFile << "MEM/WB Register:" << std::endl;
    outFile << "  RegWrite: " << MEMRB.RegWrite << std::endl;
    outFile << "  RD: " << MEMRB.rd << std::endl;
    outFile << "  RT: " << MEMRB.rt << std::endl;
    outFile << "  MemtoReg: " << MEMRB.MemtoReg << std::endl;
    outFile << "  MemData: " << MEMRB.memData << std::endl;
    outFile << "  ALUResult: " << MEMRB.ALUresult << std::endl;
    outFile << "  RegDst: " << MEMRB.RegDst << std::endl;

    outFile << "----------------------------" << std::endl;
    // Write the register file contents
    outFile << "Register File Contents:" << std::endl;
    for (int i = 0; i < 32; i++) {
        outFile << "R[" << i << "]: " << registerFile.readRS(bitset<5>(i)) << std::endl;
    }
    outFile << "----------------------------" << std::endl;
    // Write the data memory contents
    outFile << "Data Memory Contents:" << std::endl;
    for (int i = 0; i < 64; i++) {
        outFile << "Mem[" << i << "]: " << dataMemory.read(bitset<6>(i)) << std::endl;
    }

    outFile << "----------------------------" << std::endl;

    outFile.close();
}




int main(){
    
    bool exucte=1;
    int cycles=0;
    PC pc,next_pc;
    IFIDreg IFID,next_IFID;
    IDExreg IDEx,next_IDEx;
    MEMRBreg MEMRB,next_MEMRB;
    EXMEMreg EXMEM,next_EXMEM;

     
    


    
    while(i<7){
        
        writePipelineRegistersToFile(pc, IFID, IDEx, EXMEM, MEMRB,Rfile,DATA_MEM, "log.txt",i  );
        bitset<5> rs=bitset<5>((IFID.instruction.to_ulong() >> 21) & 0b11111);
        bitset<5> rt=bitset<5>((IFID.instruction.to_ulong() >> 16) & 0b11111);
        bitset<5> rd=bitset<5>((IFID.instruction.to_ulong() >> 11) & 0b11111);

        bool PCwrite=1,IFID_Write=1,nopSel=1,IDIF_reset=1;
        detectHazard( IDEx.MemReadEn, IDEx.rt,rs, rt,IDEx.branch,IDEx.BranchNotEqual,PCwrite,IFID_Write,nopSel,IDIF_reset);

        //fitch
        bitset<6> pcplus1 = std::bitset<6>(pc.pc.to_ulong() + 1);
        bitset<6> nextpc=pcplus1;
        bitset<6> imm6=IFID.instruction.to_ulong() & 0b111111;
        bitset<16> imm16=IFID.instruction.to_ulong() & 0b1111111111111111;
        bitset<32>Eximm=signExtendImmediate(IFID.instruction);
        if (IDEx.branch){
            nextpc=calculateTarget(IFID.pcplus1,imm6);
        }
        if (PCwrite){
        next_pc.pc=nextpc;
        }
        
        next_IFID.pcplus1=pcplus1;
        if (IDIF_reset){
            next_IFID.instruction=INSTRUCTION_MEM.read(pcplus1);
        }else{
            IFID.instruction=0;
            next_IFID.instruction=0;
        }
        //decode
        
        bitset<5> shamt = std::bitset<5>((IFID.instruction.to_ulong() >> 6) & 0b11111);
        bitset<32>readdata1,readdata2;
        bitset<26> jumpAddress = std::bitset<26>(IFID.instruction.to_ulong() & 0x3FFFFFF);

        readdata1=Rfile.readRS(rs);
        readdata2=Rfile.readRS(rt);
        bool zero=0;
        if (readdata1==readdata2){
                zero=1;
        }
        
        bitset<6> opcode, functionField;
        opcode = bitset<6>((IFID.instruction.to_ulong() >> 26) & 0b111111);
        functionField=bitset<6>(IFID.instruction.to_ulong() & 0b111111);


        bool RegDst,BranchEqual, MemReadEn,MemtoReg, MemWriteEn, RegWriteEn,ALUSrc, Jump, BranchNotEqual;
        
        bitset<3> ALUOp;
        
        next_IDEx.branch=zero&BranchEqual;
        cout<<"oprcode" <<opcode<<endl;
        controlUnit(opcode,RegDst,BranchEqual, MemReadEn,MemtoReg, ALUOp,MemWriteEn, RegWriteEn,ALUSrc, Jump, BranchNotEqual);
        cout<< IDEx.Jump<<endl;
        cout<< Jump<<endl;
        cout<< "Cycle"<<i<<endl;

         if (nopSel){
         next_IDEx.opCode=opcode;
         next_IDEx.RegDst=RegDst;
         next_IDEx.BranchEqual=BranchEqual; 
         next_IDEx.MemReadEn=MemReadEn;
         next_IDEx.MemtoReg=MemtoReg;
         next_IDEx.MemWriteEn=MemWriteEn;
         next_IDEx.RegWriteEn=RegWriteEn;
         next_IDEx.ALUSrc=ALUSrc; 
         next_IDEx.Jump=Jump;
         next_IDEx.BranchNotEqual=BranchNotEqual;    
         next_IDEx.ALUOp=ALUOp;
         next_IDEx.rs=rs;
         next_IDEx.rt=rt;
         next_IDEx.rd=rd;
         next_IDEx.Eximm=Eximm;
         next_IDEx.funct=functionField;
         next_IDEx.shamnt=shamt;
         }else{
         next_IDEx.RegDst=0;
         next_IDEx.BranchEqual=0; 
         next_IDEx.MemReadEn=0;
         next_IDEx.MemtoReg=0;
         next_IDEx.MemWriteEn=0;
         next_IDEx.RegWriteEn=0;
         next_IDEx.ALUSrc=0; 
         next_IDEx.Jump=0;
         next_IDEx.BranchNotEqual=0;    
         next_IDEx.ALUOp=0;

         IDEx.RegDst=0;
         IDEx.BranchEqual=0; 
         IDEx.MemReadEn=0;
         IDEx.MemtoReg=0;
         IDEx.MemWriteEn=0;
         IDEx.RegWriteEn=0;
         IDEx.ALUSrc=0; 
         IDEx.Jump=0;
         IDEx.BranchNotEqual=0;    
         IDEx.ALUOp=0;

         }



         //excute

        
            
        
        bitset<32>A;
        bitset<32>B;
        

        bitset<2>ForwardA;
        bitset<2>ForwardB;
        forwardingUnit(IDEx.rs,IDEx.rt,EXMEM.rd,MEMRB.rd, EXMEM.RegWrite,MEMRB.RegWrite, ForwardA, ForwardB); 
        bitset<32>WBDATA;
        if (MEMRB.MemtoReg){
            WBDATA=MEMRB.memData;
        }else{
            WBDATA=MEMRB.ALUresult;

        }
        if (ForwardA == bitset<2>("00")) {
            A = IDEx.ReadData1; // If ForwardA is 00, a = rs
        } else if (ForwardA == bitset<2>("01")) {
            A = WBDATA ; // If ForwardA is 01, a = rs
        } else if (ForwardA == bitset<2>("10")) {
            A =EXMEM.ALURESULT ; // If ForwardA is 10, a = rss
        }
        if (ForwardB == bitset<2>("00")) {
            B = IDEx.ReadData2 ; // If ForwardA is 00, a = rs
        } else if (ForwardB == bitset<2>("01")) {
            B = WBDATA; // If ForwardA is 01, a = rs
        } else if (ForwardB == bitset<2>("10")) {
            B = EXMEM.ALURESULT ; // If ForwardA is 10, a = rss

        }

    if (IDEx.ALUSrc){
        B=IDEx.Eximm;

    }
    bitset<4> opSel;
    bool JumpReg;
    ALUcontrol( IDEx.ALUOp,IDEx.funct,JumpReg,opSel);         

    
    bitset<32>res= alu( A,B, opSel,IDEx.shamnt);
    
    next_EXMEM.RegWrite=IDEx.RegWriteEn;
    next_EXMEM.MemReadEn=IDEx.MemReadEn;
    next_EXMEM.MemWriteEn=IDEx.MemWriteEn; 
    next_EXMEM.rd=IDEx.rd;
    next_EXMEM.rt=IDEx.rt;
    next_EXMEM.MemtoReg=IDEx.MemtoReg;
    next_EXMEM.RegDst=IDEx.RegDst;
    next_EXMEM.ALURESULT=res; 
    next_EXMEM.ReadData2=IDEx.ReadData2;

    //mem stage (the wrting only happends on the new_mem/New_Rf)
    bitset<32>data;
    bitset<6> address = EXMEM.ALURESULT.to_ulong() & 0x3F;
    if (EXMEM.MemWriteEn){
        next_DATA_MEM.write(address,EXMEM.ReadData2);
        
    }else if (EXMEM.MemReadEn){
           data= DATA_MEM.read(address);
    }

    next_MEMRB.RegWrite=EXMEM.RegWrite;
    next_MEMRB.rd=EXMEM.rd;
    next_MEMRB.RegDst=EXMEM.RegDst;
    next_MEMRB.rt=EXMEM.rt;
    next_MEMRB.MemtoReg=EXMEM.MemtoReg;
    next_MEMRB.memData=data;
    next_MEMRB.ALUresult=EXMEM.ALURESULT;

    //write back

    bitset<32> wbdata=0;
    if (MEMRB.MemtoReg){
        wbdata=MEMRB.memData;
    }else{
       wbdata=MEMRB.ALUresult; 
    }
    bitset<5> wbadress=0;
    if (MEMRB.RegDst){
        wbadress=MEMRB.rd;

    }else{
        wbadress=MEMRB.rt;
    }
    if (MEMRB.RegWrite){
        next_Rfile.write(wbadress,wbdata);

    }


        pc=next_pc;
        IFID=next_IFID;
        IDEx=next_IDEx;
        MEMRB=next_MEMRB;
        EXMEM=next_EXMEM;
        DATA_MEM=next_DATA_MEM;
        INSTRUCTION_MEM=next_INSTRUCTION_MEM;
        Rfile= next_Rfile;
        i++;

    }



          
return 0;
}






