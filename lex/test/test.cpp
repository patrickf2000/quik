#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <string.h>

#include "test.hh"
#include "../strutils.hh"
#include "../lex.hh"

//Our equivalent of assert
void confirm(std::vector<Token> actual, std::vector<Token> expected) {
	if (actual.size() != expected.size()) {
		std::cout << "Error: Sizes do not match!" << std::endl;
		std::exit(1);
	}

	for (int i = 0; i<actual.size(); i++) {
		auto t1 = actual.at(i);
		auto t2 = expected.at(i);
		
		if (t1.type != t2.type) {
			std::cout << "Error: Types do not match!" << std::endl;
			std::exit(1);
		}
		
		//TODO: Compare IDs
	}
}

//Tests our lexical function
//To simplify things, we use an external test file
void test(bool verbose) {
	std::cout << "Lexical test..." << std::endl;
	std::vector<std::string> content;
	
	//First, load the test file
	std::ifstream reader("../test/lex-test");
	if (reader.is_open()) {
		std::string ln = "";
		
		while (std::getline(reader, ln)) {
			if (ln.length() == 0) {
				continue;
			}
			
			content.push_back(ln);
		}
		
		reader.close();
	} else {
		std::cout << "Error: Unable to open lexical test file." << std::endl;
		std::exit(1);
	}
	
	//Now test everything
	for (int i = 0; i<content.size(); i+=2) {
		//Get our strings
		std::string input = content.at(i);
		std::string expected_str = content.at(i+1);
		
		std::cout << "[TEST] " << input << std::endl;
		
		//Break up the expected string
		std::vector<Token> expected;
		std::string current = "";
		
		for (char c : expected_str) {
			if (c == '~') {
				Token t;
			
				if (contains(current, '>')) {
					std::string type = strtok((char *)current.c_str(), ">");
					t.id = strtok(NULL, " ");
					t.type = str2type(type);
				} else {
					t.type = str2type(current);
				}
				
				expected.push_back(t);
				current = "";
			} else {
				current += c;
			}
		}
		
		auto actual = tokenize(input);
		confirm(actual, expected);
	}
	
	std::cout << std::endl;
}

