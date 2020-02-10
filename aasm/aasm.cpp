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
		
		//Setup
		case A_Asm::Setup: ln = "setup"; break;
		
		//Leave
		case A_Asm::Leave: ln = "leave"; break;
		
		//Return
		case A_Asm::Ret: {
			ln = "ret";
		} break;
	}
	
	return ln;
}
