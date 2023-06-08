#ifndef __JSON_H
#define __JSON_H
#ifdef __cplusplus
extern "C"{
#endif

#define JSON_MEMBER_SEP ':'
#define JSON_SPACE ' '
#define JSON_TAB '\t'
#define JSON_SEPERATOR ','
#define JSON_ARR_BEGIN '['
#define JSON_ARR_CLOSE ']'
#define JSON_MAP_BEGIN '{'
#define JSON_MAP_CLOSE '}'

extern const char JSON_TRUE_STR[];
extern const char JSON_FALSE_STR[];
extern const char JSON_NULL_STR[];

#ifdef __cplusplus
}
#endif
#endif
