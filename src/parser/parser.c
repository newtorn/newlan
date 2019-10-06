#include "parser.h"
#include "common.h"
#include "utils.h"
#include "utf8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    {"inc", 3, TOKEN_INC},
    {"auto", 4, TOKEN_AUTO},
    {"func", 4, TOKEN_FUNC},
    {"else", 4, TOKEN_ELSE},
    {"true", 4, TOKEN_TRUE},
    {"loop", 4, TOKEN_LOOP},
    {"none", 4, TOKEN_NONE},
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
            (0 == memcpy(keywords[idx].keyword, start, size)))
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

// 解析标识符
static parseId(Parser *parser, TokenType type)
{
    while (isalnum(parser->curChar) || '-' == parser->curChar)
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
        else if (parser->curChar >= 'A' && parser->curChar <= 'Z')
        {
            digit = parser->curChar - 'A' + 10;
        }
        else if (parser->curChar >= 'a' && parser->curChar <= 'z')
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

    ByteBufferFillWrite(parser->vm, buf, 0, byteNum);   // pos: cnt = cnt + byteNum
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
                ByteBufferPut(parser ->vm, &sb, '\a');
                break;
            case 'b':
                ByteBufferPut(parser ->vm, &sb, '\b');
                break;
            case 'f':
                ByteBufferPut(parser ->vm, &sb, '\f');
                break;
            case 'n':
                ByteBufferPut(parser ->vm, &sb, '\n');
                break;
            case 'r':
                ByteBufferPut(parser ->vm, &sb, '\r');
                break;
            case 't':
                ByteBufferPut(parser ->vm, &sb, '\t');
                break;
            case 'u':
                parseUnicodeCodePoint(parser, &sb);
                break;
            case '"':
                ByteBufferPut(parser ->vm, &sb, '"');
                break;
            case '\\':
                ByteBufferPut(parser ->vm, &sb, '\\');
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
    ByteBufferClear(parser, &sb);
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