#include "parse.hh"

//Build an array node
AstArrayDec *QkParser::buildArray(TokenType dataType) {
    AstArrayDec *arr = new AstArrayDec;
    arr->set_type(ttype2dtype(dataType));
    
    // Get the array count element
    auto token = getNext();
    
    if (token != TokenType::NO) {
        syntax_error(getLnNo(), getCurrentLn(),
            "No or invalid size specified for array.");
    }
    
    int val = getIVal();
    arr->set_size(val);
    
    // The next token should be the right bracket
    token = getNext();
    
    if (token != TokenType::R_BRACKET) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid array declaration syntax.");
    }
    
    // The next token should be the name
    token = getNext();
    
    if (token != TokenType::ID) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid array declaration syntax: Missing name");
    }
    
    auto name = getSVal();
    arr->set_name(name);
    
    // The next token should be the assignment operator
    token = getNext();
    
    if (token != TokenType::ASSIGN) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid array declaration syntax.");
    }
    
    buildVarParts(arr);
    return arr;
}

//Builds a structure declaration
/*AstStructDec *QkParser::build_struct_dec(Line ln) {
	AstStructDec *dec = new AstStructDec;
	
	Token name = ln.tokens.at(1);
	if (name.type != TokenType::ID)
		syntax_error(ln, "Invalid structure identifier.");
		
	dec->set_name(name.id);
	
	return dec;
}*/

//Builds a structure operation
/*AstStruct *QkParser::build_struct_var(Line ln) {
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
}*/

