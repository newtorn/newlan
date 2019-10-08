#ifndef _OBJECT_OBJTHREAD_H
#define _OBJECT_OBJTHREAD_H
#include "objfunction.h"

// 线程对象
typedef struct objectThread
{
    ObjectHeader objectHeader;   // 对象头
    Value *ebp;                  // 运行时栈的栈底
    Value *esp;                  // 运行时栈的栈顶
    uint32_t stackCap;           // 运行时栈的容量
    Frame *frames;               // 所有调用帧
    uint32_t frameCap;           // 调用帧的容量
    uint32_t frameCnt;           // 已使用的调用帧数量
    ObjectUpvalue *openUpvalues; // 开启的上值链首节点
    Value errorObject;           // 运行时栈的错误对象
    struct ObjectThread *caller; // 当前线程的调度者
} ObjectThread;

// 准备调用帧函数声明 sits(start in the thread stack)
void prepareFrame(ObjectThread *ot, ObjectClosure *oc, Value *sits);

// 创建线程对象函数声明
ObjectThread *makeObjectThread(VM *vm, ObjectClosure *oc);

// 重置线程对象函数声明
void threadReset(ObjectThread *ot, ObjectClosure *oc);

#endif