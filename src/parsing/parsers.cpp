#include "parsers.hpp"
#include <charconv>
#include <algorithm>

namespace parsers {
	bool ignorable_char(char c) {
		return (c == ' ') || (c == '\r') || (c == '\f') || (c == '\n') || (c == '\t') || (c == ',') || (c == ';');
	}

	bool special_identifier_char(char c) {
		return (c == '!') || (c == '=') || (c == '<') || (c == '>');
	}

	bool breaking_char(char c) {
		return ignorable_char(c) || (c == '{') || (c == '}') || special_identifier_char(c) || (c == '\"') || (c == '\'') || (c == '#');
	}

	bool not_special_identifier_char(char c) {
		return !special_identifier_char(c);
	}

	bool line_termination(char c) {
		return (c == '\r') || (c == '\n');
	}

	bool double_quote_termination(char c) {
		return (c == '\r') || (c == '\n') || (c == '\"');
	}

	bool single_quote_termination(char c) {
		return (c == '\r') || (c == '\n') || (c == '\'');
	}

	template<size_t N>
	bool has_fixed_prefix(const char* start, const char* end, const char(&t)[N]) {
		if(end - start < ((std::ptrdiff_t)N - 1))
			return false;
		for(unsigned int i = 0; i < N - 1; ++i) {
			if(start[i] != t[i])
				return false;
		}
		return true;
	}

	template<size_t N>
	bool is_fixed_token(const char* start, const char* end, const char(&t)[N]) {
		if(end - start != (N - 1))
			return false;
		for(unsigned int i = 0; i < N - 1; ++i) {
			if(start[i] != t[i])
				return false;
		}
		return true;
	}

	template<size_t N>
	bool is_fixed_token_ci(const char* start, const char* end, const char(&t)[N]) {
		if(end - start != (N - 1))
			return false;
		for(unsigned int i = 0; i < N - 1; ++i) {
			if(tolower(start[i]) != t[i])
				return false;
		}
		return true;
	}

	bool is_positive_integer(const char* start, const char* end) {
		if(start == end)
			return false;
		while(start < end) {
			if(!isdigit(*start))
				return false;
			++start;
		}
		return true;
	}

	bool is_integer(const char* start, const char* end) {
		if(start == end)
			return false;
		if(*start == '-')
			return is_positive_integer(start + 1, end);
		else
			return is_positive_integer(start, end);
	}

	bool is_positive_fp(const char* start, const char* end) {
		const auto decimal = std::find(start, end, '.');
		if(decimal == end) {
			return is_positive_integer(start, end);
		} else if(decimal == start) {
			return is_positive_integer(decimal + 1, end);
		} else {
			return is_positive_integer(start, decimal) && (decimal + 1 == end || is_positive_integer(decimal + 1, end));
		}
	}

	bool is_fp(const char* start, const char* end) {
		if(start == end)
			return false;
		if(*start == '-')
			return is_positive_fp(start + 1, end);
		else
			return is_positive_fp(start, end);
	}

	template<typename T>
	char const* scan_for_match(char const* start, char const* end, int32_t& current_line, T&& condition) {
		while(start < end) {
			if(condition(*start))
				return start;
			if(*start == '\n')
				++current_line;
			++start;
		}
		return start;
	}
	template<typename T>
	char const* scan_for_not_match(char const* start, char const* end, int32_t& current_line, T&& condition) {
		while(start < end) {
			if(!condition(*start))
				return start;
			if(*start == '\n')
				++current_line;
			++start;
		}
		return start;
	}

	char const* advance_position_to_next_line(char const* start, char const* end, int32_t& current_line) {
		const auto start_lterm = scan_for_match(start, end, current_line, line_termination);
		return scan_for_not_match(start_lterm, end, current_line, line_termination);
	}


	char const* advance_position_to_non_whitespace(char const* start, char const* end, int32_t& current_line) {
		return scan_for_not_match(start, end, current_line, ignorable_char);
	}

