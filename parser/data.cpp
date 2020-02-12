#include "parse.hh"

//Build an array node
AstArrayDec *QkParser::build_array(Line ln, bool is_float) {
	auto tokens = ln.tokens;
	auto type = tokens.at(0).type;

	AstArrayDec *arr = new AstArrayDec;
	arr->set_type(ttype2dtype(type));
	
	//Syntax checking
	auto lb = tokens.at(1).type;
	auto count = tokens.at(2);
	auto rb = tokens.at(3).type;
	auto name = tokens.at(4);
	
	if (lb != TokenType::L_BRACKET || rb != TokenType::R_BRACKET)
		syntax_error(ln, "Invalid array declaration syntax.");
		
	if (count.type != TokenType::NO)
		syntax_error(ln, "No or invalid size specified for array.");
		
	//Set the name and size
	arr->set_name(name.id);
	arr->set_size(std::stoi(count.id));
	
	//Set the elements
	for (int i = 6; i<tokens.size(); i++) {
		auto t = tokens.at(i);
		
		switch (t.type) {
			case TokenType::NO: {
				int no = std::stoi(t.id);
				AstInt *i = new AstInt(no);
				arr->children.push_back(i);
			} break;
			
			case TokenType::DEC: {
				double no = std::stod(t.id);
				AstFloat *i = new AstFloat(no);
				arr->children.push_back(i);
			} break;
			
			case TokenType::B_TRUE: {
				AstBool *bl = new AstBool(true);
				arr->children.push_back(bl);
			} break;	
			
			case TokenType::B_FALSE: {
				AstBool *bl = new AstBool(false);
				arr->children.push_back(bl);
				
			} break;
			
			case TokenType::STRING: {
				AstString *i = new AstString(t.id);
				arr->children.push_back(i);
			} break;
			
			case TokenType::ID: {
				AstID *id = new AstID(t.id);
				arr->children.push_back(id);
			} break;
			
			case TokenType::COMMA: continue;
		}
	}
	
	return arr;
}

//Builds a structure declaration
AstStructDec *QkParser::build_struct_dec(Line ln) {
	AstStructDec *dec = new AstStructDec;
	
	Token name = ln.tokens.at(1);
	if (name.type != TokenType::ID)
		syntax_error(ln, "Invalid structure identifier.");
		
	dec->set_name(name.id);
	
	return dec;
}

//Builds a structure operation
AstStruct *QkParser::build_struct_var(Line ln) {
	AstStruct *op = new AstStruct;
	auto tokens = ln.tokens;
	
	//Perform a size check
	if (tokens.size() != 3)
		syntax_error(ln, "Invalid tokens in structure declaration.");
		
	auto str_name = tokens.at(1);
	auto var_name = tokens.at(2);
	
	//Perform some syntax checking
	if (str_name.type != TokenType::ID)
		syntax_error(ln, "Invalid or non-existent structure being referenced.");
		
	if (var_name.type != TokenType::ID)
		syntax_error(ln, "Invalid structure variable name.");
		
	//Create the AST type
	op->str_name = str_name.id;
	op->var_name = var_name.id;
	
	return op;
}

