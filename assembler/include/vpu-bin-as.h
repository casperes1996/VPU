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

// Tiny utility functions placed in header instead of c file
bool isRegister(char* token) {
    if((strncasecmp(token,"R", 1) == 0 && token[1] >= '0') || (strcasecmp(token,"PC") == 0) || (strcasecmp(token,"SP") == 0) || (strcasecmp(token,"BP") == 0) || (strcasecmp(token,"FLAGS") == 0)) {
        return true;
    }
    return false;
}

uint8_t getValidRegisterNumber(char* token) {
    if(strncasecmp(token, "R", 1) == 0) {
        return (uint8_t) atoi(token + 1);
    } else if(strcasecmp(token, "PC") == 0) {
        return 255;
    } else if(strcasecmp(token, "SP") == 0) {
        return 254;
    } else if(strcasecmp(token, "BP") == 0) {
        return 253;
    } else if(strcasecmp(token, "FLAGS") == 0) {
        return 252;
    } else {
        fprintf(stderr, "Error: Invalid register name %s\n", token);
        exit(1);
    }
}

void printInstructions(DynamicArray_InstructionPtr* instructions);

uint8_t* createBinaryInstructionStream(DynamicArray_InstructionPtr* instructions, uint64_t* bytesWritten);

void writeBinaryStreamToFile(uint8_t* binaryStream, uint64_t bytesWritten, char* fileName);

void writeOperandsToBinaryStream(uint8_t* binaryStream,uint64_t* offset, Instruction* instruction);

#define INSTRUCTION_BYTE_WIDTH 4 // All instructions are 4 bytes wide

#endif