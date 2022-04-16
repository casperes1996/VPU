// only include once
#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

// Macro based generic Dynamic Array

#define decDynamicArray(type, name) \
	typedef struct DynamicArray_##name##_st { \
		type *array; \
		uint64_t used; \
		size_t size; \
	} DynamicArray_##name

#define defInitArray(type, name) \
	DynamicArray_##name* initArray_##name(size_t initialSize) { \
		DynamicArray_##name* a = (DynamicArray_##name*) malloc(sizeof(DynamicArray_##name)); \
		a->array = (type*) malloc(initialSize * sizeof(type*)); \
		a->used = 0; \
		a->size = initialSize; \
		return a; \
	}

// a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
// Therefore a->used can go up to a->size
#define defInsertArray(type, name) \
	void insertArray_##name(DynamicArray_##name* a, type value) { \
		if (a->used == a->size) { \
			a->size *= 2; \
			a->array = (type*) realloc(a->array, a->size * sizeof(type*)); \
		} \
		a->array[a->used++] = value; \
	}

#define defFreeArray(type, name) \
	void freeArray_##name(DynamicArray_##name* a) { \
		free(a->array); \
		a->array = NULL; \
		a->used = 0; \
		a->size = 0; \
	}

#define defineDynamicArray(type, name) \
	defInitArray(type, name); \
	defInsertArray(type, name); \
	defFreeArray(type, name)

#define declareDynamicArray(type, name) \
	decDynamicArray(type, name); \
	DynamicArray_##name* initArray_##name(size_t initialSize); \
	void insertArray_##name(DynamicArray_##name* a, type value); \
	void freeArray_##name(DynamicArray_##name* a)

declareDynamicArray(uint8_t, uint8_t);

#endif // DYNAMIC_ARRAY_H