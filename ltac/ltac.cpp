#include "ltac.hh"

//Translates an AASM node to a string
std::string ltac_translate(AsmNode *node) {
	std::string ln = "";
	
	switch (node->type) {
		//Extern labels
		case ltac::Extern: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "extern " + s->val;
		} break;
	
		//Labels
		case ltac::Label: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "lbl " + s->val;
		} break;
		
		//Setup
		case ltac::Setup: ln = "setup\n"; break;
		
		//Leave
		case ltac::Leave: ln = "leave"; break;
		
		//Return
		case ltac::Ret: {
			ln = "ret";
		} break;
		
		//Call
		case ltac::Call: {
			AsmString *s = static_cast<AsmString *>(node->children[0]);
			ln = "call " + s->val;
			ln += "\n";
		} break;
		
		//Move
		case ltac::Mov: {
			ln = "mov ";
		
			AsmNode *arg1 = node->children[0];
			AsmNode *arg2 = node->children[1];
			
			switch (arg1->type) {
				case ltac::Reg: ln += "rx,"; break;
				
				//TODO: Add the rest
			}
			
			switch (arg2->type) {
				case ltac::Ptr: ln += " *" + arg2->val; break;
			}
		} break;
	}
	
	return ln;
}
