#include "core.h"
#include "vm.h"
#include "utils.h"
#include "objmodel.h"
#include <string.h>
#include <sys/stat.h>

// 根目录
char *rootDir = NULL;

#define CORE_MODULE VT_TO_VALUE(VT_NONE)

// 读取源文件
char *readFile(const char *path)
{
    FILE *fptr = fopen(path, "r");
    if (NULL == fptr)
    {
        IO_ERROR("could not open file \"%s\".", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = fileStat.st_size;
    char *fileData = (char *)malloc(fileSize + 1);
    if (NULL == fileData)
    {
        MEM_ERROR("could not allocate memory for reading file \"%s\"", path);
    }

    size_t readSize = fread(fileData, sizeof(char), fileSize, fptr);
    if (readSize < fileSize)
    {
        IO_ERROR("could not read file \"%s\"", path);
    }
    fileData[fileSize] = '\0';

    fclose(fptr);
    return fileData;
}

// 执行模块代码
VMResult execModule(VM *vm, Value moduleName, const char *moduleCode)
{
    return VMR_ERROR;
}

// 编译核心模块
void buildCore(VM *vm)
{
    // 核心模块名为空
    ObjectModule *coreModule = makeObjectModule(vm, NULL);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJECT_TO_VALUE(coreModule));
}