/* Single-line comment */

/*
 * Multi-line comment
 */

/* Includes are in alphabetical order */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Non-std libraries get included after std libraries */
#include "style-guide.h"

/*
 * Sticking to C# conventions with a few funny exceptions.
 *

 */

/* Pascal case for name and members */
enum ExampleEnum
{
    One,
    Two
};

/* 
 * Pascal case for name, snake case for members.
 *
 * Only use typedef for opaque objects.
 */
struct ExampleStruct
{
    int descriptive_name;
    float cool;
};

/*
 * "Public" functions use pascal case, Implementation-
 * specific functions use snake case.
 *
 * Local variables use snake case.
 */
int ExamplePublicFunction(int param_one, int param_two)
{
    int local = param_one + param_two;

    return local;
}

/*
 * Implementation-specific functions can be named less
 * descriptively.
 */
int example_impl_function(int a, int b)
{
    return a + b;
}