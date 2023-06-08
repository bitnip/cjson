#include <stdlib.h>
#include "json_parser.h"
#include "json_unparser.h"
#include "cutil/src/assertion.h"

#include "cutil/src/map/map.h"
#include "cutil/src/error.h"

void testJSONUnparseEmptySequence() {
    char input[] = "[]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {0, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, "[]");
    free(output);
}

void testJSONUnparseSeqOfSeq() {
    char input[] = "[[],[],[]]";
    char expected[] = "[[],[],[]]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {0, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, expected);
    free(output);
}

void testJSONUnparseWhitespaceMap() {
    char input[] = "{\r\n    \"a\": 0,\r\n    \"b\": 1\r\n}";
    // TODO: Order of elements is not defined by cjson...
    char expected[] = "{\r\n    \"b\": 1,\r\n    \"a\": 0\r\n}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {4, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, expected);
    free(output);
}

void testJSONUnparseWhitespaceArray() {
    char input[] = "[\r\n    0,\r\n    1\r\n]";
    char expected[] = "[\r\n    0,\r\n    1\r\n]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {4, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, expected);
    free(output);
}

void testJSONUnparseWhitespaceEmptyMap() {
    char input[] = "{}";
    // TODO: Order of elements is not defined by cjson...
    char expected[] = "{}";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {4, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, expected);
    free(output);
}

void testJSONUnparseWhitespaceEmptyArray() {
    char input[] = "[]";
    // TODO: Order of elements is not defined by cjson...
    char expected[] = "[]";
    struct Generic *generic;
    int result = parseJSON(&generic, input);
    assertIntegersEqual(result, STATUS_OK);

    char* output;
    unsigned int outputLength;
    struct JSONFormat fmt = {4, 0, 0};
    result = unparseJSON(generic, &output, &outputLength, fmt);
    assertIntegersEqual(result, STATUS_OK);
    assertStringsEqual(output, expected);
    free(output);
}

void testJSONUnparser() {
    testJSONUnparseEmptySequence();
    testJSONUnparseSeqOfSeq();
    testJSONUnparseWhitespaceMap();
    testJSONUnparseWhitespaceArray();
    testJSONUnparseWhitespaceEmptyMap();
    testJSONUnparseWhitespaceEmptyArray();
}