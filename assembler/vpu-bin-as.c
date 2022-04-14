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

// Note currently no memory is freed anywhere. we expect the program to only take very little memory and run for such a short amount of time
// it all quickly gets freed again anyway. This is not considered a problem for now, we are just hacking it together to work for now.


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
    free(fileData.data);
    debug("Pre-normalisation tokens:\n");
    printInstructions(tokens); // debug only
    normaliseInstructionList(tokens);
    printInstructions(tokens); // debug only
    DynamicArray_uint8_t* binaryInstructionStream = createBinaryInstructionStream(tokens);
    // argv[1] file with .bin extension
    char* fileName = malloc(strlen(argv[1]) + 5);
    strcpy(fileName, argv[1]);
    strcat(fileName, ".bin");
    writeBinaryStreamToFile(binaryInstructionStream->array, binaryInstructionStream->used, fileName);
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


void writeOperandsToBinaryStream(DynamicArray_uint8_t* binaryStream, Instruction* instruction) {
    if(instruction->operand1 != NULL) {
        if(instruction->operand1[0] == '$') { // A constant value moved to a register
            insertArray_uint8_t(binaryStream, (uint8_t) strtol(instruction->operand1+1, NULL, 0)); // Gets the value of the constant. With base 0 either base 10, hex or octal can work depending on input form
        } else { // A register argment
            insertArray_uint8_t(binaryStream, getValidRegisterNumber(instruction->operand1));
        }
    } else { // NULL
        insertArray_uint8_t(binaryStream,0x00);
    }
    if(instruction->operand2 != NULL) {
        if(instruction->operand2[0] == '$') { // A constant value moved to a register
            insertArray_uint8_t(binaryStream, (uint8_t) strtol(instruction->operand2+1, NULL, 0)); // Gets the value of the constant. With base 0 either base 10, hex or octal can work depending on input form
        } else { // A register argment
            insertArray_uint8_t(binaryStream, getValidRegisterNumber(instruction->operand2));
        }
    } else { // NULL
        insertArray_uint8_t(binaryStream, 0x00);
    }
    if(instruction->operand3 != NULL) {
        if(instruction->operand3[0] == '$') { // A constant value moved to a register
            insertArray_uint8_t(binaryStream, (uint8_t) strtol(instruction->operand3+1, NULL, 0)); // Gets the value of the constant. With base 0 either base 10, hex or octal can work depending on input form
        } else { // A register argment
            insertArray_uint8_t(binaryStream, getValidRegisterNumber(instruction->operand3));
        }
    } else { // NULL
        insertArray_uint8_t(binaryStream, 0x00);
    }
}


