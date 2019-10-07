#ifndef _INCLUDE_UTILS_H
#define _INCLUDE_UTILS_H
#include "common.h"

// 内存控制
void *memCtl(VM *vmPtr, void *ptr, uint32_t oldSize, uint32_t newSize);

// 分配指针类型
#define ALLOCATE(vmPtr, type) \
    (type *)memCtl(vmPtr, NULL, 0, sizeof(type))

// 分配柔性类型
#define ALLOCATE_FLEX(vmPtr, type, flexSize) \
    (type *)memCtl(vmPtr, NULL, 0, sizeof(type) + flexSize)

// 分配连续空间
#define ALLOCATE_ARRAY(vmPtr, type, size) \
    (type *)memCtl(vmPtr, NULL, 0, sizeof(type) * size)

// 释放连续空间
#define DEALLOCATE_ARRAY(vmPtr, arrPtr, size) \
    memCtl(vmPtr, arrPtr, sizeof(arrPtr[0]) * size, 0)

// 释放指针类型
#define DEALLOCATE(vmPtr, ptr) memCtl(vmPtr, ptr, 0, 0)

// n最近的2次幂
uint32_t roundUpToPowerOf2(uint32_t n);

// 字符串
typedef struct
{
    char *str;
    uint32_t size;
} String;

// 字符串缓冲区
typedef struct
{
    uint32_t size;
    char start[0];
} CharValue;

// 声明缓冲类型和方法
#define DECLARE_BUFFER_TYPE(type)                                                  \
    typedef struct                                                                 \
    {                                                                              \
        type *datas;                                                               \
        uint32_t cnt;                                                              \
        uint32_t cap;                                                              \
    } type##Buffer;                                                                \
    void type##BufferInit(type##Buffer *buf);                                      \
    void type##BufferPut(VM *vm, type##Buffer *buf, type data);                    \
    void type##BufferFill(VM *vm, type##Buffer *buf, type data, uint32_t fillCnt); \
    void type##BufferClear(VM *vm, type##Buffer *buf);

// 定义缓冲类型处理方法
#define DEFINE_BUFFER_FUNC(type)                                                  \
    void type##BufferInit(type##Buffer *buf)                                      \
    {                                                                             \
        buf->datas = NULL;                                                        \
        buf->cnt = buf->cap = 0;                                                  \
    }                                                                             \
                                                                                  \
    void type##BufferFill(VM *vm, type##Buffer *buf, type data, uint32_t fillCnt) \
    {                                                                             \
        uint32_t newCnt = buf->cnt + fillCnt;                                     \
        if (newCnt > buf->cap)                                                    \
        {                                                                         \
            size_t oldSize = buf->cap * sizeof(type);                             \
            buf->cap = roundUpToPowerOf2(newCnt);                                 \
            size_t newSize = buf->cap * sizeof(type);                             \
            ASSERT(newSize > oldSize, "faint...memory allocate");                \
            buf->datas = (type *)memCtl(vm, buf->datas, oldSize, newSize);        \
        }                                                                         \
        uint32_t cnt = 0;                                                         \
        while (cnt < fillCnt)                                                     \
        {                                                                         \
            buf->datas[buf->cnt++] = data;                                        \
            ++cnt;                                                                \
        }                                                                         \
    }                                                                             \
                                                                                  \
    void type##BufferPut(VM *vm, type##Buffer *buf, type data)                    \
    {                                                                             \
        type##BufferFill(vm, buf, data, 1);                                       \
    }                                                                             \
                                                                                  \
    void type##BufferClear(VM *vm, type##Buffer *buf)                             \
    {                                                                             \
        size_t oldSize = buf->cap * sizeof(buf->datas[0]);                        \
        memCtl(vm, buf->datas, oldSize, 0);                                       \
        type##BufferInit(buf);                                                    \
    }

// 类型定义以及函数声明
typedef uint8_t Byte;
typedef char Char;
typedef int Int;
DECLARE_BUFFER_TYPE(Int)
DECLARE_BUFFER_TYPE(Char)
DECLARE_BUFFER_TYPE(Byte)
DECLARE_BUFFER_TYPE(String)
#define SymbolTable StringBuffer
#define DEFAULT_BUFFER_SIZE 512

// 错误常量
typedef enum
{
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
    ERROR_RUNTIME
} ErrorType;

// 符号表清空和通用报错函数声明
void symbolTableClear(VM *vm, SymbolTable *st);
void errorReport(void *parser, ErrorType et, const char *fmt, ...);

// 报错类型宏定义
#define IO_ERROR(...) errorReport(NULL, ERROR_IO, __VA_ARGS__)
#define MEM_ERROR(...) errorReport(NULL, ERROR_MEM, __VA_ARGS__)
#define LEX_ERROR(parser, ...) errorReport(parser, ERROR_LEX, __VA_ARGS__)
#define COMPILE_ERROR(parser, ...) errorReport(parser, ERROR_COMPILE, __VA_ARGS__)
#define RUNTIME_ERROR(...) errorReport(NULL, ERROR_RUNTIME, __VA_ARGS__)

#endif
