#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key-value.h"

struct AscLinkedList
{
    struct KeyValueSize size;
    void *head;

    size_t length;
};
typedef struct AscLinkedList AscLinkedList;

AscLinkedList NewAssociativeLinkedList(size_t key_size, size_t value_size);

size_t Asc_GetNodeSize(size_t key_size, size_t value_size);

void *Asc_Find(struct KeyValueSize *size, void *head, void *key);

uint8_t Asc_AddFirstAtNode(struct KeyValueSize *size, void *head, void *key, void *value);

uint8_t Asc_AddFirst(AscLinkedList *list, void *key, void *value);

void *Asc_IndexNode(struct KeyValueSize *size, void *head, size_t index);

struct KeyValuePtr Asc_Index(AscLinkedList *list, size_t index);

uint8_t Asc_Remove();