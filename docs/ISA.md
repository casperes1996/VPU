# The VPU ISA

VPU uses a 4 byte, fixed-length instruction encoding. For now I can only think of a need for 3 bytes but I'm giving myself a fourth buffer byte in case I find out later I need it. I'll use it for instructions that could be represented otherwise where I feel it makes sense as well

The VPU is 64-bit; All registers are 64-bit wide. Registers are numbered 0-255. 0-15 are the only general purpose registers available in VPU 1.0. Special registers like PC will start at 255 and get closer to 0 as more become necessary. We start with PC at 255, SP at 254, BP at 253 and FLAGS at 252.
All operations in this repository will follow the "from to" dialect of assembly. In other words, "MOVE r0 r1" means move the value in register 0 to register 1. 
Constant values can only be 1 byte long in assembly - If wider constants are desired, the assembler program could implement the logic necessary to achieve this transparently though this will hide the true instruction cost from the programmer; In other words, if x is a 2 byte value
"MOVE $x r0" would first move the high order byte of x to r0, then "SHFT $8 r0 $0" to move the value in r0 8 bits left, and then OR the low order bits of x with r0 to move in the remaining bits. 

In the following table (r) refers to a numbered register, like r0. Special tokens are allowed for special registers like PC for the program counter, SP for the stack pointer and FLAGS for flags.
$c means a byte-wide constant value.
In the encoding column, "r" simply means the numerical value of the register. So MOVE r0 r1 would encode to 00 00 01 00. (MOVE = 00. r0 = 00. r1 = 01. ARG3 = N/A = 00)

### General Purpose Registers:

[r0;r15] Integer general purpose registers. 64-bit wide

### Special Registers

PC, SP, BP, FLAGS - All 64-bit wide.

FLAGS meaning:
-------------------------------------------------------------------------------------------------------------
BIT NUMBER      NAME                                            DESCRIPTION
[0]             COMPARE                                         Set to 1 as a result of some COMP instructions and can be used for conditional behaviour in JUMPs.
[1;63]          RESERVED                                        Currently reserved and meaningless, will potentially add semantics later. 

### Table of instructions
-------------------------------------------------------------------------------------------------------------
INST    ARG1    ARG2    ARG3        ENCODING                    DESCRIPTION
MOVE    (r)     (r)     N/A         00 r  r  00                 Moves the value of the first register into the second (as a copy)
MOVE    $c      (r)     N/A         01 c  r  00                 Moves the constant value c into the register
JUMP    $c      (r)     N/A         02 c  r  00                 Jumps to the address specified by ARG2. ARG1 is a boolean value (0 or anything else) and if true, the jump will only happen if the COMPARE FLAG is set
COMP    (r)     (r)     $c          03 r  r  c                  Compare two registers. COMPARE FLAG is set to true if, for ARG3 = 0: ARG1 == ARG2. For ARG3 = 1: ARG1 > ARG2. For ARG3 = 2 ARG1 < ARG2. Otherwise explicitly sets the bit to 0
COMP    $c      (r)     $c          04 c  c  c                  Similar to above comparison instruction, but against a constant in ARG1 instead of a register. 
LOAD    (r)     (r)     N/A         05 r  r  00                 Loads what is stored at the memory address of ARG1 into the register specified by ARG2. 
SAVE    (r)     (r)     N/A         06 r  r  00                 Store what is specified by ARG2 to memory address referenced by ARG1
PLUS    (r)     (r)     N/A         07 r  r  00                 Add the valuee in ARG1 to the value in ARG2 and save the result in ARG2
PLUS    $c      (r)     N/A         08 c  r  00                 Same as above but with a constant for ARG1
MULT    (r)     (r)     N/A         09 r  r  00                 Multiply ARG1 with ARG2 and save the result in ARG2
MULT    $c      (r)     N/A         0a c  r  00                 Multiply constant in ARG1 with ARG2 and save result in ARG2
DIVI    (r)     (r)     N/A         0b r  r  00                 Divide ARG2 with ARG1 result saved in ARG2
DIVI    $c      (r)     N/A         0c c  r  00                 Divide ARG2 with ARG1 and save result in ARG2. Can be extended to use ARG3 to specify reverse order division, so it would be ARG2/ARG1 if necessary for efficiency
SUBT    (r)     (r)     N/A         0d r  r  00                 Subtract ARG1 from ARG2 and save result in ARG2
SUBT    $c      (r)     N/A         0e c  r  00                 Subtract ARG1 from ARG2 and save result in ARG2. Semantics ARG2=ARG1-ARG2. See divi for potential extension
SHFT    $c      (r)     $c          0f c  r  c                  Shift ARG2 ARG1 bits. If ARG3 = 0 the shift is leftwards. If ARG3 != 0 the shift is rightward.
BAND    (r)     (r)     N/A         10 r  r  00                 Bitwise AND of two registers, result saved in ARG2
BAND    $c      (r)     N/A         11 r  r  00                 Bitwise AND of a constant and a register, result saved in ARG2
BXOR    (r)     (r)     N/A         12 r  r  00                 Bitwise XOR of two registers, result saved in ARG2
BXOR    $c      (r)     N/A         13 c  r  00                 Bitwise XOR of a constant and a register, result saved in ARG2
BORR    (r)     (r)     N/A         14 r  r  00                 Bitwise OR of two registers, result saved in ARG2
BORR    $c      (r)     N/A         15 c  r  00                 Bitwise OR of a constant and a register, result saved in ARG2
BNOT    (r)     N/A     N/A         16 r  r  00                 Negate the value in ARG1 (all bits negated) and store the result in ARG1
NOOP    N/A     N/A     N/A         ff 00 00 00                 The no-op. Moves PC to the next instruction doing nothing else


----- THE JUMP SHORTHAND -----
The JUMP instruction can be a bit heavy to write out with pure addressing.
To allow easier assembly labels are allowed in the form
JUMP $c (r) LABEL. This is similar to the encoded JUMP but instead of jumping to the register specified as ARG2, it jumps to the label specified by ARG3,
using the register in ARG2 for the address - In other words you are choosing a register to give up for the assembler to create the JUMP.
Be aware that this expands to a lot more than 1 instructions, breaking the traditional 1:1 mapping of writing assembly.
Compilers may often be advised to not use this and instead MOVE PC to a register and compute the offset followed by an actual JUMP encoding


## If new instructions are added, remember to update VALID_INSTRUCTIONS in vpu-bin-as.h, assembler code generation and emulator runtime