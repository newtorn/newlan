#ifndef _OBJECT_OBJSET_H
#define _OBJECT_OBJSET_H
#include "objmap.h"
#include "objmodel.h"
#include "common.h"

// 集合对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    ObjectMap *map;            // 存放集合的哈希表
    Value PRESENT;             // 元素值作为key，虚拟值作为value
} ObjectSet;

// 集合相关方法声明
ObjectSet *makeObjectSet(VM *vm);
void setAdd(VM *vm, ObjectSet *os, Value v);
bool setHas(ObjectSet *os, Value v);
void setClear(VM *vm, ObjectSet *os);

#endif