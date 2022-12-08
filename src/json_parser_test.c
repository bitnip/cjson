#include "json_lexer.h"
#include "json_parser.h"
#include "cutil/src/assertion.h"

#include "cutil/src/error.h"
#include "cutil/src/map/map.h"

void testJSONParseEmptySequence() {
    char input[] = "[]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONParseSeqOfSeq() {
    char input[] = "[[],[],[]]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONParseEmptyMap() {
    char input[] = "{}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONParseMapOfMap() {
    char input[] = "{\"a\":{},\"b\":{},\"c\":{}}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONParseEmptyString() {
    char input[] = "\"\"";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONMixed() {
    char input[] = "{ \"a\" : [ \"foo\" , 9 ] , \"b\" : { \"d\": 21.1 , \"e\": null } , \"c\" : {\"t\": true, \"f\": false} }";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
    assertNotNull(generic);

    char paths[][5] = {
        "a",
        "a.0",
        "a.1",
        "b",
        "b.d",
        "b.e",
        "c"
    };
    // Check the root map has three elements.
    struct Map *root = (struct Map*)genericData(generic);
    assertIntegersEqual(root->size, 3);
    // Check the array at `a`.
    struct Generic *element = getAt(generic, paths[0]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Array.object);
    // Check the string at `a.0`.
    element = getAt(generic, paths[1]);
    assertNotNull(element);
    assertPointersEqual(element->object, &String);
    // Check the integer at `a.1`.
    element = getAt(generic, paths[2]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Integer);
    long integerValue = *((long*)genericData(element));
    assertIntegersEqual(integerValue, 9);

    // Check the map at `b`.
    element = getAt(generic, paths[3]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Map.object);
    // Check the float at `b.d`.
    element = getAt(generic, paths[4]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Float);
    float floatValue = *((float*)genericData(element));
    assertFloatsEqual(floatValue, 21.1);
    // Check Null at `b.e`.
    element = getAt(generic, paths[5]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Pointer);
    void* pointerValue = *((void**)genericData(element));
    assertPointersEqual(pointerValue, NULL);

    element = getAt(generic, paths[6]);
    assertNotNull(element);
    assertPointersEqual(element->object, &Map.object);
}

void testJSONParseObjectNoComma() {
    char input[] = "{\"a\": 1 \"b\": 2}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParseObjectNoClosingBracket() {
    char input[] = "{\"a\": 1, \"b\": 2";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParseObjectNoOpeningBracket() {
    char input[] = "\"a\": 1, \"b\": 2}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParseArrayNoComma() {
    char input[] = "[1 2]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParseArrayNoClosingBracket() {
    char input[] = "[1, 2";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParseArrayNoOpeningBracket() {
    char input[] = "1, 2]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_PARSE_ERR);
}

void testJSONParserFormatted() {
    char input[] = "{\"a\":2,\"b\":{},\"c\":{}}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);
}

void testJSONParser() {
    testJSONParseEmptySequence();
    testJSONParseSeqOfSeq();

    testJSONParseEmptyMap();
    testJSONParseMapOfMap();
    testJSONParseEmptyString();
    testJSONMixed();
    testJSONParseObjectNoComma();
    testJSONParseObjectNoClosingBracket();
    testJSONParseObjectNoOpeningBracket();

    testJSONParserFormatted();

    testJSONParseArrayNoComma();
    testJSONParseArrayNoClosingBracket();
    testJSONParseArrayNoOpeningBracket();
}