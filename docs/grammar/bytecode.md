# newlan字节码

### 指令名称

- [ 指令流, 栈 ]
- 指令功能
- 指令影响

### LOAD_CONSTANT

- [ , ]
- 把索引为index的常量(constants[index])压入栈
- 只能压入1个操作数入栈

### PUSH_NONE

- [ , ]
- 往栈中压入none
- 使栈增加1个slot

### PUSH_FALSE

- [ , ]
- 往栈中压入false
- 使栈增加1个slot

### PUSH_TRUE

- [ , ]
- 往栈中压入true
- 使栈增加一个slot

### LOAD_LOCAL_VAR

- [ index, ]
- 把索引为index的局部变量的值(stack[index])压入栈
- 使栈增加一个slot

### STORE_LOCAL_VAR

- [ index, 栈顶 ]
- 把栈顶数据存入索引为index的局部变量(stack[index])中
- 不改变栈大小

### LOAD_UPVALUE

- [ index, ]
- 把索引为index的upvalue(upvalues[index])压入栈中
- 使栈增加一个slot

### STORE_UPVALUE

- [ index, 栈顶 ]
- 把栈顶数据存入索引为index的upvalue(upvalues[index])中
- 不改变栈的大小

### LOAD_MODULE_VAR

- [ index, ]
- 把索引为index的模块变量的值(varValues[index])压入栈中
- 使栈增加一个slot

### STORE_MODULE_VAR

- [ index, 栈顶 ]
- 把栈顶数据存入索引为index的模块变量(varValues[index])中
- 不改变栈的大小

### LOAD_THIS_ATTR

- [ index, ]
- 把this对象中索引为index的属性(this.attrs[index])压入栈中
- 使栈增加一个slot

### STORE_THIS_ATTR

- [ index, 栈顶 ]
- 把栈顶数据存入索引为index的属性(this.attrs[index])中
- 不改变栈的大小

### LOAD_ATTR

- [ index, ]
- 弹出栈顶元素作为实例对象，并将该实例对象中索引为index的属性(该对象.attrs[index])压入栈中
- 一弹一压，不改变栈的大小

### STORE_ATTR

- [ index, (栈顶，次栈顶) ]
- 弹出栈顶元素作为实例对象，并把次栈顶的数据存入该实例对象中索引为index的属性(该对象.attrs[index])中
- 使栈减少一个slot

### CALL0~16

- [ index,  (栈顶以下是所调用行为的实参数组args，参数个数加一为占用栈slot数，加一原因是this对象) ]
- 调用本模型中索引为index的行为(this.actions[index])，行为执行后，主调方会弹出参数所占的栈空间
- CALL族操作码如`CALL 参数个数`，注意，`参数个数`是CALL所调用的行为所需要的参数个数(并不是CALL本身需要的操作数个数)；CALL族需要的是一个2字节大小的操作数)，这些参数位于栈中，执行完成后会弹出所有参数，故使栈减少`参数个数`个slot

### SUPER0~16

- [ (action_index，super_index), (栈顶以下是所调用行为的实参数组args，参数个数加一为占用栈slot数) ]
- 把索引为super_index的常量(constants[super_index])作为父模型super，调用super中索引为aciton_index的行为；(super.actions[action_index])，执行后由主调方回收参数所占的栈空间
- 操作码形如`SUPER 参数个数`，`参数个数`的意同CALL族操作码一样，执行完后会弹出所有参数，故使栈减少`参数个数`个slot

### JUMP

- [ offset, ]
- 无条件向前跳转：offset是跳转`正`偏移量，即向前(往高地址)跳到指令流pc+=offset(instrStream[pc+offset])处；其中pc是下一条指令地址
- 不改变栈的大小

### LOOP

- [ offset, ]
- 无条件向回跳转：offset是跳转`正`偏移量，即向回(往低地址)跳到指令流中pc-=offset(instrStream[pc-offset])处；其中pc是下一条指令地址
- 不改变栈的大小

### JUMP_IF_FALSE

- [ offset, 栈顶 ]
- offset是跳转`正`偏移量，将栈顶弹出作为条件，若条件为false则向前(往高地址)跳到指令流中pc+=offset(instrStream[pc+offset])处
- 弹出了栈顶数据，使栈减少一个slot