	char const* advance_position_to_non_comment(char const* start, char const* end, int32_t& current_line) {
		auto position = advance_position_to_non_whitespace(start, end, current_line);
		while(position < end && *position == '#') {
			auto start_of_new_line = advance_position_to_next_line(position, end, current_line);
			position = advance_position_to_non_whitespace(start_of_new_line, end, current_line);
		}
		return position;
	}

	char const* advance_position_to_breaking_char(char const* start, char const* end, int32_t& current_line) {
		return scan_for_match(start, end, current_line, breaking_char);
	}

	token_and_type token_generator::internal_next() {
		if(position >= file_end)
			return token_and_type{ std::string_view(), current_line, token_type::unknown };

		auto non_ws = advance_position_to_non_comment(position, file_end, current_line);
		if(non_ws < file_end) {
			if(*non_ws == '{') {
				position = non_ws + 1;
				return token_and_type{ std::string_view(non_ws, 1), current_line, token_type::open_brace };
			} else if(*non_ws == '}') {
				position = non_ws + 1;
				return token_and_type{ std::string_view(non_ws, 1), current_line, token_type::close_brace };
			} else if(*non_ws == '\"') {
				const auto close = scan_for_match(non_ws + 1, file_end, current_line, double_quote_termination);
				position = close + 1;
				return token_and_type{ std::string_view(non_ws + 1, close - (non_ws + 1)), current_line, token_type::quoted_string };
			} else if(*non_ws == '\'') {
				const auto close = scan_for_match(non_ws + 1, file_end, current_line, single_quote_termination);
				position = close + 1;
				return token_and_type{ std::string_view(non_ws + 1, close - (non_ws + 1)), current_line, token_type::quoted_string };
			} else if(has_fixed_prefix(non_ws, file_end, "==") || has_fixed_prefix(non_ws, file_end, "<=")
				|| has_fixed_prefix(non_ws, file_end, ">=") || has_fixed_prefix(non_ws, file_end, "<>")
				|| has_fixed_prefix(non_ws, file_end, "!=")) {


				position = non_ws + 2;
				return token_and_type{ std::string_view(non_ws, 2), current_line, token_type::special_identifier };
			} else if(*non_ws == '<' || *non_ws == '>' || *non_ws == '=') {

				position = non_ws + 1;
				return token_and_type{ std::string_view(non_ws, 1), current_line, token_type::special_identifier };
			} else {
				position = advance_position_to_breaking_char(non_ws, file_end, current_line);
				return token_and_type{ std::string_view(non_ws, position - non_ws), current_line, token_type::identifier };
			}
		} else {
			position = file_end;
			return token_and_type{ std::string_view(), current_line, token_type::unknown };
		}

	}

	token_and_type token_generator::get() {
		if(peek_1.type != token_type::unknown) {
			auto const temp = peek_1;
			peek_1 = peek_2;
			peek_2.type = token_type::unknown;
			return temp;
		}

		return internal_next();
	}

	token_and_type token_generator::next() {
		if(peek_1.type == token_type::unknown) {
			peek_1 = internal_next();
		}
		return peek_1;
	}

	token_and_type token_generator::next_next() {
		if(peek_1.type == token_type::unknown) {
			peek_1 = internal_next();
		}
		if(peek_2.type == token_type::unknown) {
			peek_2 = internal_next();
		}
		return peek_2;
	}

	void token_generator::discard_group() {
		int32_t brace_count = 0;

		while(brace_count >= 0 && !at_end()) {
			auto gotten = get();
			if(gotten.type == token_type::open_brace) {
				brace_count++;
			} else if(gotten.type == token_type::close_brace) {
				brace_count--;
			}
		}
	}


	bool parse_bool(std::string_view content, int32_t, error_handler&) {
		if(content.length() == 0)
			return false;
		else
			return (content[0] == 'Y') || (content[0] == 'y') || (content[0] == '1');
	}

	float parse_float(std::string_view content, int32_t line, error_handler& err) {
		float rvalue = 0.0f;

		if(!float_from_chars(content.data(), content.data() + content.length(), rvalue)) {
			err.bad_float(content, line);
		}

		return rvalue;
	}

