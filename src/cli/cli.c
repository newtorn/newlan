#include "cli.h"
#include "vm.h"
#include "core.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

// void runTest()
// {
// test_utils();
// test_utf8();
// }

static void runFile(const char *path)
{
    // uint32_t i;
    // const char sep[] = {'/', '\\', '\0'};
    // const char *lastSlash = NULL;

    // i = 0;
    // while ('\0' != *(sep + i))
    // {
    //     lastSlash = strrchr(path, *(sep + i++));
    //     if (NULL != lastSlash)
    //     {
    //         i = ('\\' == *(sep + i) && '\\' == *(lastSlash + 1)) ? 1 : 0; // 处理双反斜杠
    //         char *root = (char *)malloc(lastSlash - path + 2 + i);
    //         memcpy(root, path, lastSlash - path + 1 + i);
    //         root[lastSlash - path + 1 + i] = '\0';
    //         rootDir = root;
    //         break;
    //     }
    // }

    const char *lastSlash = strrchr(path, '/');
    if (NULL != lastSlash)
    {
        char *root = (char *)malloc(lastSlash - path + 2);
        memcpy(root, path, lastSlash - path + 1);
        root[lastSlash - path + 1] = '\0';
        rootDir = root;
    }

    VM *vm = makeVM();
    const char *source = readFile(path);

    struct parser parser;
    initParser(vm, &parser, path, source);

    #include "token.list"
    while (TOKEN_EOF != parser.curToken.type)
    {
        getNextToken(&parser);
        printf("%dL: %s [", parser.curToken.lineNum, tokenReprs[parser.curToken.type]);
        uint32_t i = 0;
        while (i < parser.curToken.size)
        {
            printf("%c", *(parser.curToken.start + i++));
        }
        printf("]\n");
    }
}

int main(int argc, const char **argv)
{
    if (1 == argc)
    {
        ;
    }
    else
    {
        runFile(argv[1]);
    }
    return 0;
}