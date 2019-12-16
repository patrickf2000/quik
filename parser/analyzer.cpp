#include "analyzer.hh"

//Scans the tree for variable declarations
void find_variables(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	AstScope *scope = dynamic_cast<AstScope *>(top);

	for (auto node : top->children) {
		if (node->type == AstType::FuncDec) {
			AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
			AstScope *next = dynamic_cast<AstScope *>(fd->children.at(0));
			next->vars = scope->vars;
			
			for (auto v : fd->args) {
				next->vars[v.name] = v;
			}
			
			find_variables(next);
		} else if (node->type == AstType::VarDec) {
			AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
		
			Var v;
			v.name = vd->get_name();
			v.type = vd->get_type();
			scope->vars[vd->get_name()] = v;
		}
	}
}

//Locates variable assignments and assigns the right type to them
// We also make sure there are no undeclared variables here
void find_assign(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	AstScope *scope = dynamic_cast<AstScope *>(top);

	for (auto node : top->children) {
		if (node->type == AstType::FuncDec) {
			AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
			AstScope *next = dynamic_cast<AstScope *>(fd->children.at(0));
			
			find_assign(next);
		} else if (node->type == AstType::VarAssign) {
			AstVarAssign *va = dynamic_cast<AstVarAssign *>(node);
		
			Var v = scope->vars[va->get_name()];
			va->set_type(v.type);
		}
	}
}

//Scans the tree and adds empty return statements to
// functions that don't have any (needed for the backend)
void check_return(AstNode *top) {
	for (auto node : top->children) {
		if (node->type == AstType::FuncDec && node->children.size() > 0) {
			auto scope = node->children.at(0);
			auto children = scope->children;
			auto last = children.at(children.size()-1);
			
			if (last->type != AstType::Return) {
				AstReturn *ret = new AstReturn;
				scope->children.push_back(ret);
			}
		}
	}
}
