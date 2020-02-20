#include "utils.hh"

//Returns the parent path of a file
std::string get_path(std::string in) {
	int last_slash = in.find_last_of("/");
	
	if (last_slash == -1)
		return "./";
		
	std::string path = "";
	
	for (int i = 0; i<=last_slash; i++) {
		path += in[i];
	}
	
	return path;
}

//Returns the basename of the path
std::string get_basename(std::string in) {
	int last_slash = in.find_last_of("/");
	int last_dot = in.find_last_of(".");
		
	if (last_dot == -1)
		last_dot = in.length();

	std::string base = "";
	
	for (int i = last_slash + 1; i<last_dot; i++) {
		base += in[i];
	}
	
	return base;
}
