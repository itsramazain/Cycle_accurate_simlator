"""
Notes from Bader
FUNCTION: This script reads a file containing MIPS assembly instructions and converts them to hex.
The hex instructions are then written to a file in the format of a memory initialization file (MIF).

instructions are read from a file called instructions.txt
instructions are not case sensitive (i.e. ADDI, addi, Addi are all valid)

The script can handle the following instruction formats:
R-Format: add, sub, and, or, slt, nor, sll, srl, jr, xor
I-Format: addi, lw, sw, beq, bne, ori, xori, andi, slti
J-Format: j, jal

addresses and immediate values are valid in decimal or hexadecimal format

Updates:
    - Added support for .data section
    - Added support for pseudo instructions bgez and bltz
    - Added support for labels in the .data section
    - Added support for lw and sw instructions with labels
    - Added support for comments in the input file
"""

from conversion import *


symbol_table = {}  # Store variables and their memory addresses
symbol_instructions = {}  # Store instructions with labels
data_memory = []   # Store the parsed data values
data_address = 0   # Start address of the .data section
text_address = 0   # Instruction memory address for .text section
instructions = []  # Store text instructions
data_segment = True  # Flag to determine if we're in the .data section

# Read the input file
with open('instructions.txt', 'r') as file:
    for line in file:
        line = line.strip().split('#')[0].lower()  # Remove comments and trim spaces
        if not line:
            continue
        if line.startswith('.data'):
            data_segment = True
            continue
        elif line.startswith('.text'):
            data_segment = False
            continue
        if data_segment:
            # Handle the .data section
            if ':' in line:
                label, value_str = line.split(':')
                label = label.strip()
                values = value_str.split('.word')[1].strip().split(',')
                values = [int(v, 0) for v in values]  # Convert values to integers
                symbol_table[label] = data_address
                for value in values:
                    data_memory.append(value)
                    data_address += 1  # Increment by 1 word (4 bytes per word)
        else: # text segment
            if ':' in line:
                label, instruction = line.split(':')
                label = label.strip()

                # trim spaces in instruction
                symbol_instructions[label] = text_address

                if instruction.replace(' ', '') != '':
                    instructions.append(instruction.strip())
                    text_address += 1
            else:
                if "bgez" in line or "bltz" in line:
                    inst = line.split(' ', 1)
                    if inst[0] == "bgez":
                        text_address += 2
                        rs, imm = inst[1].split(',')
                        instructions.append(f"sgt $29,$0,{rs}")
                        instructions.append(f"beq $29,$0,{imm}")

                    elif inst[0] == "bltz":
                        text_address += 2
                        rs, imm = inst[1].split(',')
                        instructions.append(f"slt $29,{rs},$0")
                        instructions.append(f"bne $29,$0,{imm}")
                else:
                    instructions.append(line.strip())
                    text_address += 1


# Generate Data Memory Initialization File

with open('../dataMemoryInitializationFile.mif', 'w') as data_mif_file:
    data_mif_file.write("WIDTH=32;\n")
    data_mif_file.write("DEPTH=256;\n")
    data_mif_file.write("ADDRESS_RADIX=UNS;\n")
    data_mif_file.write("DATA_RADIX=UNS;\n\n")
    data_mif_file.write("CONTENT BEGIN\n")
    for i, value in enumerate(data_memory):
        data_mif_file.write(f"\t{i} : {value};\n")
    # Fill remaining memory with zeros
    if data_address < 256:
        data_mif_file.write(f"\t[{data_address}..255] : 0;\n")
    data_mif_file.write("END;\n")

# Generate Instruction Memory Initialization File
with open('../instructionMemoryInitializationFile.mif', 'w') as mif_file:
    mif_file.write("WIDTH=32;\n")
    mif_file.write("DEPTH=256;\n")
    mif_file.write("ADDRESS_RADIX=UNS;\n")
    mif_file.write("DATA_RADIX=HEX;\n\n")
    mif_file.write("CONTENT BEGIN\n")
    for i, instruction in enumerate(instructions):
        # Replace variable with address for load/store
        if 'lw' in instruction or 'sw' in instruction:
            for symbol in symbol_table:
                if symbol in instruction:
                    instruction = instruction.replace(symbol, str(symbol_table[symbol]))
                    break
        if 'j' in instruction or 'jal' in instruction:
            for symbol in symbol_instructions:
                if symbol in instruction:
                    instruction = instruction.replace(symbol, str(symbol_instructions[symbol]))
                    break

        if 'beq' in instruction or 'bne' in instruction:
            for symbol in symbol_instructions:
                if symbol in instruction:
                    instruction = instruction.replace(symbol, str(symbol_instructions[symbol]-i-1))

        binary_instruction = convert_to_binary(instruction)
        mif_file.write(f"\t{i} : {binary_instruction};\n")
    mif_file.write(f"\t[{len(instructions)}..255] : 00000000;\n")
    mif_file.write("END;\n")

print("Data and Instructions converted successfully!")
