// only include once
#ifndef EMULATOR_EMU_H
#define EMULATOR_EMU_H
#include <stdint.h>
#include "vpu.h"
#include "dynamicArray.h"

DynamicArray_uint8_t* readInBinFile(char* filename);

int main(int argc, char** argv);

void parseArgs(int argc, char** argv, uint8_t* toSetVerbosity, uint64_t* toSetMemsize, bool* endAtImageOvershoot);

void printUsage();

void runEmulator(DynamicArray_uint8_t* binary);

// The two following functions are used to print state at exit, whether forceful or intended
void exitPrint();

void loadBinToVPUStartingMemory(DynamicArray_uint8_t* bin);

void signalHandler(int signal);

#endif //EMULATOR_EMU_H