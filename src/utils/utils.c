#include "utils.h"
#include "vm.h"
#include "parser.h"
#include <stdlib.h>
#include <stdarg.h>

// 内存控制
// 申请内存
// 修改内存
// 释放内存
void *memCtl(VM *vm, void *ptr, uint32_t oldSize, uint32_t newSize)
{
    // 累计分配总内存
    vm->allBytes += newSize - oldSize;

    // 处理realloc(NULL, 0)产生的新地址不方便释放
    if (newSize == 0)
    {
        free(ptr);
        return NULL;
    }
    return realloc(ptr, newSize);
}

// n最近的2次幂
uint32_t roundUpToPowerOf2(uint32_t n)
{
    // 0时取1
    n += (n == 0);
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

DEFINE_BUFFER_FUNC(Int)
DEFINE_BUFFER_FUNC(Char)
DEFINE_BUFFER_FUNC(Byte)
DEFINE_BUFFER_FUNC(String)

void symbolTableClear(VM *vm, SymbolTable *st)
{
    uint32_t idx = 0;
    while (idx < st->cnt)
    {
        memCtl(vm, st->datas[idx++].str, 0, 0);
    }
    StringBufferClear(vm, st);
}

void errorReport(Parser *parser, ErrorType et, const char* fmt, ...)
{
    char buf[DEFAULT_BUFFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf();
}