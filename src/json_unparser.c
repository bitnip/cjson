#include <stdlib.h>
#include <stdio.h>
#include "json.h"
#include "json_unparser.h"
#include "cutil/src/string.h"
#include "cutil/src/map/map.h"
#include "cutil/src/error.h"
#include "json.h"

static unsigned int addStringToken(
        struct List* tokens,
        const char* stringValue) {
    struct JSONToken token;

    unsigned int valueLength = strlen(stringValue) + 3;
    token.lexeme = malloc(valueLength * sizeof(char));

    if(token.lexeme == NULL) return STATUS_ALLOC_ERR;

    snprintf(token.lexeme, valueLength, "\"%s\"", (char*)stringValue);
    token.token = JSON_TOKEN_STRING;

    return listAddTail(tokens, tokenCopy(&token));
}

static unsigned int addWhitespaceToken(
        struct List* tokens,
        struct JSONFormat fmt) {
    unsigned int indentLevel = fmt.indent*fmt.level;
    if(indentLevel == 0) return STATUS_OK;

    struct JSONToken token;
    token.token = JSON_TOKEN_WHITESPACE;
    token.lexeme = malloc((indentLevel + 1) * sizeof(char));
    if(token.lexeme == NULL) return STATUS_ALLOC_ERR;
    // Null terminate the token.
    token.lexeme[indentLevel] = 0;
    // Fill token with desired whitespace.
    for(unsigned char i = 0; i < indentLevel; i++) {
        token.lexeme[i] = fmt.useTabs ? JSON_TAB : JSON_SPACE;
    }
    return listAddTail(tokens, tokenCopy(&token));
}

static unsigned int addNewlineToken(struct List* tokens) {
    struct JSONToken token;
    token.lexeme = strCopy(ASCII_V_DELIMITERS);
    if(token.lexeme == NULL) return STATUS_ALLOC_ERR;
    token.token = JSON_TOKEN_NEWLINE;
    return listAddTail(tokens, tokenCopy(&token));
}

unsigned int unparseMember(
        const void* key,
        struct Generic* element,
        struct List* tokens,
        struct JSONFormat fmt);

unsigned int unparseMembers(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct Collection* collection = (struct Collection*)generic->object;
    struct Iterator iterator = collection->iterator(genericData(generic));

    struct JSONToken token;
    token.token = JSON_TOKEN_SYMBOL;

    unsigned int result;
    struct Generic* element = NULL;
    const void* key;
    while((key = mapKey(&iterator))) {
        if(fmt.indent > 0) {
            result = addNewlineToken(tokens);
            if(result) return result;
        }

        element = collection->next(&iterator);
        result = unparseMember(key, element, tokens, fmt);
        if(result) return result;

        if((key = mapKey(&iterator))) {
            const char seperator[] = {JSON_SEPERATOR, 0};
            token.lexeme = strCopy(seperator);
            if(!token.lexeme) return STATUS_ALLOC_ERR;
            result = listAddTail(tokens, tokenCopy(&token));
            if(result) return result;
        } else {
            if(fmt.indent > 0) {
                result = addNewlineToken(tokens);
                if(result) return result;
            }
        }
    }

    return STATUS_OK;
}

unsigned int unparseElement(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt);

unsigned int unparseElements(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct Collection* collection = (struct Collection*)generic->object;
    struct Iterator iterator = collection->iterator(genericData(generic));

    struct JSONToken token;
    token.token = JSON_TOKEN_SYMBOL;
    unsigned int result;
    struct Generic* element = collection->next(&iterator);
    while(element) {
        if(fmt.indent > 0) {
            result = addNewlineToken(tokens);
            if(result) return result;
        }

        result = addWhitespaceToken(tokens, fmt);
        if(result) return result;
        result = unparseElement(element, tokens, fmt);
        if(result) return result;

        if(!(element = collection->next(&iterator))) {
            if(fmt.indent > 0) {
                result = addNewlineToken(tokens);
                if(result) return result;
            }
            break;
        }

        const char seperator[] = {JSON_SEPERATOR, 0};
        token.lexeme = strCopy(seperator);
        if(!token.lexeme) return STATUS_ALLOC_ERR;

        result = listAddTail(tokens, tokenCopy(&token));
        if(result) return result;
    }

    return STATUS_OK;
}

unsigned int unparseArray(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    if(generic->object != &Array.object) return STATUS_PARSE_ERR;
    struct JSONToken token;
    token.token = JSON_TOKEN_SYMBOL;

    const char arrayBegin[] = {JSON_ARR_BEGIN, 0};
    token.lexeme = strCopy(arrayBegin);
    if(!token.lexeme) return STATUS_ALLOC_ERR;

    int result = listAddTail(tokens, tokenCopy(&token));
    if(result) return result;

    fmt.level++;

    result = unparseElements(generic, tokens, fmt);
    if(result) return result;

    fmt.level--;

    result = addWhitespaceToken(tokens, fmt);
    if(result) return result;

    const char arrayClose[] = {JSON_ARR_CLOSE, 0};
    token.lexeme = strCopy(arrayClose);

    result = listAddTail(tokens, tokenCopy(&token));
    if(result) return result;

    return result;
}

