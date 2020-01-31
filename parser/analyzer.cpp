#include <map>

#include "analyzer.hh"
#include "types.hh"
#include "parse.hh"

//Scans the tree for variable declarations
void find_variables(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	AstScope *scope = dynamic_cast<AstScope *>(top);

	for (auto node : top->children) {
		switch (node->type) {
			case AstType::FuncDec: {
				AstFuncDec *fd = dynamic_cast<AstFuncDec *>(node);
				AstScope *next = dynamic_cast<AstScope *>(fd->children.at(0));
				next->vars = scope->vars;
				
				for (auto v : fd->args) {
					next->vars[v.name] = v;
				}
				
				find_variables(next);
			} break;
			
			case AstType::If:
			case AstType::Elif:
			case AstType::Else:
			case AstType::While: {
				AstScope *s = new AstScope;
				s->children = node->children;
				
				find_variables(s);
				scope->vars.insert(s->vars.begin(), s->vars.end());
			} break;
			
			case AstType::ForEach: {
				AstScope *s = new AstScope;
				s->children = node->children;
				
				find_variables(s);
				scope->vars.insert(s->vars.begin(), s->vars.end());
				
				AstForEach *fe = static_cast<AstForEach *>(node);
				Var v;
				v.name = fe->i_var;
				v.is_param = false;
				v.is_array = false;
				scope->vars[fe->i_var] = v;
			} break;
			
			case AstType::ArrayDec:
			case AstType::VarDec: {
				AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
			
				Var v;
				v.name = vd->get_name();
				v.type = vd->get_type();
				v.is_param = false;
				scope->vars[vd->get_name()] = v;
			} break;
		}
	}
}

//Locates variable assignments and assigns the right type to them
// We also make sure there are no undeclared variables here
void find_assign(AstNode *top, AstScope *scope) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::FuncDec: {
				auto child = node->children.at(0);
				auto n_scope = dynamic_cast<AstScope *>(child);
				find_assign(child, n_scope); 
			} break;
			
			case AstType::ArrayAssign:
			case AstType::ArrayAccess:
			case AstType::VarAssign: {
				AstVarDec *va = dynamic_cast<AstVarDec *>(node);
			
				Var v = scope->vars[va->get_name()];
				va->set_type(v.type);
			} break;
			default: {
				if (node->children.size() > 0) {
					find_assign(node, scope);
				}
			}
		}
	}
}

//Finds conditional nodes, and adds end-if blocks
void find_cond(AstNode *top) {
	AstType last_type;

	for (int i = 0; i<top->children.size(); i++) {
		auto node = top->children.at(i);
		
		if (node->type == AstType::FuncDec) {
			AstScope *next = dynamic_cast<AstScope *>(node->children.at(0));
			find_cond(next);
		} else if (node->type == AstType::While) {
			find_cond(node);
		} else if (node->type == AstType::If || node->type == AstType::Elif || node->type == AstType::Else) {
			if (node->type == AstType::Elif && last_type != AstType::If) {
				syntax_error(node->ln, "Elif without previous If statement.");
			}
		
			if (node->type == AstType::If || node->type == AstType::Elif) {
				find_cond(node);
			}
		
			if (i+1 >= top->children.size()) {
				AstNode *end = new AstNode(AstType::EndIf);
				top->children.insert(top->children.begin()+i+1, end);
				
				continue;
			}
			
			auto next = top->children.at(i+1);
			if (next->type != AstType::Elif && next->type != AstType::Else) {
				AstNode *end = new AstNode(AstType::EndIf);
				top->children.insert(top->children.begin()+i+1, end);
			}
		}
		
		last_type = node->type;
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
