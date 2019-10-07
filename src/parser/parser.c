#include "parser.h"
#include "common.h"
#include "utils.h"
#include "utf8.h"
#include "objstring.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// 关键字
struct keywordToken
{
    char *keyword;
    uint8_t size;
    TokenType token;
};

struct keywordToken keywords[] = {
    {"if", 2, TOKEN_IF},
    {"is", 2, TOKEN_IS},
    {"inc", 3, TOKEN_INCLUDE},
    {"auto", 4, TOKEN_AUTO},
    {"func", 4, TOKEN_FUNC},
    {"else", 4, TOKEN_ELSE},
    {"true", 4, TOKEN_TRUE},
    {"loop", 4, TOKEN_LOOP},
    {"none", 4, TOKEN_NONE},
    {"this", 4, TOKEN_THIS},
    {"false", 5, TOKEN_FALSE},
    {"break", 5, TOKEN_BREAK},
    {"model", 5, TOKEN_MODEL},
    {"super", 5, TOKEN_SUPER},
    {"return", 6, TOKEN_RETURN},
    {"static", 6, TOKEN_STATIC},
    {"import", 6, TOKEN_IMPORT},
    {"continue", 8, TOKEN_CONTINUE},
    {NULL, 0, TOKEN_UNKNOWN}}; // 此元素位置不能调整

// 单词要么是关键字，要么是普通标识符
static TokenType typeFromWord(const char *start, uint32_t size)
{
    uint32_t idx = 0;
    while (NULL != keywords[idx].keyword)
    {
        if ((size == keywords[idx].size) &&
            (0 == memcmp(keywords[idx].keyword, start, size)))
        {
            return keywords[idx].token;
        }
        ++idx;
    }
    return TOKEN_ID;
}

// 看前一个字符，不改变当前字符指针位置
char lookAheadChar(Parser *parser)
{
    return *(parser->nextChar);
}

// 获取下一个字符
static void getNextChar(Parser *parser)
{
    parser->curChar = *(parser->nextChar++);
}

// 下一个字符与期望字符是否匹配
// 若匹配则读入下一个字符并返回true
// 否则不读入并返回false
static bool matchNextChar(Parser *parser, char expected)
{
    if (expected == lookAheadChar(parser))
    {
        getNextChar(parser);
        return true;
    }
    return false;
}

// 跳过连续空白字符
static void skipBlanks(Parser *parser)
{
    while (isspace(parser->curChar))
    {
        if ('\n' == parser->curChar)
        {
            parser->curToken.lineNum++;
        }
        getNextChar(parser);
    }
}

// 跳过一行
static void skipLine(Parser *parser)
{
    getNextChar(parser);
    while ('\0' != parser->curChar)
    {
        if ('\n' == parser->curChar)
        {
            parser->curToken.lineNum++;
            getNextChar(parser);
            break;
        }
        getNextChar(parser);
    }
}

// 跳过行注释或块注释
static void skipComment(Parser *parser)
{
    char nextChar = lookAheadChar(parser);
    if ('/' == parser->curChar)
    {
        skipLine(parser);
    }
    else
    {
        while ('*' != nextChar && '\0' != nextChar)
        {
            getNextChar(parser);
            if ('\n' == parser->curChar)
            {
                parser->curToken.lineNum++;
            }
            nextChar = lookAheadChar(parser);
        }
        if (matchNextChar(parser, '*'))
        {
            if (!matchNextChar(parser, '/'))
            {
                LEX_ERROR(parser, "expect '/' after '*'!");
            }
            getNextChar(parser);
        }
        else
        {
            LEX_ERROR(parser, "unterminated block comment, expect \"*/\" before file end!");
        }
    }
    skipBlanks(parser);
}

// 解析标识符
static void parseId(Parser *parser, TokenType type)
{
    while (isalnum(parser->curChar) || '_' == parser->curChar)
    {
        getNextChar(parser);
    }

    uint32_t size = (uint32_t)(parser->nextChar - parser->curToken.start - 1);
    if (TOKEN_UNKNOWN != type)
    {
        parser->curToken.type = type;
    }
    else
    {
        parser->curToken.type = typeFromWord(parser->curToken.start, size);
    }

    parser->curToken.size = size;
}

