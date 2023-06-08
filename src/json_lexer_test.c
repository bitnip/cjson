#include "json_lexer.h"
#include "cutil/src/assertion.h"

void testJSONLexTerminator() {
    char input[] = "";
    struct List tokens;
    listCompose(&tokens);
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 0);
    listRelease(&tokens);
}

void testJSONLexEmptyString() {
    char input[] = "\"\"";
    struct List tokens;
    listCompose(&tokens);
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 1);
    listRelease(&tokens);
}

void testJSONLexEmptyObject() {
    char input[] = "{}";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 2);
    listRelease(&tokens);
}

void testJSONLexNumber() {
    char input[] = "-9.1E10";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 1);
    
    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertIntegersEqual(token->token, JSON_TOKEN_NUMBER);
    assertPointersEqual(token->lexeme, input);

    listRelease(&tokens);
}

void testJSONLexNull() {
    char input[] = "null";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 1);
    
    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertIntegersEqual(token->token, JSON_TOKEN_NULL);
    assertPointersEqual(token->lexeme, input);

    listRelease(&tokens);
}

void testJSONLexTrue() {
    char input[] = "true";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 1);
    listRelease(&tokens);
}

void testJSONLexFalse() {
    char input[] = "false";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
    assertIntegersEqual(listSize(&tokens), 1);
    listRelease(&tokens);
}

void testJSONLexEmptySequence() {
    char input[] = "[]";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);
    assertIntegersEqual(result, 0);
   	assertIntegersEqual(listSize(&tokens), 2);
    listRelease(&tokens);
}

void testJSONLexSimpleMap() {
    char input[] = "{\"key\": \"value\"}";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);

    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+1);
    assertIntegersEqual(token->token, JSON_TOKEN_STRING);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+6);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+7);
    assertIntegersEqual(token->token, JSON_TOKEN_WHITESPACE);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+8);
    assertIntegersEqual(token->token, JSON_TOKEN_STRING);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+15);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertIsNull(token);

    assertIntegersEqual(result, 0);

    listRelease(&tokens);
}

void testJSONLexSimpleArray() {
    char input[] = "[\"key\", 9.5, false]";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);

    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+1);
    assertIntegersEqual(token->token, JSON_TOKEN_STRING);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+6);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+7);
    assertIntegersEqual(token->token, JSON_TOKEN_WHITESPACE);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+8);
    assertIntegersEqual(token->token, JSON_TOKEN_NUMBER);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+11);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+12);
    assertIntegersEqual(token->token, JSON_TOKEN_WHITESPACE);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+13);
    assertIntegersEqual(token->token, JSON_TOKEN_BOOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+18);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertIsNull(token);

    assertIntegersEqual(result, 0);

    listRelease(&tokens);
}

void testJSONLexMultiLine() {
    char input[] = "[null,\nnull]";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);

    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+1);
    assertIntegersEqual(token->token, JSON_TOKEN_NULL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+5);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+6);
    assertIntegersEqual(token->token, JSON_TOKEN_NEWLINE);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+7);
    assertIntegersEqual(token->token, JSON_TOKEN_NULL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+11);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertIsNull(token);

    assertIntegersEqual(result, 0);

    listRelease(&tokens);
}

void testJSONLexWithInvalid() {
    char input[] = "@{$$}**";
    struct List tokens;
    listCompose(&tokens);
    tokens.freeData = (void (*)(void *))tokenRelease;
    unsigned int result = lexJSON(&tokens, input);

    struct Iterator iterator = listIterator(&tokens);
    struct JSONToken *token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input);
    assertIntegersEqual(token->token, JSON_TOKEN_INVALID);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+1);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+2);
    assertIntegersEqual(token->token, JSON_TOKEN_INVALID);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+4);
    assertIntegersEqual(token->token, JSON_TOKEN_SYMBOL);

    token = listNext(&iterator);
    assertNotNull(token);
    assertPointersEqual(token->lexeme, input+5);
    assertIntegersEqual(token->token, JSON_TOKEN_INVALID);

    assertIntegersEqual(result, 3);
    assertIntegersEqual(listSize(&tokens), 5);
    listRelease(&tokens);
}

void testJSONLexer() {

    testJSONLexTerminator();
    testJSONLexEmptyString();
    testJSONLexEmptyObject();

    testJSONLexNumber();
    testJSONLexNull();
    testJSONLexTrue();
    testJSONLexFalse();

    testJSONLexEmptySequence();
    testJSONLexSimpleMap();
    testJSONLexSimpleArray();
    testJSONLexMultiLine();
    testJSONLexWithInvalid();
}