### AND

- [ offset, 栈顶 ]
- 与运算的短路方式：左操作数为false时右操作数不用测试了，offset是跳转`正`偏移量，如果栈顶处的值(`&&`的左操作数)为false；向前(往高地址)跳到指令流中pc+=offset(instrStream[pc+offset])处，即跨过右操作数的计算步骤，否则弹栈计算右操作数
- 若弹栈则使栈减少一个slot

### OR

- [ offset, 栈顶 ]
- 或运算的短路方式：左操作数为true时右操作数不用测试了，offset是跳转`正`偏移量，如果栈顶处的值(`||`的左操作数)为true；向前(往高地址)跳到指令流中pc+=offset(instrStream[ip+offset])处，即跨过右操作数的计算步骤，否则弹栈计算右操作数
- 若弹栈则使栈减少一个slot

### CLOSE_UPVALUE

- [ , 栈顶 ]
- 栈顶中存储的是地址界限，关闭地址大于地址界限的upvalue,然后弹栈
- 弹栈使栈减少一个slot

### RETURN

- [ , 栈顶 ]
- 退出当前函数并弹出栈顶的值作为返回值
- RETURN操作码会弹出栈顶的值作为返回值，但其对栈的影响并不是减少了一个slot，而是0；原因是RETURN用于从函数返回，无论是回到主调方，还是结束程序运行及从程序的入口中返回，都是结束函数的调用；统计操作码对栈的影响，姥爹是在函数调用前，给函数分配运行时栈的空间大小做参考；主调方函数和被调方函数的运行时栈在空间上是接壤的，被调方函数返回值是存储到被调方函数的运行时栈的栈底的；所以此处恰是主调方函数的运行时栈的栈顶，主调方就可以到自己运行时栈的栈顶获得被调方函数的返回值；操作码RETURN的使命还要回收所属的函数的运行时栈，为保证返回值传给主调方，RETURN回收是除保留返回值的栈底slot外的整个运行时栈；对栈的影响取决于实际的栈大小，即回收的slot数量不定

### CREATE_CLOSURE

- [ closure_index { [bool1, upvalue_index1], [bool2, upvalue_index2],... }, ]
- 把索引为index的常量(constants[index]的函数是待创建闭包的函数)创建闭包，然后将创建的闭包压栈；大括号中的内容可选的，每一对中括号表示一个upvalue的信息，布尔值和upvalue索引；如果布尔值为true，upvalue_index是直接外层函数中的局部变量的索引，否则为直接外层函数中的upvalue索引；中括号对的数量取决于待创建闭包函数中的upvalue数量,即constants[index].upvalueCnt；如果upvalueCnt为0则没有使用到upvalue，所以说大括号可选
- 压栈操作使栈增加一个slot

### CONSTRUCT

- [ , 栈顶 ]
- 模型对象在当前运行时栈的栈底(stack[0])，该操作码创建一个模型实例，然后用该实例去替换栈底的模型对象
- 不改变栈的大小

### CREATE_MODEL

- [ 域数量, (栈顶，次栈顶) ]
- 创建一个模型，栈顶父模型，次栈顶(距离栈顶最近的已用slot)是模型名，指令流中的操作数指定了域的个数，父模型所在的栈顶slot会被回收；创建的模型存储到原来次栈顶所在的slot
- 回收父模型所占的slot，使栈减少一个slot

### INSTANCE_ACTION

- [ index, 栈顶 ]
- 函数与行为的区别是行为有所属的模型，所以创建行为就是把行为绑定到某个模型；为索引为index的行为名(allActionNames[index])创建行为，栈顶是待绑定的模型；次栈顶是待绑定的行为，完成后弹出栈顶和次栈顶
- 两个弹栈操作使栈减少2个slot

### STATIC_ACTION

- [ index, 栈顶 ]
- 同INSTANCE_ACTION模型相似，不同的是，该操作码将行为绑定到该模型指向的meta模型
- 使栈减少2个slot

### END

- [ ,  ]
- 伪指令，表示指令结束
- 不改变栈的大小
















