#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef char String;

String *NewString();

String *NewStringAlloc(size_t size);

String *NewStringCopy(char *chars);

void DestroyString(String *string);

size_t StrLength(String *string);

size_t StrCapacity(String *string);

size_t ConcatStr(String **dest, String *src);

size_t ConcatChars(String **dest, char *src);

size_t ConcatChar(String **dest, char ch);