// 解析unicode码点
static void parseUnicodeCodePoint(Parser *parser, ByteBuffer *buf)
{
    uint32_t idx = 0;
    int value = 0;
    uint8_t digit = 0;

    // \uxxxx x为4个16进制数
    while (idx++ < 4)
    {
        getNextChar(parser);
        if ('\0' == parser->curChar)
        {
            LEX_ERROR(parser, "unterminated unicode!");
        }
        if (parser->curChar >= '0' && parser->curChar <= '9')
        {
            digit = parser->curChar - '0';
        }
        else if (parser->curChar >= 'A' && parser->curChar <= 'F')
        {
            digit = parser->curChar - 'A' + 10;
        }
        else if (parser->curChar >= 'a' && parser->curChar <= 'f')
        {
            digit = parser->curChar - 'a' + 10;
        }
        else
        {
            LEX_ERROR(parser, "invalid unicode!");
        }
        value = value * 16 | digit;
    }

    uint32_t byteNum = getByteNumOfEncodeUtf8(value);
    ASSERT(byteNum != 0, "utf-8 encode byte num should between 1 and 4!");

    ByteBufferFill(parser->vm, buf, 0, byteNum);        // pos: cnt = cnt + byteNum
    encodeUtf8(buf->datas + buf->cnt - byteNum, value); // pos: datas[cnt - byteNum]
}

// 解析字符串
static void parseString(Parser *parser)
{
    ByteBuffer sb;
    ByteBufferInit(&sb);
    while (true)
    {
        getNextChar(parser);

        if ('\0' == parser->curChar)
        {
            LEX_ERROR(parser, "unterminated string!");
        }

        if ('"' == parser->curChar)
        {
            parser->curToken.type = TOKEN_STRING;
            break;
        }

        if ('%' == parser->curChar)
        {
            if (!matchNextChar(parser, '('))
            {
                LEX_ERROR(parser, "'%' should followed by '('!");
            }
            if (parser->rightParenNumofIE > 0)
            {
                COMPILE_ERROR(parser, "unsupport nest interpalotion expression!");
            }
            parser->rightParenNumofIE = 1;
            parser->curToken.type = TOKEN_INTERPOLATION;
            break;
        }

        if ('\\' == parser->curChar)
        {
            getNextChar(parser);
            switch (parser->curChar)
            {
            case '0':
                ByteBufferPut(parser->vm, &sb, '\0');
                break;
            case 'a':
                ByteBufferPut(parser->vm, &sb, '\a');
                break;
            case 'b':
                ByteBufferPut(parser->vm, &sb, '\b');
                break;
            case 'f':
                ByteBufferPut(parser->vm, &sb, '\f');
                break;
            case 'n':
                ByteBufferPut(parser->vm, &sb, '\n');
                break;
            case 'r':
                ByteBufferPut(parser->vm, &sb, '\r');
                break;
            case 't':
                ByteBufferPut(parser->vm, &sb, '\t');
                break;
            case 'u':
                parseUnicodeCodePoint(parser, &sb);
                break;
            case '"':
                ByteBufferPut(parser->vm, &sb, '"');
                break;
            case '\\':
                ByteBufferPut(parser->vm, &sb, '\\');
                break;
            default:
                LEX_ERROR(parser, "unsupport escape \\%c", parser->curChar);
                break;
            }
        }
        else
        {
            ByteBufferPut(parser->vm, &sb, parser->curChar);
        }
    }
    ObjectString *os = makeObjectString(parser->vm, (const char *)sb.datas, sb.cnt);
    parser->curToken.value = OBJECT_TO_VALUE(os);
    ByteBufferClear(parser->vm, &sb);
}

// 解析十六进制数字
static void parseHexNumber(Parser *parser)
{
    while (isxdigit(parser->curChar))
    {
        getNextChar(parser);
    }
}

// 解析十进制数字
static void parseDecNumber(Parser *parser)
{
    while (isdigit(parser->curChar))
    {
        getNextChar(parser);
    }
    if ('.' == parser->curChar && isdigit(lookAheadChar(parser)))
    {
        getNextChar(parser);
        while (isdigit(parser->curChar))
        {
            getNextChar(parser);
        }
    }
}

