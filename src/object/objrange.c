#include "objrange.h"
#include "utils.h"
#include "objmodel.h"
#include "vm.h"

// 创建范围对象
ObjectRange *makeObjectRange(VM *vm, int32_t start, int32_t end)
{
    ObjectRange *or = ALLOCATE(vm, ObjectRange);
    if (NULL != or)
    {
        initObjectHeader(vm, &(or->objectHeader), OT_RANGE, vm->rangeModel);
        or->start = start;
        or->end = end;
    }
    else
    {
        MEM_ERROR("allocate ObjectRange failed");
    }
    return or;
}