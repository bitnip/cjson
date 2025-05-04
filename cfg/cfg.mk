SOURCE= \
	src/json.c \
	src/json_lexer.c \
	src/json_parser.c \
	src/json_unparser.c
TEST_SOURCE= \
	src/test.c \
	src/json_lexer_test.c \
	src/json_parser_test.c \
	src/json_unparser_test.c
LIBRARIES=-L../cutil/bin -lcutil
INCLUDES=-I../

COVERAGE_CC=gcc
ifeq ($(shell uname -s),Darwin)
	CC=gcc
endif
ifeq ($(shell uname -s),Linux)
	CC=gcc
endif
ifeq ($(OS),Windows_NT)
	CC=x86_64-w64-mingw32-gcc
endif