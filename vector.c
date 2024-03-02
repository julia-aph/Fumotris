#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct vecHead
{
    size_t itemSize;
    size_t capacity;
    size_t used;
};

void *NewVectorAllocate(size_t itemSize, size_t capacity)
{
    void *vec = malloc(sizeof(struct vecHead) + itemSize * capacity);

    struct vecHead *head = (struct vecHead*)vec;
    *head = (struct vecHead){ itemSize, capacity, 0 };

    return (unsigned char*)vec + sizeof(struct vecHead);
}

void *NewVector(size_t itemSize)
{
    return NewVectorAllocate(itemSize, 1);
}

void DestroyVector(void *array)
{
    void *vec = (struct vecHead*)array - 1;
    free(vec);
}

void vecAdd(void **array, void *item)
{
    void *vec = (struct vecHead*)*array - 1;
    struct vecHead *head = vec;

    // If the backing array needs to be grown
    if(head->used == head->capacity)
    {
        vec = realloc(vec, sizeof(struct vecHead) + head->capacity * head->itemSize * 2);
        head = vec;
        *array = head + 1;
        head->capacity *= 2;
    }

    memcpy(*array + head->used * head->itemSize, item, head->itemSize);
    head->used++;
}

void VectorAdd(void *arrayPtr, void *item)
{
    vecAdd(arrayPtr, item);
}

void vecTrim(void **array)
{
    void *vec = (struct vecHead*)*array - 1;
    struct vecHead *head = vec;
    
    vec = realloc(vec, sizeof(struct vecHead) + head->used * head->itemSize);
    head = vec;
    *array = head + 1;

    head->capacity = head->used;
}

void VectorTrim(void *arrayPtr)
{
    vecTrim(arrayPtr);
}

size_t VectorCapacity(void *array)
{
    struct vecHead *head = (struct vecHead*)array - 1;
    return head->capacity;
}

size_t VectorLength(void *array)
{
    struct vecHead *head = (struct vecHead*)array - 1;
    return head->used;
}