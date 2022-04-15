# Emulator README

It is recommended to first read doc/ISA.md.

The idea of the emulator is that emu.c will contain the code for the program you will invoke in the shell, reading in a binary file,
and potentially parsing other parameters and flags to set up the other components.

vpu.c will be the main component and will represent the CPU wholistically. It will pass certain tasks to the ALU for encapsulating a lot of instructions, and for the mmu to deal with memory operations.
The MMU will initially be very simple in v1.0 simply having a big array and storing to and loading from it. Later it will also capture I/O MMU and potentially paging depending on how far the project goes.
