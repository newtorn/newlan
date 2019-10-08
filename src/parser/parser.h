#ifndef _PARSER_PARSER_H
#define _PARSER_PARSER_H
#include "common.h"
#include "vm.h"
#include "objmodel.h"

// 标记类型
typedef enum
{
    // 数据类型
    TOKEN_UNKNOWN,       // 未知
    TOKEN_NUMBER,        // 数字
    TOKEN_STRING,        // 字符串
    TOKEN_ID,            // 标识符
    TOKEN_INTERPOLATION, // 内嵌表达式

    // 关键字
    TOKEN_AUTO,     // auto
    TOKEN_FUNC,     // func
    TOKEN_IF,       // if
    TOKEN_ELSE,     // else
    TOKEN_TRUE,     // true
    TOKEN_FALSE,    // false
    TOKEN_LOOP,     // loop
    TOKEN_BREAK,    // break
    TOKEN_CONTINUE, // continue
    TOKEN_RETURN,   // return
    TOKEN_NONE,     // none
    TOKEN_IN,       // in

    // 模块相关
    TOKEN_MODEL,   // model
    TOKEN_THIS,    // this
    TOKEN_INCLUDE, // inc
    TOKEN_STATIC,  // static
    TOKEN_IS,      // is
    TOKEN_SUPER,   // super
    TOKEN_IMPORT,  // import

    // 分隔符
    TOKEN_COMMA,         // ,
    TOKEN_COLON,         // :
    TOKEN_LEFT_PAREN,    // (
    TOKEN_RIGHT_PAREN,   // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_LEFT_BRACE,    // {
    TOKEN_RIGHT_BRACE,   // }
    TOKEN_DOT,           // .
    TOKEN_RANGE,         // ...

    // 双目运算符
    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_EXP, // **
    TOKEN_DIV, // /
    TOKEN_MOD, // %

    // 赋值运算符
    TOKEN_ASSIGN, // =

    // 位运算符
    TOKEN_BIT_AND,         // &
    TOKEN_BIT_OR,          // |
    TOKEN_BIT_NOT,         // ~
    TOKEN_BIT_SHIFT_LEFT,  // <<
    TOKEN_BIT_SHIFT_RIGHT, // >>

    // 逻辑运算符
    TOKEN_LOGIC_AND, // &&
    TOKEN_LOGIC_OR,  // ||
    TOKEN_LOGIC_NOT, // !

    // 关系操作符
    TOKEN_EQUAL,       // ==
    TOKEN_NOT_EQUAL,   // !=
    TOKEN_LESS,        // <
    TOKEN_LESS_EQUAL,  // <=
    TOKEN_GREAT,       // >
    TOKEN_GREAT_EQUAL, // >=

    // 条件运算符
    TOKEN_QUESTION, // ?

    // 文件结束符
    TOKEN_EOF, // EOF
} TokenType;

// 标记结构
typedef struct
{
    TokenType type;    // 标记类型
    const char *start; // 标记字符在源码中的开始地址
    uint32_t size;     // 标记字符串大小
    uint32_t lineNum;  // 标记所在行号
    Value value;       // 标记的值
} Token;

// 词法分析器结构
struct parser
{
    const char *file;        // 源码文件名
    const char *source;      // 源码串地址
    const char *nextChar;    // 下一个字符在源码串中的地址
    char curChar;            // 当前字符
    Token curToken;          // 当前标记
    Token preToken;          // 前一个标记
    ObjectModule *curModule; // 词法分析器所在的模块

    // Interpolation expression expect right parenthese number
    int rightParenNumofIE; // 内嵌表达式期望的右括号数量
    struct parser *parent; // 父词法分析器
    VM *vm;                // 词法分析器所在的虚拟机
};

// 公用函数声明
#define PEEK_TOKEN(parserPtr) parserPtr->curToken.type
char lookAheadChar(Parser *parser);
void getNextToken(Parser *parser);
bool matchToken(Parser *parser, TokenType expected);
void consumeCurToken(Parser *parser, TokenType expected, const char *errMsg);
void consumeNextToken(Parser *parser, TokenType expected, const char *errMsg);
uint32_t getByteNumOfEncodeUtf8(int value);
uint8_t encodeUtf8(uint8_t *buf, int value);
void initParser(VM *vm, Parser *parser, const char *file, const char *source, ObjectModule *om);

#endif