	double parse_double(std::string_view content, int32_t line, error_handler& err) {
		double rvalue = 0.0;
		if(!double_from_chars(content.data(), content.data() + content.length(), rvalue)) {
			err.bad_float(content, line);
		}
		return rvalue;
	}

	int32_t parse_int(std::string_view content, int32_t line, error_handler& err) {
		int32_t rvalue = 0;
		auto result = std::from_chars(content.data(), content.data() + content.length(), rvalue);
		if(result.ec == std::errc::invalid_argument) {
			err.bad_int(content, line);
		}
		return rvalue;
	}

	uint32_t parse_uint(std::string_view content, int32_t line, error_handler& err) {
		uint32_t rvalue = 0;
		auto result = std::from_chars(content.data(), content.data() + content.length(), rvalue);
		if(result.ec == std::errc::invalid_argument) {
			err.bad_unsigned_int(content, line);
		}
		return rvalue;
	}

	bool starts_with(std::string_view content, char v) {
		return content.length() != 0 && content[0] == v;
	}

	association_type parse_association_type(std::string_view content, int32_t line, error_handler& err) {
		if(content.length() == 1) {
			if(content[0] == '>')
				return association_type::gt;
			else if(content[0] == '<')
				return association_type::gt;
			else if(content[0] == '=')
				return association_type::eq_default;
		} else if(content.length() == 2) {
			if(content[0] == '=' && content[1] == '=')
				return association_type::eq;
			else if(content[0] == '<' && content[1] == '=')
				return association_type::le;
			else if(content[0] == '>' && content[1] == '=')
				return association_type::ge;
			else if(content[0] == '!' && content[1] == '=')
				return association_type::ne;
			else if(content[0] == '<' && content[1] == '>')
				return association_type::ne;
		}
		err.bad_association_token(content, line);
		return association_type::none;
	}

	/*
	date_tag parse_date(std::string_view content, int32_t line, error_handler& err) {
		const auto first_dot = std::find(start, end, '.');
		const auto second_dot = std::find(first_dot + 1, end, '.');

		const auto year = static_cast<uint16_t>(parse_uint(start, first_dot));
		const auto month = static_cast<uint16_t>(parse_uint(first_dot + 1, second_dot));
		const auto day = static_cast<uint16_t>(parse_uint(second_dot + 1, end));

		return date_to_tag(boost::gregorian::date(year, month, day));
	}
	*/


	separator_scan_result csv_find_separator_token(char const* start, char const* end, char seperator) {
		while(start != end) {
			if(line_termination(*start))
				return separator_scan_result{ start, false };
			else if(*start == seperator)
				return separator_scan_result{ start, true };
			else
				++start;
		}
		return separator_scan_result{ start, false };
	}

	char const* csv_advance(char const* start, char const* end, char seperator) {
		while(start != end) {
			if(line_termination(*start))
				return start;
			else if(*start == seperator)
				return start + 1;
			else
				++start;
		}
		return start;
	}

	char const* csv_advance_n(uint32_t n, char const* start, char const* end, char seperator) {

		if(n == 0)
			return start;
		--n;

		while(start != end) {
			if(line_termination(*start))
				return start;
			else if(*start == seperator) {
				if(n == 0)
					return start + 1;
				else
					--n;
			}
			++start;
		}
		return start;
	}

	char const* csv_advance_to_next_line(char const* start, char const* end) {

		while(start != end && !line_termination(*start)) {
			++start;
		}
		while(start != end && line_termination(*start))
			++start;
		if(start == end || *start != '#')
			return start;
		else
			return csv_advance_to_next_line(start, end);
	}

	std::string_view remove_surrounding_whitespace(std::string_view txt) {
		char const* start = txt.data();
		char const* end = txt.data() + txt.length();
		for(; start < end; ++start) {
			if(*start != ' ' && *start != '\t' && *start != '\r' && *start != '\n')
				break;
		}
		for(; start < end; --end) {
			if(*(end - 1) != ' ' && *(end - 1) != '\t' && *(end - 1) != '\r' && *(end - 1) != '\n')
				break;
		}
		return std::string_view(start, end - start);
	}
}
