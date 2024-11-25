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


const int DATA_WIDTH = 32;

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
    SRL  = 0b1111
};

#include <bitset>
#include <cstdint>

const int DATA_WIDTH = 32;
const int SEL_WIDTH = 5;

// ALU function
std::bitset<DATA_WIDTH> alu(
    const std::bitset<DATA_WIDTH> operand1,
    const std::bitset<DATA_WIDTH> operand2,
    const std::bitset<SEL_WIDTH> opSel,
    uint8_t shamt
) {
    std::bitset<DATA_WIDTH> result;

    // Convert opSel to an integer value for easier comparison
    uint8_t opCode = static_cast<uint8_t>(opSel.to_ulong());

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
        case 0b0111: // SLL
            result = operand1 << shamt;
            break;
        case 0b1111: // SRL
            result = operand1 >> shamt;
            break;
        case 0b1000: { // SGT (Set Greater Than)
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
            // Default reset of control signals
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
            break;
    }
}




#include <bitset>
#include <cstdint>

// Define constants for ALU operations (opSel)
const uint8_t _ADD = 0b00000;
const uint8_t _SUB = 0b00001;
const uint8_t _AND = 0b00010;
const uint8_t _OR = 0b00011;
const uint8_t _SLT = 0b00100;
const uint8_t _XOR = 0b00101;
const uint8_t _NOR = 0b00110;
const uint8_t _SLL = 0b00111;
const uint8_t _SRL = 0b11111;
const uint8_t _SGT = 0b01000;
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
    const std::bitset<3>& ALUOp,   // 3-bit ALUOp input
    const std::bitset<6>& funct,   // 6-bit funct input
    bool& JumpReg,                 // Output: JumpReg signal
    std::bitset<5>& opSel          // Output: ALU operation selection
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
    const std::bitset<5> ID_EX_rs,     // Source register 1
    const std::bitset<5> ID_EX_rt,     // Source register 2
    const std::bitset<5> EX_MEM_rd,    // Destination register from EX/MEM
    const std::bitset<5> MEM_WB_rd,    // Destination register from MEM/WB
    bool EX_MEM_RegWrite,               // RegWrite signal from EX/MEM
    bool MEM_WB_RegWrite,               // RegWrite signal from MEM/WB
    std::bitset<2> ForwardA,           // Forwarding signal for ALU input A
    std::bitset<2> ForwardB            // Forwarding signal for ALU input B
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
    bitset<6>pc=111111;            
    
    
} ;


struct IFIDreg{            
    bitset<6>pcplus1=000000;
    bitset<32>instruction=000000;

} ;



struct IDExreg {            
    bool branch=0;
    bool opCode=0;
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

} ;


struct EXMEMreg {            
    

} ;

struct MEMRBreg{       
    
    

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

int main(){
    
    bool exucte=1;
    int cycles=0;
    PC pc,next_pc;
    IFIDreg IFID,next_IFID;
    IDExreg IDEx,next_IDEx;
    MEMRBreg MEMRB,next_MEMRB;
    EXMEMreg EXMEM,next_EXMEM;
    int i=0;
    bool PCwrite;
    bool IFID_Write;
    bool nopSel;
    bool IDIF_reset;
    while(i<5){
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
            nextpc=calculateTarget(IFID.pcplus1,(imm6));
        }
        if (PCwrite){
        next_pc.pc=nextpc;
        }
        
        next_IFID.pcplus1=pcplus1;
        if (IDIF_reset){
            next_IFID.instruction=INSTRUCTION_MEM.read(pc.pc);
        }else{
            IFID.instruction=0;
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
        bool opCode,RegDst,BranchEqual, MemReadEn,MemtoReg, MemWriteEn, RegWriteEn,ALUSrc, Jump, BranchNotEqual;
        
        bitset<3> ALUOp;
        
        next_IDEx.branch=zero&BranchEqual;

        controlUnit(opCode,RegDst,BranchEqual, MemReadEn,MemtoReg, ALUOp,MemWriteEn, RegWriteEn,ALUSrc, Jump, BranchNotEqual);


         
         next_IDEx.opCode=opCode;
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


         //excute

         



       

        

        



        




        














        

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






