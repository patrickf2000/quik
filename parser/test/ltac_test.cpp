#include <iostream>
#include <string>

#include <build.hh>
#include <ast.hh>
#include <ltac.hh>
#include <ltac_build.hh>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input file." << std::endl;
		return 1;
	}
	
	auto lines = load_source(argv[1]);
	AstNode *top = build_ast(lines, false, false);
	
	LTAC_Builder *builder = new LTAC_Builder;
	LtacFile *file = builder->build_file(top);
	
	print_ltac(file);
	
	delete builder;
	delete file;
	delete top;
	
	return 0;
}
