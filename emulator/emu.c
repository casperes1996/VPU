#include <stdio.h>
#include "dynamicArray.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <strings.h>
#include <stdbool.h>
#include "emu.h"
#include "vpu.h"
#include "mmu.h"
#include "alu.h"

bool endAtImageOvershoot = true;

int main(int argc, char** argv) {
    if(argc < 2) {
        printUsage();
        exit(1);
    }
    uint8_t toSetVerbosity = 0;
    uint64_t toSetMemSize = 256;
    parseArgs(argc, argv, &toSetVerbosity, &toSetMemSize, &endAtImageOvershoot);
    char* filename = argv[argc-1];
    DynamicArray_uint8_t* binaryImage = readInBinFile(filename);
    if(toSetVerbosity >= 1) {
        printf("Starting emulator with verbosity level %d and memory size %llu\n\
                Running file %s", toSetVerbosity, toSetMemSize, filename);
    }
    // Initialise the global VPU which in turn inits the MMU.
    initVPU(toSetMemSize, toSetVerbosity);
    atexit(exitPrint);
    signal(SIGINT, signalHandler);
    runEmulator(binaryImage);
}

void runEmulator(DynamicArray_uint8_t* binaryImage) {
    loadBinToVPUStartingMemory(binaryImage);
    if(endAtImageOvershoot) {
        while(vpu->PC <= binaryImage->used-INSTRUCTION_WIDTH) {
            decodeAndDispatchInstruction();
        }
    } else {
        while(true) {
            decodeAndDispatchInstruction();
        }
    }
}

// Expected usage: ./emu <file> [options], where options include -v # for verbosity level, -m # for memory size. Defaults are verbosity level 0 and memory size of 256MB.
void printUsage() {
    printf("Usage: ./emu [options] <file>\n");
    printf("Options:\n");
    printf("\t-v #\t\tVerbosity level. 0 is default.\n");
    printf("\t-m #\t\tMemory size in MB. Default is 256MB.\n");
    printf("\t-h\t\tPrint this help message.\n");
    printf("\t-e\t\tDisable End at image overshoot. Default is to run until PC reaches the end of the image, with no allowance to jump to instructions outside the initially loaded image. Good for debugging and running self-contained bins.\n");
}

// Expected usage: ./emu <file> [options], where options include -v # for verbosity level, -m # for memory size. Defaults are verbosity level 0 and memory size of 256MB.
void parseArgs(int argc, char** argv, uint8_t* toSetVerbosity, uint64_t* toSetMemsize, bool* endAtImageOvershoot) {
    int opt;
    while ((opt = getopt(argc, argv, "v:m:he")) != -1) {
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
                break;
            case 'e':
                *endAtImageOvershoot = false;
                break;
            default:
                printUsage();
                exit(1);
        }
        // update argc and argv
        argc -= optind;
        argv += optind;
    }
}

DynamicArray_uint8_t* readInBinFile(char* fileName) {
    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Error opening file\n");
        printUsage();
        exit(1);
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

void loadBinToVPUStartingMemory(DynamicArray_uint8_t* bin) {
    for(uint64_t i = 0; i < bin->used; i++) {
        mmu->memory[i] = bin->array[i];
    }
}

void exitPrint() {
    printf("VPU state at exit:\n");
    printf("----------------------");
    printState(0);
}

void signalHandler(int sig) {
    exit(0);
}