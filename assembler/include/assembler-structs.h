// only include once
#ifndef ASSEMBLER_STRUCTS_H
#define ASSEMBLER_STRUCTS_H
#include <sys/types.h>
#include <stdint.h>
#include "dynamicArray.h"

// In addition to instructions we also encode Labels as Instruction structs. In that case the opName will be LABL and operand1 will be the label name.
// This will only occur in the normalisation pass however, initially words will just be written in order - so the label name will be in word 1
typedef struct Instruction_st {
    char* opName;
    char* operand1;
    char* operand2;
    char* operand3;
} Instruction;

typedef struct Label_st {
    char* name;
    uint64_t address;
} Label;

typedef struct FileData_st {
    char* data;
    size_t size;
} FileData;

declareDynamicArray(Instruction*, InstructionPtr);

defineDynamicArray(Instruction*, InstructionPtr);

declareDynamicArray(Label*, LabelPtr);

defineDynamicArray(Label*, LabelPtr);


#endif //ASSEMBLER_STRUCTS_H