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

// Define constants for ALU operations (opSel)
// Define constants for ALU operations (opSel)



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


void controlUnit(
    std::bitset<6> opCode,
    std::bitset<6> funct,
    bool& RegDst,
    bool& Link,
    bool& BranchEqual,
    bool& MemReadEn,
    bool& MemtoReg,
    bool& MemWriteEn,
    bool& RegWriteEn,
    bool& ALUSrc,
    bool& Jump,
    bool& BranchNotEqual,
    bool& JumpReg,
    bool& Sgt,
    bool& Slt,
    std::bitset<4>& opSel
) {
    // Convert opcode and funct to integers for easier comparison
    uint8_t opCodeInt = static_cast<uint8_t>(opCode.to_ulong());
    uint8_t functInt = static_cast<uint8_t>(funct.to_ulong());

    // Reset all control signals
    RegDst = false;
    Link = false;
    BranchEqual = false;
    MemReadEn = false;
    MemtoReg = false;
    MemWriteEn = false;
    RegWriteEn = false;
    ALUSrc = false;
    Jump = false;
    BranchNotEqual = false;
    JumpReg = false;
    Sgt = false;
    Slt = false;
    opSel = 0b0000;

    // Opcode Parameters
    const uint8_t _RType = 0x0, _addi = 0x8, _lw = 0x23, _sw = 0x2B, _beq = 0x4, _bne = 0x5;
    const uint8_t _ori = 0xD, _xori = 0xE, _andi = 0xC, _jump = 0x2, _jumpandlink = 0x3;

    // Funct field parameters
    const uint8_t _add_ = 0x20, _sub_ = 0x22, _and_ = 0x24, _or_ = 0x25, _slt_ = 0x2A;
    const uint8_t _nor_ = 0x27, _sll_ = 0x0, _srl_ = 0x2, _jr_ = 0x8, _xor_ = 0x26, _sgt_ = 0x2B;

    // ALU Operation Parameters
    const uint8_t _ADD = 0b0000, _SUB = 0b0001, _AND = 0b0010, _OR = 0b0011, _SLT = 0b0100;
    const uint8_t _XOR = 0b0101, _NOR = 0b0110, _SLL = 0b0111, _SRL = 0b1000, _SGT = 0b1001;

    // Generate control signals based on the opcode and funct
    switch (opCodeInt) {
        case _RType:
            RegDst = true;
            RegWriteEn = true;
            ALUSrc = false;
            switch (functInt) {
                case _add_: opSel = _ADD; break;
                case _sub_: opSel = _SUB; break;
                case _and_: opSel = _AND; break;
                case _or_:  opSel = _OR; break;
                case _slt_: 
                    opSel = _SLT;
                    Slt = true;
                    break;
                case _xor_: opSel = _XOR; break;
                case _nor_: opSel = _NOR; break;
                case _sll_: opSel = _SLL; break;
                case _srl_: opSel = _SRL; break;
                case _sgt_: 
                    opSel = _SGT;
                    Sgt = true;
                    break;
                case _jr_:
                    JumpReg = true;
                    opSel = 0b0000; // No ALU operation needed for JR
                    break;
                default:
                    opSel = 0b0000; // Default case for unsupported funct
                    break;
            }
            break;

        case _addi:
            RegDst = false;
            ALUSrc = true;
            RegWriteEn = true;
            opSel = _ADD;
            break;

        case _andi:
            ALUSrc = true;
            RegWriteEn = true;
            opSel = _AND;
            break;

        case _ori:
            ALUSrc = true;
            RegWriteEn = true;
            opSel = _OR;
            break;

        case _xori:
            ALUSrc = true;
            RegWriteEn = true;
            opSel = _XOR;
            break;

        case _lw:
            MemReadEn = true;
            MemtoReg = true;
            RegWriteEn = true;
            ALUSrc = true;
            opSel = _ADD;
            break;

        case _sw:
            MemWriteEn = true;
            ALUSrc = true;
            opSel = _ADD;
            break;

        case _beq:
            BranchEqual = true;
            ALUSrc = false;
            opSel = _SUB;
            break;

        case _bne:
            BranchNotEqual = true;
            ALUSrc = false;
            opSel = _SUB;
            break;

        case _jump:
            Jump = true;
            break;

        case _jumpandlink:
            Jump = true;
            RegWriteEn = true;
            Link = true;
            opSel = _ADD;
            break;

        default:
            // All signals are already reset at the start
            break;
    }
}


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
            //cout<<"op1"<<op1<<endl;
            //cout<<"op2"<<op2<<endl;
            //cout<<"res:"<<result<<endl;
            
            
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
    

    return result;
}


void branchDetectionUnit(
    bool BranchEqual, bool BranchNotEqual, bool zero,
    bool JumpReg, bool Jump,
    bool& willBranch, bool& willJump
) {
    // Calculate willBranch and willJump based on the inputs
    willBranch = ((~zero && BranchNotEqual) || (zero && BranchEqual)) ? true : false;
    willJump = (Jump || JumpReg) ? true : false;
}

