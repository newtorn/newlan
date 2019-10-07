#include "objstring.h"
#include "vm.h"
#include "utils.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

// 字符串哈希值，FNV-1a算法
uint32_t hashString(char *str, uint32_t size)
{
    uint32_t hashCode = OFFSET_BASIS, idx = 0;
    while (idx < size)
    {
        hashCode ^= str[idx++];
        hashCode *= FNV_PRIME_BIT32;
    }
    return hashCode;
}

// 字符串对象进行哈希
void hashObjectString(ObjectString *os)
{
    os->hashCode = hashString(os->value.start, os->value.size);
}

ObjectString *makeObjectString(VM *vm, const char *str, uint32_t size)
{
    // size为0且str为NULL 或 size不为0且str不为NULL
    ASSERT(0 == size || NULL != str, "string size does not match string");

    ObjectString *os = ALLOCATE_FLEX(vm, ObjectString, size + 1);
    if (NULL != os)
    {
        initObjectHeader(vm, &(os->objectHeader), OT_STRING, vm->stringModel);
        os->value.size = size;
        if (size > 0)
        {
            memcpy(os->value.start, str, size);
        }
        os->value.start[size] = '\0';
        hashObjectString(os);
    }
    else
    {
        MEM_ERROR("allocate ObjectString falied");
    }
    return os;
}