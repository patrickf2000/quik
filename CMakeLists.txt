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
	parser/syntax.cpp
)

set(COMPILER_SRC
	compiler/main.cpp
	compiler/asmx86.cpp
	compiler/utils.cpp
)

set(STDLIB_SRC
	stdlib/test.c
	stdlib/stdio.c
)

add_library(qkparser ${PARSER_SRC})
add_library(qkstdlib SHARED ${STDLIB_SRC})

add_executable(lxtest parser/test/lex_test.cpp)
add_executable(parser_test parser/test/parser_test.cpp)
add_executable(quikc ${COMPILER_SRC})

set_target_properties(qkstdlib PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

target_link_libraries(lxtest qkparser)
target_link_libraries(parser_test qkparser)
target_link_libraries(quikc qkparser)
