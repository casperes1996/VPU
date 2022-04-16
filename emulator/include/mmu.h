// only include once
#ifndef EMULATOR_MMU_H
#define EMULATOR_MMU_H
#include <stdint.h>
#include "vpu.h"

typedef struct MMU_st {
    uint8_t* memory;
    uint64_t memsize;
} MMU;

// The global mmu instance.
MMU* mmu;

void initMMU(uint64_t memsize);

uint64_t readMemory(Register address);

void writeMemory(Register address, Register value);

#endif //EMULATOR_MMU_H
