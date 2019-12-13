cmake_minimum_required(VERSION 3.0.0)
project(upl)

include_directories(lex)
include_directories(parser)

set(LEX_SRC
	lex/lex.cpp
	lex/strutils.cpp
)

set(LEX_TEST_SRC
	lex/test/main.cpp
	lex/test/test.cpp
)

set(PARSER_SRC
	parser/ast.cpp
	parser/parse.cpp
)

set(PARSER_TEST_SRC
	parser/test/main.cpp
)

add_library(upllex ${LEX_SRC})
add_library(uplparser ${PARSER_SRC})

add_executable(lxtest ${LEX_TEST_SRC})
add_executable(parser_test ${PARSER_TEST_SRC})

target_link_libraries(lxtest upllex)
target_link_libraries(uplparser upllex)
target_link_libraries(parser_test uplparser)
