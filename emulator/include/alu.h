// only include once
#ifndef EMULATOR_ALU_H
#include <stdint.h>
#define EMULATOR_ALU_H

void alu_moveReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_moveImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_jump(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_compReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_compImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_load(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_save(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_plusReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_plusImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_multReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_multImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_diviReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_diviImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_subtReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_subtImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_shft(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_bandReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_bandImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_bxorReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_bxorImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_borrReg(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_borrImm(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_bnot(uint8_t operand1, uint8_t operand2, uint8_t operand3);
void alu_noop(uint8_t operand1, uint8_t operand2, uint8_t operand3);

#endif //EMULATOR_ALU_H