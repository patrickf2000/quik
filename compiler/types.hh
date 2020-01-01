//types
//Contains common types that will be used across the various
// backends (as I implement more)
#pragma once

#include <string>
#include <vector>

struct Config {
	std::string out_name;
	bool build_lib;
	std::vector<std::string> libs;
};
