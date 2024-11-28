#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <array>
#include"Data_memory.cpp"
#include"Instruction_memory.cpp"
#include"registerfile.cpp"


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

const int DATA_WIDTH = 32;
const int SEL_WIDTH = 4;

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

// ALU function
std::bitset<DATA_WIDTH> alu(
     std::bitset<DATA_WIDTH> operand1,
     std::bitset<DATA_WIDTH> operand2,
     std::bitset<SEL_WIDTH> opSel,
    std::bitset<5> shamt,
    bool& zero
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





struct PC{
    bitset<6>pc=0b111111;            
    
    
} ;


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


std::bitset<32> zeroExtendPC(std::bitset<6> pc) {
    // Convert the 6-bit PC to an unsigned integer
    uint8_t pcValue = static_cast<uint8_t>(pc.to_ulong());

    // Zero-extend to 32 bits by converting to uint32_t
    uint32_t extendedValue = static_cast<uint32_t>(pcValue);

    // Return the zero-extended value as a 32-bit bitset
    return std::bitset<32>(extendedValue);
}


std::bitset<6> calculateTarget(const std::bitset<6> pcplus1, const std::bitset<6> imm) {
    // Add the two 6-bit values and mask the result to 6 bits
    unsigned long result = (pcplus1.to_ulong() + imm.to_ulong()) & 0b111111;

    // Convert the result back to a 6-bit bitset and return it
    return std::bitset<6>(result);
}

int i=1;


void printState(
    RF& registerFile,
    Data_memory& dataMemory,
    PC& pc,                 // Program Counter
    std::bitset<6>& nextpc, // Next PC
    std::bitset<32>& instruction, // Current instruction
    int& i,                 // Cycle count
    bool& Jump,             // Jump signal
    bool& BranchEqual,      // Branch equal signal
    bool& BranchNotEqual,   // Branch not equal signal
    bool& MemReadEn,        // Memory Read Enable
    bool& MemWriteEn,       // Memory Write Enable
    bool& RegWriteEn,       // Register Write Enable
    bool& MemtoReg,         // Memory to Register signal
    bool& ALUSrc,           // ALU Source signal
    bool& RegDst,           // Register Destination signal
    bool& JumpAndLink,      // Jump and Link signal
    std::bitset<32>& aluResult, // ALU result
    std::bitset<32>& memData,   // Data read from memory
    const std::string& filename  // Log file name
) {
    // Open file in append mode
    std::ofstream logFile(filename, std::ios::app);

    // Ensure the file is open
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    logFile << "==================== Cycle State #" << i << " ====================\n";

    // Log PC and instruction
    logFile << "PC: 0x" << std::hex << pc.pc.to_ulong() << std::endl;
    logFile << "Next PC: 0x" << std::hex << nextpc.to_ulong() << std::endl;
    logFile << "Instruction: 0x" << std::hex << instruction.to_ulong() << std::endl;

    // Log control signals
    logFile << "\nControl Signals:\n";
    logFile << "Jump: " << Jump << std::endl;
    logFile << "BranchEqual: " << BranchEqual << std::endl;
    logFile << "BranchNotEqual: " << BranchNotEqual << std::endl;
    logFile << "MemReadEn: " << MemReadEn << std::endl;
    logFile << "MemWriteEn: " << MemWriteEn << std::endl;
    logFile << "RegWriteEn: " << RegWriteEn << std::endl;
    logFile << "MemtoReg: " << MemtoReg << std::endl;
    logFile << "ALUSrc: " << ALUSrc << std::endl;
    logFile << "RegDst: " << RegDst << std::endl;
    logFile << "JumpAndLink: " << JumpAndLink << std::endl;

    // Log intermediate values
    logFile << "\nIntermediate Values:\n";
    logFile << "ALU Result: 0x" << std::hex << aluResult.to_ulong() << std::endl;
    logFile << "Memory Data: 0x" << std::hex << memData.to_ulong() << std::endl;

    // Log register file contents
    logFile << "\nRegister File Contents:\n";
    for (int regIdx = 0; regIdx < 32; ++regIdx) {
        logFile << "R[" << regIdx << "]: "
                << registerFile.readRS(std::bitset<5>(regIdx))
                << std::endl;
    }

    // Log data memory contents
    logFile << "\nData Memory Contents:\n";
    for (int memIdx = 0; memIdx < 64; ++memIdx) {
        logFile << "Mem[" << memIdx << "]: "
                << dataMemory.read(std::bitset<6>(memIdx))
                << std::endl;
    }

    logFile << "=====================================================\n";
    logFile.close();
}





int main(){
    PC pc,next_pc;
    Data_memory DATA_MEM;
    Data_memory next_DATA_MEM;
    Instruction_Memory INSTRUCTION_MEM;
    Instruction_Memory next_INSTRUCTION_MEM;
    RF Rfile;
    RF next_Rfile;

    bool RegDst=0,BranchEqual=0, MemReadEn=0,MemtoReg=0, MemWriteEn=0, RegWriteEn=0,ALUSrc=0, Jump=0, BranchNotEqual=0,JumpAndLink=0;
    bitset<32>instruction=0;
    bitset<16> imm16=0;
    bitset<5> rs=0;
    bitset<5> rt=0;
    bitset<5> rd=0;
    bitset<6> imm6=0;
    bitset<32>Eximm=0;
    bitset<5> shamt=0;
    bitset<32>readdata1,readdata2;
    bitset<26> jumpAddress=0;
    bitset<6> opcode=0, functionField=0;
    bitset<4> opSel=0;
    bitset<32>res=0;
    bitset<32>data=0;
    
    bitset<6> address=0;
    bitset<32> wbdata=0;
    bitset<5> wbadress=0;
    bitset<6> pcplus1=0;
    bitset<6> nextpc;
    bool branch;
    bool zero=0;
    
    bool JumpReg=0;
    int i=0;


    while(i<7){
    //fetch
    if  ((BranchNotEqual & !zero) || (BranchEqual & zero) ){
        branch=1 ;
    }
    
    pcplus1 = std::bitset<6>(pc.pc.to_ulong() + 1);

    nextpc=pcplus1;
    imm6=instruction.to_ulong() & 0b111111;
    imm16=instruction.to_ulong() & 0b1111111111111111;
    Eximm=signExtendImmediate(instruction);
    if (branch){
            nextpc=calculateTarget(pcplus1,imm6);
    }

    if (Jump){
        nextpc=jumpAddress.to_ulong() & 0x3F;
    }
    next_pc.pc=nextpc;

    //decode
    imm16=instruction.to_ulong() & 0b1111111111111111;
    rs=bitset<5>((instruction.to_ulong() >> 21) & 0b11111);
    rt=bitset<5>((instruction.to_ulong() >> 16) & 0b11111);
    rd=bitset<5>((instruction.to_ulong() >> 11) & 0b11111);
    imm6=instruction.to_ulong() & 0b111111;
    Eximm=signExtendImmediate(instruction);
    shamt = std::bitset<5>((instruction.to_ulong() >> 6) & 0b11111);
    
    jumpAddress = std::bitset<26>(instruction.to_ulong() & 0x3FFFFFF);
    
    opcode = bitset<6>((instruction.to_ulong() >> 26) & 0b111111);
    functionField=bitset<6>(instruction.to_ulong() & 0b111111);

    
        
    bitset<3> ALUOp;

    controlUnit(opcode,RegDst,BranchEqual, MemReadEn,MemtoReg, ALUOp,MemWriteEn, RegWriteEn,ALUSrc, Jump, BranchNotEqual);
    JumpAndLink=Jump & RegWriteEn;
    readdata1=Rfile.readRS(rs);
    readdata2=Rfile.readRS(rt);

    //exute
    bitset<32>B;

    if (ALUSrc){
        B=Eximm;

    }else{
        B=readdata2;
    }
    
    ALUcontrol( ALUOp,functionField,JumpReg,opSel); 
    res= alu( readdata1,B, opSel,shamt,zero);


    //mem

    
    address = res.to_ulong() & 0x3F;
    if (MemWriteEn){
        next_DATA_MEM.write(address,readdata2);
        
    }else if (MemReadEn){
           data= DATA_MEM.read(address);
    }

    //wb

    
    if (MemtoReg){
        wbdata=data;
    }else{
       wbdata=res; 
    }
    if (JumpAndLink){
        wbdata=zeroExtendPC(pc.pc);
    }
    
    if (RegDst){
        wbadress=rd;

    }else{
        wbadress=rt;
    }
    
    if(JumpAndLink){
        wbadress=0b11111;
    }
    if (RegWriteEn){
        next_Rfile.write(wbadress,wbdata);

    }


    pc=next_pc;
    DATA_MEM=next_DATA_MEM;
    INSTRUCTION_MEM=next_INSTRUCTION_MEM;
    Rfile= next_Rfile;
    i++;
    
    printState(
    Rfile,                  // Register file
    DATA_MEM,               // Data memory
    pc,                     // Current PC
    nextpc,                 // Next PC
    instruction,            // Instruction
    i,                      // Cycle count
    Jump,                   // Jump signal
    BranchEqual,            // Branch equal signal
    BranchNotEqual,         // Branch not equal signal
    MemReadEn,              // Memory Read Enable
    MemWriteEn,             // Memory Write Enable
    RegWriteEn,             // Register Write Enable
    MemtoReg,               // Memory to Register signal
    ALUSrc,                 // ALU Source signal
    RegDst,                 // Register Destination signal
    JumpAndLink,            // Jump and Link signal
    res,                    // ALU result
    data,                   // Data read from memory
    "singlecyclelog.txt"    // Log file name
);


    }


    

}
