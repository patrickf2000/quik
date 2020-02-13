#include <iostream>
#include <string>

#include <ast.hh>
#include <build.hh>

#include <ltac.hh>
#include <ltac_build.hh>
#include <ltac_file.hh>

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Fatal: No input file." << std::endl;
		return 1;
	}
	
	auto lines = load_source(argv[1]);
	AstNode *top = build_ast(lines, true, false);
	
	LTAC_Builder *builder = new LTAC_Builder("out_test.asm");
	builder->assemble(top);
	LtacFile *file = builder->get_file();
	write(file);
	
	delete file;
	delete builder;
	delete top;
	
	return 0;
}
