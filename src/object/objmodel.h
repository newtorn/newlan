#ifndef _OBJECT_OBJMODEL_H
#define _OBJECT_OBJMODEL_H
#include "common.h"
#include "utils.h"
#include "objheader.h"
#include "objstring.h"
#include "objfunction.h"

// 行为类型
typedef enum
{
    AT_NONE,      // 空行为
    AT_PRIMITIVE, // 原生类型，C实现
    AT_SCRIPT,    // 脚本中定义的行为
    AT_FUNC_CALL  // 函数调用，用来实现重载
} ActionType;

// 自定义类型和结构内置转换
// 提供默认零值
#define VT_TO_VALUE(vt) ((Value){vt, {0}})

#define BOOL_TO_VALUE(bl) ((bl) ? VT_TO_VALUE(VT_TRUE) : VT_TO_VALUE(VT_FALSE))
#define VALUE_TO_BOOL(v) ((v).type == VT_TRUE ? true : false)

#define NUMBER_TO_VALUE(num) ((Value){VT_NUMBER, {num}})
#define VALUE_TO_NUMBER(v) ((v).number)

#define OBJECT_TO_VALUE(obj) ({Value v; v.type = VT_OBJECT; v.objectHeader = (ObjectHeader*)(obj); v; })

#define VALUE_TO_OBJECT(v) ((v).objectHeader)
#define VALUE_TO_STRING(v) ((ObjectString *)VALUE_TO_OBJECT(v))
#define VALUE_TO_FUNCTION(v) ((ObjectFunction *)VALUE_TO_OBJECT(v))
#define VALUE_TO_CLOSURE(v) ((ObjectClosure *)VALUE_TO_OBJECT(v))
#define VALUE_TO_MODEL(v) ((Model *)VALUE_TO_OBJECT(v))

#define VALUE_IS_UNDEFINED(v) ((v).type == VT_UNDEFINED)
#define VALUE_IS_NONE(v) ((v).type == VT_NONE)
#define VALUE_IS_TRUE(v) ((v).type == VT_TRUE)
#define VALUE_IS_FALSE(v) ((v).type == VT_FALSE)
#define VALUE_IS_NUMBER(v) ((v).type == VT_NUMBER)
#define VALUE_IS_OBJECT(v) ((v).type == VT_OBJECT)
#define VALUE_IS_TYPE(v, t) (VALUE_IS_OBJECT(v) && VALUE_TO_OBJECT(v)->type == t)
#define VALUE_IS_STRING(v) (VALUE_IS_TYPE(v, OT_STRING))
#define VALUE_IS_INSTANCE(v) (VALUE_IS_TYPE(v, OT_INSTANCE))
#define VALUE_IS_CLOSURE(v) (VALUE_IS_TYPE(v, OT_CLOSURE))
#define VALUE_IS_RANGE(v) (VALUE_IS_TYPE(v, OT_RANGE))
#define VALUE_IS_MODEL(v) (VALUE_IS_TYPE(v, OT_MODEL))
#define VALUE_IS_ZERO(v) (VALUE_IS_NUMBER(v) && (v).number == 0)

// 定义原生函数指针
typedef bool (*Primitive)(VM *vm, Value *args);

// 行为结构
typedef struct
{
    ActionType type;
    union {
        Primitive prim;    // 脚本函数关联C实现
        ObjectClosure *oc; // 指向编译后的ObjectClosure或ObjectFunction
    };
} Action;

// 定义行为缓冲区类型
DECLARE_BUFFER_TYPE(Action)

// 模型结构
struct model
{
    ObjectHeader objectHeader; // 对象头
    struct model *superModel; // 父模型
    uint32_t attrNum; // 属性数，包括父模型
    ActionBuffer actions; // 模型行为
    ObjectString name; // 模型名称
};

// 存储64位数据结构
typedef union
{
    uint64_t bits64;
    uint32_t bits32[2];
    double number;
} Bits64;

// 列表和字典的扩容系数
#define CAP_GROW_FACTOR 4

// 最小容量
#define MIN_CAP 64

#endif