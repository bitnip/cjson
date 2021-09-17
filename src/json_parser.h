#ifndef __JSON_PARSER_H
#define __JSON_PARSER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "json_lexer.h"
#include "cutil/src/generic/generic.h"

enum JSON_TYPE {
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY,
    JSON_TYPE_STRING,
    JSON_TYPE_NUMBER,
    JSON_TYPE_TRUE,
    JSON_TYPE_FALSE,
    JSON_TYPE_NULL
};

struct JSONPair {
    struct JSONPair *next;
    char *name;
    enum JSON_TYPE type;
};
struct JSONObject {
    enum JSON_TYPE type;
    struct JSONPair *pairs;
};

struct JSONValue {
    struct JSONValue *next;
    enum JSON_TYPE type;
};
struct JSONArray {
    enum JSON_TYPE type;
    struct JSONValue *values;
};

struct JSONString {
    enum JSON_TYPE type;
    char *value;
};

struct JSONNumber {
    enum JSON_TYPE type;
    int integer;
    int fraction;
    int exponent;
};

unsigned int parseJSON(struct Generic **generic, char* toCheck);

#ifdef __cplusplus
}
#endif
#endif
