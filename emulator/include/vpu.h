// only include once
#ifndef EMULATOR_VPU_H
#define EMULATOR_VPU_H
#include "mmu.h"
#include "alu.h"
#include "dynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>

typedef uint64_t Register;

typedef struct VPU_st {
    Register generalPurposeRegisters[16]; // r0-r15
    Register PC;
    Register SP;
    Register BP;
    Register FLAGS;
} VPU;

// we have a global vpu instance initialised by initVPU.
VPU* vpu;

uint8_t verbosity;

// Takes memsize because it also initialises the MMU - See mmu.h for the global.
void initVPU(uint64_t memsize, uint8_t set_verbosity);

void decodeAndDispatchInstruction(uint8_t instructions[]);

// The verbosity level specifies how high high the verbosity must be to print the state at this line. I.e. 0 always prints, higher only prints if verbosity is at least as high.
void printState(uint8_t verbosityReq);

#endif //EMULATOR_VPU_H

