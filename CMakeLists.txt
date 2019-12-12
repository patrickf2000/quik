cmake_minimum_required(VERSION 3.0.0)
project(upl)

set(LEX_SRC
	lex/lex.cpp
	lex/strutils.cpp
)

set(LEX_TEST_SRC
	lex/test/main.cpp
	lex/test/test.cpp
)

add_library(upllex ${LEX_SRC})
add_executable(lxtest ${LEX_TEST_SRC})

target_link_libraries(lxtest upllex)
