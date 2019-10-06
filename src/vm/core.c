#include "core.h"
#include "vm.h"
#include "utils.h"
#include <string.h>
#include <sys/stat.h>

// 根目录
char *rootDir = NULL;

// 读取源文件
char *readFile(const char *path)
{
    FILE *fptr = fopen(path, "r");
    if (NULL == fptr)
    {
        IO_ERROR("Could not open file \"%s\".\n", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = fileStat.st_size;
    char *fileData = (char *)malloc(fileSize + 1);
    if (NULL == fileData)
    {
        MEM_ERROR("Could not allocate memory for reading file \"%s\".\n", path);
    }

    size_t readSize = fread(fileData, sizeof(char), fileSize, fptr);
    if (readSize < fileSize)
    {
        IO_ERROR("Could not read file \"%s\".\n", path);
    }
    fileData[fileSize] = '\0';

    fclose(fptr);
    return fileData;
}