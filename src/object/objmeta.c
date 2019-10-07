#include "objmeta.h"
#include "objfunction.h"
#include "vm.h"
#include "objmodel.h"
#include <string.h>

// 创建模块
ObjectModule *makeObjectModule(VM *vm, const char *name)
{
    ObjectModule *om = ALLOCATE(vm, ObjectModule);
    if (NULL != om)
    {
        initObjectHeader(vm, &(om->objectHeader), OT_MODULE, NULL);

        StringBufferInit(&(om->varNames));
        ValueBufferInit(&(om->varValues));

        om->name = NULL; // 核心模块名为空
        if (NULL != name)
        {
            om->name = makeObjectString(vm, name, strlen(name));
        }
    }
    else
    {
        MEM_ERROR("allocate ObjectModule failed");
    }
    return om;
}

// 创建模型实例
ObjectInstance *makeObjectInstance(VM *vm, Model *model)
{
    ObjectInstance *oi = ALLOCATE_FLEX(vm, ObjectInstance, sizeof(Value) * (model->attrNum));
    if (NULL != oi)
    {
        initObjectHeader(vm, &(oi->objectHeader), OT_INSTANCE, model);
        uint32_t idx = 0;
        while (idx < model->attrNum)
        {
            oi->attrs[idx++] = VT_TO_VALUE(VT_NONE);
        }
    }
    else
    {
        MEM_ERROR("allocate ObjectInstance failed");
    }
    return oi;
}