#include <stdio.h>
#include "dynamicArray.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <strings.h>
#include "emu.h"
#include "vpu.h"
#include "mmu.h"
#include "alu.h"

int main(int argc, char** argv) {
    if(argc < 2) {
        printUsage();
        exit(1);
    }
    char* filename = argv[1];
    DynamicArray_uint8_t* binaryImage = readInBinFile(filename);
    uint8_t toSetVerbosity = 0;
    uint64_t toSetMemSize = 256;
    parseArgs(argc, argv, &toSetVerbosity, &toSetMemSize);
    if(toSetVerbosity >= 1) {
        printf("Starting emulator with verbosity level %d and memory size %llu\n\
                Running file %s", toSetVerbosity, toSetMemSize, filename);
    }
    initVPU(toSetMemSize, toSetVerbosity);
    runEmulator(binaryImage);
}

void runEmulator(DynamicArray_uint8_t* binaryImage) {
    printState(0);
}

// Expected usage: ./emu <file> [options], where options include -v # for verbosity level, -m # for memory size. Defaults are verbosity level 0 and memory size of 256MB.
void printUsage() {
    printf("Usage: ./emu <file> [options]\n");
    printf("Options:\n");
    printf("\t-v #\t\tVerbosity level. 0 is default.\n");
    printf("\t-m #\t\tMemory size in MB. Default is 256MB.\n");
    printf("\t-h\t\tPrint this help message.\n");
}

// Expected usage: ./emu <file> [options], where options include -v # for verbosity level, -m # for memory size. Defaults are verbosity level 0 and memory size of 256MB.
void parseArgs(int argc, char** argv, uint8_t* toSetVerbosity, uint64_t* toSetMemsize) {
    int opt;
    while ((opt = getopt(argc, argv, "v:m:h")) != -1) {
        switch (opt) {
            case 'v':
                *toSetVerbosity = (uint8_t) atoi(optarg);
                break;
            case 'm':
                *toSetMemsize = atoll(optarg);
                break;
            case 'h':
                printUsage();
                exit(0);
            default:
                printUsage();
                exit(1);
        }
    }
}

DynamicArray_uint8_t* readInBinFile(char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    DynamicArray_uint8_t* fileContents = initArray_uint8_t(fileSize);
    fread(fileContents->array, sizeof(uint8_t), fileSize, file);
    fileContents->used = fileSize; // We overwrite the used property since we inserted into the array in a direct way, circumventing the normal insert method
    fclose(file);
    return fileContents;
}