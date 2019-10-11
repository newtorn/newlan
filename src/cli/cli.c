#include "cli.h"
#include "vm.h"
#include "core.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

// #include "test.h"
// void runTest()
// {
//     test_utils();
//     test_utf8();
// }

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

    // struct parser parser;
    // initParser(vm, &parser, path, source, NULL);

    // #include "token.list"
    // while (TOKEN_EOF != parser.curToken.type)
    // {
    //     getNextToken(&parser);
    //     printf("%dL: %s [", parser.curToken.lineNum, tokenReprs[parser.curToken.type]);
    //     uint32_t i = 0;
    //     while (i < parser.curToken.size)
    //     {
    //         printf("%c", *(parser.curToken.start + i++));
    //     }
    //     printf("]\n");
    // }

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