#include "objlist.h"

// 列表对象创建函数声明
ObjectList *makeObjectList(VM *vm, uint32_t elemCnt)
{
    Value *ea = NULL;
    if (elemCnt > 0)
    {
        ea = ALLOCATE_ARRAY(vm, Value, elemCnt);
    }
    ObjectList *ol = ALLOCATE(vm, ObjectList);
    if (elemCnt > 0 && NULL == ea)
    {
        MEM_ERROR("allocate Value failed");
    }
    else if (NULL == ol)
    {
        MEM_ERROR("allocate ObjectList failed");
    }
    else
    {
        ol->elements.datas = ea;
        ol->elements.cap = ol->elements.cnt = elemCnt;
        initObjectHeader(vm, &(ol->objectHeader), OT_LIST, vm->listModel);
    }
    return ol;
}

// 向idx位置插入元素
void insertElement(VM *vm, ObjectList *ol, uint32_t idx, Value e)
{
    if (idx > ol->elements.cnt - 1)
    {
        RUNTIME_ERROR("list index out of range");
    }

    ValueBufferPut(vm, &(ol->elements), VT_TO_VALUE(VT_NONE));
    uint32_t i = ol->elements.cnt - 1;
    while (i > idx)
    {
        ol->elements.datas[i] = ol->elements.datas[i - 1];
        i--;
    }
    ol->elements.datas[idx] = e;
}

// 调整列表容量
static void shrinkList(VM *vm, ObjectList *ol, uint32_t newCap)
{
    uint32_t oldSize = ol->elements.cap * sizeof(Value);
    uint32_t newSize = newCap * sizeof(Value);
    memCtl(vm, ol->elements.datas, oldSize, newSize);
    ol->elements.cap = newCap;
}

// 删除idx位置的元素
Value removeElement(VM *vm, ObjectList *ol, uint32_t idx)
{
    if (idx > ol->elements.cnt - 1)
    {
        RUNTIME_ERROR("list index out of range");
    }
    Value vr = ol->elements.datas[idx];
    uint32_t i = idx;
    while (i < ol->elements.cnt)
    {
        ol->elements.datas[i] = ol->elements.datas[i + 1];
        i++;
    }
    uint32_t _cap = ol->elements.cap / CAP_GROW_FACTOR;
    if (_cap > ol->elements.cnt)
    {
        shrinkList(vm, ol, _cap);
    }
    ol->elements.cnt--;
    return vr;
}