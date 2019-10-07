# newlan文法

|     非终结符     |                            产生式                            |                             说明                             |
| :--------------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
|       数字       |   num -> 0x[0-9a-fA-F]+ \| 0[0-9]+ \| [0-9]+ ('.' [0-9]+)?   | 支持0x开头的十六进制数，支持0开头的八进制，支持十进制整数和浮点数 |
|      字符串      |                     str -> '"' [.]* '"'                      |    双引号""引起来的至少0个字符是字符的串表示，称为字符串     |
|      变量名      |                id -> [a-zA-Z_]+ [a-zA-Z_0-9]*                | 标识符以大小写字母和下划线开始，并以大小写字母、下划线和数字结尾 |
|     变量定义     |                varDef -> 'auto' id ('=' exp)?                |      变量声明或初始化，若未初始化，系统自动初始化为零值      |
| if-else条件判断  | ifStat -> 'if' '(' exp ')' (block\|statements) ('else' (block\|statements))? |      if(exp) {block} else {block} 或 if(exp) statement       |
|   loop复合循环   |  loopStat -> 'loop' (id? '(' exp ')' )?(block\|statements)   | loop {循环体} 或 loop (表达式) {循环体} 或 loop item (可迭代序列) { 循环体} |
|      break       |                     breakStat -> 'break'                     |                    关键字break，终止循环                     |
|     循环跳过     |                  continueStat -> 'continue'                  |          关键字continue，结束本轮循环，进行下轮循环          |
|     函数返回     |                 returnStat -> 'return' exp?                  |               从函数返回，可接0个或一个表达式                |
|     参数列表     |                   paramList -> id?(, id)*                    |                参数列表，可以有0个或多个参数                 |
|     函数定义     |         funcDef -> 'func' id '(' paramList ')' block         |               func 函数名 (参数列表) {函数体}                |
|     下标引用     |               subscriptCall -> id '[' exp ']'                | 用`[]`来访问模型的数据，如访问列表模型arr的第2个元素：arr[1]，再如在字典模型stds中存入Bob的年龄：stds["Bob"] = 18 |
|    getter调用    |                   getterCall -> id block?                    |          可以传入一个块参数，块参数也可以称为函数体          |
|    setter调用    |                   setterCall -> id '=' exp                   |               设置模型属性，例如：Bob.age = 18               |
|    action调用    |          actionCall -> id '(' paramList ')' block?           |     行为调用，可以传递块参数：模型名.action(参数){块参}      |
|   模型文法调用   | actionsCall -> (id '.')? (actionCall \| getterCall \| setterCall) | 在模型内部调用行为可以省略this，在模型外部调用行为需要添加实例名或模型名 |
|     调用语句     |           callStat -> subscriptCall \| actionsCall           |              调用语句包括下标引用和模型文法调用              |
|     语句列表     | statements -> (ifStat \| loopStat \| breakStat \| continueStat \| callStat \| returnStat)* |                  语句可有可无，有的话有多个                  |
|     中缀运算     | infixOp -> '+' \| '-' \| '*' \| '/' \| '%' \| '>' \| '<' \| '>=' \| '<=' \| '==' \| '!=' \| '&&' \| '\|\|' \| '&' \| '\|' \| '~' \| '>>' \| '<<' |                                                              |
|     前缀运算     |                    prefixOp -> '-' \| '!'                    |                                                              |
| 中缀运算符表达式 |               infixExpr -> exp (infixOp exp)+                |                       可接多个双目运算                       |
| 前缀运算符表达式 |                 prefixExpr -> prefixExp exp                  |                                                              |
|      表达式      | exp -> num \| str \| id \| callStat \| infixExp \| prefixExp |                      表达式递归定义自身                      |
|      语句块      |      block -> '{' ('\|' paramList '\|')? statements '}'      |              花括号括起来的语句列表，支持块参数              |
|      属性段      |                      attrSeg -> varDef                       |              模型的属性，各个模型实例自身的数据              |
|      静态段      |                staticSeg -> 'static' attrSeg                 |               模型属性，所有模型对象共享的数据               |
|        段        |              segDef -> (attrSeg \| staticSeg)*               |                                                              |
|    action定义    |      actionDef -> 'static'? id '(' paramList ')' block       |                                                              |
|    getter定义    |               getterDef -> 'static'? id block                |            逻辑上用来返回模型的属性值，但不受限制            |
|    setter定义    |        setterDef -> 'static'? id '=' '(' id ')' block        |            常用来修改模型的属性值，只支持一个参数            |
|   模型行为定义   |     actionsDef -> (actionDef \| getterDef \| setterDef)      |                                                              |
|     模型定义     | modelDef -> 'model' id ('inc' id)? '{' segDef actionsDef '}' |                      inc表示包含父模型                       |

```
#/usr/bin/newlan
import Phone

model iPhone inc Phone {
    auto version
    auto battery
    auto size
    static auto pNum = 0

    make(v, b, s) {
        super("iPhone")
        this.version = v
        this.battery = b
        size = s
        pNum = pNum + 1
        super.kind = super.kind + 1
    }

    size {
        return size
    }

    version=(v) {
        this.version = v
    }

    showInfo() {
        System.print("iPhone's version: %(version), battery: %(battery), size: %(size)")
    }

    static pNum() {
        System.print(pNum)
    }
}

// I want to create a iPhone which extends Phone
auto p = iPhone.make("1.0.0", 87, [320, 680])
p.showInfo()
System.print(p.size)
p.version = "2.0.1"
p.showInfo()
iPhone.pNum()

/*
These are bultin type creation
*/
auto ls = list.make()
auto mp = map.make()
auto ls1 = ["1", "2"]
auto mp1 = {"xm": 27, "zs": 20}
System.print(ls1[0])
System.print(mp1["zs"])

loop i (ls1) {
    System.print(i)
}

loop i (mp1) {
    System.print(mp1[i])
}

if (ls is none) {
    System.print(true)
}
else {
    System.print(false)
}

auto i = int("123")
auto s = str(0x999)

// This is a primitive function
func sum(a, b) {
    return a + b
}

auto t = sum(12, 13)
System.print(t)
```



