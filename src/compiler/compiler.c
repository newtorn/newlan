#include "compiler.h"
#include "parser.h"
#include "core.h"
#include <string.h>
#if DEBUG
#include "debug.h"
#endif

// 编译单元
struct compileUnit
{
    ObjectFunction func;                   // 编译的函数
    LocalVar localVars[MAX_LOCAL_VAR_NUM]; // 局部变量表
    uint32_t localVarCnt;                  // 已分配的局部变量
    Upvalue upvalues[MAX_UPVALUE_NUM];     // 当前函数引用的上值
    int32_t scopeDepth;                    // 当前编译的代码所处作用域
    uint32_t stackSlotCnt;                 // 当前使用的栈槽位数量
    Loop *curLoop;                         // 当前编译的循环层
    ModelBook *enclosedModelBook;          // 当前编译的模型的编译信息
    struct compileUnit *enclosedUnit;      // 包含当前编译单元的直接外层编译单元
    Parser *curParser;                     // 当前词法分析器
};

// 定义模块变量 重定义返回 -1
int32_t defineModuleVar(VM *vm, ObjectModule *om, const char *name, uint32_t size, Value v)
{
    if (size > MAX_ID_LEN)
    {
        char id[MAX_ID_LEN] = {'\0'};
        memcpy(id, name, size);
        if (NULL != vm->curParser)
        {
            COMPILE_ERROR(vm->curParser, "identifier \"%s>\" is out of max length<%d>", id, MAX_ID_LEN);
        }
        else
        {
            MEM_ERROR("identifier \"%s\" is out of max length<%d>", id, MAX_ID_LEN);
        }
    }
    int32_t symbolIdx = indexFromSymbolTable(&(om->varNames), name, size);
    if (-1 == symbolIdx)
    {
        // 添加变量名和变量值 原子操作 保持索引一致性
        symbolIdx = addSymbol(vm, &(om->varNames), name, size);
        ValueBufferPut(vm, &(om->varValue), v);
    }
    else if (VALUE_IS_NUMBER(om->varValues.datas[symbolIdx]))
    {
        // 预先声明过的模块变量 模型也是一种 不是重定义
        // 语法分析时会将变量名首次出现的行号写入缓冲区
        // 此时的值并不代表变量的值，只是供语法分析使用
        // 变量赋值是在运行阶段完成的，此时是在语法分析
        // 所以可以用这种隐式方法作为模块变量的预先声明
        om->varValues.datas[idx] = v;
    }
    else
    {
        // 重定义
        symbolIdx = -1;
    }
}