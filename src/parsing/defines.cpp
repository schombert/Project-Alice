#include <string>
#include <string_view>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include "defines.hpp"
#include "parsers.hpp"

void parsing::defines::assign_define(std::string_view text, float v) {
#define LUA_DEFINES_LIST_ELEMENT(key, const_value) \
	if(parsers::is_fixed_token_ci(text.begin(), text.end(), # key )) key = v;
	LUA_DEFINES_LIST
#undef LUA_DEFINES_LIST_ELEMENT
}

void parsing::defines::parse_line(std::string_view line) {
	auto it = line.begin();
	while(isspace(*it))
		it++;
	
	if(isalpha(*it) && isupper(*it)) {
		auto start_it = it;
		while(isalnum(*it) || *it == '_')
			it++;
		
		auto key = line.substr(std::distance(line.begin(), start_it), std::distance(start_it, it));
		while(isspace(*it))
			it++;
		
		if(*it == '=') {
			it++;
			while(isspace(*it))
				it++;
			
			// TODO is it okay to use atof? - probably not...
			auto value = float(std::atof(it));
			assign_define(key, value);
		}
	}
}

void parsing::defines::parse_file(std::string_view data) {
	std::stringstream stream{std::string(data)};
	for(std::string line; std::getline(stream, line); )
		parse_line(line);
}