void comparator(
    std::bitset<32> rsData, std::bitset<32> rtData,
    bool& zero, bool& lessthan, bool& greaterthan
) {
    // Initialize the output values
    zero = false;
    lessthan = false;
    greaterthan = false;

    // Convert bitsets to signed integers
    int32_t rsSigned = static_cast<int32_t>(rsData.to_ulong());
    int32_t rtSigned = static_cast<int32_t>(rtData.to_ulong());

    // Adjust signed interpretation using 2's complement rules (if necessary)
    if (rsData[31]) {
        rsSigned -= (1 << 31); // Correct adjustment for negative numbers
    }
    if (rtData[31]) {
        rtSigned -= (1 << 31); // Correct adjustment for negative numbers
    }

    // Perform signed comparison
    if (rsSigned == rtSigned) {
        zero = true;
    } else if (rsSigned < rtSigned) {
        lessthan = true;
    } else {
        greaterthan = true;
    }
}




void EX_Forwarding(
    bool RegWriteEn_MEMWB, bitset<5> writeRegister_MEMWB,
    bool RegWriteEn_EXMEM, bitset<5>  writeRegister_EXMEM,
    bitset<5>  rs_IDEX, bitset<5>  rt_IDEX,
    bitset<2> &ForwardA, bitset<2> &ForwardB
) {
    
    //no forwording needed
    ForwardA = 0b00; // Default value: 00 (No forwarding)
    ForwardB = 0b00; // Default value: 00 (No forwarding)

    // ForwardA: Handling first ALU input

    //if 
    // r1<--r2 op r3
    //r2<--r10 o p29
    // r3<-- r1 op r11
    //choose input to be witeback vlue
    if (RegWriteEn_EXMEM && 
        writeRegister_EXMEM != 0 && 
        writeRegister_EXMEM == rs_IDEX) {
        ForwardA = 0b10; // Forward from EX/MEM
    }

    //if 
    // r1<--r2 op r3
    // r3<-- r1 op r11
    //choose input to be aluresfrom EXMEM
    if (RegWriteEn_MEMWB && 
        writeRegister_MEMWB != 0 && 
        writeRegister_MEMWB == rs_IDEX &&
        !(RegWriteEn_EXMEM && writeRegister_EXMEM == rs_IDEX)) {
        ForwardA = 0b01; // Forward from MEM/WB
    }

    // ForwardB: Handling second ALU input
    if (RegWriteEn_EXMEM && 
        writeRegister_EXMEM != 0 && 
        writeRegister_EXMEM == rt_IDEX) {
        ForwardB = 0b10; // Forward from EX/MEM
    }
    if (RegWriteEn_MEMWB && 
        writeRegister_MEMWB != 0 && 
        writeRegister_MEMWB == rt_IDEX &&
        !(RegWriteEn_EXMEM && writeRegister_EXMEM == rt_IDEX)) {
        ForwardB = 0b01; // Forward from MEM/WB
    }
}






void HazardDetectionUnit(
    bitset<5> rs, bitset<5> rt, bitset<5> writeRegister_EXMEM, bitset<5> writeRegister_IDEX,bool Slt,bool Sgt,
    bool Slt_IDEX, bool Sgt_IDEX, bool MemReadEn_EXMEM, bool MemReadEn_IDEX,
    bool JumpReg, bool BranchEqual, bool Jump, bool BranchNotEqual, 
    bool willBranch, bool willJump,
    bool &PCwrite, bool &Write_IFID, bool &nopSel, bool &flush
) {
    bool CannotResolveBranch, CannotResolveJumpReg;

    // Resolve branch and jump conditions
    bool CannotResolveBranchCondition1 = ((rs == writeRegister_IDEX || rt == writeRegister_IDEX) &&
                                          writeRegister_IDEX != 0 && !(Slt_IDEX || Sgt_IDEX));

    bool CannotResolveBranchCondition2 = ((rs == writeRegister_EXMEM || rt == writeRegister_EXMEM) &&
                                          writeRegister_EXMEM != 0 && MemReadEn_EXMEM);

    CannotResolveBranch = CannotResolveBranchCondition1 || CannotResolveBranchCondition2;

    
    bool CannotResolveJumpRegCondition1 = (rs == writeRegister_IDEX && writeRegister_IDEX != 0 &&
                                           !(Slt_IDEX || Sgt_IDEX));

    bool CannotResolveJumpRegCondition2 = (rs == writeRegister_EXMEM && writeRegister_EXMEM != 0 &&
                                           MemReadEn_EXMEM);
    
    
    CannotResolveJumpReg = CannotResolveJumpRegCondition1 || CannotResolveJumpRegCondition2;
    bool FlushCondition = ((willBranch && !CannotResolveBranch) || (JumpReg && !CannotResolveJumpReg) || Jump);

    

    

    
   
   

    // Stall conditions
    bool FirstStallCondition = (MemReadEn_IDEX && (writeRegister_IDEX != 0) &&
                                ((writeRegister_IDEX == rs) || (writeRegister_IDEX == rt)));

    bool SecondStallCondition = ((writeRegister_IDEX == rs || writeRegister_IDEX == rt) &&
                                 writeRegister_IDEX != 0 && (BranchEqual || BranchNotEqual || JumpReg) &&
                                 !(Slt_IDEX || Sgt_IDEX));

    bool ThirdStallCondition = (MemReadEn_EXMEM && writeRegister_EXMEM != 0 &&
                                ((writeRegister_EXMEM == rs) || (writeRegister_EXMEM == rt)) &&
                                (BranchEqual || BranchNotEqual || JumpReg));

    bool FourthStallCondition = (Slt || Sgt);

    bool StallCondition = FirstStallCondition || SecondStallCondition || ThirdStallCondition ||FourthStallCondition;

    // Combine conditions into a 2-bit value
    uint8_t Conditions = (StallCondition << 1) | FlushCondition;

    // Initialize outputs
    PCwrite = true;
    Write_IFID = true;
    nopSel = false;
    flush = false;

    // Case statement for different conditions
    switch (Conditions) {
        case 0b10: // Stall
            PCwrite = false;
            Write_IFID = false;
            nopSel = true;
            flush = false;
            break;

        case 0b11: // Priority for Flush
            PCwrite = true;
            Write_IFID = true;
            nopSel = false;
            flush = true;
            break;

        case 0b01: // Flush
            PCwrite = true;
            Write_IFID = true;
            nopSel = false;
            flush = true;
            break;

        default: // No operation
            PCwrite = true;
            Write_IFID = true;
            nopSel = false;
            flush = false;
            break;
    }
}


