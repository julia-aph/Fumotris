#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <ctype.h>

#include "vector.h"

char *ReadTextFile(char *path)
{
    FILE *file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    size_t length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *string = malloc(length + 1);
    fread(string, length, 1, file);
    fclose(file);

    string[length] = 0;

    return string;
}

enum JSONType
{
    OBJECT,
    ARRAY,
    STRING,
    PRIMITIVE
};

enum Expecting
{
    COMMA,
    COLON,
    ITEM
};

struct Token
{
    enum JSONType type;

    size_t start;
    size_t length;

    void* parent;
};

void ParseJSON(char *string)
{
    struct Token *tokens = NewVector(sizeof(struct Token));

    struct Token parent =
    {
        .type = ARRAY,

        .start = 0, .length = 0,

        .parent = 0
    };

    ParseArray(string, 0, parent, tokens);
}

size_t ParseForItem(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    for(size_t i = offset; string[i] != 0; i++)
    {
        switch(string[i])
        {
            case '{':
                return ParseObject(string, i + 1, parent, tokens);

            case '[':
                return ParseArray(string, i + 1, parent, tokens);

            case '"':
                return ParseString(string, i + 1, parent, tokens);

            case ',':
            case ':': return 0;

            case ' ':
            case '\n': continue;
        }

        if(isalpha(string[i]))
        {
            return ParseUnknownString(string, i, parent, tokens);
        }

        if(isdigit(string[i]) || string[i] == '-' || string[i] == '.')
        {
            return ParseDigit(string, i, parent, tokens);
        }

        return 0;
    }

    return 0;
}

size_t ParseForSymbol(char *string, size_t offset, char symbol)
{
    for(size_t i = offset; string[i] != 0; i++)
    {
        if(string[i] == symbol)
            return i + 1;

        switch(string[i])
        {
            case ' ':
            case '\n': continue;
        }

        return 0;
    }
}

size_t ParseArray(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    struct Token self =
    {
        .type = ARRAY,

        .start = offset, .length = 0,

        .parent = &parent
    };

    enum Expecting expecting = ITEM;

    for(size_t i = offset; string[i] != 0; i++)
    {
        if(expecting == ITEM)
        {
            size_t success = ParseForItem(string, i, self, tokens);

            if(success)
            {
                i = success;
                expecting = COMMA;
                continue;
            }
            else
            {
                return 0;
            }
        }

        if(expecting == COMMA)
        {
            switch(string[i])
            {
                case ',':
                    expecting = ITEM;
                    continue;

                case ']':
                    return i;
            }
        }
    }
}

size_t ParseObject(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    struct Token self =
    {
        .type = OBJECT,

        .start = offset, .length = 0,

        .parent = &parent
    };

    enum Expecting expecting = ITEM;
    enum Expecting next = COLON;

    for(size_t i = offset; string[i] != 0; i++)
    {
        if(expecting == ITEM)
        {
            size_t success = ParseForItem(string, i, self, tokens);

            if(success)
            {
                i = success;
                expecting = next;
                next = ITEM;
                continue;
            }

            return 0;
        }

        if(expecting == COLON)
        {
            size_t success = ParseForSymbol(string, i, ':');

            if(success)
            {
                i = success;
                expecting = next;
                next = COMMA;
                continue;
            }

            return 0;
        }

        if(expecting == COMMA)
        {
            switch(string[i])
            {
                case ',':
                    expecting = ITEM;
                    continue;

                case '}':
                    return i;
            }
        }
    }
}

size_t ParseString(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    for(size_t i = offset; string[i] != 0; i++)
    {
        if(string[i] == '"')
            return i;
    }

    return 0;
}

size_t ParseUnknownString(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    static char trueChars[4] = { 'r', 'u', 'e' };
    static char falseChars[5] = { 'a', 'l', 's', 'e' };

    uint8_t matchesTrue = 0;
    uint8_t matchesFalse = 0;

    if(string[offset] == 't')
        matchesTrue = 1;
    else if(string[offset] == 'f')
        matchesFalse = 1;

    int index = 0;

    for(size_t i = offset; string[i] != 0; i++)
    {
        if(matchesTrue)
        {
            if(index == 3 && (string[i] == ' ' || string[i] == '\n'))
                return i;

            if(string[i] == trueChars[index])
            {
                index++;
                continue;
            }
        }

        if(matchesFalse)
        {
            if(index == 4 && (string[i] == ' ' || string[i] == '\n'))
                return i;

            if(string[i] == falseChars[index])
            {
                index++;
                continue;
            }
        }

        if(string[i] == ' ' || string[i] == '\n')
        {
            return i - 1;
        }
    }

    return 0;
}

size_t ParseDigit(char *string, size_t offset, struct Token parent, struct Vector *tokens)
{
    for(size_t i = offset; string[i] != 0; i++)
    {

    }
}