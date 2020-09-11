cmake_minimum_required(VERSION 3.0.0)
project(quik)

include_directories(parser config compiler)

add_compile_options(-g -std=c++11)

set(PARSER_SRC
	parser/analyzer.cpp
	parser/ast.cpp
	parser/build.cpp
	parser/conditional.cpp
	parser/data.cpp
	parser/funcs.cpp
	parser/lex.cpp
	parser/loop.cpp
	parser/parse.cpp
	parser/strutils.cpp
	parser/syntax.cpp
    parser/utils.cpp
	parser/vars.cpp
	parser/optimization/funcs.cpp
	parser/ltac/ltac.cpp
	parser/ltac/ltac_array.cpp
	parser/ltac/ltac_build.cpp
	parser/ltac/ltac_flow.cpp
	parser/ltac/ltac_func.cpp
	parser/ltac/ltac_vars.cpp
)

set(COMPILER_SRC
	compiler/arm7/arm7_funcs.cpp
	compiler/arm7/asm_arm7.cpp
	compiler/base/asm_base.cpp
	compiler/x64/asm_x64.cpp
	compiler/x64/x64_array.cpp
	compiler/x64/x64_flow.cpp
	compiler/x64/x64_funcs.cpp
	compiler/x64/x64_vars.cpp
	compiler/compiler.cpp
	compiler/main.cpp
	compiler/utils.cpp
)

add_library(qkparser ${PARSER_SRC})

add_executable(qkast parser/test/parser_test.cpp)
add_executable(qkasm parser/test/ltac_test.cpp)
add_executable(quikc ${COMPILER_SRC})

target_link_libraries(qkast qkparser)
target_link_libraries(qkasm qkparser)
target_link_libraries(quikc qkparser)

