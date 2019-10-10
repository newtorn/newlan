#ifndef _VM_CORE_H
#define _VM_CORE_H
#include "vm.h"
#include "objmodel.h"

// 源码文件根目录
extern char *rootDir;

// 核心模块名
#define CORE_MODULE VT_TO_VALUE(VT_NONE)

// 源代码读取函数声明
char *readFile(const char *path);

// 执行模块函数声明
VMResult execModule(VM *vm, Value moduleName, const char *moduleCode);

// 符号所在符号表中的索引查找函数声明
int32_t indexFromSymbolTable(SymbolTable* st, const char* symbol, uint32_t size);

// 返回符号添加在符号表中的索引函数声明
int32_t addSymbol(VM* vm, SymbolTable* st, const char* symbol, uint32_t size);

// 核心模块代码构建函数声明
void buildCore(VM *vm);

// 绑定行为
void bindAction(VM* vm, Model* model, uint32_t idx, Action action);

// 绑定父模型
void bindSuperModel(VM* vm, Model* subModel, Model* superModel);

#endif