#include <string>
#include <string_view>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include "defines.hpp"
#include "parsers.hpp"

void parsing::defines::assign_define(std::string_view text, float v, parsers::error_handler& err) {
#define LUA_DEFINES_LIST_ELEMENT(key, const_value) \
	if(parsers::is_fixed_token_ci(text.data(), text.data() + text.length(), # key )) key = v;
	LUA_DEFINES_LIST
#undef LUA_DEFINES_LIST_ELEMENT
}

void parsing::defines::parse_line(std::string_view line, parsers::error_handler& err) {
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
			
			auto value = parsers::parse_float(line, 0, err);
			assign_define(key, value, err);
		}
	}
}

void parsing::defines::parse_file(std::string_view data, parsers::error_handler& err) {
	std::stringstream stream{std::string(data)};
	for(std::string line; std::getline(stream, line); )
		parse_line(line, err);
}
