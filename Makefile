OUTDIR:=./out
CC:=clang

makedirs:
	@mkdir -p ${OUTDIR}/assembler/Target
	@mkdir -p ${OUTDIR}/emulator/Target

assembler: makedirs ${OUTDIR}/assembler/vpu-bin-as.o
	${CC} -o ${OUTDIR}/assembler/Target/vpu-bin-as ${OUTDIR}/assembler/vpu-bin-as.o

${OUTDIR}/assembler/%.o: assembler/%.c
	$(CC) -c -o $@ $^ -Iassembler/include/

${OUTDIR}/emulator/%.o: emulator/%.c
	$(CC) -c -o $@ $^

${OUTDIR}/assembler/Target/vpu-bin-as: OUTDIR/assembler/%.o
	$(CC) -o $@ $^

${OUTDIR}/emulator/Target/vpu-emu: ${OUTDIR}/emulator/%.o
	$(CC) -o $@ $^

clean: 
	@rm -rf ${OUTDIR}/assembler
	@rm -rf ${OUTDIR}/emulator
	@echo "Cleaned up!"