// only include once
#ifndef EMULATOR_EMU_H
#define EMULATOR_EMU_H
#include <stdint.h>
#include "vpu.h"
#include "dynamicArray.h"

DynamicArray_uint8_t* readInBinFile(char* filename);

int main(int argc, char** argv);

void parseArgs(int argc, char** argv, uint8_t* toSetVerbosity, uint64_t* toSetMemsize);

void printUsage();

void runEmulator(DynamicArray_uint8_t* binary);

#endif //EMULATOR_EMU_H