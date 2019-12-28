#include "strutils.hh"

//Trims to the left of a string
std::string trim_left(std::string str) {
	std::string ret = "";
	bool found = false;
	
	for (char c : str) {
		if ((c == ' ' || c == '\t') && !found) {
			continue;
		} else {
			found = true;
			ret += c;
		}
	}
	
	return ret;
}

//Trims to the right of a string
std::string trim_right(std::string str) {
	std::string ret = "";
	int end = 0;
	
	for (int i = str.length()-1; i>=0; i--) {
		if (str[i] != ' ' && str[i] != '\t') {
			end = i;
			break;
		}
	}
	
	for (int i = 0; i<=end; i++) {
		ret += str[i];
	}
	
	return ret;
}

//A wrapper function for trimming
std::string trim(std::string str) {
	str = trim_left(str);
	str = trim_right(str);
	
	if (str[0] == 0) str = "";
	return str;
}

//Checks to see if a string contains a character
bool contains(std::string str, char c) {
	for (char cc : str) {
		if (cc == c) {
			return true;
		}
	}
	
	return false;
}

//Check to see if a string contains a substring
bool contains(std::string str, std::string in) {
	if (str.find(in) != std::string::npos) {
		return true;
	}
	return false;
}

//Checks to see if we have a quote
bool is_quote(std::string str) {
	auto bk = str.length()-1;
	
	if (str[0] == '\"' || str[0] == '\'') {
		if (str[bk] == '\"' || str[bk] == '\'') {
			return true;
		}
	}
	
	return false;
}

//Remove the quotes from a string
std::string rm_quotes(std::string str) {
	std::string ret = "";
	
	for (int i = 1; i<str.length()-1; i++) {
		ret += str.at(i);
	}
	
	return ret;
}

//Does a string start with something?
bool starts_with(std::string in, std::string s) {
	std::string first = "";
	
	for (char c : in) {
		if (c == ' ')
			break;
		first += c;
	}
	
	if (first == s)
		return true;
	return false;
}

