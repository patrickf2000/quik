#include "ltac_build.hh"
#include "ltac.hh"

#include <types.hh>
#include <ast.hh>
#include <parse.hh>

//Returns the datatype of an ltac node
DataType LTAC_Builder::determine_type(LtacNode *node) {
	auto type = DataType::None;
	
	switch (node->type) {
		case ltac::Int: type = DataType::Int; break;
		case ltac::Float: type = DataType::Float; break;
		case ltac::String: type = DataType::Str; break;
		
		case ltac::Var: {
			auto var = static_cast<LtacVar *>(node);
			type = var->d_type;
		} break;
	}
	
	return type;
}

void LTAC_Builder::build_cmp(AstNode *node, bool is_loop) {
	auto cmp = static_cast<AstCond *>(node);
	auto lcmp = new LtacCmp;
	
	auto lval = convert_ast_var(cmp->lval);
	auto rval = convert_ast_var(cmp->rval);
	
	//Check to make sure right/left value types match
	//The left value determines this
	auto l_type = determine_type(lval);
	auto r_type = determine_type(rval);
	
	if (l_type != r_type) {
		Line ln;
		syntax_error(ln, "You can only compare values of the same type.");
	}
	
	//Set the comparison type
	if (l_type == DataType::Int)
		lcmp = new LtacICmp;
		
	//Set everything
	lcmp->lval = lval;
	lcmp->rval = rval;
	file->code->children.push_back(lcmp);
	
	//Add the comparison
	if (is_loop)
		build_loop_cmp(cmp);
	else
		build_cond_cmp(cmp);
}

//Builds an if/else-style conditional
void LTAC_Builder::build_cond_cmp(AstCond *cmp) {
	auto jmp = new LtacJmp;
	auto default_jmp = new LtacJmp;
	
	switch (cmp->get_op()) {
		case CondOp::Equals: jmp->op = Operator::NotEqual; break;
		case CondOp::NotEquals: jmp->op = Operator::Equal; break;
		case CondOp::Greater: jmp->op = Operator::LessEq; break;
		case CondOp::GreaterEq: jmp->op = Operator::LessEq; break;
		case CondOp::Less: jmp->op = Operator::GreaterEq; break;
		case CondOp::LessEq: jmp->op = Operator::GreaterEq; break;
	}
	
	jmp->dest = labels.top();
	default_jmp->dest = end_lbls.top();
	
	file->code->children.push_back(jmp);
	assemble(cmp);
	file->code->children.push_back(default_jmp);
}

//Builds a loop-style conditional
void LTAC_Builder::build_loop_cmp(AstCond *cmp) {

}

