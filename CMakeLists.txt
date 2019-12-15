cmake_minimum_required(VERSION 3.0.0)
project(quik)

include_directories(parser)

add_compile_options(-g -std=c++11)

set(PARSER_SRC
	parser/analyzer.cpp
	parser/ast.cpp
	parser/build.cpp
	parser/lex.cpp
	parser/parse.cpp
	parser/strutils.cpp
)

set(COMPILER_SRC
	compiler/main.cpp
	compiler/asmx86.cpp
)

add_library(qkparser ${PARSER_SRC})

add_executable(lxtest parser/test/lex_test.cpp)
add_executable(parser_test parser/test/parser_test.cpp)
add_executable(quikc ${COMPILER_SRC})

target_link_libraries(lxtest qkparser)
target_link_libraries(parser_test qkparser)
target_link_libraries(quikc qkparser)
