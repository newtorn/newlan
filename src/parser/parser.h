#ifndef _PARSER_PARSER_H
#define _PARSER_PARSER_H

typedef struct
{
    uint32_t lineNum;
} Token;


struct parser {
    const char *file;
    Token preToken;
};

#endif