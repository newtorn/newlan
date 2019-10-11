#include "objthread.h"
#include "objmodel.h"
#include "vm.h"

// 为调用的函数运行时准备调用帧
// 线程栈由所有调用帧栈连续构成
// sits表示在线程栈中的起始地址
// sits(start in the thread stack)
void prepareFrame(ObjectThread *ot, ObjectClosure *oc, Value *sits)
{
    ASSERT(ot->frameCap > ot->frameCnt, "frame not enough");
    Frame *frame = &(ot->frames[ot->frameCnt++]);
    frame->start = sits;
    frame->closure = oc;
    frame->pc = oc->func->instrStream.datas;
}

// 重置线程
void threadReset(ObjectThread *ot, ObjectClosure *oc)
{
    ot->ebp = ot->esp;
    ot->openUpvalues = NULL;
    ot->caller = NULL;
    ot->errorObject = VT_TO_VALUE(VT_NONE);
    ot->frameCnt = 0;
    ASSERT(NULL != oc, "ObjectClosure is NULL in function threadReset");
    prepareFrame(ot, oc, ot->ebp);
}

// 创建线程
ObjectThread *makeObjectThread(VM *vm, ObjectClosure *oc)
{
    ASSERT(oc != NULL, "ObjectClosure is NULL");
    Frame *frames = ALLOCATE_ARRAY(vm, Frame, INITIAL_FRAME_NUM);

    // 加1供存储消息接收者
    uint32_t statckCap = roundUpToPowerOf2(oc->func->maxStackSlot + 1);
    
    Value *newStack = ALLOCATE_ARRAY(vm, Value, statckCap);
    ObjectThread *ot = ALLOCATE(vm, ObjectThread);
    if (frames != NULL && newStack != NULL && ot != NULL)
    {
        initObjectHeader(vm, &(ot->objectHeader), OT_THREAD, vm->threadModel);
        ot->frames = frames;
        ot->frameCap = INITIAL_FRAME_NUM;
        ot->ebp = newStack;
        ot->stackCap = statckCap;
        threadReset(ot, oc);
    }
    else
    {
        MEM_ERROR("allocate memory for ObjectThread failed");
    }
    return ot;
}