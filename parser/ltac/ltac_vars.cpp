#include "ltac_build.hh"

//Build a variable declaration
void LTAC_Builder::build_var_dec(AstNode *node) {
	auto vd = static_cast<AstVarDec *>(node);

	Var v;
	v.name = vd->get_name();
	v.type = vd->get_type();
	v.is_array = false;
	v.is_param = false;
	
	inc_stack(v.type);
	
	v.stack_pos = stack_pos;
	vars[v.name] = v;
}

//Build a variable assignment
void LTAC_Builder::build_var_assign(AstNode *node) {
	auto va = static_cast<AstVarAssign *>(node);
	auto v = vars[va->get_name()];
	
	auto var = new LtacVar;
	var->pos = v.stack_pos;
	var->d_type = v.type;
	
	//Build the assigned value
	auto val = va->children[0];
	auto lval = convert_ast_var(val);
	var->children.push_back(lval);
	
	file->code->children.push_back(var);
}

//Converts an AST node to an LTAC node
LtacNode *LTAC_Builder::convert_ast_var(AstNode *val) {
	LtacNode *lval = new LtacNode;

	switch (val->type) {
		//Bytes
		case AstType::Hex: {
			auto ch = static_cast<AstHex *>(val);
			int c = (int)ch->get_val();
			
			lval = new LtacByte(c);
		} break;
	
		//Chars
		case AstType::Char: {
			auto ch = static_cast<AstChar *>(val);
			int c = (int)ch->get_val();
			
			lval = new LtacByte(c);
		} break;
	
		//Booleans
		case AstType::Bool: {
			auto bl = static_cast<AstBool *>(val);
			lval = new LtacInt((int)bl->get_val());
		} break;
	
		//Integers
		case AstType::Int: {
			auto i = static_cast<AstInt *>(val);
			lval = new LtacInt(i->get_val());
		} break;
		
		//Floats
		case AstType::Float: {
			lval = build_float(val);
		} break;
		
		//Strings
		case AstType::Str: {
			lval = build_string(val);
		} break;
		
		//Other variables
		case AstType::Id: {
			auto id = static_cast<AstID *>(val);
			auto v2 = vars[id->get_name()];
			
			auto l_id = new LtacVar;
			l_id->pos = v2.stack_pos;
			lval = l_id;
		} break;
		
		//Math
		case AstType::Math: {
			auto math = new LtacMath;
			math->init_val = convert_ast_var(val->children[0]);
			
			for (int i = 1; i<val->children.size(); i+=2) {
				auto math_op = new LtacMathOp;
			
				auto op = val->children[i];
				auto current = val->children[i+1];
				
				//Build the operator
				switch (op->type) {
					case AstType::Add: math_op->op = Operator::Add; break;
					case AstType::Sub: math_op->op = Operator::Sub; break;
					case AstType::Mul: math_op->op = Operator::Mul; break;
					case AstType::Div: math_op->op = Operator::Div; break;
					case AstType::Mod: math_op->op = Operator::Mod; break;
				}
				
				//Build the operand
				if (current->type == AstType::Math) {
					math_op->operand = new LtacNode;
					math->children.push_back(math_op);
					
					math->children.push_back(new LtacNode(ltac::Push));
					
					auto sub_math = convert_ast_var(current);
					math->children.push_back(sub_math);
					
					math->children.push_back(new LtacNode(ltac::Pop));
				} else {
					math_op->operand = convert_ast_var(current);
					math->children.push_back(math_op);
				}
			}
			
			lval = math;
		} break;
		
		//TODO: Add the rest
	}
	
	return lval;
}

