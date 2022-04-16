OUTDIR:=./out
CC:=clang
CFLAGS=-g -Wall

all: makedirs assembler emulator

makedirs:
	@mkdir -p ${OUTDIR}/assembler/Target
	@mkdir -p ${OUTDIR}/emulator/Target
	@mkdir -p ${OUTDIR}/shared/

assembler: ${OUTDIR}/assembler/vpu-bin-as.o ${OUTDIR}/shared/dynamicArray.o
	${CC} -o ${OUTDIR}/assembler/Target/vpu-bin-as $^

emulator: ${OUTDIR}/emulator/vpu.o ${OUTDIR}/emulator/emu.o ${OUTDIR}/emulator/mmu.o  ${OUTDIR}/emulator/alu.o ${OUTDIR}/shared/dynamicArray.o
	${CC} -o ${OUTDIR}/emulator/Target/emu $^

${OUTDIR}/assembler/%.o: assembler/%.c makedirs
	@$(CC) -c $(CFLAGS) -o $@ $< -Ishared/ -Iassembler/include/

${OUTDIR}/shared/%.o: shared/%.c makedirs
	@$(CC) -c $(CFLAGS) -o $@ $< -Ishared/

${OUTDIR}/emulator/%.o: emulator/%.c makedirs
	@$(CC) -c $(CFLAGS) -o $@ $< -Ishared/ -Iemulator/include/

clean: 
	@rm -rf ${OUTDIR}/assembler
	@rm -rf ${OUTDIR}/emulator
	@rm asm-examples/valid/*.bin
	@echo "Cleaned up!"