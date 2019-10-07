#ifndef _OBJECT_OBJRANGE_H
#define _OBJECT_OBJRANGE_H
#include "objmodel.h"

// 范围对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    int32_t start;             // 范围开始
    int32_t end;               // 范围结束
} ObjectRange;

// 创建范围对象函数声明
ObjectRange *makeObjectRange(VM *vm, int32_t start, int32_t end);

#endif