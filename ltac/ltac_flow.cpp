#include "ltac_build.hh"

//Build conditional
void LTAC_Builder::build_conditional(AstNode *node) {
	AstCond *cond = static_cast<AstCond *>(node);
	
	std::string label = "L" + std::to_string(lbl_count);
	++lbl_count;
	labels.push(label);
	
	if (node->type == AstType::Else)
		return;
		
	LtacNode *cmp = new LtacNode(LTAC::Cmp);
	file->children.push_back(cmp);
}
