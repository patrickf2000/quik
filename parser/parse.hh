#pragma once

#include <vector>
#include <lex.hh>

#include "ast.hh"

void syntax_error(Line ln, std::string msg, bool exit = true);
void syntax_error(int no, std::string ln, std::string msg, bool exit = true);
DataType ttype2dtype(TokenType t);

class QkParser {
public:
    AstNode *build_node(Line ln);
    AstNode *build_id(std::string name);
    int build_tree(std::vector<AstNode *> nodes, AstNode *top, int index = 0, bool in_if = false);
    
    //Function stuff
    AstFuncDec *buildFuncDec(bool isGlobal = false, bool isExtern = false);
    AstFuncCall *buildFuncCall(std::string name);
    AstReturn *buildRet();
    
    //Variables
    AstVarDec *buildVarDec(TokenType dataType);
    void buildVarParts(AstNode *vd);
    
    //Conditionals
    AstCond *build_conditional(TokenType loopType);
    
    //Loops
    AstLoop *build_loop();
    AstForEach *build_foreach();
    
    //Arrays and structures
    AstArrayDec *buildArray(TokenType dataType);
    /*AstStructDec *build_struct_dec(Line ln);
    AstStruct *build_struct_var(Line ln);*/
    
    void analyze_and_check(AstScope *top);
protected:
    //Analyzer functions
    void find_variables(AstNode *top);
    void find_assign(AstNode *top, AstScope *scope);
    void find_cond(AstNode *top);
    void check_return(AstNode *top);
private:
    // Most of this is temporary as we re-work the parser
    Line currentLn;
    int currentIndex = 1;
    
    TokenType getNext();
    std::string getSVal();
    int getIVal();
    double getFloatL();
    int getLnNo();
    std::string getCurrentLn();
};
