#pragma once

#include <string>
#include <vector>
#include <stack>
#include <iostream>

#include <ast.hh>

#include "../types.hh"

class Asm_x86 {
public:
	explicit Asm_x86();
	explicit Asm_x86(Config c);
	void assemble(std::string p, AstNode *top);
	
	//Function stuff
	void build_function(AstNode *node);
	void build_func_i386(AstFuncDec *fd);
	void build_func_x64(AstFuncDec *fd);
	void build_extern_func(AstNode *node);
	void build_func_call(AstFuncCall *fc);
	void build_func_call_i386(AstFuncCall *fc);
	void build_func_call_x64(AstFuncCall *fc);
	void build_ret(AstNode *node);
	
	//Var stuff
	void build_var_dec(AstNode *node);
	void assign_ax(std::string dest, Var v);
	void build_var_assign(AstNode *node);
	void build_int_math(AstNode *node, bool store=true);
	void build_flt_assign(AstNode *node);
	void build_floatex_assign(AstNode *node);
	
	//Array stuff
	void build_arr_dec(AstNode *node);
	void build_arr_access(AstNode *node);
	std::string build_arr_assign(AstNode *node, Var v);
	
	//Struct stuff
	void build_struct_dec(AstNode *node);
	void build_struct_var(AstNode *node);
	void build_struct_acc(AstNode *node);
	std::string build_struct_mod(AstNode *node);
	Var get_struct_child(AstNode *node);
	
	//Flow stuff
	void build_cond(AstNode *node);
	void build_while(AstNode *node);
	void build_foreach_top(AstNode *node);
	
	//Build stuff
	void write();
	void build();
	void build_link();
protected:
	void init_registers();
	std::string get_reg(std::string r);
	std::string type2asm(AstNode *node);
	std::string asm_type(Var v);
	std::string build_string(AstNode *node);
	std::string build_float(AstNode *node);
	bool contains(AstNode *node, DataType t);
	bool contains_arr(AstNode *node);
private:
	std::string path = "";
	std::vector<std::string> asm_files;
	std::vector<std::string> sec_data;
	std::vector<std::string> sec_bss;
	std::vector<std::string> sec_text;
	std::vector<std::string> extern_data;
	
	//The config structure
	Config config;
	bool x64 = true;
	
	//Variable stuff
	std::map<std::string, Var> vars;
	int stack_pos = 0;
	
	//Control variables
	bool in_main = false;
	int str_index = 1;
	int flt_index = 1;
	
	//Needed for conditionals and loops
	int lbl_index = 1;
	std::stack<std::string> labels;
	std::stack<std::string> top_lbls;
	
	//Scope data
	AstScope *current_scope;
	
	//Struct data
	std::map<std::string, AstStructDec *> av_structs;		//Available structures
	std::map<std::string, StructV> structs;					//Struct variables
};
