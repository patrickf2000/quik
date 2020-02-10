#include "aasm.hh"

//Translates an AASM node to a string
std::string aasm_translate(AsmNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		//Extern labels
		case A_Asm::Extern: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "extern " + s->val;
		} break;
	
		//Labels
		case A_Asm::Label: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "lbl " + s->val;
		} break;
	}
	
	return ln;
}
