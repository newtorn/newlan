#ifndef _OBJECT_OBJSTRING_H
#define _OBJECT_OBJSTRING_H
#include "objheader.h"

// 字符串对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    uint32_t hashCode; // 字符串哈希值
    CharValue value; // 字符串缓冲区
} ObjectString;

// 声明字符串对象操作函数
uint32_t hashString(char *str, uint32_t size);
void hashObjectString(ObjectString *os);
ObjectString *makeObjectString(VM *vm, const char *str, uint32_t size);

// FNV-1a算法相关常量
#define OFFSET_BASIS 2166136261
#define FNV_PRIME_BIT32 16777619

#endif