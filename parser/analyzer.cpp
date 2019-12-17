#include <map>
#include <iostream>
#include "analyzer.hh"
#include "types.hh"

//Analyzes the tree and renames all ids to observe scope
void reset_name(AstNode *node, std::string name, std::map<std::string, Var> *vars) {
	AstAttrNode *id = dynamic_cast<AstAttrNode *>(node);
	if (vars->find(id->get_name()) == vars->end()) {
		std::string n = name + "_" + id->get_name();
		id->set_name(n);
	}
}

void find_id2(AstNode *top, std::string name, std::map<std::string, Var> *vars) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Id:
			case AstType::VarDec:
			case AstType::VarAssign: reset_name(node, name, vars); break;
			
			case AstType::If:
			case AstType::Elif: 
			case AstType::While: {
				AstCond *cond = dynamic_cast<AstCond *>(node);
				
				if (cond->lval->type == AstType::Id)
					reset_name(cond->lval, name, vars);
				
				if (cond->rval->type == AstType::Id)
					reset_name(cond->rval, name, vars);
			} break;
		}
		
		if (node->children.size() > 0) {
			find_id2(node, name, vars);
		}
	}
}

void find_id(AstNode *top) {
	if (top->type != AstType::Scope) {
		return;
	}
	
	AstScope *scope = dynamic_cast<AstScope *>(top);

	for (auto node : top->children) {
		switch (node->type) {
			case AstType::VarDec: {
				AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
			
				Var v;
				v.name = vd->get_name();
				v.type = vd->get_type();
				scope->vars[vd->get_name()] = v;
			} break;
			case AstType::FuncDec: {
				AstScope *child = dynamic_cast<AstScope *>(node->children.at(0));
				find_id2(child, child->get_name(), &scope->vars);
			} break;
		}
	}
	
	scope->vars.clear();
}

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
			
			case AstType::VarDec: {
				AstVarDec *vd = dynamic_cast<AstVarDec *>(node);
			
				Var v;
				v.name = vd->get_name();
				v.type = vd->get_type();
				scope->vars[vd->get_name()] = v;
			} break;
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
		switch (node->type) {
			case AstType::FuncDec: find_assign(node->children.at(0)); break;
			case AstType::VarAssign: {
				AstVarAssign *va = dynamic_cast<AstVarAssign *>(node);
			
				Var v = scope->vars[va->get_name()];
				va->set_type(v.type);
			} break;
		}
	}
}

//Finds conditional nodes, and adds end-if blocks
void find_cond(AstNode *top) {
	for (int i = 0; i<top->children.size(); i++) {
		auto node = top->children.at(i);
		
		if (node->type == AstType::FuncDec) {
			AstScope *next = dynamic_cast<AstScope *>(node->children.at(0));
			find_cond(next);
		} else if (node->type == AstType::While) {
			find_cond(node);
		} else if (node->type == AstType::If || node->type == AstType::Elif || node->type == AstType::Else) {
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
