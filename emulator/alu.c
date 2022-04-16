#include "alu.h"
#include "mmu.h"
#include "vpu.h"
#include <stdio.h>
#include <stdlib.h>

void alu_moveReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_moveImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg = findRegister(operand2);
    *reg = operand1;
}

void alu_jump(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_compReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_compImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    
}

void alu_load(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_save(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_plusReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    Register* reg1 = findRegister(operand1);
    Register* reg2 = findRegister(operand2);
    *reg2 += *reg1;
}

void alu_plusImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_multReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_multImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_diviReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_diviImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_subtReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_subtImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_shft(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_bandReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_bandImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_bxorReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_bxorImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_borrReg(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_borrImm(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_bnot(uint8_t operand1, uint8_t operand2, uint8_t operand3) {

}

void alu_noop(uint8_t operand1, uint8_t operand2, uint8_t operand3) {
    // Do nothing :^)
    return;
}
