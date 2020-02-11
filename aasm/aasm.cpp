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
		case A_Asm::Setup: ln = "setup\n"; break;
		
		//Leave
		case A_Asm::Leave: ln = "leave"; break;
		
		//Return
		case A_Asm::Ret: {
			ln = "ret";
		} break;
		
		//Call
		case A_Asm::Call: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "call " + s->val;
			ln += "\n";
		} break;
		
		//Move
		case A_Asm::Mov: {
			ln = "mov ";
		
			AsmNode *arg1 = node->children[0];
			AsmNode *arg2 = node->children[1];
			
			switch (arg1->type) {
				case A_Asm::Reg: ln += "rx,"; break;
				
				//TODO: Add the rest
			}
			
			switch (arg2->type) {
				case A_Asm::Ptr: ln += " *" + arg2->val; break;
			}
		} break;
	}
	
	return ln;
}
