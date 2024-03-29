#include <stdlib.h>
#include "json.h"
#include "json_lexer.h"
#include "cutil/src/error.h"
#include "cutil/src/string.h"

struct JSONToken *tokenCopy(struct JSONToken *other) {
    struct JSONToken *token = malloc(sizeof(struct JSONToken));
    if(token == NULL) return token;
    token->token = other->token;
    token->lexeme = other->lexeme;
    token->col = other->col;
    token->row = other->row;
    return token;
}

void tokenRelease(struct JSONToken *token) {
    free(token);
}

static unsigned int lexString(struct JSONToken *t, char *toCheck) {
    char *offset = strAfterQuotedString(toCheck);
    if(offset != toCheck) {
        t->token = JSON_TOKEN_STRING;
        return offset - toCheck;
    }
    return 0;
}

static unsigned int lexNumber(struct JSONToken *t, char *toCheck) {
    const char *offset = strAfterNumber(toCheck);
    if(offset != toCheck) {
        t->token = JSON_TOKEN_NUMBER;
        return offset - toCheck;
    }
    return 0;
}

static unsigned int lexBool(struct JSONToken *t, char *toCheck) {
    const char *offset = strStartsWith(toCheck, JSON_TRUE_STR);
    if(offset) {
        t->token = JSON_TOKEN_BOOL;
        return offset - toCheck;
    }
    offset = strStartsWith(toCheck, JSON_FALSE_STR);
    if(offset) {
        t->token = JSON_TOKEN_BOOL;
        return offset - toCheck;
    }
    return 0;
}

static unsigned int lexNull(struct JSONToken *t, char *toCheck) {
    char *offset = strStartsWith(toCheck, JSON_NULL_STR);
    if(offset) {
        t->token = JSON_TOKEN_NULL;
        return offset - toCheck;
    }
    return 0;
}

static unsigned int lexWhitespace(struct JSONToken *t, char *toCheck) {
    const char *offset = strAfterLineBreak(toCheck);
    if(offset != toCheck) {
        t->col = 0;
        t->row++;
        t->token = JSON_TOKEN_NEWLINE;
        return offset - toCheck;
    }
    offset = strAfterWhitespace(toCheck);
    if(offset != toCheck) {
        t->token = JSON_TOKEN_WHITESPACE;
        return offset - toCheck;
    }
    return 0;
}

static unsigned int lexSymbol(struct JSONToken *t, char *toCheck) {
    switch(*toCheck) {
        case JSON_SEPERATOR:
        case JSON_MEMBER_SEP:
        case JSON_ARR_BEGIN:
        case JSON_ARR_CLOSE:
        case JSON_MAP_BEGIN:
        case JSON_MAP_CLOSE: {
            t->token = JSON_TOKEN_SYMBOL;
            return 1;
        }
        default:
            return 0;
    }
}

static unsigned int lexInvalid(struct JSONToken *t, char *toCheck) {
    if(t->lexeme != NULL && t->token == JSON_TOKEN_INVALID) return 0;
    t->token = JSON_TOKEN_INVALID;
    return 1;
}

unsigned int lexJSON(struct List *tokens, char *toCheck) {
    unsigned int result = STATUS_OK;

    struct JSONToken token;
    token.row = 0;
    token.col = 0;
    token.lexeme = NULL;
    token.token = JSON_TOKEN_INVALID;

    unsigned int (*lexers[])(struct JSONToken*, char*) = {
        lexWhitespace,
        lexString,
        lexNumber,
        lexBool,
        lexNull,
        lexSymbol,
        lexInvalid
    };

    unsigned int offset;
    while(*toCheck) {
        for(unsigned int i = 0; i < sizeof(lexers) / sizeof(lexers[0]); i++) {
            offset = lexers[i](&token, toCheck);
            if(offset) {
                if(token.token == JSON_TOKEN_INVALID) {
                    result = STATUS_PARSE_ERR;
                }
                token.lexeme = toCheck;
                // Create token.
                struct JSONToken *newToken = tokenCopy(&token);
                if(newToken == NULL) {
                    return STATUS_ALLOC_ERR;
                }
                // Add token to token list.
                int result = listAddTail(tokens, (void*)newToken);
                if(result) {
                    tokenRelease(newToken);
                    return result;
                }
                break;
            }
        }
        if(!offset) {
            offset = 1;
        }
        token.col += offset;
        toCheck += offset;
    }
    return result;
}