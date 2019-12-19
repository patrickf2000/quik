#include <iostream>

#include "ast.hh"

//Returns an AST type as a string (debugging purposes)
std::string ast2str(AstType type) {
	switch (type) {
		case AstType::Scope: return "Scope";
		case AstType::Include: return "Include";
		case AstType::ExternFunc:
		case AstType::FuncDec: return "FuncDec";
		case AstType::FuncCall: return "FuncCall";
		case AstType::Return: return "Return";
		case AstType::End: return "End";
		case AstType::VarDec: return "VarDec";
		case AstType::VarAssign: return "VarAssign";
		case AstType::Int: return "Int";
		case AstType::Bool: return "Bool";
		case AstType::Float: return "Float";
		case AstType::Id: return "ID";
		case AstType::Str: return "Str";
		case AstType::ArrayDec: return "ArrayDec";
		case AstType::ArrayAccess: return "ArrayAccess";
		case AstType::ArrayAssign: return "ArrayAssign";
		
		case AstType::If: return "If";
		case AstType::Elif: return "Elif";
		case AstType::Else: return "Else";
		case AstType::EndIf: return "EndIf";
		case AstType::While: return "While";
		
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

//Returns an operator to string (debugging)
std::string op2str(CondOp op) {
	switch (op) {
		case CondOp::Equals: return "==";
		case CondOp::NotEquals: return "!=";
		case CondOp::Greater: return ">";
		case CondOp::GreaterEq: return ">=";
		case CondOp::Less: return "<";
		case CondOp::LessEq: return "<=";
	}
	
	return "NONE";
}

//Prints an AST tree
void print_tree(AstNode *node, int indent, bool nl) {
	for (int i = 0; i<indent; i++) {
		std::cout << "  ";
	}
	std::cout << ast2str(node->type);
	
	if (node->type == AstType::Scope) {
		AstScope *scope = dynamic_cast<AstScope *>(node);
		
		std::cout << " [" << scope->get_name() << "]";
		
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
	} else if (node->type == AstType::FuncDec || node->type == AstType::ExternFunc) {
		AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
		if (node->type == AstType::ExternFunc) {
			std::cout << " EXT";
		}
		
		std::cout << " ["  << fd->get_name() << "] ";
			
		if (fd->args.size() > 0) {
			std::cout << " <";
			for (auto v : fd->args) {
				std::cout << "{" << v.name << "} ";
			}
			std::cout << ">";
		}
	} else if (node->type == AstType::FuncCall) {
		AstFuncCall *fc = dynamic_cast<AstFuncCall *>(node);
		std::cout << " [" << fc->get_name() << "]";
		
	} else if (node->type == AstType::VarDec || node->type == AstType::VarAssign) {
		AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
		std::cout << " [" << vd->get_name() << "] ("
			<< type2str(vd->get_type()) << ")";
			
	} else if (node->type == AstType::If || node->type == AstType::Elif || node->type == AstType::While) {
		AstCond *cond = dynamic_cast<AstCond *>(node);
		std::cout << " <" << op2str(cond->get_op()) << "> [{";
		print_tree(cond->lval, 0, false);
		std::cout << "} {";
		print_tree(cond->rval, 0, false);
		std::cout << "}]";
		
	} else if (node->type == AstType::ArrayDec) {
		AstArrayDec *arr = dynamic_cast<AstArrayDec *>(node);
		std::cout << " [" << arr->get_name() << "]";
		std::cout << " {" << arr->get_size() << ":";
		std::cout << type2str(arr->get_type()) << "}";
		
	} else if (node->type == AstType::ArrayAccess || node->type == AstType::ArrayAssign) {
		AstAttrNode *acc = dynamic_cast<AstAttrNode *>(node);
		std::cout << " [" << acc->get_name() << "]";
		
		if (node->type == AstType::ArrayAssign) {
			auto assign = dynamic_cast<AstArrayAssign *>(node);
			std::cout << " {";
		
			auto index = assign->index;
			switch (index->type) {
				case AstType::Id: {
					std::cout << dynamic_cast<AstID *>(index)->get_name();
				} break;
				
				case AstType::Int: {
					std::cout << dynamic_cast<AstInt *>(index)->get_val();
				}
			}
			
			std::cout << "}";
		}
		
	//values
	} else if (node->type == AstType::Int) {
		std::cout << " " << dynamic_cast<AstInt *>(node)->get_val();
	} else if (node->type == AstType::Bool) {
		std::cout << " " << static_cast<AstBool *>(node)->get_val();
	} else if (node->type == AstType::Float) {
		std::cout << " " << dynamic_cast<AstFloat *>(node)->get_val();
	} else if (node->type == AstType::Id) {
		std::cout << " " << dynamic_cast<AstID *>(node)->get_name();
	} else if (node->type == AstType::Str) {
		std::cout << " " << dynamic_cast<AstString *>(node)->get_val();
	}
	
	if (nl)
		std::cout << std::endl;

	if (node->children.size() > 0) {
		for (auto c : node->children) {
			print_tree(c, indent+1);
		}
	}
}