// 解析八进制数字
static void parseOctNumber(Parser *parser)
{
    while ('0' <= parser->curChar && parser->curChar <= '8')
    {
        getNextChar(parser);
    }
}

// 解析二进制数字
static void parseBinNumber(Parser *parser)
{
    while ('0' == parser->curChar || '1' == parser->curChar)
    {
        getNextChar(parser);
    }
}

// 解析二进制、八进制、十进制、十六进制数字
static void parseNumber(Parser *parser)
{
    if ('0' == parser->curChar && matchNextChar(parser, 'x'))
    {
        // 解析十六进制 前缀0x
        getNextChar(parser);
        parseHexNumber(parser);
        parser->curToken.value = NUMBER_TO_VALUE(strtol(parser->curToken.start, NULL, 16));
    }
    else if ('0' == parser->curChar && matchNextChar(parser, 'b'))
    {
        // 解析二进制 前缀0b
        getNextChar(parser);
        parseBinNumber(parser);
        parser->curToken.value = NUMBER_TO_VALUE(strtol(parser->curToken.start, NULL, 2));
    }
    else if ('0' == parser->curChar && isdigit(lookAheadChar(parser)))
    {
        // 解析十六进制 前缀0
        parseOctNumber(parser);
        parser->curToken.value = NUMBER_TO_VALUE(strtol(parser->curToken.start, NULL, 8));
    }
    else
    {
        // 解析十进制
        parseDecNumber(parser);
        parser->curToken.value = NUMBER_TO_VALUE(strtod(parser->curToken.start, NULL));
    }
    parser->curToken.size = (uint32_t)(parser->nextChar - parser->curToken.start - 1);
    parser->curToken.type = TOKEN_NUMBER;
}

