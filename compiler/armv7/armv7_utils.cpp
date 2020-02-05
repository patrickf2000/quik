#include "armv7.hh"

std::string Asm_Armv7::build_string(AstNode *node) {
	AstString *str = static_cast<AstString *>(node);

	std::string name = "STR" + std::to_string(str_lbl);
	++str_lbl;
	
	std::string ln = ".d_" + name + ":"; 
	ln += " .ascii \"" + str->get_val() + "\\000\"";
	sec_data.push_back(ln);
	
	ln = "." + name +": .word .d_" + name;
	btm_data.push_back(ln);
	
	return name;
}
