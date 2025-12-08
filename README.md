# RISC-V-PROCESSOR-ASSEMBLER
A C++ application that takes a RISC-V instruction set and converts it into corresponding binary and executable files. Made as part of Computer Organisation and Architecture Course.
## Assembler
  ### Features
  - Instruction Types Supported:

      Register-type (R-type) instructions:
      | Instruction | Syntax                         |
      |----------------------|-----------------------|
      | ADD                  | `ADD rd, rs2, rs1`    |
      | SLT                  | `SLT rd, rs2, rs1`    |
      | SLTU                 | `SLTU rd, rs2, rs1`   |
      | AND                  | `AND rd, rs2, rs1`    |
      | OR                   | `OR rd, rs2, rs1`     |
      | XOR                  | `XOR rd, rs2, rs1`    |
      | SLL                  | `SLL rd, rs2, rs1`    |
      | SRL                  | `SRL rd, rs2, rs1`    |
      | SUB                  | `SUB rd, rs2, rs1`    |
      | SRA                  | `SRA rd, rs2, rs1`    |
      | MUL                  | `MUL rd, rs2, rs1`    |

      Immediate-type (I-type) instructions:
      | Instruction           | Syntax               |
      |-----------------------|----------------------|
      | ADDI                  | `ADDI rd, rs1, imm`  |
      | SLTI                  | `SLTI rd, rs1, imm`  |
      | SLTU                  | `SLTU rd, rs1, imm`  |
      | ANDI                  | `ANDI rd, rs1, imm`  |
      | ORI                   | `ORI rd, rs1, imm`   |
      | XORI                  | `XORI rd, rs1, imm`  |
      | SLLI                  | `SLLI rd, rs1, imm`  |
      | SRLI                  | `SRLI rd, rs1, imm`  |
      | SRAI                  | `SRAI rd, rs1, imm`  |
    
      Load-type (L-type) instructions:
      | Instruction | Syntax        |
      |-------------|---------------|
      | LW          | `LW rd, rs1`  |
      | LD          | `LD rd, rs1`  |
      | LH          | `LH rd, rs1`  |
      | LB          | `LB rd, rs1`  |
      | LWU         | `LWU rd, rs1` |
      | LHU         | `LHU rd, rs1` |
      | LBU         | `LBU rd, rs1` |

      Store-type (S-type) instructions:
      | Instruction | Syntax              |
      |-------------|---------------------|
      | SD          | `SD rs2, rs1, imm`  |
      | SW          | `SW rs2, rs1, imm`  |
      | SHW         | `SHW rs2, rs1, imm` |
      | SB          | `SB rs2, rs1, imm`  |

      Branch-type (B-type) instructions:
      | Instruction | Syntax                  |
      |-------------|-------------------------|
      | BEQ          | `BEQ rs1, rs2, Label`  |
      | BNE          | `BNE rs1, rs2, Label`  |
      | BLT          | `BLT rs1, rs2, Label`  |
      | BGT          | `BGT rs1, rs2, Label`  |
      | BLTU         | `BLTU rs1, rs2, Label` |
      | BGE          | `BGE rs1, rs2, Label`  |

      Jump-type (J-type) instructions:
      | Instruction   | Syntax                  |
      |---------------|-------------------------|
      | JAL           | `JAL rd, Label`         |
      | JALR          | `JALR rd, rs1, imm`     |
      | J             | `J Label`               |
      | JR            | `JR rs1`                |

      NOP Instruction : `NOP`
    
  - Label Support: Handles labels and branches, calculating the correct offsets.

  - File Input/Output: Reads assembly instructions from an input file and writes the corresponding binary instructions to an output file, and also make an 

  - Error Handling: Reports errors for unsupported instructions, register out-of-range, immediate value out-of-range, and more.

  - Commenting with `#`:
    ```
    # This is a comment instruction
    instruction
    ```
## Pipeline
- Implemented a 5 tage pipeline with operand forwarding, to convert the binary instructions to an executable file
- 5 stages of the assembler are: Instruction Fetch, Instruction Decode, Instruction Execution, Memory Operation, Write back

## Program execution and Guidlines

  - Write your instruction set in a input file called "input.s" and save it in the same directory as "assembler.cpp"
  - Assembler ( assembler.cpp ) will write the binary encoded file of the instrucion set given ( in "input.txt" ) in "output.txt" file
  - save it in the same dir as pipeline_with_operand_forwarding.cpp file
  - Pipeline ( pipeline_with_operand_forwarding.cpp ) will take the binary encoded file ( output.txt ) and make an executable.
