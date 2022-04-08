// Only include if not already included
#ifndef VPU_BIN_AS_H
#define VPU_BIN_AS_H

// In addition to instructions we also encode Labels as Instruction structs. In that case the opName will be LABL and operand1 will be the label name.
typedef struct Instruction_st {
    char* opName;
    char* operand1;
    char* operand2;
    char* operand3;
} Instruction;

typedef struct FileData_st {
    char* data;
    size_t size;
} FileData;

Instruction** parseInstructions(FileData fileData, size_t* numInstructions);

FileData readInFile(char* fileName);

#endif