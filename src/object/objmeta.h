#ifndef _OBJECT_OBJMETA_H
#define _OBJECT_OBJMETA_H
#include "objstring.h"

// 模块对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    SymbolTable varNames; // 变量名缓冲区
    ValueBuffer varValues; // 变量值缓冲区
    ObjectString *name; // 模块名
} ObjectModule;

// 实例对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    Value attrs[0];
} ObjectInstance;

// 声明模块和实例创建函数
ObjectModule *makeObjectModule(VM *vm, const char *name);
ObjectInstance *makeObjectInstance(VM *vm, Model *model);

#endif