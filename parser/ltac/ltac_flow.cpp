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

//Builds while-loop nodes
void LTAC_Builder::build_while(AstNode *node) {
    //Generate the label names
    std::string top_lbl = "L" + std::to_string(lbl_count);
    ++lbl_count;
    labels.push(top_lbl);

    std::string cmp_lbl = "L" + std::to_string(lbl_count);
    ++lbl_count;

    //Jump to the comparison label
    auto jmp = new LtacJmp;
    jmp->dest = cmp_lbl;
    file->code->children.push_back(jmp);

    auto lbl = new LtacLabel(top_lbl);
    file->code->children.push_back(lbl);

    //Assemble the body
    assemble(node);

    //Insert the comparison label
    lbl = new LtacLabel(cmp_lbl);
    file->code->children.push_back(lbl);

    //Build the comparison
    build_cmp(node, true);
}

//Build loop nodes
void LTAC_Builder::build_loop(AstNode *node) {
    auto loop = static_cast<AstLoop *>(node);

    //Generate the label names
    std::string top_lbl = "L" + std::to_string(lbl_count);
    ++lbl_count;

    std::string cmp_lbl = "L" + std::to_string(lbl_count);
    ++lbl_count;
    
    //Build the internal index
    auto ivar = new LtacVar;
    int pos = 0;
    
    if (enable_rvar) {
        pos = rvar_index;
        ++rvar_index;
        ivar->rvar = pos;
    } else {
        stack_pos += 4;
        pos = stack_pos;
        ivar->pos = pos;
    }
    
    ivar->d_type = DataType::Int;
    ivar->children.push_back(new LtacInt(0));
    
    file->code->children.push_back(ivar);
    
    //Generate the top label
    auto lbl = new LtacLabel(top_lbl);
    file->code->children.push_back(lbl);
    
    //Assemble the body
    assemble(node);
    
    //Increment the internal index
    ivar->children.clear();
    
    auto math = new LtacMath;
    math->init_val = ivar;
    
    auto mathop = new LtacMathOp;
    mathop->op = Operator::Add;
    mathop->operand = new LtacInt(1);
    math->children.push_back(mathop);
    
    auto ivar2 = new LtacVar;
    ivar2->d_type = DataType::Int;
    
    if (enable_rvar) {
        ivar2->rvar = pos;
    } else {
        ivar2->pos = pos;
    }
    
    ivar2->children.push_back(math);
    file->code->children.push_back(ivar2);

    //Insert the comparison label
    lbl = new LtacLabel(cmp_lbl);
    file->code->children.push_back(lbl);

    //Build the comparison
    auto cmp = new LtacICmp;
    cmp->lval = ivar;
    cmp->rval = convert_ast_var(loop->param);
    file->code->children.push_back(cmp);
    
    auto jmp = new LtacJmp;
    jmp->op = Operator::Less;
    jmp->dest = top_lbl;
    file->code->children.push_back(jmp);
}

//Builds conditional statements
void LTAC_Builder::build_cmp(AstNode *node, bool is_loop) {
    auto cmp = static_cast<AstCond *>(node);
    auto lcmp = new LtacCmp;
    
    auto lval = convert_ast_var(cmp->lval);
    auto rval = convert_ast_var(cmp->rval);
    
    //Check to make sure right/left value types match
    //The left value determines this
    auto l_type = determine_type(lval);
    auto r_type = determine_type(rval);
    
    //TODO: Remove me
    //This is for testing
    if (cmp->lval->type == AstType::FuncCall) {
        l_type = DataType::Int;
    }
    
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
        case CondOp::GreaterEq: jmp->op = Operator::Less; break;
        case CondOp::Less: jmp->op = Operator::GreaterEq; break;
        case CondOp::LessEq: jmp->op = Operator::Greater; break;
    }
    
    jmp->dest = labels.top();
    default_jmp->dest = end_lbls.top();
    
    labels.pop();
    
    file->code->children.push_back(jmp);
    assemble(cmp);
    file->code->children.push_back(default_jmp);
}

//Builds a loop-style conditional
void LTAC_Builder::build_loop_cmp(AstCond *cmp) {
    auto jmp = new LtacJmp;
    jmp->dest = labels.top();
    
    labels.pop();
    
    switch (cmp->get_op()) {
        case CondOp::Equals: jmp->op = Operator::Equal; break;
        case CondOp::NotEquals: jmp->op = Operator::NotEqual; break;
        case CondOp::Greater: jmp->op = Operator::Greater; break;
        case CondOp::GreaterEq: jmp->op = Operator::GreaterEq; break;
        case CondOp::Less: jmp->op = Operator::Less; break;
        case CondOp::LessEq: jmp->op = Operator::LessEq; break;
    }
    
    file->code->children.push_back(jmp);
}

