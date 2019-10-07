#ifndef _VM_VM_H
#define _VM_VM_H
#include "common.h"
#include "objheader.h"

// 虚拟机
struct vm
{
    uint32_t allBytes;        // 总分配空间
    Parser *curParser;        // 当前词法分析器
    Model *stringModel;       // 字符串模型链
    ObjectHeader *allObjects; // 所有已分配的对象链
};

// 初始化虚拟机和创建虚拟机函数声明
void initVM(VM *vm);
VM *makeVM(void);

#endif