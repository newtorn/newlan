#include "objmap.h"
#include "objmodel.h"
#include "vm.h"
#include "objstring.h"
#include "objrange.h"

// 创建哈希表对象
ObjectMap *makeObjectMap(VM *vm)
{
    ObjectMap *om = ALLOCATE(vm, ObjectMap);
    if (NULL != om)
    {
        initObjectHeader(vm, &(om->objectHeader), OT_MAP, vm->mapModel);
        om->cap = om->cnt = 0;
        om->entries = NULL;
    }
    else
    {
        MEM_ERROR("allocate ObjectMap failed");
    }
    return om;
}

// 计算数字哈希码
static uint32_t hashNumber(double n)
{
    Bits64 bits64;
    bits64.number = n;
    return bits64.bits32[0] ^ bits64.bits32[1];
}

// 计算对象的哈希码
static uint32_t hashObject(ObjectHeader *oh)
{
    switch (oh->type)
    {
    case OT_MODEL:
        return hashString(((Model *)oh)->name.value.start, ((Model *)oh)->name.value.size);

    case OT_RANGE:
        return hashNumber(((ObjectRange *)oh)->start) ^ hashNumber(((ObjectRange *)oh)->end);

    case OT_STRING:
        return ((ObjectString *)oh)->hashCode;

    default:
        RUNTIME_ERROR("the hashable are ObjectString, ObjectRange, ObjectClass");
    }
    return 0;
}

// 计算值的哈希码
static uint32_t hashValue(Value v)
{
    switch (v.type)
    {
    case VT_FALSE:
        return 0;

    case VT_TRUE:
        return 1;

    case VT_NONE:
        return 3;

    case VT_NUMBER:
        return hashNumber(v.number);

    case VT_OBJECT:
        return hashObject(v.objectHeader);

    default:
        RUNTIME_ERROR("unsupport type hashed");
    }
    return 0;
}

// 在键值对链上添加键值对，如果是新键返回true
static bool addEntry(Entry *entries, uint32_t cap, Value k, Value v)
{
    uint32_t idx = hashValue(k) % cap;
    while (true)
    {
        if (entries[idx].key.type == VT_UNDEFINED)
        {
            entries[idx].key = k;
            entries[idx].value = v;
            return true;
        }
        else if (valueIsEqual(entries[idx].key, k))
        {
            entries[idx].value = v;
            return false;
        }
        idx = (idx + 1) % cap;
    }
}

// 调整哈希表容量
static void mapResize(VM *vm, ObjectMap *om, uint32_t newCap)
{
    Entry *ne = ALLOCATE_ARRAY(vm, Entry, newCap);
    if (NULL != ne)
    {
        uint32_t idx = 0;
        while (idx < newCap)
        {
            ne[idx].key = VT_TO_VALUE(VT_UNDEFINED);
            ne[idx].value = VT_TO_VALUE(VT_FALSE);
            ++idx;
        }
        if (om->cap > 0)
        {
            Entry *oe = om->entries;
            idx = 0;
            while (idx < om->cap)
            {
                if (oe[idx].key.type != VT_UNDEFINED)
                {
                    addEntry(ne, newCap, oe[idx].key, oe[idx].value);
                }
                ++idx;
            }
        }
        DEALLOCATE_ARRAY(vm, om->entries, om->cnt);
        om->entries = ne;
        om->cap = newCap;
    }
    else
    {
        MEM_ERROR("allocate Entry failed");
    }
}

// 根据键在键值对链查找键值对
static Entry *findEntryByKey(ObjectMap *om, Value k)
{
    if (0 == om->cap)
    {
        return NULL;
    }
    uint32_t idx = hashValue(k) % om->cap;
    Entry *e = NULL;
    while (true)
    {
        e = &(om->entries[idx]);

        if (valueIsEqual(e->key, k))
        {
            return e;
        }

        if (VALUE_IS_UNDEFINED(e->key) && VALUE_IS_FALSE(e->value))
        {
            return NULL;
        }

        idx = (idx + 1) % om->cap;
    }
}

// 根据值在键值对链查找键值对
static Entry *findEntryByValue(ObjectMap *om, Value v)
{
    if (0 == om->cap)
    {
        return NULL;
    }
    uint32_t idx = 0;
    Entry *e = NULL;
    while (idx < om->cnt)
    {
        e = &(om->entries[idx++]);

        if (valueIsEqual(e->value, v))
        {
            return e;
        }
    }
    return NULL;
}

// 设置键对应的值
void mapSet(VM *vm, ObjectMap *om, Value k, Value v)
{
    if (om->cnt + 1 > om->cap * MAP_LOAD_PRECENT)
    {
        uint32_t newCap = om->cap * CAP_GROW_FACTOR;
        if (newCap < MIN_CAP)
        {
            newCap = MIN_CAP;
        }
        mapResize(vm, om, newCap);
    }
    if (addEntry(om->entries, om->cap, k, v))
    {
        om->cnt++;
    }
}

// 获取键对应的值
Value mapGetByKey(ObjectMap *om, Value k)
{
    Entry *e = findEntryByKey(om, k);
    if (NULL == e)
    {
        return VT_TO_VALUE(VT_UNDEFINED);
    }
    return e->value;
}

// 获取值对应的键
Value mapGetByValue(ObjectMap *om, Value v)
{
    Entry *e = findEntryByValue(om, v);
    if (NULL == e)
    {
        return VT_TO_VALUE(VT_UNDEFINED);
    }
    return e->key;
}

bool mapHasKey(ObjectMap *om, Value k)
{
    Entry *e = findEntryByKey(om, k);
    return NULL != e;
}

bool mapHasValue(ObjectMap *om, Value v)
{
    Entry *e = findEntryByValue(om, v);
    return NULL != e;
}

// 哈希表清空
void mapClear(VM *vm, ObjectMap *om)
{
    DEALLOCATE_ARRAY(vm, om->entries, om->cnt);
    om->entries = NULL;
    om->cap = om->cnt = 0;
}

// 删除键对应的键值对 伪删除
Value mapRemove(VM *vm, ObjectMap *om, Value k)
{
    Entry *e = findEntryByKey(om, k);
    if (NULL == e)
    {
        return VT_TO_VALUE(VT_NONE);
    }

    Value v = e->value;
    e->key = VT_TO_VALUE(VT_UNDEFINED);
    e->value = VT_TO_VALUE(VT_TRUE);

    om->cnt--;
    if (0 == om->cnt)
    {
        mapClear(vm, om);
    }
    else if (om->cnt > MIN_CAP &&
             om->cnt < (om->cap / CAP_GROW_FACTOR * MAP_LOAD_PRECENT))
    {
        // 当空间利用率太低时调整容量
        uint32_t newCap = om->cap;
        if (newCap < MIN_CAP)
        {
            newCap = MIN_CAP;
        }
        mapResize(vm, om, newCap);
    }
    return v;
}