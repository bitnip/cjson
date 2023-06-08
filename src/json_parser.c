#include <stdlib.h>
#include "json.h"
#include "json_parser.h"
#include "cutil/src/error.h"
#include "cutil/src/string.h"
#include "cutil/src/map/map.h"

unsigned int parseElement(struct Generic** generic, struct Iterator* iterator);
static inline unsigned int parseMember(char** key, struct Generic** value, struct Iterator* iterator);

unsigned int parseWhitespace(struct Iterator* iterator) {
    struct JSONToken* token = listCurrent(iterator);
    while(token) {
        if(token->token != JSON_TOKEN_WHITESPACE && token->token != JSON_TOKEN_NEWLINE) break;
        token = listNext(iterator);
    }
    return STATUS_OK;
}

unsigned int parseNumber(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;
    struct JSONToken* token = listCurrent(&myIt);
    if(token && token->token != JSON_TOKEN_NUMBER) return STATUS_PARSE_ERR;
    // TODO: Too permissive, should require beginning with only hyphen or 1-9.

    const char* end = strAfterNumber(token->lexeme);
    const char* tokenStr = strCopyN(token->lexeme, end-token->lexeme);
    if(tokenStr == NULL) return STATUS_ALLOC_ERR;

    if(strAfterDigits(token->lexeme) == end) {
        *generic = genericCompose(&Integer);
        long integerValue = atoi(tokenStr);
        *((long*)genericData(*generic)) = integerValue;
    } else {
        *generic = genericCompose(&Float);
        *((float*)genericData(*generic)) = atof(tokenStr);
    }

    listNext(&myIt);

    *iterator = myIt;
    return STATUS_OK;
}

unsigned int parseString(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;
    struct JSONToken* token = listCurrent(&myIt);
    if(token && token->token != JSON_TOKEN_STRING) return STATUS_PARSE_ERR; // TODO:

    // Trim quotation marks.
    // TODO: Unescape string.
    char* endQuote = strAfterQuotedString(token->lexeme);
    if(endQuote != token->lexeme) {
        token->lexeme++;
        *(endQuote-1) = 0;
    }

    struct Generic* result = genericCompose(&String);
    if(!result) {
        return STATUS_ALLOC_ERR;
    } 

    char* value = strCopy(token->lexeme);
    if(!value) {
        genericRelease(result);
        return STATUS_ALLOC_ERR;
    }
    *((char**)genericData(result)) = value;
    *generic = result;

    listNext(&myIt);

    *iterator = myIt;
    return STATUS_OK;
}

unsigned int parseBoolean(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;
    struct JSONToken* token = listCurrent(&myIt);
    if(token && token->token != JSON_TOKEN_BOOL) return STATUS_PARSE_ERR;

    if(*token->lexeme == *JSON_TRUE_STR) {
        *generic = genericCompose(&Bool);
        *((char*)genericData(*generic)) = 1;
    } else if(*token->lexeme == *JSON_FALSE_STR) {
        *generic = genericCompose(&Bool);
        *((char*)genericData(*generic)) = 0;
    } else {
        return STATUS_PARSE_ERR;
    }

    listNext(&myIt);

    *iterator = myIt;
    return STATUS_OK;
}

unsigned int parseNull(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;
    struct JSONToken* token = listCurrent(&myIt);
    if(token && token->token != JSON_TOKEN_NULL) return STATUS_PARSE_ERR;

    *generic = genericCompose(&Pointer);
    *((void**)genericData(*generic)) = NULL;

    listNext(&myIt);

    *iterator = myIt;
    return STATUS_OK;
}

unsigned int parseElements(struct Generic* vector, struct Iterator* iterator) {
    unsigned int elementCount = 0;
    while(1) {
        struct Generic* generic = NULL;
        unsigned int result = parseElement(&generic, iterator);
        if(result == STATUS_OK) {
            // TODO: Handle result.
            genericAdd(vector, "-1", generic);
            elementCount++;
        }
        struct JSONToken* token = listCurrent(iterator);
        if(!token || *token->lexeme != JSON_SEPERATOR) {
            break;
        }
        token = listNext(iterator);
    }
    return STATUS_OK;
}

