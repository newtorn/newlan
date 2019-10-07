#ifndef _OBJECT_OBJLIST_H
#define _OBJECT_OBJLIST_H
#include "objmodel.h"
#include "vm.h"

// 列表对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    ValueBuffer elements;      // 所有元素缓冲区
} ObjectList;

// 列表对象创建函数声明
ObjectList *makeObjectList(VM *vm, uint32_t elemCnt);

// 删除idx位置的元素
Value removeElement(VM *vm, ObjectList *ol, uint32_t idx);

// 向idx位置插入元素
void insertElement(VM *vm, ObjectList *ol, uint32_t idx, Value e);

#endif