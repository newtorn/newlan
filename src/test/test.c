#include "test.h"
#include "common.h"
#include "vm.h"
#include "utils.h"
#include "utf8.h"
#include <stdio.h>
#include <stdlib.h>

void test_utils()
{
    int idx;
    VM *vm = makeVM();
    char str[] = "HELLOWORLD";

    CharBuffer *cb = ALLOCATE(vm, Char);
    CharBufferInit(cb);

    idx = 0;
    while (*(str+idx))
    {
        CharBufferPut(vm, cb, *(str+idx++));
    }

    printf("Add %d chars to buffer:\n", cb->cnt);
    printf("VM alloacted bytes: %d\n", vm->allBytes);
    printf("Char buffer capacity: %d, count: %d\n", cb->cap, cb->cnt);
    
    idx = 0;
    while (idx < cb->cnt)
    {
        printf("%c|", *(cb->datas+idx++));
    }
    printf("\n");

    CharBufferClear(vm, cb);
    printf("Clear buffer:\n");
    printf("VM alloacted bytes: %d\n", vm->allBytes);
    printf("Char buffer capacity: %d, count: %d\n", cb->cap, cb->cnt);

    DEALLOCATE(vm, cb);

    IO_ERROR("IO Dead Lock");
}

void test_utf8()
{
    int idx;
    VM *vm = makeVM();
    uint8_t *buf = ALLOCATE_ARRAY(vm, uint8_t, 2);

    const char str[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    
    idx = 0;
    while (*(str+idx))
    {
        encodeUtf8(buf+idx, *(str+idx));
        printf("%d|", *(buf+idx++));
    }
    printf("\n");
    
    idx = 0;
    while (*(str+idx))
    {
        printf("%c|", decodeUtf8(buf+idx++, 1));
    }
    printf("\n");

    DEALLOCATE(vm, buf);
}
