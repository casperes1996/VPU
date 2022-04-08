#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include "vpu-bin-as.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }
    FileData fileData = readInFile(argv[1]);

    free(fileData.data);
    printf("File size = %zu", fileData.size);
    parseInstructions(fileData, NULL);
    return 0;
}

FileData readInFile(char* fileName) {
    FILE* f = fopen(fileName, "r");
    if (!f) {
        fprintf(stderr, "Error: Could not open file %s\n", fileName);
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = malloc(size);
    if (!data) {
        fprintf(stderr, "Error: Could not allocate %zu bytes\n", size);
        exit(1);
    }
    if (fread(data, 1, size, f) != size) {
        fprintf(stderr, "Error: Could not read %zu bytes\n", size);
        exit(1);
    }
    fclose(f);
    return (FileData) {
        .data = data,
        .size = size
    };
}

Instruction** parseInstructions(FileData fileData, size_t* numOfInstructions) {
    char* duplicateOfFile = strdup(fileData.data);
    printf("\n%s\n", duplicateOfFile);
    return NULL;
}