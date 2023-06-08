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