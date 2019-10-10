#ifndef _VM_VM_H
#define _VM_VM_H
#include "common.h"
#include "objheader.h"
#include "objmap.h"
#include "objthread.h"

// 为opcode.inc中的操作码添加opcode前缀OC_
#define OPCODE_SLOTS(opcode, effect) OC_##opcode,

// 操作码
typedef enum
{
    OC_UNKNOWN,       // 未知opcode
#include "opcode.inc" // 展开opcode
} OpCode;

// 虚拟机执行结果
typedef enum
{
    VMR_SUCCESS,
    VMR_ERROR
} VMResult;

// 虚拟机
struct vm
{
    Model *mapModel;            // 哈希表模型
    Model *setModel;            // 集合模型
    Model *noneModel;           // 空模型
    Model *metaModel;           // 元模型
    Model *listModel;           // 列表模型
    Model *boolModel;           // 布尔模型
    Model *rangeModel;          // 范围模型
    Model *numberModel;         // 数字模型
    Model *stringModel;         // 字符串模型
    Model *objectModel;         // 对象模型
    Model *threadModel;         // 线程模型
    Model *modelOfModel;        // 模型模型
    Model *functionModel;       // 函数模型
    uint32_t allBytes;          // 总分配空间
    Parser *curParser;          // 当前词法分析器
    ObjectMap *allModules;      // 所有模块
    ObjectThread *curThread;    // 当前线程
    ObjectHeader *allObjects;   // 所有已分配的对象链
    SymbolTable allActionNames; // 所有模型的行为名
};

// 初始化虚拟机和创建虚拟机函数声明
void initVM(VM *vm);
VM *makeVM(void);

#endif