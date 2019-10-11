#include "objmodel.h"
#include "common.h"
#include "objrange.h"
#include "core.h"
#include "vm.h"
#include <string.h>

// 定义行为缓冲区方法
DEFINE_BUFFER_FUNC(Action)

// 判断两个值是否相等
bool valueIsEqual(Value a, Value b)
{
    if (a.type != b.type)
    {
        return false;
    }

    if (a.type == VT_NUMBER)
    {
        return a.number == b.number;
    }

    if (a.objectHeader == b.objectHeader)
    {
        return true;
    }

    if (a.objectHeader->type != b.objectHeader->type)
    {
        return false;
    }

    if (a.objectHeader->type == OT_STRING)
    {
        ObjectString *sa = VALUE_TO_STRING(a);
        ObjectString *sb = VALUE_TO_STRING(b);
        return (sa->value.size == sb->value.size) &&
               (0 == memcmp(sa->value.start, sb->value.start, sa->value.size));
    }

    if (a.objectHeader->type == OT_RANGE)
    {
        ObjectRange *ra = VALUE_TO_RANGE(a);
        ObjectRange *rb = VALUE_TO_RANGE(b);
        return (ra->start == rb->start) && (ra->end == rb->end);
    }

    return false;
}

// 创建一个原始模型 原始模型没有元模型
Model* makeRawModel(VM *vm, const char *name, uint32_t attrCnt)
{
    Model *model = ALLOCATE(vm, Model);
    if (model != NULL)
    {
        initObjectHeader(vm, &(model->objectHeader), OT_MODEL, NULL);
        model->name = makeObjectString(vm, name, strlen(name));
        model->attrCnt = attrCnt;
        
        // 默认没有父模型
        model->superModel = NULL;
        ActionBufferInit(&(model->actions));
    }
    else
    {
        MEM_ERROR("allocate Model failed");
    }
    return model;
}

// 获取对象所属的模型
inline Model* getModelOfObject(VM *vm, Value object)
{
    switch (object.type)
    {
    case VT_NONE:
        return vm->noneModel;
    case VT_TRUE:
    case VT_FALSE:
        return vm->boolModel;
    case VT_NUMBER:
        return vm->numberModel;
    case VT_OBJECT:
        return VALUE_TO_OBJECT(object)->model;
    default:
        NOT_REACHED();
    }
    return NULL;
}
