# newlan功能

`newlan`英文为`new language`，中文意为`新语言`，它是一个`纯粹`的`面向对象`的`脚本语言`，基于MA(`Model-Action`)模式思想。

## 变量

使用关键字let定义，支持局部变量和全局变量定义，变量采用引用机制。

## 基本数据类型

* 数值类型
  - 整数：65535
  - 浮点数：3.1415
* 字符串
  * ascii：newlan
  * unicode：\u6F6F
  * 支持下标调用：string[index | range]，可以传入下标或切片范围
* 列表
  * list支持构造方法创建：list.make
  * 字面量创建：[ "hello", "newlan" ]
  * 支持下标引用（list_identifier[list_index]）：arr[2]
* 字典
  * map支持构造方法创建：map.make
  * 字面量创建：{ k1 : v1, k2 : v2 … }
  * 支持下标引用（map_identifier[map_key]）：lans["newlan"]
* 范围
  * range可用于切片
  * [start…end]：start表示开始，end表示结束
  * 闭区间：既包含start，又包含end
  * [1…10]：将产生1到10的范围

## 运算

* 算术运算：`+、-、*、/、%`
* 关系运算：`>、>=、<、<=、==、!=`
* 逻辑运算：`&&、||、!`
* 位运算符：`&、|、~、>>、<<`
* 方法调用：`.`
* 下标引用：`[]`
* 赋值运算：`=`
* 杂项运算：条件表达式`?:`、字符串连接`+`、串内表达式`%`

## 控制结构

* if-else
* loop
* break
* continue
* return

## 函数

* 使用关键字func实现函数的定义，函数底层使用类机制实现
* 支持传统意义上的函数重载

## 模型

* 使用关键字model定义一个模型，类似于传统意义的class
* 支持模型包含，使用inc关键字，类似于传统意义的继承，所有模型都是object的子模型
* 模型属性：必须先声明后使用
* 模型行为
  * 支持块带参数，类似ruby中的实现：{ <paramList> … }
  * 包括实例动作：actionName(paramList) {...}
  * 下标访问：subscript
  * 下标设置：subscriptSetter
  * 属性设置获取：setter、getter
  * 支持静态行为：static actionName { … }

## 线程

- 支持线程创建：Thread.make
- 支持线程调度：Thread.call

## 模块

* 支持模块执行
* 支持单独导入模块内的模块变量

## 注释

* 行注释：// code line comments
* 块注释：/* code block comments */