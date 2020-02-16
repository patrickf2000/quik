#include <iostream>

#include "ltac_build.hh"
#include "ltac.hh"

LTAC_Builder::LTAC_Builder(std::string output) {
	file = new LtacFile(output);
}

//The main assembly function
void LTAC_Builder::assemble(AstNode *top) {
	for (auto node : top->children) {
		switch (node->type) {
			case AstType::Scope: assemble(node); break;
			
			//Function stuff
			case AstType::ExternFunc: build_extern(node); break;
			case AstType::FuncDec: {
				build_func_dec(node);
				assemble(node);
			} break;
			
			case AstType::FuncCall: build_func_call(node); break;
			
			case AstType::Return: build_ret(node); break;
			
			//Variable stuff
			case AstType::VarDec: build_var_dec(node); break;
			case AstType::VarAssign: build_var_assign(node); break;
			
			//Conditionals
			case AstType::If: {
				std::string label = "L" + std::to_string(lbl_count);
				++lbl_count;
				top_labels.push(label);
			}
			
			case AstType::Elif:
			case AstType::Else: {
				build_conditional(node);
				assemble(node);
				
				LtacNode *end_jmp = new LtacNode(LTAC::Jmp);
				end_jmp->val = top_labels.top();
				file->children.push_back(end_jmp);
				
				LtacNode *lbl = new LtacNode(LTAC::Label);
				lbl->val = labels.top();
				file->children.push_back(lbl);
				
				labels.pop();
			} break;
			
			case AstType::EndIf: {
				std::string label = top_labels.top();
				top_labels.pop();
				
				LtacNode *lbl = new LtacNode(LTAC::Label);
				lbl->val = label;
				file->children.push_back(lbl);
			} break;
		}
	}
}

//Increments the stack position based on variable datatype
int LTAC_Builder::size_of(DataType t) {
	switch (t) {
		case DataType::Int: return 4;
		
		//TODO: Add the rest
	}
	
	return 1;
}



