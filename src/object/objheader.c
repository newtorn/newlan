#include "objheader.h"
#include "vm.h"
#include "objmodel.h"

// 定义Value缓冲区类型方法
DEFINE_BUFFER_FUNC(Value)

// 初始化对象头
void initObjectHeader(VM *vm, ObjectHeader *oh, ObjectType ot, Model *model)
{
    oh->type = ot;
    oh->model = model;
    oh->accessible = false;
    oh->next = vm->allObjects;
    vm->allBytes = oh;
}
