#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key-value.h"
#include "linked.h"

struct AscLinkedList
{
    struct KeyValueSize size;
    void *head;

    size_t length;
};

struct AscLinkedList NewAssociativeLinkedList(size_t key_size, size_t value_size);

size_t Asc_GetNodeSize(size_t key_size, size_t value_size);

void *AscNode_Find(struct KeyValueSize *size, void *head, void *key);
void *Asc_Find(struct AscLinkedList *list, void *key);

Node *AscNode_AddFirst(struct KeyValueSize *size, void *head, void *key, void *value);
bool Asc_AddFirst(struct AscLinkedList *list, void *key, void *value);

void *AscNode_Index(struct KeyValueSize *size, void *head, size_t index);
struct KeyValuePtr Asc_Index(struct AscLinkedList *list, size_t index);

bool AscNode_Remove(struct KeyValueSize *size, Node *head, void *key);
bool Asc_Remove(struct AscLinkedList *list, void *key);