#include "alu.h"
#include "mmu.h"
#include "vpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void alu_moveReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 = *reg1;
}

void alu_moveImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg = operand1;
}

void alu_jump(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    uint8_t conditional = operand1;
    Register* reg = findRegister(operand2);
    bool shouldJump = conditional == 0 ? true : (vpu->FLAGS & FLAG_COMPARE); // JUMP if condition is 0 or 1 && FLAG_COMPARE is set.
    if(shouldJump) {
        vpu->PC = *reg; // Normally this would be enough, but when returning from thi, PC gets incremented by 4, so:
        vpu->PC -= 4; // PC = PC - 4
    }
}

static void alu_compShared(Register* operand1, Register* reg2, uint8_t operand3) {
    if(operand3 == 0) {
        vpu->FLAGS |= *operand1 == *reg2 ? FLAG_COMPARE : 0;
    } else if(operand3 == 1) {
        vpu->FLAGS |= *operand1 > *reg2 ? FLAG_COMPARE : 0;
    } else if(operand3 == 2) {
        vpu->FLAGS |= *operand1 < *reg2 ? FLAG_COMPARE : 0;
    } else {
        fprintf(stderr, "Invalid operand3 value %hhu for COMP instruction.\n", operand3);
    }
}

void alu_compReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2); // cast to make it match shared function signature.
    alu_compShared(reg1, reg2, operand3);
}

void alu_compImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    uint64_t imm = (uint64_t) operand1;
    alu_compShared(&imm, reg, operand3); 
}

void alu_load(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 = readMemory(*reg1);
}

void alu_save(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    writeMemory(*reg1, *reg2);
}

void alu_plusReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 += *reg1;
}

void alu_plusImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg += operand1;
}

void alu_multReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 *= *reg1;
}

void alu_multImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg *= operand1;
}

void alu_diviReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 /= *reg1;
}

void alu_diviImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg /= operand1;
}

void alu_subtReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 -= *reg1;
}

void alu_subtImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg -= operand1;
}

void alu_shft(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    if(operand3 == 0) {
        *reg <<= operand1;
    } else {
        *reg >>= operand1;
    }
}

void alu_bandReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 &= *reg1;
}

void alu_bandImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg &= operand1;
}

void alu_bxorReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 ^= *reg1;
}

void alu_bxorImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg ^= operand1;
}

void alu_borrReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 |= *reg1;
}

void alu_borrImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg |= operand1;
}

void alu_bnot(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand1);
    *reg = ~*reg;
}

void alu_noop(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    // Do nothing :^)
    return;
}
