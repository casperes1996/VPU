// Only include if not already included
#ifndef VPU_BIN_AS_H
#define VPU_BIN_AS_H
#include "dynamicArray.h"
#include "assembler-structs.h"
#include <stdbool.h>

DynamicArray_InstructionPtr* tokeniseInstructions(FileData fileData);

FileData readInFile(char* fileName);

void normaliseInstructionList(DynamicArray_InstructionPtr* instructions);  

bool isValidInstruction(Instruction* instruction);

#define NUMBER_OF_INSTRUCTIONS (sizeof(VALID_INSTRUCTIONS)/sizeof(VALID_INSTRUCTIONS[0]))

const char* const VALID_INSTRUCTIONS[] = {
    "MOVE",
    "JUMP",
    "COMP",
    "LOAD",
    "SAVE",
    "PLUS",
    "MULT",
    "DIVI",
    "SUBT",
    "SHFT",
    "BAND",
    "BXOR",
    "BORR",
    "BNOT",
    "NOOP"
};

void printInstructions(DynamicArray_InstructionPtr* instructions);

uint8_t* createBinaryInstructionStream(DynamicArray_InstructionPtr* instructions);

#define INSTRUCTION_BYTE_WIDTH 4 // All instructions are 4 bytes wide

#endif