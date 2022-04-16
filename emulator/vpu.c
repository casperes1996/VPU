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
        printf("\n");
        printf("PC:\t 0x%llx\n", vpu->PC);
        printf("SP:\t 0x%llx\n", vpu->SP);
        printf("BP:\t 0x%llx\n", vpu->BP);
        printf("FLAGS:\t %llu\n", vpu->FLAGS); // TODO:- Eventually brint flags as a set of nicely formatted textual meanings of enabled bits.
        printf("----------------------\n");
        printf("General Purpose Registers:\n");
        for(int i = 0; i < 16; i++) {
            printf("r%d:\t %llu\n", i, vpu->generalPurposeRegisters[i]);
        }
        printf("\n");
    }
}

void decodeAndDispatchInstruction() {
    Register PC =  vpu->PC;
    uint64_t instruction = readMemory(PC);
    uint8_t opcode = (instruction) & 0xFF;
    uint8_t operand1 = (instruction >> 8) & 0xFF;
    uint8_t operand2 = (instruction >> 16) & 0xFF;
    uint8_t operand3 = (instruction >> 24) & 0xFF;
    void (*alu_function)(uint8_t, uint8_t, uint8_t);
    switch (opcode) {
        case MOVE_REG:
            alu_function = alu_moveReg;
            break;
        case MOVE_IMM:
            alu_function = alu_moveImm;
            break;
        case JUMP:
            alu_function = alu_jump;
            break;
        case COMP_REG:
            alu_function = alu_compReg;
            break;
        case COMP_IMM:
            alu_function = alu_compImm;
            break;
        case LOAD:
            alu_function = alu_load;
            break;
        case SAVE:
            alu_function = alu_save;
            break;
        case PLUS_REG:
            alu_function = alu_plusReg;
            break;
        case PLUS_IMM:
            alu_function = alu_plusImm;
            break;
        case MULT_REG:
            alu_function = alu_multReg;
            break;
        case MULT_IMM:
            alu_function = alu_multImm;
            break;
        case DIVI_REG:
            alu_function = alu_diviReg;
            break;
        case DIVI_IMM:
            alu_function = alu_diviImm;
            break;
        case SUBT_REG:
            alu_function = alu_subtReg;
            break;
        case SUBT_IMM:
            alu_function = alu_subtImm;
            break;
        case SHFT:
            alu_function = alu_shft;
            break;
        case BAND_REG:
            alu_function = alu_bandReg;
            break;
        case BAND_IMM:
            alu_function = alu_bandImm;
            break;
        case BXOR_REG:
            alu_function = alu_bxorReg;
            break;
        case BXOR_IMM:
            alu_function = alu_bxorImm;
            break;
        case BORR_REG:
            alu_function = alu_borrReg;
            break;
        case BORR_IMM:
            alu_function = alu_borrImm;
            break;
        case BNOT:
            alu_function = alu_bnot;
            break;
        case NOOP:
            alu_function = alu_noop;
            break;
        default:
            printf("Unimplemented opcode: %d\n", opcode);
            fprintf(stderr, "ENCOUNTERED UNSUPPORTED INSTRUCTION; CPU FAULT\n");
            exit(0); // CPU FAULTED EXITING
            break;
    }
    alu_function(operand1, operand2, operand3);
    vpu->PC+=INSTRUCTION_WIDTH;
}

Register* findRegister(uint8_t registerNumber) {
    if(registerNumber < 16) {
        return &vpu->generalPurposeRegisters[registerNumber];
    }
    else if(registerNumber == PC_NUM) {
        return &vpu->PC;
    }
    else if(registerNumber == SP_NUM) {
        return &vpu->SP;
    }
    else if(registerNumber == BP_NUM) {
        return &vpu->BP;
    }
    else if(registerNumber == FLAGS_NUM) {
        return &vpu->FLAGS;
    }
    else {
        printf("Unimplemented register number: %d\n", registerNumber);
        fprintf(stderr, "ENCOUNTERED UNSUPPORTED REGISTER NUMBER; CPU FAULT\n");
        exit(0); // CPU FAULTED EXITING
    }
}