#include "core.h"
#include "vm.h"
#include "utils.h"
#include "compiler.h"
#include <string.h>
#include <sys/stat.h>

// 根目录
char *rootDir = NULL;

// 返回值设置并返回
#define RETURN_VALUE(v) \
    do                  \
    {                   \
        args[0] = v;    \
        return true;    \
    } while (0);

// 返回各种类型
#define RETURN_OBJECT(obj) RETURN_VALUE(OBJECT_TO_VALUE(obj))
#define RETURN_BOOL(bl) RETURN_VALUE(BOOL_TO_VALUE(bl))
#define RETURN_NUMBER(num) RETURN_VALUE(NUMBER_TO_VALUE(num))
#define RETURN_NONE RETURN_VALUE(VT_TO_VALUE(VT_NONE))
#define RETURN_TRUE RETURN_TRUE(VT_TO_VALUE(VT_TRUE))
#define RETURN_FALSE RETURN_FALSE(VT_TO_VALUE(VT_FALSE))

// 设置线程报错
#define SET_ERROR_FALSE(vm, err)                                     \
    do                                                               \
    {                                                                \
        vm->curThread->errorObject =                                 \
            OBJECT_TO_VALUE(makeObjectString(vm, err, strlen(err))); \
        return false;                                                \
    } while (0);

// 绑定原生函数到模型
#define PRIM_FUNC_BIND(model, actionName, primFunc)                                        \
    {                                                                                      \
        uint32_t size = strlen(actionName);                                                \
        int32_t globalIdx = indexFromSymbolTable(&(vm->allActionNames), actionName, size); \
        if (globalIdx == -1)                                                               \
        {                                                                                  \
            globalIdx = addSymbol(vm, &(vm->allActionNames), actionName, size);            \
        }                                                                                  \
        Action action;                                                                     \
        action.type = AT_PRIMITIVE;                                                        \
        action.prim = primFunc;                                                            \
        bindAction(vm, model, (uint32_t)globalIdx, action);                                \
    }

// 读取源文件
char *readFile(const char *path)
{
    FILE *fptr = fopen(path, "r");
    if (fptr == NULL)
    {
        IO_ERROR("could not open file \"%s\".", path);
    }

    struct stat fileStat;
    stat(path, &fileStat);
    size_t fileSize = fileStat.st_size;
    char *fileData = (char *)malloc(fileSize + 1);
    if (fileData == NULL)
    {
        MEM_ERROR("could not allocate memory for reading file \"%s\"", path);
    }

    size_t readSize = fread(fileData, sizeof(char), fileSize, fptr);
    if (readSize < fileSize)
    {
        IO_ERROR("could not read file \"%s\"", path);
    }
    fileData[fileSize] = '\0';

    fclose(fptr);
    return fileData;
}

// !object：对象取反结果为false
static bool primObjectNot(VM *vm UNUSED, Value *args)
{
    RETURN_VALUE(VT_TO_VALUE(VT_FALSE));
}

// args[0] == args[1]：比较对象是否相等
static bool primObjectEqual(VM *vm UNUSED, Value *args)
{
    Value blv = BOOL_TO_VALUE(valueIsEqual(args[0], args[1]));
    RETURN_VALUE(blv);
}

// args[0] != args[1]：比较对象是否不等
static bool primObjectNotEqual(VM *vm UNUSED, Value *args)
{
    Value blv = BOOL_TO_VALUE(!valueIsEqual(args[0], args[1]));
    RETURN_VALUE(blv);
}

// args[0] is args[1]：对象args[0]是否是args[1]的子模型
static bool primObjectIs(VM *vm, Value *args)
{
    // args[1]必须是model
    if (!VALUE_IS_MODEL(args[1]))
    {
        RUNTIME_ERROR("argument must be model");
    }

    Model *thisModel = getModelOfObject(vm, args[0]);
    Model *baseModel = (Model *)(args[1].objectHeader);
    while (baseModel != NULL)
    {
        if (thisModel == baseModel)
        {
            RETURN_VALUE(VT_TO_VALUE(VT_TRUE));
        }
        baseModel = baseModel->superModel;
    }
    RETURN_VALUE(VT_TO_VALUE(VT_FALSE));
}

