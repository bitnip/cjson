#ifndef __JSON_PARSER_H
#define __JSON_PARSER_H
#ifdef __cplusplus
extern "C"{
#endif

#include "json_lexer.h"
#include "cutil/src/generic/generic.h"

unsigned int parseJSON(struct Generic **generic, char* toCheck);

#ifdef __cplusplus
}
#endif
#endif
