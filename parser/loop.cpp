#include "parse.hh"

//Builds a loop statement
AstLoop *QkParser::build_loop() {
	AstLoop *lp = new AstLoop;
	lp->param = nullptr;
    
    auto token = getNext();
	
	if (token != TokenType::NL) {
		switch (token) {
			case TokenType::NO: {
				int i = getIVal();
				lp->param = new AstInt(i);
			} break;
			
			case TokenType::ID: {
				AstID *id = new AstID(getSVal());
				lp->param = id;
			} break;
			
			default: {
				std::string er_msg = "Error: Only integers and integer variables ";
				er_msg += "may be used with the loop statement.";
				syntax_error(getLnNo(), getCurrentLn(), er_msg);
			}
		}
	}
    
    token = getNext();
    if (token != TokenType::NL) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Too many arguments for loop statement.");
    }
	
	return lp;
}

//Builds a foreach statement
AstForEach *QkParser::build_foreach(Line ln) {
	auto tokens = ln.tokens;
	if (tokens.size() != 4)
		syntax_error(ln, "Insufficient or too many arguments.");
		
	if (tokens.at(2).type != TokenType::COLON)
		syntax_error(ln, "Invalid foreach syntax.");

	AstForEach *fe = new AstForEach;
	auto i_var = tokens.at(1);
	auto r_var = tokens.at(3);
	
	if (i_var.type != TokenType::ID)
		syntax_error(ln, "Iterator must be a variable.");
		
	if (r_var.type != TokenType::ID)
		syntax_error(ln, "Range must be an array or vector variable.");
		
	fe->i_var = i_var.id;
	fe->r_var = r_var.id;
	
	return fe;
}

