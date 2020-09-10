#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <algorithm>

#include "build.hh"
#include "strutils.hh"
#include "parse.hh"
#include "syntax.hh"

#include "optimization/optimize.hh"

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
		if (ln.length() > 0 && ln[0] != '#') {
			Line l;
			l.no = ln_no;
			l.original = ln;
			//l.tokens = tokenize(ln);
			lines.push_back(l);
		}
		
		++ln_no;
	}
	
	reader.close();
	
	//Now check for includes
	std::vector<std::string> included;
	bool found_include = false;
	
	//Loop until there are no includes
	do {
		found_include = false;
	
		auto old_lines = lines;
		lines.clear();
		
		for (auto l : old_lines) {
			if (starts_with(l.original, "include")) {
				found_include = true;
			
				int sp = l.original.find_first_of(" ");
				int len = l.original.length();
				std::string name = l.original.substr(sp+1, len);
				
				if (std::find(included.begin(), included.end(), name) != included.end()) {
					continue;
				}
				
				included.push_back(name);
				
				std::string path = "/usr/local/include/quik/";
				path += name + ".qk";
				
				std::ifstream reader2(path.c_str());
				if (!reader2.is_open()) {
					std::cout << "Fatal: Invalid include." << std::endl;
					std::cout << l.original << std::endl;
					std::exit(1);
				}
				
				while (std::getline(reader2, ln)) {
					ln = trim(ln);
					if (ln.length() > 0 && ln[0] != '#') {
						Line l2;
						l2.original = ln;
						lines.push_back(l2);
					}
				}
				
				reader2.close();
			} else {
				lines.push_back(l);
			}
		}
	} while (found_include);
    
    // Write the lines back to a temporary file
    std::ofstream writer("tmp.qk");
    
    for (auto ln : lines) {
        writer << ln.original << std::endl;
    }
    
    writer.close();
	
	//Now tokenize
    Scanner *scanner = new Scanner;
    
    // Use the new method
    auto lines2 = scanner->tokenize2("tmp.qk");
    
    // Use the old method
	for (int i = 0; i<lines.size(); i++) {
		Line *l = &lines.at(i);
		l->tokens = scanner->tokenize(l->original);
	}
    
    delete scanner;
    
    // Run the check
    std::cout << "Running lexical check..." << std::endl;
    
    if (lines2.size() != lines.size()) {
        std::cout << "Error: Lexical check failed- invalid line size." << std::endl;
        std::cout << "Expected: " << lines.size() << std::endl;
        std::cout << "Actual: " << lines2.size() << std::endl;
    } else {
        for (int i = 0; i<lines.size(); i++) {
            auto l1 = lines[i];
            auto l2 = lines2[i];
            
            if (l1.tokens.size() != l2.tokens.size()) {
                std::cout << "Error: Lexical check failed- invalid token size." << std::endl;
                std::cout << "Expected: " << l1.tokens.size() << std::endl;
                std::cout << "Actual: " << l2.tokens.size() << std::endl;
                std::cout << std::endl;
                std::cout << "Error is at: " << i << std::endl;
                break;
            }
            
            for (int j = 0; j<l1.tokens.size(); j++) {
                auto t1 = l1.tokens[j];
                auto t2 = l2.tokens[j];
                
                if (t1.type != t2.type) {
                    std::cout << "Error: Token at " << j << " is invalid." << std::endl;
                    std::cout << "Expected: " << t1.type << std::endl;
                    std::cout << "Actual: " << t2.type << std::endl;
                    std::cout << std::endl;
                }
                
                switch (t1.type) {
                    case TokenType::NO:
                    case TokenType::DEC:
                    case TokenType::HEX:
                    case TokenType::STRING:
                    case TokenType::CHAR: {
                        if (t1.id != t2.id) {
                            std::cout << "Error: Token ID at " << j << " is invalid." << std::endl;
                            std::cout << "Expected: " << t1.id << std::endl;
                            std::cout << "Actual: " << t2.id << std::endl;
                            std::cout << std::endl;  
                        }
                    } break;
                }
            }
        }
    }
	
	return lines;
}

//Takes each line and builds the AST
AstTree *build_ast(std::vector<Line> lines, bool fail, bool optimize) {
	AstScope *top = new AstScope;
	std::vector<AstNode *> nodes;
	
	QkParser *parser = new QkParser;
	
	for (auto ln : lines) {
		auto n = parser->build_node(ln);
		n->ln = ln;
		
		if (n->children.size() > 0) {
			for (int i = 0; i<n->children.size(); i++) {
				n->children.at(i)->ln = ln;
			}
		}
		
		if (n == nullptr) {
			syntax_error(ln, "Unknown input");
		} else {
			//This inserts an end statement before an elif statementn
			if (n->type == AstType::Elif || n->type == AstType::Else) {
				AstNode *end = new AstNode(AstType::End);
				nodes.push_back(end);
			}
			
			nodes.push_back(n);
		}
	}
	
	parser->build_tree(nodes, top);
	parser->analyze_and_check(top);
	
	delete parser;
	
	//Perform syntax checking
	SyntaxCheck check;
	check.check_global(top);
	check.check_vars(top, top->vars);
	check.check_lp_vars(top, top->vars);
	check.evaluate(fail);		//TODO: Something else rather than bomb out in function?
	
	//Optimize
	if (optimize) {
		OptimizeEngine *engine = new OptimizeEngine(top);
		engine->remove_uncalled();
		
		delete engine;
	}
	
    AstTree *tree = new AstTree("file");
    tree->children.push_back(top);
	return tree;
}
