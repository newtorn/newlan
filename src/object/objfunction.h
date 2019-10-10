#ifndef _OBJECT_OBJFUNCTION_H
#define _OBJECT_OBJFUNCTION_H
#include "utils.h"
#include "objmeta.h"

// 函数调试结构
typedef struct
{
    char *funcName;     // 函数名
    IntBuffer lineNums; // 行号
} FunctionDebug;

// 函数对象
typedef struct
{
    ObjectHeader objectHeader; // 对象头
    ByteBuffer instrStream;    // 指令流
    ValueBuffer constants;     // 常量池
    ObjectModule *module;      // 所属模块
    uint32_t maxStackSlot;     // 使用的栈峰值
    uint32_t upvalueCnt;       // 上值数量
    uint8_t argc;              // 期望参数个数
#if DEBUG
    FunctionDebug *debug;
#endif
} ObjectFunction;

// 上值对象
typedef struct upvalue
{
    ObjectHeader objectHeader; // 对象头
    Value *localVar;           // 指向关联的局部变量
    Value closed;              // 已经关闭的上值
    struct upvalue *next;      // 仍然开放的上值链
} ObjectUpvalue;

// 闭包对象
typedef struct
{
    ObjectHeader objectHeader;  // 对象头
    ObjectFunction *func;       // 所需引用函数
    ObjectUpvalue *upvalues[0]; //弹性数组，存储已经关闭的上值
} ObjectClosure;

// 调用帧
typedef struct
{
    uint8_t *pc;            // 程序指针，指向下一个将被执行的指令
    ObjectClosure *closure; // 执行的闭包函数
    Value *start;           // 调用帧所在线程运行时栈的起始地址
} Frame;

// 初始调用帧数量
#define INITIAL_FRAME_NUM 4

// 函数和闭包相关对象创建函数声明
ObjectUpvalue *makeObjectUpvalue(VM *vm, Value *localVar);
ObjectClosure *makeObjectClosure(VM *vm, ObjectFunction *of);
ObjectFunction *makeObjectFunction(VM *vm, ObjectModule *om, uint32_t maxStackSlot);

#endif