// args[0].toString：返回args[0]所属的模型名
static bool primObjectToString(VM *vm UNUSED, Value *args)
{
    Model *model = args[0].objectHeader->model;
    Value nv = OBJECT_TO_VALUE(model->name);
    RETURN_VALUE(nv);
}

// args[0].type：返回对象args[0]的模型
static bool primObjectType(VM *vm, Value *args)
{
    Model *model = getModelOfObject(vm, args[0]);
    RETURN_OBJECT(model);
}

// args[0].name：返回对象args[0]的模型名
static bool primModelName(VM *vm UNUSED, Value *args)
{
    RETURN_OBJECT(VALUE_TO_MODEL(args[0]));
}

// args[0].superType：返回args[0]的父模型
static bool primModelSuperType(VM *vm UNUSED, Value *args)
{
    Model *model = VALUE_TO_MODEL(args[0]);
    if (model->superModel != NULL)
    {
        RETURN_OBJECT(model->superModel);
    }
    RETURN_VALUE(VT_TO_VALUE(VT_NONE));
}

// args[0].toString：返回模型名
static bool primModelToString(VM *vm UNUSED, Value *args)
{
    RETURN_OBJECT(VALUE_TO_MODEL(args[0])->name);
}

// args[0].same(args[0], args[1])：返回args[1]和args[2]是否相等
static bool primObjectMetaSame(VM *vm UNUSED, Value *args)
{
    Value blv = BOOL_TO_VALUE(valueIsEqual(args[1], args[2]));
    RETURN_VALUE(blv);
}


// 根据模块名从模块表中获取模块
static ObjectModule *getModule(VM *vm, Value moduleName)
{
    Value v = mapGetByKey(vm->allModules, moduleName);
    if (v.type == VT_UNDEFINED)
    {
        return NULL;
    }
    return VALUE_TO_OBJECT(v);
}

// 载入模块并编译 返回模块线程
static ObjectThread *loadModule(VM *vm, Value moduleName, const char *moduleCode)
{
    ObjectModule *module = getModule(vm, moduleName);

    if (module == NULL)
    {
        // 创建模块并且挂载
        ObjectString *mn = VALUE_TO_STRING(moduleName);
        ASSERT('\0' != mn->value.start[mn->value.size], "value of string is unterminated");
        module = makeObjectModule(vm, mn->value.start);
        mapSet(vm, vm->allModules, moduleName, OBJECT_TO_VALUE(module));

        // 继承核心模块变量
        ObjectModule *coreModule = getModule(vm, CORE_MODULE);
        uint32_t idx = 0;
        while (idx < coreModule->varNames.cnt)
        {
            defineModuleVar(vm, module,
                            coreModule->varNames.datas[idx].str,
                            strlen(coreModule->varNames.datas[idx].str),
                            coreModule->varValues.datas[idx]);
            ++idx;
        }
    }

    // 创建模块线程
    ObjectFunction *func = compileModule(vm, module, moduleCode);
    ObjectClosure *oc = makeObjectClosure(vm, func);
    ObjectThread *ot = makeObjectThread(vm, oc);

    return ot;
}

// 执行模块
VMResult execModule(VM *vm, Value moduleName, const char *moduleCode)
{
    ObjectThread *ot = loadModule(vm, moduleName, moduleCode);
    return VMR_ERROR;
}

// 返回符号在符号表中的索引 不存在返回-1
int32_t indexFromSymbolTable(SymbolTable *st, const char *symbol, uint32_t size)
{
    ASSERT(0 != size, "length of symbol is 0");
    uint32_t idx = 0;
    while (idx < st->cnt)
    {
        if (st->datas[idx].size == size &&
            memcmp(st->datas[idx].str, symbol, size) == 0)
        {
            return idx;
        }
        ++idx;
    }
    return -1;
}

