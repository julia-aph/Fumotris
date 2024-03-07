/*#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "linked-list.h"
#include "str.h"

enum Type
{
    OBJECT,
    ARRAY,
    STRING
};

struct Token
{
    enum Type type;

    char *first;
    char *last;

    uint8_t *children;
};

static struct Token stack_pop(LinkedList *stack)
{
    struct Token tmp = *(struct Token*)LinkedList_Index(stack, 0);
    LinkedList_Remove(stack, 0);

    return tmp;
}

static struct Token *stack_peep(LinkedList *stack)
{
    return LinkedList_Index(stack, 0);
}

struct Token Parse(String *json)
{
    struct Token parent = { 0, 0, 0 };
    
    LinkedList stack = NewLinkedList(sizeof(struct Token));
    LinkedList items = NewLinkedList(sizeof(struct Token));

    for(size_t i = 0; i < StrLength(json); i++)
    {
        switch(json[i])
        {
            case ' ':
            case '\t':
            case '\n': continue;

            case '{':
                struct Token new_token = (struct Token){ OBJECT, json + i + 1, 0 };
                LinkedList_AddFirst(&stack, &new_token);
                break;

            case '}':
                struct Token last_token = stack_pop(&stack);
                last_token.last = json + i - 1;

                LinkedList_AddLast(&items, &last_token);
                break;

            case '[':
                struct Token new_token = (struct Token){ ARRAY, json + i + 1, 0 };
                LinkedList_AddFirst(&stack, &new_token);
                break;

            case ']':
                struct Token *last_token = stack_pop(&stack);
                last_token->last = json + i - 1;

                LinkedList_AddLast(&items, last_token);
                break;
        }
    }

    return *(struct Token*)LinkedList_Index(&items, 0);
}

int main()
{
    printf("entry\n");

    struct Token tok = Parse(NewStringCopy("{ cool }"));

    printf("done\n");
    printf("tok: type=%u, first=%u, last=%u\n", tok.type, tok.first, tok.last);

    for(char *i = tok.first; i != tok.last; i++)
    {
        printf("%c ", *i);
    } printf("\n");
    
    return 0;
}*/