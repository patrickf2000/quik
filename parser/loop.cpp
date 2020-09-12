#include "parse.hh"

//Builds a loop statement
AstLoop *QkParser::build_loop() {
    AstLoop *lp = new AstLoop;
    
    auto token = getNext();
    
    if (token != TokenType::NL) {
        switch (token) {
            case TokenType::NO: {
                int i = getIVal();
                lp->setParam(new AstInt(i));
            } break;
            
            case TokenType::ID: {
                auto *id = new AstId(getSVal());
                lp->setParam(id);
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
AstForEach *QkParser::build_foreach() {   
    auto token = getNext();
    
    if (token != TokenType::LEFT_PAREN) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid foreach syntax: Expected \'(\'");
    }
    
    AstForEach *fe = new AstForEach;
    
    // The iterator
    token = getNext();
    
    if (token != TokenType::ID) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Iterator must be a variable.");
    }
    
    fe->i_var = getSVal();
    
    // The middle token
    token = getNext();
    
    if (token != TokenType::COLON) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid foreach syntax: Expected \':\'");
    }
    
    // The range variable
    token = getNext();
    
    if (token != TokenType::ID) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Range must be an array or vector variable.");
    }
    
    fe->r_var = getSVal();
    
    // Final syntax check
    token = getNext();
    auto end = getNext();
    
    if (token != TokenType::RIGHT_PAREN || token != TokenType::NL) {
        syntax_error(getLnNo(), getCurrentLn(),
            "Invalid foreach syntax. Expected \')\' and \'\\n\' at end.");
    }
    
    return fe;
}