void forwardingUnitID(
    bool RegWriteEn_MEMWB, bitset<5> writeRegister_MEMWB, 
    bool RegWriteEn_EXMEM, bitset<5> writeRegister_EXMEM,
    bitset<5> writeRegister_IDEX, bool RegWriteEn_IDEX,
    bitset<5> rs, bitset<5> rt, 
    bool Sgt_IDEX, bool Slt_IDEX,
    bitset<2> &ForwardA, bitset<2> &ForwardB
) {
    // Default values: No forwarding
    ForwardA = 0b00;
    ForwardB = 0b00;

    // ForwardA: Handling Rs
    if (RegWriteEn_EXMEM && writeRegister_EXMEM != 0 && writeRegister_EXMEM == rs &&
        !(Slt_IDEX || Sgt_IDEX) && writeRegister_IDEX == rt) {
        ForwardA = 0b10; // Forward from EX/MEM
    }
    if (RegWriteEn_MEMWB && writeRegister_MEMWB != 0 && writeRegister_MEMWB == rs &&
        !(RegWriteEn_EXMEM && writeRegister_EXMEM == rs) &&
        !(Slt_IDEX || Sgt_IDEX) && writeRegister_IDEX == rt) {
        ForwardA = 0b01; // Forward from MEM/WB
    }
    if (RegWriteEn_IDEX && writeRegister_IDEX != 0 && writeRegister_IDEX == rs &&
        (Slt_IDEX || Sgt_IDEX)) {
        ForwardA = 0b11; // Forward value from EX stage
    }

    // ForwardB: Handling Rt
    if (RegWriteEn_EXMEM && writeRegister_EXMEM != 0 && writeRegister_EXMEM == rt &&
        !(Slt_IDEX || Sgt_IDEX) && writeRegister_IDEX == rt) {
        ForwardB = 0b10; // Forward from EX/MEM
    }
    if (RegWriteEn_MEMWB && writeRegister_MEMWB != 0 && writeRegister_MEMWB == rt &&
        !(RegWriteEn_EXMEM && writeRegister_EXMEM == rt) &&
        !(Slt_IDEX || Sgt_IDEX) && writeRegister_IDEX == rt) {
        ForwardB = 0b01; // Forward from MEM/WB
    }
    if (RegWriteEn_IDEX && writeRegister_IDEX != 0 && writeRegister_IDEX == rt &&
        (Slt_IDEX || Sgt_IDEX)) {
        ForwardB = 0b11; // Forward value from EX stage
    }
}

void fwdValue(
    bool Link_IDEX, bool Slt_IDEX, bool Sgt_IDEX,
    bitset<6> PCPlus1_IDEX, bool lessthan_IDEX, bool greaterthan_IDEX,
    bitset<32> &FWDvalue
) {
    // Default value: 0
    FWDvalue = 0;

    // Logic for Link_IDEX
    if (Link_IDEX) {
        FWDvalue = zeroExtendPC(PCPlus1_IDEX);
    }

    // Logic for Slt_IDEX
    if (Slt_IDEX) {
        FWDvalue = bitset<32>(lessthan_IDEX ? 1 : 0); // {32'b0, lessthan_IDEX}
    }

    // Logic for Sgt_IDEX
    if (Sgt_IDEX) {
        FWDvalue = bitset<32>(greaterthan_IDEX ? 1 : 0); // {32'b0, greaterthan_IDEX}
    }
}


struct IFID_reg {
    bool flush=0;
    bitset<6> PCPlus1=0;
    bitset<32> instruction=0;
};
struct IDEX_reg {
    // 1-bit values are represented as `bit` or `logic` for compactness
    bool MemtoReg=0;
    bool RegWriteEn=0;
    bool MemReadEn=0;
    bool MemWriteEn=0;
    bool lessthan=0;
    bool greaterthan=0;
    bool ALUSrc=0;
    bool Link=0;
    bool Sgt=0;
    bool Slt=0;
    bool flush=0;
    bool BranchEqual=0;
    bool BranchNotEqual=0;
    bool zero=0; 
    bool JumpReg=0;
    bool Jump=0; 
    bool willBranch=0;
    bool willJump=0;


    bitset<6> PCPlus1=0;
    bitset<5> rs=0;
    bitset<5> rt=0;
    bitset<5> writeRegister=0;
    bitset<5> shamt=0;
    bitset<32> rsData=0;
    bitset<32> rtData=0;
    bitset<32> extImm=0;
    bitset<4> opSel=0;
    bitset<32> instruction=0;

} ;


struct EXMEM_reg {
    // 1-bit values as bool
    bool MemtoReg=0;
    bool RegWriteEn=0;
    bool MemReadEn=0;
    bool MemWriteEn=0;

    // Multi-bit values using bitset
    bitset<5> writeRegister=0;
    bitset<32> ALUResult=0;
    bitset<32> rtData=0;
    bitset<32> instruction=0;
};


