cmake_minimum_required(VERSION 3.0.0)
project(quik)

include_directories(parser ltac)

add_compile_options(-g -std=c++11)

set(PARSER_SRC
	parser/analyzer.cpp
	parser/ast.cpp
	parser/build.cpp
	parser/lex.cpp
	parser/optimize.cpp
	parser/parse.cpp
	parser/strutils.cpp
	parser/syntax.cpp
)

set(COMPILER_SRC
	compiler/main.cpp
	compiler/utils.cpp
	compiler/armv7/armv7.cpp
	compiler/armv7/armv7_flow.cpp
	compiler/armv7/armv7_funcs.cpp
	compiler/armv7/armv7_utils.cpp
	compiler/armv7/armv7_vars.cpp
	compiler/x86/asmx86.cpp
	compiler/x86/x86_array.cpp
	compiler/x86/x86_flow.cpp
	compiler/x86/x86_funcs.cpp
	compiler/x86/x86_struct.cpp
	compiler/x86/x86_utils.cpp
	compiler/x86/x86_vars.cpp
)

add_library(qkparser ${PARSER_SRC})

add_executable(lextest parser/test/lex_test.cpp)
add_executable(qkast parser/test/parser_test.cpp)
add_executable(quikc ${COMPILER_SRC})

target_link_libraries(lextest qkparser)
target_link_libraries(qkast qkparser)
target_link_libraries(quikc qkparser)

