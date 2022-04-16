#include "vpu.h"
#include "mmu.h"
#include "alu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <strings.h>
#include <stdbool.h>

void initVPU(uint64_t memsize, uint8_t set_verbosity) {
    verbosity = set_verbosity;
    vpu = calloc(1,sizeof(VPU)); // Registers are in-line in the struct and calloc defaults to 0. All registers are thus 0 initially.
    initMMU(memsize);
}

void printState(uint8_t verbosityReq) {
    if(verbosityReq <= verbosity) {
        printf("\n\n");
        printf("PC:\t %llu\n", vpu->PC);
        printf("SP:\t %llu\n", vpu->SP);
        printf("BP:\t %llu\n", vpu->BP);
        printf("FLAGS:\t %llu\n", vpu->FLAGS);
        printf("\n");
        printf("General Purpose Registers:\n");
        for(int i = 0; i < 16; i++) {
            printf("r%d:\t %llu\n", i, vpu->generalPurposeRegisters[i]);
        }
        printf("\n\n");
    }
}