#ifndef _OBJECT_OBJMAP_H
#define _OBJECT_OBJMAP_H
#include "objheader.h"

#define MAP_LOAD_PRECENT 0.8

// 记录(键值对)对象
typedef struct
{
    Value key;   // 键
    Value value; // 值
} Entry;

// 哈希表对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    uint32_t cap;              // 容量
    uint32_t cnt;              // 数量
    Entry *entries;            // 键值对链
} ObjectMap;

// 哈希表相关函数声明
ObjectMap *makeObjectMap(VM *vm);
void mapSet(VM *vm, ObjectMap *om, Value k, Value v);
Value mapGetByKey(ObjectMap *om, Value k);
Value mapGetByValue(ObjectMap *om, Value v);
bool mapHasKey(ObjectMap *om, Value k);
bool mapHasValue(ObjectMap *om, Value v);
void mapClear(VM *vm, ObjectMap *om);
Value mapRemove(VM *vm, ObjectMap *om, Value k);

#endif
