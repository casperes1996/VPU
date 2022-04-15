# Assembler README:

It is strongly recommended that you begin by reading docs/ISA.md

The general idea and flow of the assembler is as follows:
• Read in the vasm file.
• Create a vector of instructions parsed as operation, operand1, operand2, operand3.
• "Normalise it". In other words, get labels and assembler directives in a nice form for later. The way we originally parse things would have the label in the opName field
We move this around so the opName is LABL and the operand1 is the name of the label. We do a similar move for assembler directives and clean out the dots and colons.
• Write an instruction stream of uint8_t bytes based on the opName and args as per the docs.
• Write the byte stream to file. Currently same name and path as input file with .bin appended.