struct MEMWB_reg {
    // 1-bit values as bool
    bool MemtoReg=0;
    bool RegWriteEn=0;

    // Multi-bit values using bitset
    bitset<5> writeRegister=0;
    bitset<32> ALUResult=0;
    bitset<32> memoryReadData=0;
    bitset<32> instruction=0;
};

int i=0;


void printState(
    RF registerFile,
    Data_memory dataMemory,
    PC pc,
    IFID_reg ifid,
    IDEX_reg idex,
    EXMEM_reg exmem,
    MEMWB_reg memwb,
    int cycle,
    std::bitset<6> opCode,
    std::bitset<6> funct,
    bool RegDst,
    bool Link,
    bool BranchEqual,
    bool MemReadEn,
    bool MemtoReg,
    bool MemWriteEn,
    bool RegWriteEn,
    bool ALUSrc,
    bool Jump,
    bool BranchNotEqual,
    bool JumpReg,
    bool Sgt,
    bool Slt,
    std::bitset<4> opSel,
    const std::string filename,
    bitset<32>ALUin1,
    bitset<32>ALUin2,
    bitset<2>ForwordA,
    bitset<2>ForwordB
    

) {
    // Open file in append mode
    std::ofstream logFile(filename, std::ios::app);

    // Ensure the file is open
    if (!logFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    logFile << "==================== Cycle State #" << cycle << " ====================\n";

    // Log PC and instruction
    logFile << "PC: 0x" << std::hex << pc.pc.to_ulong() << std::endl;

    // Log IFID stage
    logFile << "\nIFID Stage:\n";
    logFile << "Flush: " << ifid.flush << std::endl;
    logFile << "PCPlus1: 0x" << ifid.PCPlus1.to_ulong() << std::endl;
    logFile << "Instruction: 0x" << ifid.instruction.to_ulong() << std::endl;

    // Log IDEX stage
    logFile << "\nIDEX Stage:\n";
    logFile << "MemtoReg: " << idex.MemtoReg << std::endl;
    logFile << "RegWriteEn: " << idex.RegWriteEn << std::endl;
    logFile << "MemReadEn: " << idex.MemReadEn << std::endl;
    logFile << "MemWriteEn: " << idex.MemWriteEn << std::endl;
    logFile << "lessthan: " << idex.lessthan << std::endl;
    logFile << "greaterthan: " << idex.greaterthan << std::endl;
    logFile << "ALUSrc: " << idex.ALUSrc << std::endl;
    logFile << "Link: " << idex.Link << std::endl;
    logFile << "Sgt: " << idex.Sgt << std::endl;
    logFile << "Slt: " << idex.Slt << std::endl;
    logFile << "PCPlus1: 0x" << idex.PCPlus1.to_ulong() << std::endl;
    logFile << "rs: 0x" << idex.rs.to_ulong() << std::endl;
    logFile << "rt: 0x" << idex.rt.to_ulong() << std::endl;
    logFile << "writeRegister: 0x" << idex.writeRegister.to_ulong() << std::endl;
    logFile << "shamt: 0x" << idex.shamt.to_ulong() << std::endl;
    logFile << "rsData: 0x" << idex.rsData.to_ulong() << std::endl;
    logFile << "rtData: 0x" << idex.rtData.to_ulong() << std::endl;
    logFile << "extImm: 0x" << idex.extImm.to_ulong() << std::endl;
    logFile << "opSel: 0x" << idex.opSel.to_ulong() << std::endl;
    logFile << "Instruction: 0x" << idex.instruction.to_ulong() << std::endl;

    // Log EXMEM stage
    logFile << "\nEXMEM Stage:\n";
    logFile << "MemtoReg: " << exmem.MemtoReg << std::endl;
    logFile << "RegWriteEn: " << exmem.RegWriteEn << std::endl;
    logFile << "MemReadEn: " << exmem.MemReadEn << std::endl;
    logFile << "MemWriteEn: " << exmem.MemWriteEn << std::endl;
    logFile << "writeRegister: 0x" << exmem.writeRegister.to_ulong() << std::endl;
    logFile << "ALUResult: 0x" << exmem.ALUResult.to_ulong() << std::endl;
    logFile << "rtData: 0x" << exmem.rtData.to_ulong() << std::endl;
    logFile << "Instruction: 0x" << exmem.instruction.to_ulong() << std::endl;
    logFile << "aluin1: 0x" << ALUin1<< std::endl;
    logFile << "aluin2: 0x" << ALUin2<< std::endl;
    logFile << "forwordA: 0x" << ForwordA<< std::endl;
    logFile << "forwordB: 0x" << ForwordB<< std::endl;

    // Log MEMWB stage
    logFile << "\nMEMWB Stage:\n";
    logFile << "MemtoReg: " << memwb.MemtoReg << std::endl;
    logFile << "RegWriteEn: " << memwb.RegWriteEn << std::endl;
    logFile << "writeRegister: 0x" << memwb.writeRegister.to_ulong() << std::endl;
    logFile << "ALUResult: 0x" << memwb.ALUResult.to_ulong() << std::endl;
    logFile << "memoryReadData: 0x" << memwb.memoryReadData.to_ulong() << std::endl;
    logFile << "Instruction: 0x" << memwb.instruction.to_ulong() << std::endl;

    // Log additional control signals
    logFile << "\nControl Signals:\n";
    logFile << "opCode: 0x" << opCode.to_ulong() << std::endl;
    logFile << "funct: 0x" << funct.to_ulong() << std::endl;
    logFile << "RegDst: " << RegDst << std::endl;
    logFile << "Link: " << Link << std::endl;
    logFile << "BranchEqual: " << BranchEqual << std::endl;
    logFile << "MemReadEn: " << MemReadEn << std::endl;
    logFile << "MemtoReg: " << MemtoReg << std::endl;
    logFile << "MemWriteEn: " << MemWriteEn << std::endl;
    logFile << "RegWriteEn: " << RegWriteEn << std::endl;
    logFile << "ALUSrc: " << ALUSrc << std::endl;
    logFile << "Jump: " << Jump << std::endl;
    logFile << "BranchNotEqual: " << BranchNotEqual << std::endl;
    logFile << "JumpReg: " << JumpReg << std::endl;
    logFile << "Sgt: " << Sgt << std::endl;
    logFile << "Slt: " << Slt << std::endl;
    logFile << "opSel: 0x" << opSel.to_ulong() << std::endl;

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
    
    

    //definig wires

    // 32-bit wires
    bitset<32> instruction = 0;
    bitset<32> writeData = 0;
    bitset<32> readData1 = 0;
    bitset<32> readData2 = 0;
    bitset<32> extImm = 0;
    bitset<32> ALUin2 = 0;
    bitset<32> ALUin1 = 0;
    bitset<32> _ALUResult = 0;
    bitset<32> ALUResult = 0;
    bitset<32> memoryReadData = 0;
    bitset<32> rtData = 0;
    bitset<32> rsData = 0;
    bitset<32> instruction_IFID = 0;
    bitset<32> readData1_IDEX = 0;
    bitset<32> rsData_IDEX = 0;
    bitset<32> _rtData_IDEX = 0;
    bitset<32> rtData_IDEX = 0;
    bitset<32> rtData_EXMEM = 0;
    bitset<32> extImm_IDEX = 0;
    bitset<32> ALUResult_EXMEM = 0;
    bitset<32> ALUResult_MEMWB = 0;
    bitset<32> memoryReadData_MEMWB = 0;
    

    // 6-bit wires
    bitset<6> opCode = 0;
    bitset<6> funct = 0;
    bitset<6> FetchPC = 0;
    bitset<6> nextPC = 0;
    bitset<6> PCPlus1 = 0;
    bitset<6> branch_address = 0;
    bitset<6> PCPlus1_IFID = 0;
    bitset<6> PCPlus1_IDEX = 0;
    bitset<6> jump_address = 0;
    bitset<32> FWDvalue = 0;
    bitset<6> jump_to_address = 0;
    bitset<6> PC_or_Branch = 0;

    // 5-bit wires
    bitset<5> rs = 0;
    bitset<5> rt = 0;
    bitset<5> rd = 0;
    bitset<5> writeRegister = 0;
    bitset<5> shamt = 0;
    bitset<5> returnReg = 0;
    bitset<5> rs_IDEX = 0;
    bitset<5> rt_IDEX = 0;
    bitset<5> rd_IDEX = 0;
    bitset<5> writeRegister_EXMEM = 0;
    bitset<5> writeRegister_MEMWB = 0;
    bitset<5> writeRegister_IDEX = 0;
    bitset<5> shamt_IDEX = 0;
    bitset<5> writeRegister_rdrt = 0;

    // 4-bit wires
    bitset<4> _opSel = 0;
    bitset<4> opSel = 0;
    bitset<4> opSel_IDEX = 0;

    // 2-bit wires
    bitset<2> IDForwardA = 0;
    bitset<2> IDForwardB = 0;
    bitset<2> EXForwardA = 0;
    bitset<2> EXForwardB = 0;

    // 1-bit wires (bool)
    
    bool PCwrite = 1;
    bool RegDst = 0;
    bool Branch = 0;
    bool MemReadEn = 0;
    bool MemtoReg = 0;
    bool MemWriteEn = 0;
    bool RegWriteEn = 0;
    bool ALUSrc = 0;
    bool ALUSrc_IDEX = 0;
    bool JumpReg = 0;
    bool Jump = 0;
    bool Link = 0;
    bool Link_IDEX = 0;
    bool flush = 0;
    bool zero = 0;
    bool nopSel = 0;
    bool slt_IDEX = 0;
    bool sgt_IDEX = 0;
    bool willBranch = 0;
    bool willJump = 0;
    bool BranchNotEqual = 0;
    bool BranchEqual = 0;
    bool Write_IFID = 0;
    bool Sgt = 0;
    bool Slt = 0;
    bool Slt_IDEX = 0;
    bool Sgt_IDEX = 0;
    bool lessthan = 0;
    bool greaterthan = 0;
    bool lessthan_IDEX = 0;
    bool greaterthan_IDEX = 0;
    bool RegDst_IDEX = 0;
    bool MemReadEn_IDEX = 0;
    bool MemReadEn_EXMEM = 0;
    bool MemtoReg_IDEX = 0;
    bool MemtoReg_EXMEM = 0;
    bool MemtoReg_MEMWB = 0;
    bool MemWriteEn_IDEX = 0;
    bool MemWriteEn_EXMEM = 0;
    bool RegWriteEn_IDEX = 0;
    bool RegWriteEn_EXMEM = 0;
    bool RegWriteEn_MEMWB = 0;
    bool PassALU = 0;
    bool _RegDst = 0;
    bool _Link = 0;
    bool _BranchEqual = 0;
    bool _MemReadEn = 0;
    bool _MemtoReg = 0;
    bool _MemWriteEn = 0;
    bool _RegWriteEn = 0;
    bool _ALUSrc = 0;
    bool _Jump = 0;
    bool _BranchNotEqual = 0;
    bool _JumpReg = 0;
    bool _Slt = 0;
    bool _Sgt = 0;
    bool Will_branch;
    bool Will_jump;
    bitset<6>imm6;
    bitset<6>address;
    bitset<16> imm;
    bitset<32>FWDrtData;
   

    // Clear "logfile2.txt"
    std::ofstream clearFile("logfile2.txt", std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error: Could not open logfile2.txt for clearing." << std::endl;
        return 1;
    }
    clearFile.close();

    // Clear "pipe.log"
    clearFile.open("pipe.log", std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error: Could not open pipe.log for clearing." << std::endl;
        return 1;
    }
    clearFile.close();

    // Clear "memout.log"
    clearFile.open("memout.log", std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error: Could not open memout.log for clearing." << std::endl;
        return 1;
    }
    clearFile.close();
    clearFile.open("regfileoutput.log", std::ios::trunc);
    if (!clearFile.is_open()) {
        std::cerr << "Error: Could not open memout.log for clearing." << std::endl;
        return 1;
    }
    clearFile.close();
    

    EXMEM_reg EXMEM,next_EXMEM;
    IDEX_reg IDEX,next_IDEX;
    IFID_reg IFID,next_IFID;
    MEMWB_reg MEMWB,next_MEMWB;


    PC pc,next_pc;
    Data_memory DATA_MEM;
    Data_memory next_DATA_MEM;
    Instruction_Memory INSTRUCTION_MEM;
    Instruction_Memory next_INSTRUCTION_MEM;
    RF Rfile;
    RF next_Rfile;
    int num_ofinst=INSTRUCTION_MEM.num();


    while (i<20){
        
        cout<<"cycle"<<i<<endl;
        cout<<"nopSel"<<nopSel<<endl;
        //ftech
        //before we can fetch anything we have 
        //to check if the instruction that is 
        //in the id stage in the pervius stage wasnt 
        //a beq and taken
        //another thing that might happen is jal and jr this means that im supposed to take the value from the forwording unit

        
        printState(Rfile, DATA_MEM, pc, IFID, IDEX, EXMEM, MEMWB, i,
               opCode, funct, RegDst, Link, BranchEqual, MemReadEn, MemtoReg,
               MemWriteEn, RegWriteEn, ALUSrc, Jump, BranchNotEqual, JumpReg,
               Sgt, Slt, opSel, "pipe.log",ALUin2,ALUin1, EXForwardA, EXForwardB);

        
        std::ofstream reg("regfileoutput.log", std::ios::app);

        //at the end dump the values of the memory and register file in a file
        reg<<"reg content at cycle: "<<i<<endl;
            for (int regIdx = 0; regIdx < 32; ++regIdx) {
        reg << Rfile.readRS(std::bitset<5>(regIdx))
                << std::endl;
        }
    
        std::ofstream mem("memout.log", std::ios::app);
        mem<<"mem content at cycle: "<<i<<endl;
            for (int memIdx = 0; memIdx < 64; ++memIdx) {
        mem << DATA_MEM.read(std::bitset<6>(memIdx))
                << std::endl;
    }
        
        
        
        PCPlus1=bitset<6>(pc.pc.to_ulong() + 1);
        nextPC=PCPlus1;
        if (willBranch){
            nextPC=branch_address;
            cout<<"-----------"<<branch_address<<endl;
            
            /*cout<<branch_address<<endl;*/
        }
        if (willJump){
            cout<<"-----------"<<jump_address<<endl;
            
            nextPC=jump_address;

        }
        if (JumpReg){
            nextPC=rsData.to_ulong() & 0b111111;
        }
        


        if (PCwrite){
            FetchPC=nextPC;
        }else{
            
            FetchPC=pc.pc;
        }
        
        instruction=INSTRUCTION_MEM.read(FetchPC);
        
        cout<<"instruction"<<instruction<<endl;
        /*cout<<"instruction"<<instruction<<endl;
        cout<<"FetchPC"<<FetchPC<<endl;*/
       
        if (flush){
           /* cout<<instruction<<endl;*/
           /*cout<<FetchPC<<endl;
           cout<<instruction<<endl;*/
           cout<<"it was flushed at cycle:"<<i<<endl;
            IFID.instruction=0;
        }

        //update 
        if (PCwrite){
            
            next_pc.pc=nextPC;
        }
        cout<<"FetchPC"<<FetchPC<<endl;
        /*cout<<i<<endl;
        cout<<"willBranch"<<willBranch<<endl;
        cout<<"FetchPC"<<FetchPC<<endl;
        cout<<"instruction"<<instruction<<endl;
        cout<<"branch_address"<<branch_address<<endl;
        cout<<"flush"<<flush<<endl;
        cout<<"nopsel"<<nopSel<<endl;*/

        
        next_IFID.instruction=instruction;
        next_IFID.PCPlus1=PCPlus1;
        next_IFID.flush=flush;

        //decode

        opCode = (IFID.instruction >> 26).to_ulong();     // Bits 31:26
        rs = (IFID.instruction>> 21).to_ulong() & 0x1F;  // Bits 25:21
        rt = (IFID.instruction >> 16).to_ulong() & 0x1F;  // Bits 20:16
        rd = (IFID.instruction >> 11).to_ulong() & 0x1F;  // Bits 15:11
        shamt = (IFID.instruction >> 6).to_ulong() & 0x1F; // Bits 10:6
        imm = IFID.instruction.to_ulong() & 0xFFFF;      // Bits 15:0
        funct = IFID.instruction.to_ulong() & 0x3F;       // Bits 5:0
        
        
        controlUnit(opCode, funct, RegDst, Link, BranchEqual, MemReadEn, MemtoReg,
                MemWriteEn, RegWriteEn, ALUSrc, Jump, BranchNotEqual, JumpReg, Sgt, Slt, opSel);

        

        

        
        if (MEMWB.MemtoReg){
            writeData=MEMWB.memoryReadData;
            
            
        }else{
            writeData=MEMWB.ALUResult;

        } 
        
        
        

        if (MEMWB.RegWriteEn){
            Rfile.write(MEMWB.writeRegister,writeData);

        }
        
        
        

        
        readData1=Rfile.readRS(rs);
        readData2=Rfile.readRT(rt);

        

        
        

        forwardingUnitID(
        MEMWB.RegWriteEn, MEMWB.writeRegister,
        EXMEM.RegWriteEn, EXMEM.writeRegister,
        IDEX.writeRegister, IDEX.RegWriteEn,
        rs, rt,
        IDEX.Sgt, IDEX.Slt,
        IDForwardA, IDForwardB
        );

        fwdValue(IDEX.Link, IDEX.Slt, IDEX.Sgt, IDEX.PCPlus1, IDEX.lessthan, IDEX.greaterthan, FWDvalue);

        cout<<"IDForwardA"<<IDForwardA<<endl;
        cout<<"IDForwardB"<<IDForwardB<<endl;



        //get the right value for reg
        if (IDForwardA==0b00){
             rsData=readData1;
        }else if (IDForwardA==0b01){
            rsData=writeData;
        }else if (IDForwardA==0b10){
            rsData=EXMEM.ALUResult;
        }else if(IDForwardA==0b11) {
            rsData=FWDvalue;
           
        }

        
        

        //get the right value for reg
        if (IDForwardB==0b00){
             rtData=readData2;
            
        }else if (IDForwardB==0b01){
            rtData=writeData;
        }else if (IDForwardB==0b10){
            rtData=EXMEM.ALUResult;
        }else if(IDForwardB==0b11) {
            rtData=FWDvalue;
           
        }

        comparator(rsData, rtData, zero, lessthan, greaterthan);
        cout<<"lessthan"<<endl;
        cout<<"greaterthan"<<endl;
        cout<<"zero"<<endl;
        branchDetectionUnit(BranchEqual, BranchNotEqual, zero, JumpReg, Jump, willBranch, willJump);
        extImm=signExtendImmediate(IFID.instruction);
        imm6=IFID.instruction.to_ulong() & 0b111111;
        branch_address=calculateTarget(PCPlus1,imm6);
        jump_address = (IFID.instruction.to_ulong() & 0x3F); // Bits 5:0

        

        HazardDetectionUnit(
        rs, rt, EXMEM.writeRegister, IDEX.writeRegister,
        IDEX.Slt,IDEX.Sgt,Sgt,Slt, EXMEM.MemReadEn, IDEX.MemReadEn,
        JumpReg, BranchEqual, Jump, BranchNotEqual,
        willBranch, willJump,
        PCwrite, Write_IFID, nopSel, flush);
        cout<<"willBranch"<<willBranch<<endl;
        cout<<"IFIDinstruction"<<IFID.instruction<<endl;


        /*cout<<i<<endl;
        cout<<"willBranch"<<willBranch<<endl;
        cout<<"FetchPC"<<FetchPC<<endl;
        cout<<"IFIDinstruction"<<IFID.instruction<<endl;
        cout<<"branch_address"<<branch_address<<endl;
        cout<<"flush"<<flush<<endl;
        cout<<"nopsel"<<nopSel<<endl;*/
        next_IFID.instruction=instruction;
        next_IFID.PCPlus1=PCPlus1;
        next_IFID.flush=flush;

        /*if (IFID.instruction==0b00010000000000000000000000000010){
            cout<<"idstage"<<endl;
            cout<<IFID.instruction<<endl;
            cout<<"opecde"<<opCode<<endl;
            cout<<readData1<<endl;
            cout<<readData2<<endl;
            cout<<BranchEqual<<endl;
            cout<<flush<<endl;
            cout<<"zero"<<zero<<endl;
            cout<<"willbranch"<<willBranch<<endl;
        }*/

        /*if (IFID.instruction==0b00010000000000000000000000000010){
            cout<<rs<<endl;
            cout<<rs<<endl;
        }*/



        //writeregistervalue
        writeRegister=rt;
        if (RegDst){
            writeRegister=rd;
        }
        if(Link){
            writeRegister=0b11111;

        }
        
        



        

          // Assign values to struct fields
        next_IDEX.MemtoReg = MemtoReg;
        next_IDEX.flush=flush;
        next_IDEX.RegWriteEn = RegWriteEn;
        next_IDEX.MemReadEn = MemReadEn;
        next_IDEX.MemWriteEn = MemWriteEn;
        next_IDEX.lessthan = lessthan;
        next_IDEX.greaterthan = greaterthan;
        next_IDEX.ALUSrc = ALUSrc;
        next_IDEX.Link = Link;
        next_IDEX.Sgt = Sgt;
        next_IDEX.Slt = Slt;
        next_IDEX.PCPlus1 = IFID.PCPlus1;
        next_IDEX.rs = rs;
        next_IDEX.rt = rt;
        next_IDEX.writeRegister = writeRegister;
        next_IDEX.shamt = shamt;
        next_IDEX.rsData = rsData;
        next_IDEX.rtData = rtData;
        next_IDEX.extImm = extImm;
        next_IDEX.opSel = opSel;
        next_IDEX.instruction = IFID.instruction;
        next_IDEX.BranchEqual=BranchEqual;
        next_IDEX.BranchNotEqual=BranchNotEqual;
        next_IDEX.zero=zero;
        next_IDEX.JumpReg=JumpReg;
        next_IDEX.Jump=Jump;
        next_IDEX.willBranch=willBranch;
        next_IDEX.willJump=willJump;
   
        

        

        
        if(nopSel){
        next_IDEX.MemtoReg = 0;
        next_IDEX.RegWriteEn = 0;
        next_IDEX.MemReadEn = 0;
        next_IDEX.MemWriteEn = 0;
        next_IDEX.lessthan = 0;
        next_IDEX.greaterthan = 0;
        next_IDEX.ALUSrc = 0;
        next_IDEX.Link = 0;
        next_IDEX.Sgt = 0;
        next_IDEX.Slt = 0;
        }
        
        //excute stage

          EX_Forwarding(
        MEMWB.RegWriteEn, MEMWB.writeRegister,
        EXMEM.RegWriteEn, EXMEM.writeRegister,
        IDEX.rs, IDEX.rt,
        EXForwardA, EXForwardB);

       



       
        

        if (MEMWB.MemtoReg){
            writeData=MEMWB.memoryReadData;
        }else{
            writeData=MEMWB.ALUResult; 
        }


        //get the right value for reg
        if (EXForwardA==0b00){
             ALUin1=IDEX.rsData;
        }else if (EXForwardA==0b01){
            ALUin1=writeData;
        }else if (EXForwardA==0b10){
            ALUin1=EXMEM.ALUResult;
        }
        

       
        

        //get the right value for reg
        if (EXForwardB==0b00){
             FWDrtData=IDEX.rtData;
        }else if (EXForwardB==0b01){
            FWDrtData=writeData;
        } else if (EXForwardB==0b10){
            FWDrtData=EXMEM.ALUResult;
        }
        
        

        if (IDEX.ALUSrc){
            ALUin2=IDEX.extImm;
        }else{
            ALUin2=FWDrtData;

        }

        cout<<"EXForwardA"<<EXForwardA<<endl;
        cout<<"EXForwardB"<<EXForwardB<<endl;

        
        
        

        _ALUResult = alu(ALUin1, ALUin2, IDEX.opSel, IDEX.shamt);
        cout<<_ALUResult<<endl;
        /*if(IDEX.MemReadEn){
        cout<<IDEX.instruction<<endl;
        cout<<IDEX.rs<<endl;
        cout<<IDEX.rt<<endl;
        cout<<IDEX.rsData<<endl;
        cout<<IDEX.rtData<<endl;
        cout<<IDEX.opSel<<endl;
        }*/

        
        
        
        if (IDEX.Link){
            _ALUResult=FWDvalue;

        }
        
        
        next_EXMEM.MemtoReg=IDEX.MemtoReg;
        next_EXMEM.RegWriteEn=IDEX.RegWriteEn;
        next_EXMEM.MemReadEn=IDEX.MemReadEn;
        next_EXMEM.MemWriteEn=IDEX.MemWriteEn;
        next_EXMEM.writeRegister=IDEX.writeRegister;
        next_EXMEM.ALUResult=_ALUResult;
        next_EXMEM.rtData=FWDrtData;
        next_EXMEM.instruction=IDEX.instruction;
        
        

        //mem

        address = EXMEM.ALUResult.to_ulong() & 0x3F;
        
         

        if (EXMEM.MemWriteEn){
            /*cout<<address<<endl;
            cout<<EXMEM.rtData<<endl;*/
            DATA_MEM.write(address,EXMEM.rtData);

            
        
        }
        if (EXMEM.MemReadEn){

           memoryReadData= DATA_MEM.read(address);
            /*cout<<address<<endl;
            cout<<memoryReadData<<endl;*/
           
           
        }
        
        next_MEMWB.MemtoReg= EXMEM.MemtoReg;
        next_MEMWB.RegWriteEn=EXMEM.RegWriteEn;
        next_MEMWB.writeRegister=EXMEM.writeRegister;
        next_MEMWB.writeRegister=EXMEM.writeRegister;
        next_MEMWB.ALUResult=EXMEM.ALUResult;
        next_MEMWB.memoryReadData=memoryReadData;
        next_MEMWB.instruction=EXMEM.instruction;
        //wb
        
        if (MEMWB.MemtoReg){
            writeData=MEMWB.memoryReadData;
            
            
        }else{
            writeData=MEMWB.ALUResult;

        } 
        
        
        

        if (MEMWB.RegWriteEn){
            Rfile.write(MEMWB.writeRegister,writeData);

        }


        pc=next_pc;
        INSTRUCTION_MEM=next_INSTRUCTION_MEM;
        EXMEM=next_EXMEM;
        IDEX=next_IDEX;
        IFID=next_IFID;
        MEMWB=next_MEMWB;
        i++;

        

        if (pc.pc==num_ofinst+5){
            break;
        }
        
        


    
    
    }

    
    std::ofstream mem("memout.log", std::ios::app);
    mem <<"num_of_cycles_taken"<<i<<endl;
    





}















