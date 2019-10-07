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

    if (VT_NUMBER == a.type)
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