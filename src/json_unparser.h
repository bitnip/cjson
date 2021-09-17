#ifndef __JSON_UNPARSER_H
#define __JSON_UNPARSER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "json_lexer.h"
#include "cutil/src/generic/generic.h"

struct JSONFormat {
    unsigned char indent;
    unsigned char level;
    unsigned char useTabs;
};

unsigned int unparseJSON(
    struct Generic *generic,
    char** output,
    unsigned int *outputLength,
    struct JSONFormat fmt);

#ifdef __cplusplus
}
#endif
#endif
