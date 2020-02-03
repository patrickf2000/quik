//types
//Contains common types that will be used across the various
// backends (as I implement more)
#pragma once

#include <string>
#include <vector>
#include <map>

#include <ast.hh>

struct Config {
	std::string out_name;
	bool build_lib;
	bool obj_only;
	std::vector<std::string> libs;
	std::string arch;
};

struct StructV {
	std::string name;
	int stack_pos;
	int size;
	std::map<std::string, Var> vars;
};
