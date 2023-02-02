#include <string>
#include <string_view>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include "defines.hpp"
#include "parsers.hpp"
#include "system_state.hpp"

void parsing::defines::assign_define(sys::state& state, std::string_view text, float v, parsers::error_handler& err) {
#define LUA_DEFINES_LIST_ELEMENT(key, const_value) \
	if(parsers::is_fixed_token_ci(text.data(), text.data() + text.length(), # key )) key = v;
	LUA_DEFINES_LIST
#undef LUA_DEFINES_LIST_ELEMENT
}

void parsing::defines::parse_line(sys::state& state, std::string_view line, parsers::error_handler& err) {
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
			assign_define(state, key, value, err);
		}
	} else if(isalpha(*it) && islower(*it)) {
		auto start_it = it;
		while(isalnum(*it) || *it == '_')
			it++;
		std::string key = std::string(line.substr(std::distance(line.begin(), start_it), std::distance(start_it, it)));
		sys::year_month_day ymd = { 1778, 1, 1 };

		while(isspace(*it))
			it++;
		if(*it == '=') {
			it++;
			while(isspace(*it))
				it++;
			if(*it == '"' || *it == '\'') {
				it++;
				// TODO for now this will do, but we need a method to parse dates
				ymd.year = parsers::parse_int(it, 0, err);
				while(isdigit(*it))
					it++;
				if(*it == '.') {
					it++;
					ymd.month = uint16_t(parsers::parse_uint(it, 0, err));
					while(isdigit(*it))
						it++;
					if(*it == '.') {
						it++;
						ymd.day = uint16_t(parsers::parse_uint(it, 0, err));
					}
				}
			}
		}

		if(parsers::is_fixed_token_ci(key.data(), key.data() + key.length(), "start_date" )) {
			state.start_date = sys::absolute_time_point(ymd);
		} else if(parsers::is_fixed_token_ci(key.data(), key.data() + key.length(), "end_date" )) {
			state.end_date = sys::absolute_time_point(ymd);
		}
	}
}

void parsing::defines::parse_file(sys::state& state, std::string_view data, parsers::error_handler& err) {
	std::stringstream stream{std::string(data)};
	for(std::string line; std::getline(stream, line); )
		parse_line(state, line, err);
}
