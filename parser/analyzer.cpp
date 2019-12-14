#include "analyzer.hh"

//Scans the tree for variable declarations
void find_variables(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	AstScope *scope = dynamic_cast<AstScope *>(top);

	for (auto node : top->children) {
		if (node->type == AstType::FuncDec) {
			AstScope *next = dynamic_cast<AstScope *>(node->children.at(0));
			next->vars = scope->vars;
			find_variables(next);
		} else if (node->type == AstType::VarDec) {
			AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
		
			Var v;
			v.name = vd->get_name();
			v.type = vd->get_type();
			scope->vars.push_back(v);
		}
	}
}
