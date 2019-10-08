#include "objset.h"
#include "vm.h"

// 创建集合对象
ObjectSet *makeObjectSet(VM *vm)
{
    ObjectSet *os = ALLOCATE(vm, ObjectSet);
    ObjectMap *map = makeObjectMap(vm);
    if (NULL != os)
    {
        initObjectHeader(vm, &(os->objectHeader), OT_SET, vm->setModel);
        initObjectHeader(vm, os->PRESENT.objectHeader, OT_MODEL, vm->objectModel);
        os->map = map;
    }
    else
    {
        MEM_ERROR("allocate ObjectSet failed");
    }
    return os;
}

// 添加元素到集合
void setAdd(VM *vm, ObjectSet *os, Value v)
{
    mapSet(vm, os->map, v, os->PRESENT);
}

// 是否含有该元素
bool setHas(ObjectSet *os, Value v)
{
    Value val = mapGet(os->map, v);
    if (VALUE_IS_UNDEFINED(val))
    {
        return false;
    }
    return valueIsEqual(val, os->PRESENT);
}

// 清空集合
void setClear(VM *vm, ObjectSet *os)
{
    mapClear(vm, os->map);
}
