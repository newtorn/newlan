#include "vm.h"
#include "core.h"
#include "utils.h"
#include <stdlib.h>

void initVM(VM *vm)
{
    vm->allBytes = 0;
    vm->allObjects = NULL;
    StringBufferInit(&(vm->allActionNames));
    vm->allModules = makeObjectMap(vm);
    vm->curParser = NULL;
}

VM *makeVM()
{
    VM *vm = (VM *)malloc(sizeof(VM));
    if (vm == NULL)
    {
        MEM_ERROR("allocate VM failed");
    }
    initVM(vm);
    buildCore(vm);
    return vm;
}