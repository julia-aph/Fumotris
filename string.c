#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

typedef char String;

struct strHead
{
    size_t length;
    size_t capacity;
};

typedef struct strHead strHead;

String *NewString()
{
    static size_t initialCapacity = 1;

    uint8_t *object = malloc(sizeof(strHead) + initialCapacity);

    strHead *head = (strHead*)(object);
    *head = (strHead){
        .length = 0, .capacity = initialCapacity
    };

    char *string = (char*)(object + sizeof(strHead));
    string[0] = 0;

    return string;
}

String *NewStringAlloc(size_t size)
{
    uint8_t *object = malloc(sizeof(strHead) + size);

    strHead *head = (strHead*)(object);
    *head = (strHead){
        .length = 0, .capacity = size
    };

    char *string = (char*)(object + sizeof(strHead));
    string[0] = 0;

    return string;
}

String *NewStringCopy(char *chars)
{
    size_t length = strlen(chars);
    uint8_t *object = malloc(sizeof(strHead) + length + 1);

    strHead *head = (strHead*)(object);
    *head = (strHead){
        .length = length, .capacity = length + 1
    };

    char *string = (char*)(object + sizeof(strHead));
    memcpy(string, chars, length + 1);

    return string;
}

strHead *getHead(String *string)
{
    uint8_t *object = string - sizeof(strHead);

    strHead *head = (strHead*)(object);
    return head;
}

void DestroyString(String *string)
{
    strHead *head = getHead(string);
    free(head);
}

size_t StrLength(String *string)
{
    strHead *head = getHead(string);
    return head->length;
}

size_t StrCapacity(String *string)
{
    strHead *head = getHead(string);
    return head->capacity;
}

void strRealloc(strHead **head, String **string, size_t size)
{
    *head = realloc(*head, sizeof(strHead) + size);
    (*head)->capacity = size;
    
    uint8_t *object = (uint8_t*)*head;

    *string = object + sizeof(strHead);
}

size_t ConcatStr(String **dest, String *src)
{
    strHead *destHead = getHead(*dest);
    strHead *srcHead = getHead(src);

    size_t newLength = destHead->length + srcHead->length;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        strRealloc(&destHead, dest, newCapacity);

    destHead->length = newLength;
    memcpy(*dest + srcHead->length, src, srcHead->length + 1);

    return newLength;
}

size_t ConcatChars(String **dest, char *src)
{
    strHead *destHead = getHead(*dest);
    size_t srcLength = strlen(src);

    size_t newLength = destHead->length + srcLength;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        strRealloc(&destHead, dest, newCapacity);

    destHead->length = newLength;
    memcpy(*dest + srcLength, src, srcLength + 1);

    return newLength;
}

size_t ConcatChar(String **dest, char ch)
{
    strHead *destHead = getHead(*dest);
    
    size_t newLength = destHead->length + 1;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        strRealloc(&destHead, dest, newCapacity);

    destHead->length = newLength;
    (*dest)[newLength + 1] = ch;

    return newLength;
}

int main()
{
    String *string = NewStringCopy("asdfghjk");

    printf("%s\n", string);
    printf("length: %u, capacity: %u\n", StrLength(string), StrCapacity(string));

    ConcatStr(&string, NewStringCopy(", wow"));
    
    printf("%s\n", string);
    printf("length: %u, capacity: %u\n", StrLength(string), StrCapacity(string));

    ConcatChar(&string, '.');
    printf("%s\n", string);
    printf("length: %u, capacity: %u\n", StrLength(string), StrCapacity(string));

    return 0;
}