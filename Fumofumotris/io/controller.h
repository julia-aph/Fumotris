#pragma once
#include <iso646.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Ctrl {
    struct ctrl_dict *dict;
};
typedef struct Ctrl Ctrl;

bool CtrlUpdateKey(Ctrl *ctrl, event);