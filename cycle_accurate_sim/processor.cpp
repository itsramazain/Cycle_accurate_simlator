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

std::bitset<DATA_WIDTH> alu(
    const std::bitset<DATA_WIDTH>& operand1,
    const std::bitset<DATA_WIDTH>& operand2,
    uint8_t opSel,
    uint8_t shamt,
    bool& zero
) {
    std::bitset<DATA_WIDTH> result;

    switch (opSel) {
        case ADD: {
            uint32_t op1 = operand1.to_ulong();
            uint32_t op2 = operand2.to_ulong();
            result = op1 + op2;
            break;
        }
        case SUB: {
            uint32_t op1 = operand1.to_ulong();
            uint32_t op2 = operand2.to_ulong();
            result = op1 - op2;
            break;
        }
        case AND:
            result = operand1 & operand2;
            break;
        case OR:
            result = operand1 | operand2;
            break;
        case SLT:
            result = (operand1.to_ulong() < operand2.to_ulong()) ? 1 : 0;
            break;
        case XOR:
            result = operand1 ^ operand2;
            break;
        case NOR:
            result = ~(operand1 | operand2);
            break;
        case SLL:
            result = operand1 << shamt;
            break;
        case SRL:
            result = operand1 >> shamt;
            break;
        default:
            result = 0xFFFFFFFF; // Undefined operation
            break;
    }

    // Set the zero flag
    zero = (result.none());
    return result;
}




const int DATA_WIDTH = 32;
const int SEL_WIDTH = 4;

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
        default:
            result = 0xFFFFFFFF; // Undefined operation
            break;
    }

    
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
    const std::bitset<6>& opCode,
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




// Define constants for ALU operations (opSel)
const uint8_t _ADD = 0b0000;
const uint8_t _SUB = 0b0001;
const uint8_t _AND = 0b0010;
const uint8_t _OR = 0b0011;
const uint8_t _SLT = 0b0100;
const uint8_t _XOR = 0b0101;
const uint8_t _NOR = 0b0110;
const uint8_t _SLL = 0b0111;
const uint8_t _SRL = 0b1111;
const uint8_t _DONTCARE = 0b1111; // Placeholder for undefined behavior

// Define constants for funct field
const uint8_t _add_ = 0x20;
const uint8_t _sub_ = 0x22;
const uint8_t _and_ = 0x24;
const uint8_t _or_ = 0x25;
const uint8_t _slt_ = 0x2a;
const uint8_t _nor_ = 0x27;
const uint8_t _sll_ = 0x00;
const uint8_t _srl_ = 0x02;
const uint8_t _jr_ = 0x08;
const uint8_t _xor_ = 0x26;
const uint8_t _sgt_ = 0x2b;

// ALU Control Function
void ALUcontrol(
    const std::bitset<3>& ALUOp,   // 3-bit ALUOp input
    const std::bitset<6>& funct,   // 6-bit funct input
    bool& JumpReg,                 // Output: JumpReg signal
    std::bitset<4>& opSel          // Output: ALU operation selection
) {
    // Reset outputs
    JumpReg = false;
    opSel = _DONTCARE;

    // Combine ALUOp and funct into a single key for comparison
    uint8_t ALUOpInt = static_cast<uint8_t>(ALUOp.to_ulong());
    uint8_t functInt = static_cast<uint8_t>(funct.to_ulong());

    // Match the combined key with the corresponding operation
    switch ((ALUOpInt << 6) | functInt) {
        // ALUOp-independent cases
        case (0b000 << 6): opSel = _ADD; break; // lw or sw or addi
        case (0b001 << 6): opSel = _SUB; break; // beq
        case (0b010 << 6): opSel = _XOR; break; // xori
        case (0b011 << 6): opSel = _OR; break;  // ori
        case (0b100 << 6): opSel = _AND; break; // andi
        case (0b101 << 6): opSel = _SLT; break; // slt

        // ALUOp-dependent (R-Type) cases
        case ((0b110 << 6) | _add_): opSel = _ADD; break; // add
        case ((0b110 << 6) | _sub_): opSel = _SUB; break; // sub
        case ((0b110 << 6) | _and_): opSel = _AND; break; // and
        case ((0b110 << 6) | _or_):  opSel = _OR; break;  // or
        case ((0b110 << 6) | _slt_): opSel = _SLT; break; // slt
        case ((0b110 << 6) | _xor_): opSel = _XOR; break; // xor
        case ((0b110 << 6) | _nor_): opSel = _NOR; break; // nor
        case ((0b110 << 6) | _sll_): opSel = _SLL; break; // sll
        case ((0b110 << 6) | _srl_): opSel = _SRL; break; // srl
        case ((0b110 << 6) | _jr_):  JumpReg = true; break; // JumpReg

        // Default case
        default: opSel = _DONTCARE; break;
    }
}






struct PC{            
    
    
} ;


struct IFIDreg{            
    
} ;



struct IDExreg {            
    

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

        

        



        




        














        

        pc=next_pc;
        IFID=next_IFID;
        IDEx=next_IDEx;
        MEMRB=next_MEMRB;
        EXMEM=next_EXMEM;
        DATA_MEM=next_DATA_MEM;
        INSTRUCTION_MEM=next_INSTRUCTION_MEM;
        Rfile= next_Rfile;

    }



          
return 0;
}






