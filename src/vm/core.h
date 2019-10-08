#ifndef _VM_CORE_H
#define _VM_CORE_H
#include "vm.h"

// 源码文件根目录
extern char *rootDir;

// 源代码读取函数声明
char *readFile(const char *path);

// 执行模块函数声明
VMResult execModule(VM* vm, Value moduleName, const char* moduleCode);

// 核心模块代码构建函数声明
void buildCore(VM *vm);

#endif