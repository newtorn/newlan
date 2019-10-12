#include "cli.h"
#include "vm.h"
#include "core.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

static void runFile(const char *path)
{
    uint32_t i;
    const char sep[] = {'/', '\\', '\0'};
    const char *lastSlash = NULL;

    i = 0;
    while (*(sep + i) != '\0')
    {
        lastSlash = strrchr(path, *(sep + i++));
        if (lastSlash != NULL)
        {
            i = (*(sep + i) == '\\' && *(lastSlash + 1) == '\\') ? 1 : 0; // 处理双反斜杠
            char *root = (char *)malloc(lastSlash - path + 2 + i);
            memcpy(root, path, lastSlash - path + 1 + i);
            root[lastSlash - path + 1 + i] = '\0';
            rootDir = root;
            break;
        }
    }

    VM *vm = makeVM();
    const char *source = readFile(path);

    execModule(vm, OBJECT_TO_VALUE(makeObjectString(vm, path, strlen(path))), source);
}

int main(int argc, const char **argv)
{
    if (argc == 1)
    {
        ;
    }
    else
    {
        runFile(argv[1]);
    }
    return 0;
}