unsigned int parseArray(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;
    struct JSONToken* token = listCurrent(&myIt);
    if(token && *token->lexeme != JSON_ARR_BEGIN) {
        return STATUS_PARSE_ERR;
    }
    token = listNext(&myIt);

    struct Generic* vector = genericCompose(&Array.object);
    if(token && *token->lexeme == JSON_ARR_CLOSE) {
        listNext(&myIt);
        *generic = vector;
        *iterator = myIt;
        return STATUS_OK;
    }

    parseElements(vector, &myIt);
    token = listCurrent(&myIt);
    if(!token) return STATUS_PARSE_ERR;

    if(token && *token->lexeme == JSON_ARR_CLOSE) {
        token = listNext(&myIt);
        *generic = vector;
        *iterator = myIt;
        return STATUS_OK;
    }

    genericRelease(vector);
    return STATUS_PARSE_ERR;
}

unsigned int parseMembers(struct Generic* map, struct Iterator* iterator) {
    struct JSONToken* token = NULL;
    while(1) {
        char* key = NULL;
        struct Generic* value = NULL;
        unsigned int result = parseMember(&key, &value, iterator);
        if(result == STATUS_OK) {
            // TODO: Handle result.
            genericAdd(map, key, value);
        }
        token = listCurrent(iterator);
        if(!token || *token->lexeme != JSON_SEPERATOR) break;
        token = listNext(iterator);
    }
    return STATUS_OK;
}

static unsigned int parseObject(struct Generic** generic, struct Iterator* iterator) {
    struct Iterator myIt = *iterator;

    struct JSONToken* token = listCurrent(&myIt);
    if(token && *token->lexeme != JSON_MAP_BEGIN) {
        return STATUS_PARSE_ERR;
    }
    token = listNext(&myIt);

    struct Generic* map = genericCompose(&Map.object);
    if(token && *token->lexeme == JSON_MAP_CLOSE) {
        listNext(&myIt);
        *generic = map;
        *iterator = myIt;
        return STATUS_OK;
    }

    parseMembers(map, &myIt);
    token = listCurrent(&myIt);
    if(!token) return STATUS_PARSE_ERR;

    if(token && *token->lexeme == JSON_MAP_CLOSE) {
        token = listNext(&myIt);
        *generic = map;
        *iterator = myIt;
        return STATUS_OK;
    }

    genericRelease(map);
    return STATUS_PARSE_ERR;
}

unsigned int parseElement(struct Generic** generic, struct Iterator* iterator) {
    parseWhitespace(iterator);/*
    unsigned int (*parsers[])(struct Generic**, struct Iterator*) = {
        parseArray,
        parseObject,
        parseString,
        parseNumber,
        parseBoolean,
        parseNull
    };
    for(unsigned int i = 0; i < sizeof(parsers) / sizeof(parsers[0]); i++) {
        unsigned int result = parsers[i](generic, iterator);
        parseWhitespace(iterator);
        if(result == STATUS_OK) return result;
    }
    return STATUS_PARSE_ERR;*/


    unsigned int status;
    if( !(status = parseArray(generic, iterator)) ||
        !(status = parseObject(generic, iterator)) ||
        !(status = parseString(generic, iterator)) ||
        !(status = parseNumber(generic, iterator)) ||
        !(status = parseBoolean(generic, iterator)) ||
        !(status = parseNull(generic, iterator)));
    parseWhitespace(iterator);
    return status;
}

static inline unsigned int parseMember(char** key, struct Generic** value, struct Iterator* iterator) {
    parseWhitespace(iterator);
    struct Generic* string = NULL;
    unsigned int result = parseString(&string, iterator);
    if(result) return result;
    // TODO: Make generic indexes?
    *key = *((char**)genericData(string));
    free(string);

    struct JSONToken* token = listCurrent(iterator);
    parseWhitespace(iterator);
    token = listCurrent(iterator);

    if(token && *token->lexeme != JSON_MEMBER_SEP) {
        free(*key);
        return STATUS_PARSE_ERR;
    }
    token = listNext(iterator);
    result = parseElement(value, iterator);
    if(result) {
        free(*key);
        return result;
    }
    return STATUS_OK;
}

unsigned int parseJSON(struct Generic** generic, char* toCheck) {
    *generic = NULL;
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void*))tokenRelease;

    int result = lexJSON(&tokens, toCheck);
    if(result) {
        listRelease(&tokens);
        return STATUS_PARSE_ERR;
    }

    struct Iterator iterator = listIterator(&tokens);
    result = parseElement(generic, &iterator);

    if(listNext(&iterator)) {
        // Garbage followed valid JSON.
        genericRelease(*generic);
        result = STATUS_PARSE_ERR;
    }

    listRelease(&tokens);
    return result;
}
