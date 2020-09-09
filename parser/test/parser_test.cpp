#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include <lex.hh>
#include <ast.hh>

#include "build.hh"
#include "parse.hh"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Error: No input file." << std::endl;
        return 1;
	}
    
    bool optimize = false;
    if (argc == 3) {
        if (std::string(argv[2]) == "--optimize") {
            optimize = true;
        }
    }
	
    auto lines = load_source(argv[1]);
    AstTree *tree = build_ast(lines, false, optimize);
    
    std::ofstream writer("ast.dot");
    writer << tree->writeDot() << std::endl;
    writer.close();
    
    system("dot ast.dot -Tpng -o ast.png");
		
    delete tree;
	
	return 0;
}
