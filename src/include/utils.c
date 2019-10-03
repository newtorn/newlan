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

// 缓冲类型处理函数定义
DEFINE_BUFFER_FUNC(Int)
DEFINE_BUFFER_FUNC(Char)
DEFINE_BUFFER_FUNC(Byte)
DEFINE_BUFFER_FUNC(String)

// 符号表清空
void symbolTableClear(VM *vm, SymbolTable *st)
{
    uint32_t idx = 0;
    while (idx < st->cnt)
    {
        memCtl(vm, st->datas[idx++].str, 0, 0);
    }
    StringBufferClear(vm, st);
}

// 通用报错函数
void errorReport(Parser *parser, ErrorType et, const char *fmt, ...)
{
    char buf[DEFAULT_BUFFER_SIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, DEFAULT_BUFFER_SIZE, fmt, ap);
    va_end(ap);

    switch (et)
    {
    case ERROR_IO:
    case ERROR_MEM:
        fprintf(stderr, "%s:%d In function %s():%s\n", __FILE__, __LINE__, __func__, buf);
        break;
    case ERROR_LEX:
    case ERROR_COMPILE:
        ASSERT(parser != NULL, "parser is null!");
        fprintf(stderr, "%s:%d \"%s\"\n", parser->file, parser->preToken.lineNum, buf);
        break;
    case ERROR_RUNTIME:
        fprintf(stderr, "%s\n", buf);
        break;
    default:
        NOT_REACHED();
    }
    exit(1);
}