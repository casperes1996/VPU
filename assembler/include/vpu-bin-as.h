// Only include if not already included
#ifndef VPU_BIN_AS_H
#define VPU_BIN_AS_H
#include "dynamicArray.h"
#include "assembler-structs.h"
#include <stdbool.h>

#define MOVE_REG_OPCDE  0x00
#define MOVE_IMM_OPCDE  0x01
#define JUMP_OPCODE     0x02
#define COMP_REG_OPCODE 0x03
#define COMP_IMM_OPCODE 0x04
#define LOAD_OPCODE     0x05
#define SAVE_OPCODE     0x06
#define PLUS_REG_OPCODE 0x07
#define PLUS_IMM_OPCODE 0x08
#define MULT_REG_OPCODE 0x09
#define MULT_IMM_OPCODE 0x0A
#define DIVI_REG_OPCODE 0x0B
#define DIVI_IMM_OPCODE 0x0C
#define SUBT_REG_OPCODE 0x0D
#define SUBT_IMM_OPCODE 0x0E
#define SHFT_OPCODE     0x0F
#define BAND_REG_OPCODE 0x10
#define BAND_IMM_OPCODE 0x11
#define BXOR_REG_OPCODE 0x12
#define BXOR_IMM_OPCODE 0x13
#define BORR_REG_OPCODE 0x14
#define BORR_IMM_OPCODE 0x15
#define BNOT_OPCODE     0x16
#define NOOP_OPCODE     0xFF

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

DynamicArray_uint8_t* createBinaryInstructionStream(DynamicArray_InstructionPtr* instructions);

void writeBinaryStreamToFile(uint8_t* binaryStream,uint64_t numberOfBytes, char* fileName);

void writeOperandsToBinaryStream(DynamicArray_uint8_t* binaryStream, Instruction* instruction);

#define INSTRUCTION_BYTE_WIDTH 4 // All instructions are 4 bytes wide

void rewriteLabelJump(DynamicArray_uint8_t* binaryStream, uint8_t reg, Label* label);

Label* findLabelIfExistsForJump(Instruction* instruction, DynamicArray_LabelPtr* labels);

#endif