#include "vm.h"
#include "utils.h"
#include <stdlib.h>

void initVM(VM *vm)
{
    vm->allBytes = 0;
    vm->allObjects = NULL;
    vm->curParser = NULL;
}

VM *makeVM()
{
    VM *vm = (VM *)malloc(sizeof(VM));
    if (NULL == vm)
    {
        MEM_ERROR("allocate VM failed!\n");
    }
    initVM(vm);
    return vm;
}