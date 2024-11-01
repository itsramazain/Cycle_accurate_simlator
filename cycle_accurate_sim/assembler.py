def convert_to_binary(asm_instruction):
    R_Format = ["add", "sub", "and", "or", "slt",]
    I_Format = ["addi", "lw", "sw", "beq"]
    funct_codes = {
        "add": "100000",
        "sub": "100010",
        "and": "100100",
        "or": "100101"
    }
    I_opcode = {
        "addi": "001000",
        "lw": "100011",
        "sw": "101011",
        "beq": "000100"
    }
    opcode = "000000"
    rs = ""
    rt = ""
    rd = ""
    shamt = "00000"
    funct = ""
    imm = ""
    result = ""
    inst = asm_instruction.split(' ')
    regs = inst[1].split(',')

    if inst[0] in I_Format:
        opcode = I_opcode[inst[0]]
        if inst[0] == "lw" or inst[0] == "sw":
            rt = format(int(regs[0][1:]), '05b')
            imm = format(int(regs[1].split('(')[0]), '016b')
            rs = format(int(regs[1].split('(')[1][1:-1]), '05b')
            result = opcode + rs + rt + imm
            return hex(int(result, 2))
        if inst[0] == "beq":
            rs = format(int(regs[0][1:]), '05b')
            rt = format(int(regs[1][1:]), '05b')
        elif inst[0] == "addi":
            rs = format(int(regs[1][1:]), '05b')
            rt = format(int(regs[0][1:]), '05b')
        imm_value = int(regs[2], 0)
        if imm_value < 0:
            imm = format((1 << 16) + imm_value, '016b')
        else:
            imm = format(imm_value, '016b')
        result = opcode + rs + rt + imm
        return hex(int(result, 2))
    elif inst[0] in R_Format:
        rd = format(int(regs[0][1:]), '05b')
        rs = format(int(regs[1][1:]), '05b')
        rt = format(int(regs[2][1:]), '05b')
        funct = funct_codes[inst[0]]
        result = opcode + rs + rt + rd + shamt + funct
        return hex(int(result, 2))



with open("inputcode.asm", 'r') as input_file:
            with open("Instruction_memory.txt", 'w') as output_file:
                for line in input_file:
                    # Process the line using assembler function
                    processed_line = convert_to_binary(line.strip())
                    # Write processed line to output file
                    output_file.write(processed_line + '\n')


