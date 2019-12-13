cmake_minimum_required(VERSION 3.0.0)
project(upl)

include_directories(parser)

set(PARSER_SRC
	parser/ast.cpp
	parser/build.cpp
	parser/lex.cpp
	parser/parse.cpp
	parser/strutils.cpp
)

add_library(uplparser ${PARSER_SRC})

add_executable(lxtest parser/test/lex_test.cpp)
add_executable(parser_test parser/test/parser_test.cpp)

target_link_libraries(lxtest uplparser)
target_link_libraries(parser_test uplparser)
