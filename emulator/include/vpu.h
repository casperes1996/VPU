// only include once
#ifndef EMULATOR_VPU_H
#define EMULATOR_VPU_H
#include <stdint.h>
typedef uint64_t Register;
#define INSTRUCTION_WIDTH 4
#include "mmu.h"
#include "alu.h"
#include "dynamicArray.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>


typedef enum OPCODES {
    MOVE_REG = 0x00,
    MOVE_IMM = 0x01,
    JUMP     = 0x02,
    COMP_REG = 0x03,
    COMP_IMM = 0x04,
    LOAD     = 0x05,
    SAVE     = 0x06,
    PLUS_REG = 0x07,
    PLUS_IMM = 0x08,
    MULT_REG = 0x09,
    MULT_IMM = 0x0A,
    DIVI_REG = 0x0B,
    DIVI_IMM = 0x0C,
    SUBT_REG = 0x0D,
    SUBT_IMM = 0x0E,
    SHFT     = 0x0F,
    BAND_REG = 0x10,
    BAND_IMM = 0x11,
    BXOR_REG = 0x12,
    BXOR_IMM = 0x13,
    BORR_REG = 0x14,
    BORR_IMM = 0x15,
    BNOT     = 0x16,
    NOOP     = 0xFF,
} OpCodes;

typedef struct VPU_st {
    Register generalPurposeRegisters[16]; // r0-r15
    Register PC;
    Register SP;
    Register BP;
    Register FLAGS;
} VPU;

#define PC_NUM      255
#define SP_NUM      254
#define BP_NUM      253
#define FLAGS_NUM   252

// we have a global vpu instance initialised by initVPU.
VPU* vpu;

uint8_t verbosity;

// Takes memsize because it also initialises the MMU - See mmu.h for the global.
void initVPU(uint64_t memsize, uint8_t set_verbosity);

void decodeAndDispatchInstruction();

Register* findRegister(uint8_t registerNumber);

// The verbosity level specifies how high high the verbosity must be to print the state at this line. I.e. 0 always prints, higher only prints if verbosity is at least as high.
void printState(uint8_t verbosityReq);

#endif //EMULATOR_VPU_H

