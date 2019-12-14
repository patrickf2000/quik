#include <iostream>

#include "ast.hh"

//Returns an AST type as a string (debugging purposes)
std::string ast2str(AstType type) {
	switch (type) {
		case AstType::Scope: return "Scope";
		case AstType::Include: return "Include";
		case AstType::FuncDec: return "FuncDec";
		case AstType::FuncCall: return "FuncCall";
		case AstType::Return: return "Return";
		case AstType::End: return "End";
		case AstType::VarDec: return "VarDec";
		case AstType::Int: return "Int";
		case AstType::Id: return "ID";
		case AstType::Str: return "Str";
		
		case AstType::Add: return "OP: +";
		case AstType::Sub: return "OP: -";
		case AstType::Mul: return "OP: *";
		case AstType::Div: return "OP: /";
		case AstType::Mod: return "OP: %";
	}
	
	return "NONE";
}

//Returns a data type as a string (debugging purposes)
std::string type2str(DataType type) {
	switch (type) {
		case DataType::Byte: return "Byte";
		case DataType::Char: return "Char";
		case DataType::Short: return "Short";
		case DataType::Int: return "Int";
		case DataType::Long: return "Long";
		case DataType::Float: return "Float";
		case DataType::Bool: return "Bool";
		case DataType::Str: return "Str";
	}
	
	return "NONE";
}

//Prints an AST tree
void print_tree(AstNode *node, int indent) {
	for (int i = 0; i<indent; i++) {
		std::cout << "  ";
	}
	std::cout << ast2str(node->type);
	
	if (node->type == AstType::Scope) {
		AstScope *scope = dynamic_cast<AstScope *>(node);
		
		if (scope->vars.size() > 0) {
			std::cout << " <";
			for (auto v : scope->vars) {
				std::cout << "{" << v.second.name << "} ";
			}
			std::cout << ">";
		}
	} else if (node->type == AstType::Include) {
		std::cout << " [" 
			<< dynamic_cast<AstInclude *>(node)->get_include()
			<< "]";
	} else if (node->type == AstType::FuncDec) {
		std::cout << " [" 
			<< dynamic_cast<AstFuncDec *>(node)->get_name()
			<< "]";
	} else if (node->type == AstType::FuncCall) {
		AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
		std::cout << " [" << fc->get_name() << "]";
		
	} else if (node->type == AstType::VarDec) {
		AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
		std::cout << " [" << vd->get_name() << "] ("
			<< type2str(vd->get_type()) << ")";
		
	//values
	} else if (node->type == AstType::Int) {
		std::cout << " " << dynamic_cast<AstInt *>(node)->get_val();
	} else if (node->type == AstType::Id) {
		std::cout << " " << dynamic_cast<AstID *>(node)->get_name();
	} else if (node->type == AstType::Str) {
		std::cout << " " << dynamic_cast<AstString *>(node)->get_val();
	}
	
	std::cout << std::endl;

	if (node->children.size() > 0) {
		for (auto c : node->children) {
			print_tree(c, indent+1);
		}
	}
}
