#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <algorithm>

#include "build.hh"
#include "strutils.hh"
#include "parse.hh"
#include "syntax.hh"

#include "optimization/optimize.hh"

//Load the source file and run it through lex
std::vector<Line> load_source(const char *path) {
    std::vector<Line> lines;
    std::ifstream reader(path);
    
    if (!reader.is_open()) {
        std::cout << "Fatal: Unable to open input file." << std::endl;
        std::exit(1);
    }
    
    std::string ln = "";
    int ln_no = 1;
    
    while (std::getline(reader, ln)) {
        ln = trim(ln);
        if (ln.length() > 0 && ln[0] != '#') {
            Line l;
            l.no = ln_no;
            l.original = ln;
            lines.push_back(l);
        }
        
        ++ln_no;
    }
    
    reader.close();
    
    //Now check for includes
    std::vector<std::string> included;
    bool found_include = false;
    
    //Loop until there are no includes
    do {
        found_include = false;
        
        auto old_lines = lines;
        lines.clear();
        
        for (auto l : old_lines) {
            if (starts_with(l.original, "include")) {
                found_include = true;
            
                int sp = l.original.find_first_of(" ");
                int len = l.original.length();
                std::string name = l.original.substr(sp+1, len);
                
                if (std::find(included.begin(), included.end(), name) != included.end()) {
                    continue;
                }
                
                included.push_back(name);
                
                std::string path = "/usr/local/include/quik/";
                path += name + ".qk";
                
                std::ifstream reader2(path.c_str());
                if (!reader2.is_open()) {
                    std::cout << "Fatal: Invalid include." << std::endl;
                    std::cout << l.original << std::endl;
                    std::exit(1);
                }
                
                while (std::getline(reader2, ln)) {
                    ln = trim(ln);
                    if (ln.length() > 0 && ln[0] != '#') {
                        Line l2;
                        l2.original = ln;
                        l2.no = -1;
                        lines.push_back(l2);
                    }
                }
                
                reader2.close();
            } else {
                lines.push_back(l);
            }
        }
    } while (found_include);
    
    // Write the lines back to a temporary file
    std::ofstream writer("/tmp/tmp.qki");
    
    for (auto ln : lines) {
        writer << ln.no << "|" << ln.original << std::endl;
    }
    
    writer.close();
    
    //Now tokenize
    Scanner *scanner = new Scanner;
    auto lines2 = scanner->tokenize("/tmp/tmp.qki", true);
    
    delete scanner;
    remove("/tmp/tmp.qki");
    
    return lines2;
}

//Takes each line and builds the AST
AstTree *build_ast(std::vector<Line> lines, bool fail, bool optimize) {
    AstScope *top = new AstScope;
    std::vector<AstNode *> nodes;
    
    QkParser *parser = new QkParser;
    
    for (auto ln : lines) {
        auto n = parser->build_node(ln);
        n->ln = ln;
        
        if (n->children.size() > 0) {
            for (int i = 0; i<n->children.size(); i++) {
                n->children.at(i)->ln = ln;
            }
        }
        
        if (n == nullptr) {
            syntax_error(ln, "Unknown input");
        } else {
            //This inserts an end statement before an elif statementn
            if (n->type == AstType::Elif || n->type == AstType::Else) {
                AstNode *end = new AstNode(AstType::End);
                nodes.push_back(end);
            }
            
            nodes.push_back(n);
        }
    }
    
    parser->build_tree(nodes, top);
    parser->analyze_and_check(top);
    
    delete parser;
    
    //Perform syntax checking
    SyntaxCheck check;
    check.check_global(top);
    check.check_vars(top, top->vars);
    check.check_lp_vars(top, top->vars);
    check.evaluate(fail);        //TODO: Something else rather than bomb out in function?
    
    //Optimize
    if (optimize) {
        OptimizeEngine *engine = new OptimizeEngine(top);
        engine->remove_uncalled();
        
        delete engine;
    }
    
    AstTree *tree = new AstTree("file");
    tree->children.push_back(top);
    return tree;
}
