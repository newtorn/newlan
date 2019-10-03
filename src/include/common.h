#ifndef _INCLUDE_COMMON_H
#define _INCLUDE_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct vm VM;
typedef struct parser Parser;
typedef struct model Model;

#define bool char
#define true 1
#define false 0
#define UNUSED __attribute__((unused))

#ifdef DEBUG
#define ASSERT(cond, err)                                                   \
    do                                                                      \
    {                                                                       \
        if (!cond)                                                          \
        {                                                                   \
            fprintf(stderr, "ASSERT failed! %s:%d in function: %s(): %s\n", \
                    __FILE__, __LINE__, __func__, err);                     \
            abort();                                                        \
        }                                                                   \
    } while (0);
#else
#define ASSERT(cond, err) ((void)0);
#endif

#define NOT_REACHED()                                                 \
    do                                                                \
    {                                                                 \
        fprintf(stderr, "Not REACHED: %s:%d in function: %s()\n", \
                __FILE__, __LINE__, __func__);                        \
        while (1)                                                     \
        {                                                             \
        }                                                             \
    } while (0);

#endif