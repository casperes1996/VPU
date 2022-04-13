#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "vpu-bin-as.h"
#include "dynamicArray.h"

static bool debug = false;

#define debug(fmt, ...) \
    if (debug) fprintf(stdout, fmt, ##__VA_ARGS__);

#define debugerr(fmt, ...) \
    if (debug) fprintf(stderr, fmt, ##__VA_ARGS__);


int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    } if(argc >= 3) {
        if(strcasecmp(argv[2], "-v") == 0) { //enable verbosity and debug
            debug = true;
        }
    }
    FileData fileData = readInFile(argv[1]);

    DynamicArray_InstructionPtr* tokens = tokeniseInstructions(fileData);
    debug("Pre-normalisation tokens:\n");
    printInstructions(tokens); // debug only
    normaliseInstructionList(tokens);
    printInstructions(tokens); // debug only
    free(fileData.data);
    return 0;
}


FileData readInFile(char* fileName) {
    FILE* f = fopen(fileName, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open file %s\n", fileName);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = malloc(size);
    if (!data) {
        fprintf(stderr, "Error: Could not allocate %zu bytes\n", size);
        exit(1);
    }
    if (fread(data, 1, size, f) != size) {
        fprintf(stderr, "Error: Could not read %zu bytes\n", size);
        exit(1);
    }
    fclose(f);
    return (FileData) {
        .data = data,
        .size = size
    };
}


DynamicArray_InstructionPtr* tokeniseInstructions(FileData fileData) {
    char* duplicateOfFile = strdup(fileData.data);
    char** strsepMovingPointer = &duplicateOfFile;
    char* line = NULL;
    // Reserve enough space for 4096 instructions initially - This should be 32KiB of space and enough for a lot of programs to be assembled without needing to re-alloc
    DynamicArray_InstructionPtr* instructions = initArray_InstructionPtr(4096);
    int lineNumber = 0;
    while ((line = strsep(strsepMovingPointer, "\n")) != NULL) {
        if (line[0] == '\n') { // strsep only moves one step at a time even if there's multiply newline characters, so we filter an extra step.
            continue;
        }
        Instruction* instructionBuilder = (Instruction*) calloc(1,sizeof(Instruction));
        char** strsepMovingPointer2 = &line;
        char* token = NULL;
        while ((token = strsep(strsepMovingPointer2, " \t")) != NULL) {
            if (*token == ' ' || *token == '\t' || *token == '\0') {
                continue;
            } 
            if (*token == '#' || *token == ';') { // Comment
                break;
            }
            if(instructionBuilder->opName == NULL) {
                instructionBuilder->opName = strdup(token);
            } else if (instructionBuilder->operand1 == NULL) {
                instructionBuilder->operand1 = strdup(token);
            } else if (instructionBuilder->operand2 == NULL) {
                instructionBuilder->operand2 = strdup(token);
            } else if (instructionBuilder->operand3 == NULL) {
                instructionBuilder->operand3 = strdup(token);
            } else {
                fprintf(stderr, "Error: Too many operands on line %d\n", lineNumber);
                exit(1);
            }
            lineNumber += 1;
        }
        insertArray_InstructionPtr(instructions, instructionBuilder);
    }

    free(duplicateOfFile);
    return instructions;
}


void normaliseInstructionList(DynamicArray_InstructionPtr* instructions) {
    for(size_t i = 0; i < instructions->used; i++) {
        if(!isValidInstruction(instructions->array[i])) {
            // The line is not a valid instruction. We can either transform it to a label or an assembler command. If neither seem relevant, fail and exit.
            char* opName = instructions->array[i]->opName;
            if(*opName == '.') {
                if(opName[1] != '\0') {
                    instructions->array[i]->operand3 = instructions->array[i]->operand2;
                    instructions->array[i]->operand2 = instructions->array[i]->operand1;
                    instructions->array[i]->operand1 = strdup(opName + 1);
                    instructions->array[i]->opName = "ACMD"; //Assembler command. Not used in v1.0
                } else {
                    fprintf(stderr, "Error: Invalid assembler command. May not have . followed by nothing\n");
                    exit(2);
                }
            }
            size_t lastCharIndex = strlen(opName) - 1;
            if(opName[lastCharIndex] == ':') {
                opName[lastCharIndex] = '\0';
                if(opName[0] == '\0') {
                    fprintf(stderr, "Error: Invalid label. May not have a label with no name\n");
                    exit(3);
                }
                instructions->array[i]->operand1 = strdup(opName);
                instructions->array[i]->opName = "LABL"; //Label
            }
        }
    }
}


// List of all valid instructions exists in vpu-bin-as.h
bool isValidInstruction(Instruction* instruction) {
    if (instruction->opName == NULL) {
        fprintf(stderr, "Encountered unexpected state - somehow stored a NULL instruction when parsing and tokenising the file\n");
        exit(-1);
    }
    for(uint8_t i = 0; i < NUMBER_OF_INSTRUCTIONS; i++) {
        if(strcasecmp(instruction->opName, VALID_INSTRUCTIONS[i]) == 0) {
            return true;
        }
    }
    return false;
}


uint8_t* createBinaryInstructionStream(DynamicArray_InstructionPtr* instructions) {
    uint8_t* binaryInstructionStream = calloc(INSTRUCTION_BYTE_WIDTH*instructions->used, sizeof(uint8_t)); // Amount of instructions times the width of each instruction in bytes
    uint64_t numberOfBytes = 0;
    DynamicArray_LabelPtr* labels = initArray_LabelPtr(8); // Start with just enough space for 8 labels - We will use the labels list to not have to go through all instructions again when resolving a JUMP
    for(size_t i = 0; i < instructions->used; i++) {
        char* opName = instructions->array[i]->opName; // The primary determining factor for encoding instruction.
        if(strcasecmp(opName, "ACMD") == 0) {
            // Assembler command. Not used in v1.0
            fprintf(stderr, "Error: Assembler command encountered. This is not supported in v1.0\n");
            exit(4);
        } else if(strcasecmp(opName, "LABL") == 0) {
            Label* label = calloc(1, sizeof(Label));
            label->name = strdup(instructions->array[i]->operand1);
            label->address = numberOfBytes; // offset from the start of the program
            insertArray_LabelPtr(labels, label);
        }
    }
    return binaryInstructionStream;
}


void printInstructions(DynamicArray_InstructionPtr* instructions) {
    for(size_t i = 0; i < instructions->used; i++) {
        Instruction* instruction = instructions->array[i];
        debug("%s\t%s\t%s\t%s\n", instruction->opName, instruction->operand1, instruction->operand2, instruction->operand3);
    }
    debug("\n");
}