#include "mmu.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include <strings.h>
#include <stdbool.h>

void initMMU(uint64_t memsize) {
    memsize *= pow(2,20);
    mmu = calloc(1,sizeof(MMU));
    mmu->memory = calloc(memsize,sizeof(uint8_t));
    mmu->memsize = memsize;
}

uint64_t readMemory(Register address) {
    uint64_t value = 0;
    if(address < mmu->memsize) {
        uint8_t* addressToLoadFrom = mmu->memory+address;
        value = *((uint64_t*)(addressToLoadFrom));
    } else {
        fprintf(stderr, "TRYING TO READ MEMORY NOT IN MMU MAPPINGS %llx\n", address);
        exit(0);
    }
    return value;
}