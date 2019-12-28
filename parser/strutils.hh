#pragma once

#include <string>

//The trim functions
std::string trim_left(std::string str);
std::string trim_right(std::string str);
std::string trim(std::string str);

//The contains functions
bool contains(std::string str, char c);
bool contains(std::string str, std::string in);

//Quote functions
bool is_quote(std::string str);
std::string rm_quotes(std::string str);

//Other functions
bool starts_with(std::string in, std::string s);