// 获取下一个标记
void getNextToken(Parser *parser)
{
    parser->preToken = parser->curToken;
    skipBlanks(parser);
    parser->curToken.type = TOKEN_EOF;
    parser->curToken.size = 0;
    parser->curToken.start = parser->nextChar - 1;
    while ('\0' != parser->curChar)
    {
        switch (parser->curChar)
        {
        case ',':
            parser->curToken.type = TOKEN_COMMA;
            break;
        case ':':
            parser->curToken.type = TOKEN_COLON;
            break;
        case '(':
            if (parser->rightParenNumofIE > 0)
            {
                parser->rightParenNumofIE++;
            }
            parser->curToken.type = TOKEN_LEFT_PAREN;
            break;
        case ')':
            if (parser->rightParenNumofIE > 0)
            {
                parser->rightParenNumofIE--;
                if (parser->rightParenNumofIE == 0)
                {
                    // 解析%(xxx)完了额之后的字符串
                    parseString(parser);
                    break;
                }
            }
            parser->curToken.type = TOKEN_RIGHT_PAREN;
            break;
        case '[':
            parser->curToken.type = TOKEN_LEFT_BRACKET;
            break;
        case ']':
            parser->curToken.type = TOKEN_RIGHT_BRACKET;
            break;
        case '{':
            parser->curToken.type = TOKEN_LEFT_BRACE;
            break;
        case '}':
            parser->curToken.type = TOKEN_RIGHT_BRACE;
            break;
        case '.':
            if (matchNextChar(parser, '.'))
            {
                // if (!matchNextChar(parser, '.'))
                // {
                //     LEX_ERROR(parser, "range operator error: expect \"...\" but got \"..\"!");
                // }
                parser->curToken.type = TOKEN_RANGE_DOT;
            }
            else
            {
                parser->curToken.type = TOKEN_DOT;
            }
            break;
        case '=':
            if (matchNextChar(parser, '='))
            {
                parser->curToken.type = TOKEN_EQUAL;
            }
            else
            {
                parser->curToken.type = TOKEN_ASSIGN;
            }
            break;
        case '+':
            parser->curToken.type = TOKEN_ADD;
            break;
        case '-':
            parser->curToken.type = TOKEN_SUB;
            break;
        case '*':
            parser->curToken.type = TOKEN_MUL;
            break;
        case '/':
            if (matchNextChar(parser, '/') || matchNextChar(parser, '*'))
            {
                skipComment(parser);
                parser->curToken.start = parser->nextChar - 1;
                continue;
            }
            else
            {
                parser->curToken.type = TOKEN_DIV;
            }
            break;
        case '%':
            parser->curToken.type = TOKEN_MOD;
            break;
        case '&':
            if (matchNextChar(parser, '&'))
            {
                parser->curToken.type = TOKEN_LOGIC_AND;
            }
            else
            {
                parser->curToken.type = TOKEN_BIT_AND;
            }
            break;
        case '|':
            if (matchNextChar(parser, '|'))
            {
                parser->curToken.type = TOKEN_LOGIC_OR;
            }
            else
            {
                parser->curToken.type = TOKEN_BIT_OR;
            }
            break;
        case '~':
            parser->curToken.type = TOKEN_BIT_NOT;
            break;
        case '?':
            parser->curToken.type = TOKEN_QUESTION;
            break;
        case '>':
            if (matchNextChar(parser, '='))
            {
                parser->curToken.type = TOKEN_GREAT_EQUAL;
            }
            else if (matchNextChar(parser, '>'))
            {
                parser->curToken.type = TOKEN_BIT_SHIFT_RIGHT;
            }
            else
            {
                parser->curToken.type = TOKEN_GREAT;
            }
            break;
        case '<':
            if (matchNextChar(parser, '='))
            {
                parser->curToken.type = TOKEN_LESS_EQUAL;
            }
            else if (matchNextChar(parser, '<'))
            {
                parser->curToken.type = TOKEN_BIT_SHIFT_LEFT;
            }
            else
            {
                parser->curToken.type = TOKEN_LESS;
            }
            break;
        case '!':
            if (matchNextChar(parser, '='))
            {
                parser->curToken.type = TOKEN_NOT_EQUAL;
            }
            else
            {
                parser->curToken.type = TOKEN_LOGIC_NOT;
            }
            break;
        case '"':
            parseString(parser);
            break;
        default:
            if (isalpha(parser->curChar) || '_' == parser->curChar)
            {
                parseId(parser, TOKEN_UNKNOWN);
            }
            else if (isdigit(parser->curChar))
            {
                parseNumber(parser);
            }
            else
            {
                if ('#' == parser->curChar && matchNextChar(parser, '!'))
                {
                    skipLine(parser);
                    parser->curToken.start = parser->nextChar - 1;
                    continue;
                }
                LEX_ERROR(parser, "unsupport char: '%c', quit.", parser->curChar);
            }
            return;
        }
        parser->curToken.size = (uint32_t)(parser->nextChar - parser->curToken.start);
        getNextChar(parser);
        return;
    }
}

// 当前标记是否与期望标记匹配
// 若匹配则读入下一个token并返回true
// 否则不读入并返回false
bool matchToken(Parser *parser, TokenType expected)
{
    if (expected == parser->curToken.type)
    {
        getNextToken(parser);
        return true;
    }
    return false;
}

// 断言当前标记为期望标记
// 若断言成功，则读入下一个标记
// 否则报错
void consumeCurToken(Parser *parser, TokenType expected, const char *errMsg)
{
    if (expected != parser->curToken.type)
    {
        COMPILE_ERROR(parser, errMsg);
    }
    getNextToken(parser);
}

// 断言下一个标记为期望标记，否则报错
void consumeNextToken(Parser *parser, TokenType expected, const char *errMsg)
{
    getNextToken(parser);
    if (expected != parser->curToken.type)
    {
        COMPILE_ERROR(parser, errMsg);
    }
}

// 初始化词法分析器
void initParser(VM *vm, Parser *parser, const char *file, const char *source, ObjectModule *om)
{
    parser->file = file;
    parser->source = source;
    parser->curChar = *(parser->source);
    parser->nextChar = parser->source + 1;
    parser->curToken.lineNum = 1;
    parser->curToken.type = TOKEN_UNKNOWN;
    parser->curToken.start = NULL;
    parser->curToken.size = 0;
    parser->preToken = parser->curToken;
    parser->rightParenNumofIE = 0;
    parser->vm = vm;
    parser->curModule = om;
}
