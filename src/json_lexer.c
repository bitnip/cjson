#include "json_lexer.h"
#include "cutil/src/string.h"

struct JSONToken* tokenCopy(struct JSONToken* other) {
    struct JSONToken* token = malloc(sizeof(struct JSONToken));
    token->token = other->token;
    token->lexeme = other->lexeme;
    token->col = other->col;
    token->row = other->row;
    return token;
}

void tokenRelease(struct JSONToken* token) {
    free(token);
}

unsigned int lexString(struct JSONToken* t, char* toCheck) {
    char* offset = strAfterQuotedString(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_STRING;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexNumber(struct JSONToken* t, char* toCheck) {
    const char* offset = strAfterNumber(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_NUMBER;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexBool(struct JSONToken* t, char* toCheck) {
    const char* offset = strStartsWith(toCheck, "true");
    if(offset) {
        t->lexeme = toCheck;
        t->token = JSON_BOOL;
        return offset - toCheck;
    }
    offset = strStartsWith(toCheck, "false");
    if(offset) {
        t->lexeme = toCheck;
        t->token = JSON_BOOL;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexNull(struct JSONToken* t, char* toCheck) {
    char* offset = strStartsWith(toCheck, "null");
    if(offset) {
        t->lexeme = toCheck;
        t->token = JSON_NULL;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexWhitespace(struct JSONToken* t, char* toCheck) {
    const char* offset = strAfterLineBreak(toCheck);
    if(offset != toCheck) {
        t->col = 0;
        t->row++;
        t->lexeme = toCheck;
        t->token = JSON_NEWLINE;
        return offset - toCheck;
    }
    offset = strAfterWhitespace(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_WHITESPACE;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexSymbol(struct JSONToken* t, char* toCheck) {
    switch(*toCheck) {
        case ',':
        case ':':
        case '[':
        case ']':
        case '{':
        case '}': {
            t->lexeme = toCheck;
            t->token = JSON_SYMBOL;
            return 1;
        }
        default:
            return 0;
    }
}

unsigned int lexJSON(struct List* tokens, char* toCheck) {
    unsigned int invalid = 0;

    struct JSONToken token;
    token.row = 0;
    token.col = 0;
    token.lexeme = NULL;
    token.token = JSON_INVALID;

    unsigned int offset;
    while(*toCheck) {
        if(     (offset = lexWhitespace(&token, toCheck)) ||
                (offset = lexString(&token, toCheck)) ||
                (offset = lexNumber(&token, toCheck)) ||
                (offset = lexBool(&token, toCheck)) ||
                (offset = lexNull(&token, toCheck)) ||
                (offset = lexSymbol(&token, toCheck))) {
            listAddTail(tokens, (void*)tokenCopy(&token)); // TODO: test for failure.
        } else {
            offset=1;
            if(listTail(tokens) == NULL || ((struct JSONToken*)listTail(tokens))->token != JSON_INVALID) {
                invalid++;
                token.lexeme = toCheck;
                token.token = JSON_INVALID;
                listAddTail(tokens, (void*)tokenCopy(&token));
            }
        }
        token.col += offset;
        toCheck += offset;
    }
    return invalid;
}