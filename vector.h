#include <stdint.h>

void *NewVectorAllocate(size_t itemSize, size_t capacity);

void *NewVector(size_t itemSize);

void DestroyVector(void *array);

void VectorAdd(void *arrayPtr, void *item);

void VectorTrim(void *arrayPtr);

size_t VectorCapacity(void *array);

size_t VectorLength(void *array);