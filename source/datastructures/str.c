#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

strHead *get_head(String *string)
{
    char *object = string - sizeof(strHead);

    strHead *head = (strHead*)(object);
    return head;
}

void DestroyString(String *string)
{
    strHead *head = get_head(string);
    free(head);
}

size_t StrLength(String *string)
{
    strHead *head = get_head(string);
    return head->length;
}

void SetStrLength(String *string, size_t length)
{
    strHead *head = get_head(string);
    head->length = length;
}

size_t StrCapacity(String *string)
{
    strHead *head = get_head(string);
    return head->capacity;
}

void str_realloc(strHead **head, String **string, size_t size)
{
    *head = realloc(*head, sizeof(strHead) + size);
    (*head)->capacity = size;
    
    char *object = (char*)*head;

    *string = object + sizeof(strHead);
}

size_t ConcatStr(String **dest, String *src)
{
    strHead *destHead = get_head(*dest);
    strHead *srcHead = get_head(src);

    size_t newLength = destHead->length + srcHead->length;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        str_realloc(&destHead, dest, newCapacity);

    memcpy(*dest + destHead->length, src, srcHead->length + 1);

    destHead->length = newLength;

    return newLength;
}

size_t ConcatChars(String **dest, char *src)
{
    strHead *destHead = get_head(*dest);
    size_t srcLength = strlen(src);

    size_t newLength = destHead->length + srcLength;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        str_realloc(&destHead, dest, newCapacity);

    memcpy(*dest + destHead->length, src, srcLength + 1);

    destHead->length = newLength;

    return newLength;
}

size_t ConcatChar(String **dest, char ch)
{
    strHead *destHead = get_head(*dest);
    
    size_t newLength = destHead->length + 1;
    size_t newCapacity = newLength + 1;

    if(newCapacity > destHead->capacity)
        str_realloc(&destHead, dest, newCapacity);

    (*dest)[newLength-1] = ch;
    (*dest)[newLength] = 0;

    destHead->length = newLength;

    return newLength;
}

void strDebug(String *string)
{
    strHead *head = get_head(string);

    uint8_t *object = (uint8_t*)head;

    printf("          ");
    for(int i = 0; i < sizeof(strHead) + head->capacity; i++)
    {
        printf("%u ", i);
    }

    printf("\nstrDebug: ");
    for(int i = 0; i < sizeof(strHead); i++)
    {
        printf("%u_", object[i]);
    }
    printf(" ");
    for(int i = sizeof(strHead); i < sizeof(strHead) + head->capacity; i++)
    {
        printf("%u_", object[i]);
    }
    printf(".\n");
}