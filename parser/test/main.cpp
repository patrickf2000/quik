#include <iostream>
#include <ast.hh>

int main(int argc, char *argv[]) {
	AstNode *file = new AstNode(AstType::Global);
	
	AstNode *i1 = new AstNode(AstType::Include);
	AstNode *i2 = new AstNode(AstType::Include);
	
	AstNode *fmain = new AstNode(AstType::FuncDec);
	AstNode *fc1 = new AstNode(AstType::FuncCall);
	AstNode *fc2 = new AstNode(AstType::FuncCall);
	
	file->children.push_back(i1);
	file->children.push_back(i2);
	file->children.push_back(fmain);
	
	fmain->children.push_back(fc1);
	fmain->children.push_back(fc2);
	
	print_tree(file);
	
	delete fc1;
	delete fc2;
	
	delete fmain;
	delete i1;
	delete i2;
	
	delete file;
	
	return 0;
}
