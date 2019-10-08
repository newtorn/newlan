#ifndef _OBJECT_OBJHEADER_H
#define _OBJECT_OBJHEADER_H
#include "utils.h"

// 对象类型
typedef enum
{
    OT_MAP,
    OT_SET,
    OT_LIST,
    OT_MODEL,
    OT_RANGE,
    OT_STRING,
    OT_THREAD,
    OT_MODULE,
    OT_CLOSURE,
    OT_UPVALUE,
    OT_INSTANCE,
    OT_FUNCTION
} ObjectType;

// 对象头
typedef struct objectHeader
{
    ObjectType type;
    bool accessible;
    Model *model;
    struct objectHeader *next;
} ObjectHeader;

// 值类型
typedef enum
{
    VT_NONE,
    VT_TRUE,
    VT_FALSE,
    VT_NUMBER,
    VT_OBJECT,
    VT_UNDEFINED
} ValueType;

// 值结构
typedef struct
{
    ValueType type;
    union {
        double number;
        ObjectHeader *objectHeader;
    };
} Value;

// 声明Value缓冲区类型
DECLARE_BUFFER_TYPE(Value)

// 声明初始化对象头函数
void initObjectHeader(VM *vm, ObjectHeader *oh, ObjectType ot, Model *model);

#endif