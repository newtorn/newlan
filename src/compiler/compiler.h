#ifndef _COMPILER_COMPILER_H
#define _COMPILER_COMPILER_H
#include "objfunction.h"

// 局部变量最大数量
#define MAX_LOCAL_VAR_NUM 128

// 上值最大数量
#define MAX_UPVALUE_NUM 128

// 标识符最大长度
#define MAX_ID_LEN 128

// 行为名最大长度
#define MAX_ACTION_NAME_LEN MAX_ID_LEN

// 参数最大数量
#define MAX_ARG_NUM 16

// 签名最大长度
#define MAX_SIGN_LEN MAX_ACTION_NAME + MAX_ARG_NUM * 2 + 1

// 属性最大数量
#define MAX_ATTR_NUM 128

// 上值结构
typedef struct
{
    bool isEnclosedLocalVar; // 是否是直接外层函数的局部变量
    uint32_t idx;            // 外层函数局部变量或上值的索引
} Upvalue;

// 局部变量
typedef struct
{
    const char *name;   // 变量名
    uint32_t size;      // 变量名长度
    int32_t scopeDepth; // 变量作用域
    bool isUpvalue;     // 是否是上值
} LocalVar;

// 签名类型
typedef enum
{
    ST_CONSTRUCT,        // 构造行为
    ST_ACTION,           // 普通行为
    ST_GETTER,           // getter行为
    ST_SETTER,           // setter行为
    ST_SUBSCRIPT_GETTER, // getter下标引用
    ST_SUBSCRIPT_SETTER, // setter下标引用
} SignatureType;

// 签名结构
typedef struct
{
    SignatureType *type; // 签名类型
    const char *name;    // 签名串
    uint32_t size;       // 签名长度
    uint32_t argc;       // 参数个数
} Signature;

// 循环结构
typedef struct loop
{
    int32_t condStartIdx;      // 循环条件的地址
    int32_t bodyStartIdx;      // 循环体的地址
    int32_t scopeDepth;        // break后进入的作用域
    int32_t exitIdx;           // 循环体退出时的目标地址
    struct loop *enclosedLoop; // 外层循环
} Loop;

// 模型编译时信息记录结构
typedef struct
{
    ObjectString *name;       // 模型名
    SymbolTable attrs;        // 模型属性符号表
    bool inStatic;            // 是否正在编译静态方法
    IntBuffer instantActions; // 实例行为
    IntBuffer staticActions;  // 静态行为
    Signature *signature;     // 正在编译的签名
} ModelBook;

// 编译单元声明
typedef struct compileUnit CompileUnit;

// 定义模块变量
int32_t defineModuleVar(VM *vm, ObjectModule *om, const char *name, uint32_t size, Value v);

// 编译模块 返回模块调用入口
ObjectFunction *compileModule(VM *vm, ObjectModule *om, const char *moduleCode);

#endif