// bytesWritten will be = numberOfBytes in the returned array of bytes
DynamicArray_uint8_t* createBinaryInstructionStream(DynamicArray_InstructionPtr* instructions) {
    DynamicArray_uint8_t* binaryInstructionStream = initArray_uint8_t(INSTRUCTION_BYTE_WIDTH*instructions->used); // Initial size; Amount of instructions without JUMP reconstructions
    DynamicArray_LabelPtr* labels = initArray_LabelPtr(8); // Start with just enough space for 8 labels - We will use the labels list to not have to go through all instructions again when resolving a JUMP
    for(size_t i = 0; i < instructions->used; i++) { // Instruction parsing loop.
        char* opName = instructions->array[i]->opName; // The primary determining factor for encoding instruction.

        if(strcasecmp(opName, "ACMD") == 0) {
            // Assembler command. Not used in v1.0
            fprintf(stderr, "Error: Assembler command encountered. This is not supported in v1.0\n");
            continue;
        } 
        else if(strcasecmp(opName, "LABL") == 0) { // Add to our list of labels for future lookups
            Label* label = calloc(1, sizeof(Label));
            label->name = strdup(instructions->array[i]->operand1);
            label->address = binaryInstructionStream->used; // offset from the start of the program
            insertArray_LabelPtr(labels, label);
            continue;
        }
        else if(strcasecmp(opName, "MOVE") == 0) { // Move instructions - figure out if constant or register type
            if(instructions->array[i]->operand1[0] == '$') { // A constant value moved to a register
                insertArray_uint8_t(binaryInstructionStream, MOVE_IMM_OPCDE); // Set the opcode to MOVE with Constant
            } else { // Register to Register
                insertArray_uint8_t(binaryInstructionStream, MOVE_REG_OPCDE); // Set the opcode to MOVE with Register
            }
        }
        else if(strcasecmp(opName, "JUMP") == 0) {
            Label* potentialLabel = findLabelIfExistsForJump(instructions->array[i], labels);
            
            if(potentialLabel != NULL) {
                // This wil insert instructions such that the label address is placed in the second operand register. After this the form should be a normal JUMP.
                rewriteLabelJump(binaryInstructionStream, getValidRegisterNumber(instructions->array[i]->operand2), potentialLabel);
                instructions->array[i]->operand3 = NULL; // We don't need to store the label name in the instruction anymore
            }
            insertArray_uint8_t(binaryInstructionStream, JUMP_OPCODE);
        }
        else if(strcasecmp(opName, "PLUS") == 0) { // Add instructions - figure out if constant or register type
            if(instructions->array[i]->operand1[0] == '$') { // A constant value added to a register
                insertArray_uint8_t(binaryInstructionStream, PLUS_IMM_OPCODE); // Set the opcode to PLUS with Constant
            } else {
                insertArray_uint8_t(binaryInstructionStream, PLUS_REG_OPCODE);
            }
        }
        else if(strcasecmp(opName, "SUBT") == 0) { // Subtract instructions - figure out if constant or register type
            if(instructions->array[i]->operand1[0] == '$') { // A constant value subtracted from a register
                insertArray_uint8_t(binaryInstructionStream, SUBT_IMM_OPCODE);
            } else {
                insertArray_uint8_t(binaryInstructionStream, SUBT_REG_OPCODE);
            }
        }
        else if(strcasecmp(opName, "MULT") == 0) { // Multiply instructions - figure out if constant or register type
            if(instructions->array[i]->operand1[0] == '$') { // A constant value multiplied by a register
                insertArray_uint8_t(binaryInstructionStream, MULT_IMM_OPCODE); // Set the opcode to MULT with Constant
            } else {
                insertArray_uint8_t(binaryInstructionStream, MULT_REG_OPCODE);
            }
        }
        else if(strcasecmp(opName, "NOOP") == 0) {
            insertArray_uint8_t(binaryInstructionStream, NOOP_OPCODE);
        } 
        else {
            fprintf(stderr, "Encountered unrecognised instructions: %s\n", opName);
            exit(-1);
        } // Done finding opCode

        // MARK: - From here we only set operands. Same code can be used for all instructions.
        writeOperandsToBinaryStream(binaryInstructionStream, instructions->array[i]);
    } // end of for loop

    return binaryInstructionStream;
}


Label* findLabelIfExistsForJump(Instruction* instruction, DynamicArray_LabelPtr* labels) {
    if(instruction->operand3 != NULL) { // WE HAVE A LABEL. Use shorthand (see docs/ISA.md)
        // Find the label in our list of labels
        Label* label = NULL;
        for(size_t j = 0; j < labels->used; j++) {
            if(strcasecmp(labels->array[j]->name, instruction->operand3) == 0) {
                label = labels->array[j];
                break;
            }
        }
        if(label == NULL) {
            fprintf(stderr, "Error: Label %s not found\n", instruction->operand3);
            exit(4);
        }
        return label;
    }
    return NULL;
}


#define byteMask 0xFF
void rewriteLabelJump(DynamicArray_uint8_t* binaryStream, uint8_t reg, Label* label) {
    uint64_t labelAddress = label->address;

    // We begin by BXORing out the register to get a clean 0 starting point to fill in the address
    insertArray_uint8_t(binaryStream, BXOR_REG_OPCODE);
    insertArray_uint8_t(binaryStream, reg);
    insertArray_uint8_t(binaryStream, reg);
    
    // We now add the individual bytes of the address and shift them in place 
    for(int i = 0; i<8; i++) {
        insertArray_uint8_t(binaryStream, SHFT_OPCODE); //shift
        insertArray_uint8_t(binaryStream, 0x08); // 8 bytes
        insertArray_uint8_t(binaryStream, reg); // from register
        insertArray_uint8_t(binaryStream, 0x00); // left
        insertArray_uint8_t(binaryStream, BORR_IMM_OPCODE); // ORR add byte
        uint8_t byteToAddFromAddress = ((labelAddress >> (i*8)) & byteMask); // From address
        insertArray_uint8_t(binaryStream, reg); // to register
        insertArray_uint8_t(binaryStream, 0x00); // required for fixed-width encoding
    }
}


void writeBinaryStreamToFile(uint8_t* binaryStream, uint64_t numberOfBytes, char* fileName) {
    FILE* file = fopen(fileName, "wb");
    if(file == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", fileName);
        exit(-1);
    }
    fwrite(binaryStream, sizeof(uint8_t), numberOfBytes, file);
    fclose(file);
}


void printInstructions(DynamicArray_InstructionPtr* instructions) {
    for(size_t i = 0; i < instructions->used; i++) {
        Instruction* instruction = instructions->array[i];
        debug("%s\t%s\t%s\t%s\n", instruction->opName, instruction->operand1, instruction->operand2, instruction->operand3);
    }
    debug("\n");
}