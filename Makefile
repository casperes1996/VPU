OUTDIR:=./out
CC:=clang
CFLAGS=-g -Wall

makedirs:
	@mkdir -p ${OUTDIR}/assembler/Target
	@mkdir -p ${OUTDIR}/emulator/Target

assembler: makedirs ${OUTDIR}/assembler/vpu-bin-as.o
	${CC} -o ${OUTDIR}/assembler/Target/vpu-bin-as ${OUTDIR}/assembler/vpu-bin-as.o

${OUTDIR}/assembler/%.o: assembler/%.c
	$(CC) -c $(CFLAGS) -o $@ $^ -Ishared/ -Iassembler/include/

${OUTDIR}/emulator/%.o: emulator/%.c
	$(CC) -c $(CFLAGS) -o $@ $^ -Ishared/ -Iemulator/include/

clean: 
	@rm -rf ${OUTDIR}/assembler
	@rm -rf ${OUTDIR}/emulator
	@rm asm-examples/valid/*.bin
	@echo "Cleaned up!"