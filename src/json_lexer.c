#include "json_lexer.h"
#include "cutil/src/string.h"

struct Token *tokenCopy(struct Token* other) {
    struct Token* token = malloc(sizeof(struct Token));
    token->token = other->token;
    token->lexeme = other->lexeme;
    token->col = other->col;
    token->row = other->row;
    return token;
}

void tokenRelease(struct Token* token) {
    free(token);
}

unsigned int lexString(struct Token *t, char* toCheck) {
    char *offset = afterQuotedString(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_STRING;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexNumber(struct Token *t, char* toCheck) {
    char* offset = afterNumber(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_NUMBER;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexBool(struct Token *t, char* toCheck) {
    char *offset = strStartsWith(toCheck, "true");
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

unsigned int lexNull(struct Token *t, char* toCheck) {
    char *offset = strStartsWith(toCheck, "null");
    if(offset) {
        t->lexeme = toCheck;
        t->token = JSON_NULL;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexWhitespace(struct Token *t, char* toCheck) {
    char *offset = afterLineBreak(toCheck);
    if(offset != toCheck) {
        t->col = 0;
        t->row++;
        t->lexeme = toCheck;
        t->token = JSON_NEWLINE;
        return offset - toCheck;
    }
    offset = afterWhitespace(toCheck);
    if(offset != toCheck) {
        t->lexeme = toCheck;
        t->token = JSON_WHITESPACE;
        return offset - toCheck;
    }
    return 0;
}

unsigned int lexSymbol(struct Token *t, char* toCheck) {
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

    struct Token token;
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
            if(listTail(tokens) == NULL || ((struct Token*)listTail(tokens))->token != JSON_INVALID) {
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