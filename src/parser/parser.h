#ifndef _PARSER_PARSER_H
#define _PARSER_PARSER_H
#include "common.h"
#include "vm.h"

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

    // 模块相关
    TOKEN_MODEL,  // model
    TOKEN_THIS,   // this
    TOKEN_INC,    // inc
    TOKEN_STATIC, // static
    TOKEN_IS,     // is
    TOKEN_SUPER,  // super
    TOKEN_IMPORT, // import

    // 分隔符
    TOKEN_COMMA,         // ,
    TOKEN_COLON,         // ;
    TOKEN_LEFT_PAREN,    // (
    TOKEN_RIGHT_PAREN,   // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_LEFT_BRACE,    // {
    TOKEN_RIGHT_BRACE,   // }
    TOKEN_DOT,           // .
    TOKEN_RANGE_DOT,     // ...

    // 双目运算符
    TOKEN_ADD, // +
    TOKEN_SUB, // -
    TOKEN_MUL, // *
    TOKEN_EXP, // **
    TOKEN_DIV, // /
    TOKEN_MOD, // %

    // 赋值运算符
    TOKEN_ASSIN, // =

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
    TokenType type;
    const char *start;
    uint32_t size;
    uint32_t lineNum;
} Token;

// 词法分析器结构
struct parser
{
    const char *file;
    const char *source;
    const char *nextChar;
    char curChar;
    Token curToken;
    Token preToken;

    // Interpolation expression expect right parenthese number
    uint32_t rightParenNumofIE;

    VM *vm;
};

// 公用函数声明
#define PEEK_TOKEN(parser) parser->curToken.type
char lookAheadChar(Parser *parser);
void getNextToken(Parser *parser);
bool matchToken(Parser *parser, TokenType expected);
void consumeCurToken(Parser *parser, TokenType expected, const char *errMsg);
void consumeNextToken(Parser *parser, TokenType expected, const char *errMsg);
uint32_t getByteNumOfEncodeUtf8(int value);
uint8_t encodeUtf8(uint8_t *buf, int value);
void initParser(VM *vm, Parser *parser, const char *file, const char *source);

#endif