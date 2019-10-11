#include "compiler.h"
#include "parser.h"
#include "core.h"
#include <string.h>
#if DEBUG
#include "debug.h"
#endif

// 编译单元
struct compileUnit
{
    ObjectFunction *func;                  // 编译的函数
    LocalVar localVars[MAX_LOCAL_VAR_NUM]; // 局部变量表
    uint32_t localVarCnt;                  // 已分配的局部变量
    Upvalue upvalues[MAX_UPVALUE_NUM];     // 当前函数引用的上值
    int32_t scopeDepth;                    // 当前编译的代码所处作用域
    uint32_t stackSlotCnt;                 // 当前使用的栈槽位数量
    Loop *curLoop;                         // 当前编译的循环层
    ModelBook *enclosedModelBook;          // 当前编译的模型的编译信息
    struct compileUnit *enclosedUnit;      // 包含当前编译单元的直接外层编译单元
    Parser *curParser;                     // 当前词法分析器
};

#define OPCODE_SLOTS(opCode, effect) effect,
static const int32_t opCodeSlotsUsed[] = {
#include "opcode.inc"
};
#undef OPCODE_SLOTS

// 初始化编译单元
static void initCompileUnit(Parser *parser, CompileUnit *cu, CompileUnit *enclosedUnit, bool isAction)
{
    parser->curCompileUnit = cu;
    cu->curParser = parser;
    cu->enclosedUnit = enclosedUnit;
    cu->curLoop = NULL;
    cu->enclosedModelBook = NULL;
    if (enclosedUnit == NULL)
    {
        // 如果没有外层，则属于模块作用域
        cu->scopeDepth = -1;
        cu->localVarCnt = 0;
    }
    else
    {
        // 如果是内层，则属于局部作用域
        if (isAction)
        {
            // 若为模型中的行为 设定this为第0个变量
            // this即实例对象 它是行为的消息接收者
            cu->localVars[0].name = "this";
            cu->localVars[0].size = 4;
        }
        else
        {
            // 若为普通函数 空出第0个变量 保持统一
            cu->localVars[0].name = NULL;
            cu->localVars[0].size = 0;
        }
        // 第0个变量特殊照顾 将其设为模块作用域级别
        cu->localVars[0].scopeDepth = -1;
        cu->localVars[0].isUpvalue = false;
        cu->localVarCnt = 1;

        // 对于普通函数和模块行为 初始作用域为局部作用域
        // 0 为局部作用域的最外层作用域
        cu->scopeDepth = 0;
    }
    // 局部变量在栈中 栈已用数量等于局部变量数
    cu->stackSlotCnt = cu->localVarCnt;
    cu->func = makeObjectFunction(cu->curParser->vm, cu->curParser->curModule, cu->localVarCnt);
}

// 向指令流中写入1字节 并返回写入的索引
static int32_t writeByte(CompileUnit *cu, int32_t byte)
{
#if DEBUG
    IntBufferPut(cu->curParser->vm, &(cu->func->debug->lineNum), cu->curParser->preToken.lineNum);
#endif
    ByteBufferPut(cu->curParser->vm, &(cu->func->instrStream), (uint8_t)byte);
    return cu->func->instrStream.cnt - 1;
}

// 写入操作码
static void writeOpCode(CompileUnit *cu, OpCode opCode)
{
    writeByte(cu, opCode);
    cu->stackSlotCnt += opCodeSlotsUsed[opCode];
    if (cu->stackSlotCnt > cu->func->maxStackSlot)
    {
        cu->func->maxStackSlot = cu->stackSlotCnt;
    }
}

// 写入一个字节的操作数 返回写入的索引
static int32_t writeByteOperand(CompileUnit *cu, int32_t operand)
{
    return writeByte(cu, operand);
}

// 写入2字节操作数 大端序写入 返回写入的低字节索引
inline static int32_t writeShortOperand(CompileUnit *cu, int32_t operand)
{
    writeByte(cu, (operand >> 8) & 0xff); // 先写高8位
    return writeByte(cu, operand & 0xff); // 再写低8位
}

// 写入操作数为1字节大小的指令
static int32_t writeOpCodeByteOperand(CompileUnit *cu, OpCode opCode, int32_t operand)
{
    writeOpCode(cu, opCode);
    return writeByteOperand(cu, operand);
}

// 写入操作数为2字节大小的指令
static int32_t writeOpCodeShortOperand(CompileUnit *cu, OpCode opCode, int32_t operand)
{
    writeOpCode(cu, opCode);
    return writeShortOperand(cu, operand);
}

// 定义模块变量 重定义返回 -1
int32_t defineModuleVar(VM *vm, ObjectModule *om, const char *name, uint32_t size, Value v)
{
    if (size > MAX_ID_LEN)
    {
        char id[MAX_ID_LEN] = {'\0'};
        memcpy(id, name, size);
        if (vm->curParser != NULL)
        {
            COMPILE_ERROR(vm->curParser, "identifier \"%s>\" is out of max length<%d>", id, MAX_ID_LEN);
        }
        else
        {
            MEM_ERROR("identifier \"%s\" is out of max length<%d>", id, MAX_ID_LEN);
        }
    }
    int32_t symbolIdx = indexFromSymbolTable(&(om->varNames), name, size);
    if (symbolIdx == -1)
    {
        // 添加变量名和变量值 原子操作 保持索引一致性
        symbolIdx = addSymbol(vm, &(om->varNames), name, size);
        ValueBufferPut(vm, &(om->varValues), v);
    }
    else if (VALUE_IS_NUMBER(om->varValues.datas[symbolIdx]))
    {
        // 预先声明过的模块变量 模型也是一种 不是重定义
        // 语法分析时会将变量名首次出现的行号写入缓冲区
        // 此时的值并不代表变量的值，只是供语法分析使用
        // 变量赋值是在运行阶段完成的，此时是在语法分析
        // 所以可以用这种隐式方法作为模块变量的预先声明
        om->varValues.datas[symbolIdx] = v;
    }
    else
    {
        // 重定义
        symbolIdx = -1;
    }
}

// 编译程序
static void compileProgram(CompileUnit *cu)
{
    ;
}

// 编译模块 返回模块调用入口
ObjectFunction *compileModule(VM *vm, ObjectModule *om, const char *moduleCode)
{
    Parser parser;
    parser.parent = vm->curParser;
    vm->curParser = &parser;

    if (om->name == CORE_MODULE_STR_NAME)
    {
        initParser(vm, &parser, CORE_MODULE_CODE_PATH, moduleCode, om);
    }
    else
    {
        initParser(vm, &parser, (const char *)(om->name->value.start), moduleCode, om);
    }
    
    CompileUnit mcu;
    initCompileUnit(&parser, &mcu, NULL, false);

    uint32_t moduleVarCntBefore = om->varValues.cnt;

    getNextToken(&parser);

    while (!matchToken(&parser, TOKEN_EOF))
    {
        compileProgram(&mcu);
    }

    return NULL;
}
