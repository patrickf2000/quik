#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include "build.hh"
#include "strutils.hh"
#include "parse.hh"
#include "analyzer.hh"

//Load the source file and run it through lex
std::vector<Line> load_source(const char *path) {
	std::vector<Line> lines;
	std::ifstream reader(path);
	
	if (!reader.is_open()) {
		std::cout << "Fatal: Unable to open input file." << std::endl;
		std::exit(1);
	}
	
	std::string ln = "";
	int ln_no = 1;
	
	while (std::getline(reader, ln)) {
		ln = trim(ln);
		if (ln.length() > 0) {
			Line l;
			l.no = ln_no;
			l.original = ln;
			l.tokens = tokenize(ln);
			lines.push_back(l);
		}
		
		++ln_no;
	}
	
	reader.close();
	return lines;
}

//Takes each line and builds the AST
AstNode *build_ast(std::vector<Line> lines) {
	AstScope *top = new AstScope;
	std::vector<AstNode *> nodes;
	
	for (auto ln : lines) {
		auto n = build_node(ln);
		
		if (n == nullptr) {
			syntax_error(ln, "Unknown input");
		} else {
			nodes.push_back(n);
		}
	}
	
	build_tree(nodes, top);
	find_variables(top);
	return top;
}
