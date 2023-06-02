#include <string>
#include <string_view>
#include <stdlib.h>
#include <ctype.h>
#include "defines.hpp"
#include "parsers.hpp"
#include "system_state.hpp"

void parsing::defines::assign_define(sys::state &state, int32_t line, std::string_view text, float v, parsers::error_handler &err) {
#define LUA_DEFINES_LIST_ELEMENT(key, const_value)                                  \
	if (parsers::is_fixed_token_ci(text.data(), text.data() + text.length(), #key)) \
		key = v;
	LUA_DEFINES_LIST
#undef LUA_DEFINES_LIST_ELEMENT
}

void parsing::defines::parse_line(sys::state &state, int32_t line, std::string_view data, parsers::error_handler &err) {
	char const *start = data.data();
	char const *end = data.data() + data.length();
	char const *position = start;

	for (; position < end && isspace(*position); ++position)
		;
	auto first_non_ws = position;

	for (; position < end && !isspace(*position) && *position != '='; ++position) // advances to end of identifier
		;

	auto identifier_end = position;

	for (; position < end && *position != '='; ++position) // advances to equal sign
		;
	++position;                                              // advances past equality
	for (; position < end && isspace(*position); ++position) // advances to next identifier
		;

	auto value_start = position;
	for (; position < end && !isspace(*position) && *position != ','; ++position) // advances to next identifier
		;

	auto value_end = position;

	if (parsers::is_fixed_token_ci(first_non_ws, identifier_end, "start_date")) {
		position = value_start;

		for (; position < value_end && !isdigit(*position); ++position) // advance to year
			;
		auto year_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to year end
			;
		auto year_end = position;

		for (; position < value_end && !isdigit(*position); ++position) // advance to month
			;
		auto month_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to month end
			;
		auto month_end = position;

		for (; position < value_end && !isdigit(*position); ++position) // advance to day
			;
		auto day_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to day end
			;
		auto day_end = position;

		state.start_date = sys::absolute_time_point(sys::year_month_day{
		    parsers::parse_int(std::string_view(year_start, year_end - year_start), line, err),
		    uint16_t(parsers::parse_uint(std::string_view(month_start, month_end - month_start), line, err)),
		    uint16_t(parsers::parse_uint(std::string_view(day_start, day_end - day_start), line, err))});
	} else if (parsers::is_fixed_token_ci(first_non_ws, identifier_end, "end_date")) {
		position = value_start;

		for (; position < value_end && !isdigit(*position); ++position) // advance to year
			;
		auto year_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to year end
			;
		auto year_end = position;

		for (; position < value_end && !isdigit(*position); ++position) // advance to month
			;
		auto month_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to month end
			;
		auto month_end = position;

		for (; position < value_end && !isdigit(*position); ++position) // advance to day
			;
		auto day_start = position;
		for (; position < value_end && isdigit(*position); ++position) // advance to day end
			;
		auto day_end = position;

		state.end_date = sys::absolute_time_point(sys::year_month_day{
		    parsers::parse_int(std::string_view(year_start, year_end - year_start), line, err),
		    uint16_t(parsers::parse_uint(std::string_view(month_start, month_end - month_start), line, err)),
		    uint16_t(parsers::parse_uint(std::string_view(day_start, day_end - day_start), line, err))});
	} else {
		const auto value = parsers::parse_float(std::string_view(value_start, value_end - value_start), line, err);
		assign_define(state, line, std::string_view(first_non_ws, identifier_end - first_non_ws), value, err);
	}
}

void parsing::defines::parse_file(sys::state &state, std::string_view data, parsers::error_handler &err) {
	const auto remove_comments = [](std::string_view text) -> std::string_view {
		auto end_it = text.end();
		for (auto it = text.begin(); it != text.end(); it++) {
			if (it != text.end() && (it + 1) != text.end() && it[0] == '-' && it[1] == '-') {
				end_it = it;
				break;
			}
		}
		return text.substr(0, std::distance(text.begin(), end_it));
	};

	auto start_it = data.begin();
	auto line_num = 1;
	for (auto it = data.begin(); it != data.end(); it++) {
		if (*it == '\n') {
			const auto line_text = data.substr(std::distance(data.begin(), start_it), std::distance(start_it, it));
			parse_line(state, line_num, remove_comments(line_text), err);
			start_it = it + 1;
			line_num++;
		}
	}
}
