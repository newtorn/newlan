#include "vm.h"
#include "utils.h"
#include "core.h"
#include <string.h>
#include <sys/stat.h>

// 根目录
char *rootDir = NULL;

char *readSourceFile(const char *path)
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