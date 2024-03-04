#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "key-value.h"
#include "linked.h"

typedef void Associative;

struct ImplementAssociativeDatastructure
{
    size_t (*GetNodeSize)(
        size_t key_size,
        size_t value_size
    );
    
    void *(*Node_Find)(
        struct KeyValueSize *size,
        Node *head,
        void *key
    );

    void *(*Find)(
        Associative *data_struct,
        void *key
    );

    Node *(*Node_AddFirst)(
        struct KeyValueSize *size,
        Node *head,
        void *key,
        void *value
    );

    bool (*AddFirst)(
        Associative *data_struct,
        void *key,
        void *value
    );

    bool (*Node_Remove)(
        struct KeyValueSize *size,
        void *head,
        void *key
    );

    bool (*Remove)(
        Associative *data_struct,
        void *key
    );
};