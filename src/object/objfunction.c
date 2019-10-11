#include "objfunction.h"
#include "objmeta.h"
#include "objmodel.h"
#include "vm.h"

// 创建函数对象
ObjectFunction *makeObjectFunction(VM *vm, ObjectModule *om, uint32_t maxStackSlot)
{
    ObjectFunction *of = ALLOCATE(vm, ObjectFunction);
    if (of != NULL)
    {
        initObjectHeader(vm, &(of->objectHeader), OT_FUNCTION, vm->functionModel);
        ByteBufferInit(&(of->instrStream));
        ValueBufferInit(&(of->constants));
        of->module = om;
        of->maxStackSlot = maxStackSlot;
        of->upvalueCnt = of->argc = 0;
        #ifdef DEBUG
        of->debug = ALLOCATE(vm, FunctionDebug);
        of->debug->funcName = NULL;
        IntBufferInit(&(of->debug->lineNum));
        #endif
    }
    else
    {
        MEM_ERROR("allocate ObjectFunction failed");
    }
    return of;
}

// 创建闭包对象
ObjectClosure *makeObjectClosure(VM *vm, ObjectFunction *of)
{
    ObjectClosure *oc = ALLOCATE_FLEX(vm, ObjectClosure, sizeof(ObjectUpvalue) * (of->argc));
    if (oc != NULL)
    {
        initObjectHeader(vm, &(oc->objectHeader), OT_CLOSURE, vm->functionModel);
        oc->func = of;
        uint32_t idx = 0;
        while (idx < of->upvalueCnt)
        {
            oc->upvalues[idx++] = NULL;
        }
    }
    else
    {
        MEM_ERROR("allocate ObjectClosure failed");
    }
    return oc;
}

// 创建上值对象
ObjectUpvalue *makeObjectUpvalue(VM *vm, Value *localVar)
{
    ObjectUpvalue *ou = ALLOCATE(vm, ObjectUpvalue);
    if (ou != NULL)
    {
        initObjectHeader(vm, &(ou->objectHeader), OT_UPVALUE, NULL);
        ou->localVar = localVar;
        ou->closed = VT_TO_VALUE(VT_NONE);
        ou->next = NULL;
    }
    else
    {
        MEM_ERROR("allocate ObjectUpvalue failed");
    }
    return ou;
}