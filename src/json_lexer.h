#ifndef __JSON_LEXER_H
#define __JSON_LEXER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "cutil/src/list/list.h"

enum ENCODING{
    UTF32BE,
    UTF32LE,
    UTF16BE,
    UTF16LE,
    UTF8
};

enum JSON_BUILTIN {
    JSON_TOKEN_INVALID,
    JSON_TOKEN_NEWLINE,
    JSON_TOKEN_WHITESPACE,
    JSON_TOKEN_SYMBOL,
    JSON_TOKEN_OBJECT,
    JSON_TOKEN_ARRAY,
    JSON_TOKEN_STRING,
    JSON_TOKEN_NUMBER,
    JSON_TOKEN_BOOL,
    JSON_TOKEN_NULL
};

struct JSONToken {
    enum JSON_BUILTIN token;
    char* lexeme;
    unsigned int col;
    unsigned int row;
};

struct JSONToken* tokenCompose();
void tokenRelease(struct JSONToken*);
struct JSONToken* tokenCopy(struct JSONToken* other);

unsigned int lexJSON(struct List* tokens, char *toCheck);

#ifdef __cplusplus
}
#endif
#endif
