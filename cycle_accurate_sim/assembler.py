"""
Notes from Bader
FUNCTION: This script reads a file containing MIPS assembly instructions and converts them to hex.
The hex instructions are then written to a file in the format of a memory initialization file (MIF).

instructions are read from a file called instructions.txt
instructions are not case sensitive (i.e. ADDI, addi, Addi are all valid)
a blank line is considered a NOP instruction (i.e. 0x00000000) to avoid errors

The script can handle the following instruction formats:
R-Format: add, sub, and, or, slt, nor, sll, srl, jr, xor
I-Format: addi, lw, sw, beq, bne, ori, xori, andi, slti
J-Format: j, jal

addresses and immediate values are in decimal or hexadecimal format

"""
def convert_to_binary(asm_instruction):
    if asm_instruction.replace(' ' ,'') == "" or asm_instruction.lower().replace(' ' , '') == "nop":
        return "0x00000000"
    # R-Format Instructions
    R_Format = ["add", "sub", "and", "or", "slt","nor", "sll", "srl", "jr","xor","sgt"]
    # I-Format Instructions
    I_Format = ["addi", "lw", "sw", "beq","bne","ori","xori","andi","slti"]
    # J-Format Instructions
    J_Format = ["j", "jal"]
    funct_codes = {
        "add": "100000",
        "sub": "100010",
        "and": "100100",
        "or":  "100101",
        "slt": "101010",
        "nor": "100111",
        "sll": "000000",
        "srl": "000010",
        "jr":  "001000",
        "xor": "100110",
        "sgt": "101011" # opcode given randomly because sgt is not included in MIPS green sheet
    }

    I_opcode = {
        "addi": "001000",
        "lw":   "100011",
        "sw":   "101011",
        "beq":  "000100",
        "bne":  "000101",
        "ori":  "001101",
        "xori": "001110",
        "andi": "001100",
        "slti": "001010"
    }
    J_opcode = {
        "j":    "000010",
        "jal":  "000011"
    }
    opcode = "000000"
    rs = "00000"
    rt = "00000"
    rd = "00000"
    shamt = "00000"
    funct = ""
    imm = ""
    result = ""
    address = "00000000000000000000000000"
    inst = asm_instruction.lower().split(' ')
    regs = inst[1].split(',')

    # I-Format instructions
    if inst[0] in I_Format:
        opcode = I_opcode[inst[0]]
        if inst[0] == "lw" or inst[0] == "sw":
            rt = format(int(regs[0][1:]), '05b')
            imm = format(int(regs[1].split('(')[0]), '016b')
            rs = format(int(regs[1].split('(')[1][1:-1]), '05b')
            result = opcode + rs + rt + imm
            return format(int(result, 2), '08X')  # Return hex string without "0x" prefix
        if inst[0] == "beq" or inst[0] == "bne":
            rs = format(int(regs[0][1:]), '05b')
            rt = format(int(regs[1][1:]), '05b')
        else:
            rs = format(int(regs[1][1:]), '05b')
            rt = format(int(regs[0][1:]), '05b')
        imm_value = int(regs[2], 0)
        #to handle negative numbers
        if imm_value < 0:
            imm = format((1 << 16) + imm_value, '016b')
        else:
            imm = format(imm_value, '016b')
        result = opcode + rs + rt + imm
        return format(int(result, 2), '08X')  # Return hex string without "0x" prefix

    # J-Format instructions (can handle hex or decimal addresses)
    elif inst[0] in J_Format:
        address = format(int(inst[1], 0), '026b')
        result = J_opcode[inst[0]] + address
        return format(int(result, 2), '08X')  # Return hex string without "0x" prefix


    # R-Format instructions
    elif inst[0] in R_Format:
        if inst[0] == "sll" or inst[0] == "srl":
            rd = format(int(regs[0][1:]), '05b')
            rt = format(int(regs[1][1:]), '05b')
            shamt = format(int(regs[2]), '05b')
        elif inst[0] == "jr":
            rs = format(int(regs[0][1:]), '05b')
        else:
            rd = format(int(regs[0][1:]), '05b')
            rs = format(int(regs[1][1:]), '05b')
            rt = format(int(regs[2][1:]), '05b')
        funct = funct_codes[inst[0]]
        result = opcode + rs + rt + rd + shamt + funct
        return format(int(result, 2), '08X')  # Return hex string without "0x" prefix



instructions = []

# Open the file and read line by line
with open('instructions.txt', 'r') as file:
    # Read each line in the file and strip newline characters
    instructions = [line.strip() for line in file]


with open('instructionMemoryInitializationFile.mif', 'w') as mif_file:
    mif_file.write("WIDTH=32;\n")
    mif_file.write("DEPTH=64;\n")
    mif_file.write("ADDRESS_RADIX=UNS;\n")
    mif_file.write("DATA_RADIX=HEX;\n\n")
    mif_file.write("CONTENT BEGIN\n")



    for i, instruction in enumerate(instructions):
        binary_instruction = convert_to_binary(instruction)
        mif_file.write(f"\t{i} : {binary_instruction};\n")
        pass
    mif_file.write(f"\t[{i+1}..63] : 00000000000000000000000000000000;\n")
    mif_file.write("END;\n")