unsigned int unparseObject(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    if(generic->object != &Map.object) return STATUS_PARSE_ERR;
    struct JSONToken token;
    token.token = JSON_TOKEN_SYMBOL;

    const char mapOpen[] = {JSON_MAP_BEGIN, 0};
    token.lexeme = strCopy(mapOpen);
    if(!token.lexeme) return STATUS_ALLOC_ERR;

    int result = listAddTail(tokens, tokenCopy(&token));
    if(result) return result;

    fmt.level++;

    result = unparseMembers(generic, tokens, fmt);
    if(result) return result;

    fmt.level--;

    result = addWhitespaceToken(tokens, fmt);
    if(result) return result;

    const char mapClose[] = {JSON_MAP_CLOSE, 0};
    token.lexeme = strCopy(mapClose);
    if(!token.lexeme) return STATUS_ALLOC_ERR;

    return listAddTail(tokens, tokenCopy(&token)); // TODO: Check tokenCopy.
}

unsigned int unparseNull(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct JSONToken token;
    token.token = JSON_TOKEN_NULL;

    if(generic->object != &Pointer) return STATUS_PARSE_ERR;
    void* pointerValue = *((void**)genericData(generic));
    if(pointerValue != NULL) return STATUS_INPUT_ERR;

    token.lexeme = strCopy(JSON_NULL_STR);
    if(!token.lexeme) return STATUS_ALLOC_ERR;

    return listAddTail(tokens, tokenCopy(&token)); // TODO:
}

unsigned int unparseBoolean(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct JSONToken token;

    if(generic->object != &Bool) return STATUS_PARSE_ERR;
    char boolValue = *((char*)genericData(generic));

    const char* value = boolValue ? JSON_TRUE_STR : JSON_FALSE_STR;

    token.lexeme = strCopy(value);
    if(!token.lexeme) return STATUS_ALLOC_ERR;
    token.token = JSON_TOKEN_BOOL;
    return listAddTail(tokens, tokenCopy(&token)); // TODO:
}

unsigned int unparseString(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    if(generic->object != &String) return STATUS_PARSE_ERR;
    return addStringToken(tokens, *((char**)genericData(generic)));
}

unsigned int unparseNumber(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct JSONToken token;
    char buffer[100];
    if(generic->object == &Integer) {
        long integerValue = *((long*)genericData(generic));
        snprintf(buffer, 100, "%ld", integerValue);
        token.lexeme = strCopy(buffer);
    } else if (generic->object == &Float) {
        double doubleValue = *((float*)genericData(generic));
        snprintf(buffer, 100, "%f", doubleValue);
        token.lexeme = strCopy(buffer);
    } else {
        return STATUS_INPUT_ERR;
    }
    if(!token.lexeme) return STATUS_ALLOC_ERR;
    token.token = JSON_TOKEN_NUMBER;
    return listAddTail(tokens, tokenCopy(&token)); // TODO:
}

unsigned int unparseValue(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    unsigned int (*unparsers[])(struct Generic*, struct List*, struct JSONFormat) = {
        unparseArray,
        unparseObject,
        unparseString,
        unparseNumber,
        unparseBoolean,
        unparseNull
    };
    for(unsigned int i = 0; i < sizeof(unparsers) / sizeof(unparsers[0]); i++) {
        unsigned int result = unparsers[i](generic, tokens, fmt);
        if(result == STATUS_OK) return result;
    }
    return STATUS_PARSE_ERR;
}

unsigned int unparseElement(
        struct Generic* generic,
        struct List* tokens,
        struct JSONFormat fmt) {
    return unparseValue(generic, tokens, fmt);
}

unsigned int unparseMember(
        const void* key,
        struct Generic* element,
        struct List* tokens,
        struct JSONFormat fmt) {
    struct JSONToken token;
    token.token = JSON_TOKEN_SYMBOL;

    unsigned int result = addWhitespaceToken(tokens, fmt);
    if(result) return result;
    result = addStringToken(tokens, key);
    if(result) return result;

    const char memberSep[] = {JSON_MEMBER_SEP, 0}; 
    token.lexeme = strCopy(memberSep);
    if(!token.lexeme) return STATUS_ALLOC_ERR;

    result = listAddTail(tokens, tokenCopy(&token)); // TODO:
    if(result) return result;

    struct JSONFormat fmtSpace = {1, 1, 0};
    result = addWhitespaceToken(tokens, fmtSpace);
    if(result) return result;
    return unparseElement(element, tokens, fmt);
}

unsigned int unparseJSON(
        struct Generic* generic,
        char** output,
        unsigned int* outputLength,
        struct JSONFormat fmt) {
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void*))tokenRelease; // TODO: ALL SORTS OF MEMORY ISSUES.

    int result = unparseElement(generic, &tokens, fmt);

    *outputLength = 0;
    struct Iterator it = listIterator(&tokens);
    struct JSONToken* token;
    while((token = listNext(&it))) {
        *outputLength += strlen(token->lexeme);
    }

    *output = malloc(*outputLength+1);
    char* ptr = *output;
    it = listIterator(&tokens);
    while((token = listNext(&it))) {
        char* ptr2 = token->lexeme;
        while(*ptr2) *ptr++ = *ptr2++;
    }
    *ptr = 0;

    listRelease(&tokens);
    return result;
}