// 返回符号添加在符号表中的索引
int32_t addSymbol(VM *vm, SymbolTable *st, const char *symbol, uint32_t size)
{
    ASSERT(size != 0, "length of symbol is 0");
    String s;
    s.str = ALLOCATE_ARRAY(vm, char, size + 1);
    if (s.str == NULL)
    {
        MEM_ERROR("allocate char array failed");
    }
    memcpy(s.str, symbol, size);
    s.str[size] = '\0';
    s.size = size;
    StringBufferPut(vm, st, s);
    return st->cnt - 1;
}

// 定义模型
static Model* defineModel(VM *vm, ObjectModule *om, const char *name)
{
    Model *model = makeRawModel(vm, name, 0);
    defineModuleVar(vm, om, name, strlen(name), OBJECT_TO_VALUE(model));
    return model;
}

// 绑定行为
void  bindAction(VM *vm, Model *model, uint32_t idx, Action action)
{
    if (idx >= model->actions.cnt)
    {
        Action pad = {AT_NONE, {0}};
        ActionBufferFill(vm, &(model->actions), pad, idx-model->actions.cnt+1);
    }
    model->actions.datas[idx] = action;
}

// 绑定父模型
void bindSuperModel(VM *vm, Model *subModel, Model *superModel)
{
    subModel->superModel = superModel;
    subModel->attrCnt += superModel->attrCnt;
    uint32_t idx = 0;
    while (idx < superModel->actions.cnt)
    {
        bindAction(vm, subModel, idx, superModel->actions.datas[idx]);
        ++idx;
    }
}

// 编译核心模块
void buildCore(VM *vm)
{
    // 创建核心模块 载入核心模块 名为空
    ObjectModule *coreModule = makeObjectModule(vm, NULL);
    mapSet(vm, vm->allModules, CORE_MODULE, OBJECT_TO_VALUE(coreModule));

    // Model: ojbect
    // 创建对象模型并绑定行为
    // 它是所有模型实例的超模型(对象模型)
    // 超模型是实例的父模型链的终结节点
    // 当一个模型未显式指定父模型，那超模型就是对象模型
    vm->objectModel = defineModel(vm, coreModule, "object");
    PRIM_FUNC_BIND(vm->objectModel, "!", primObjectNot);
    PRIM_FUNC_BIND(vm->objectModel, "==(_)", primObjectEqual);
    PRIM_FUNC_BIND(vm->objectModel, "!=(_)", primObjectNotEqual);
    PRIM_FUNC_BIND(vm->objectModel, "is(_)", primObjectIs);
    PRIM_FUNC_BIND(vm->objectModel, "toString", primObjectToString);
    PRIM_FUNC_BIND(vm->objectModel, "type", primObjectType);

    // Model: model
    // 创建modelOfModel
    // 它是所有元信息模型的元信息模型以及元信息模型的父模型
    vm->modelOfModel = defineModel(vm, coreModule, "model");

    // object模型是任何模型的父模型
    bindSuperModel(vm, vm->modelOfModel, vm->objectModel);
    PRIM_FUNC_BIND(vm->modelOfModel, "name", primModelName);
    PRIM_FUNC_BIND(vm->modelOfModel, "superType", primModelSuperType);
    PRIM_FUNC_BIND(vm->modelOfModel, "toString", primModelToString);

    // Model: meta
    // 对用户不可见
    // 创建object模型的元信息模型
    vm->metaModel = defineModel(vm, coreModule, "objectMeta");

    // 它是所有元信息模型的元信息模型以及元信息模型的父模型
    bindSuperModel(vm, vm->metaModel, vm->modelOfModel);

    // 类型比较绑定
    PRIM_FUNC_BIND(vm->metaModel, "same(_,_)", primObjectMetaSame);

    // 绑定元信息模型  元信息模型需要闭合闭合
    vm->objectModel->objectHeader.model = vm->metaModel;
    vm->metaModel->objectHeader.model = vm->modelOfModel;
    vm->modelOfModel->objectHeader.model = vm->modelOfModel;
}
