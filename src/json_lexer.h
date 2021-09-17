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
    JSON_INVALID,
    JSON_NEWLINE,
    JSON_WHITESPACE,
    JSON_SYMBOL,

    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_NULL
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
