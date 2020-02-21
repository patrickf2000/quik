#include <vector>
#include <ast.hh>

#include "ltac_build.hh"

std::map<std::string, bool> funcs;

//Builds a function
void LTAC_Builder::build_func(AstNode *node, bool is_extern) {
	auto fd = static_cast<AstFuncDec *>(node);
	
	//Check for overloading
	std::string fn_name = fd->get_name();
	
	if (funcs.find(fn_name) == funcs.end()) {		//Not found
		funcs[fn_name] = false;
	} else {
		funcs[fn_name] = true;
		
		std::string overload = fn_name;
		
		for (auto arg : fd->args) {
			switch (arg.type) {
				case DataType::Int: overload += "_INT"; break;
				case DataType::Str: overload += "_STR"; break;
				
				//TODO: add the rest
			}
		}
		
		fn_name = overload;
		funcs[overload] = false;
	}
	
	//Start building
	auto l_fd = new LtacFunc(fn_name);
	file->code->children.push_back(l_fd);
	l_fd->is_global = fd->is_global;
	
	//If its extern, build that
	if (is_extern) {
		l_fd->is_extern = true;
		return;
	}
	
	//Add the arguments
	for (auto v : fd->args) {
		inc_stack(v.type);
		v.stack_pos = stack_pos;
		vars[v.name] = v;
		
		auto var = new LtacVar;
		var->pos = stack_pos;
		var->d_type = v.type;
		
		l_fd->children.push_back(var);
	}
}

//Builds a function call
void LTAC_Builder::build_func_call(AstNode *node) {
	auto fc = static_cast<AstFuncCall *>(node);
	auto fn_name = fc->get_name();
	
	//First, we need to determine if we have an overloaded function
	bool overload = false;
	
	if (funcs[fn_name])
		overload = true;
	
	//Start building
	auto l_fc = new LtacFuncCall(fn_name);
	file->code->children.push_back(l_fc);
	
	//Build the arguments
	for (auto arg : fc->children) {
		switch (arg->type) {
			//Push an integer
			case AstType::Int: {
				auto i = static_cast<AstInt *>(arg);
				auto li = new LtacInt(i->get_val());
				l_fc->children.push_back(li);
				
				if (overload) fn_name += "_INT";
			} break;
		
			//Push a string
			case AstType::Str: {
				auto lstr = build_string(arg);
				l_fc->children.push_back(lstr);
				
				if (overload) fn_name += "_STR";
			} break;
			
			//Push another variable
			case AstType::Id: {
				auto id = static_cast<AstID *>(arg);
				auto v = vars[id->get_name()];
				
				auto var = new LtacVar;
				var->pos = v.stack_pos;
				var->d_type = v.type;
				l_fc->children.push_back(var);
				
				if (overload) {
					switch (v.type) {
						case DataType::Int: fn_name += "_INT"; break;
						case DataType::Str: fn_name += "_STR"; break;
						
						//TODO: Add rest
					}
				}
			} break;
			
			//TODO: Add the rest
		}
	}
	
	//Assign the function name
	if (funcs.find(fn_name) != funcs.end()) {
		l_fc->name = fn_name;
	}
}

//Scans the file to handle function overload
void LTAC_Builder::check_overload() {
	//First, scan function declarations
	
	//Next, rename based on parameters
	
	//Finally, adjust the function calls
}

