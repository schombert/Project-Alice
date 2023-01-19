#pragma once

#include <string_view>
#include <stdint.h>
#include <string>

/*
* must support 
* class token_generator
*	--- member function get()  of type token_and_type
* *	--- member function next()  of type token_and_type
* *	--- member function next_next()  of type token_and_type
*	--- member function discard_group()
* struct token_and_type
*	--- member type of enum class token_type
*	--- member string_view content
*   --- member line_number int32_t
* class error_handler
*	--- member function unhandled_free_set(token_and_type)
*	--- member function unhandled_group_key(token_and_type)
*	--- member function unhandled_association_key(token_and_type)
*/

namespace parsers {
	enum class token_type {
		identifier,
		quoted_string,
		special_identifier,
		brace,
		open_brace,
		close_brace,
		unknown
	};

	enum class association_type : unsigned short {
		none, eq, lt, le, gt, ge, ne, eq_default, list
	};


	struct token_and_type {
		std::string_view content;
		int32_t line = 0;
		token_type type = token_type::unknown;
	};

	class token_generator {
	private:
		char const* position;
		char const* const file_end;
		int32_t current_line = 1;

		token_and_type peek_1;
		token_and_type peek_2;

		token_and_type internal_next();
	public:
		token_generator(char const* file_start, char const* fe) : position(file_start), file_end(fe) {
		}

		bool at_end() const {
			return peek_2.type == token_type::unknown && peek_1.type == token_type::unknown && position >= file_end;
		}
		token_and_type get();
		token_and_type next();
		token_and_type next_next();
		void discard_group();
	};

	class error_handler {
	public:
		std::string file_name;
		std::string accumulated_errors;

		error_handler(std::string file_name) : file_name(std::move(file_name)) { }

		void unhandled_group_key(token_and_type const& t) {
			accumulated_errors += "unexpected group key " + std::string(t.content) + " found on line " + std::to_string(t.line) + " of file " + file_name + "\n";
		}
		void unhandled_association_key(token_and_type const& t) {
			accumulated_errors += "unexpected group key " + std::string(t.content) + " found on line " + std::to_string(t.line) + " of file " + file_name + "\n";
		}
		void unhandled_free_value(token_and_type const& t) {
			accumulated_errors += "unexpected free value " + std::string(t.content) + " found on line " + std::to_string(t.line) + " of file " + file_name + "\n";
		}
		void unhandled_free_group(token_and_type const& t) {
			accumulated_errors += "unhandled free set beggining with  " + std::string(t.content) + " found on line " + std::to_string(t.line) + " of file " + file_name + "\n";
		}
		void bad_date(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as a date on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_float(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as a float on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_double(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as a double on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_bool(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as a boolean on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_int(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as an integer on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_unsigned_int(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as an unsigned integer on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
		void bad_association_token(std::string_view s, int32_t l) {
			accumulated_errors += "tried to parse  " + std::string(s) + " as equality or comparison on line " + std::to_string(l) + " of file " + file_name + "\n";
		}
	};

	bool float_from_chars(char const* start, char const* end, float& float_out); // returns true on success
	bool double_from_chars(char const* start, char const* end, double& dbl_out); // returns true on success

	std::string_view remove_surrounding_whitespace(std::string_view txt);

	float parse_float(std::string_view content, int32_t line, error_handler& err);
	double parse_double(std::string_view content, int32_t line, error_handler& err);
	bool parse_bool(std::string_view content, int32_t line, error_handler& err);
	int32_t parse_int(std::string_view content, int32_t line, error_handler& err);
	uint32_t parse_uint(std::string_view content, int32_t line, error_handler& err);
	association_type parse_association_type(std::string_view content, int32_t line, error_handler& err);
	inline std::string_view parse_text(std::string_view content, int32_t line, error_handler& err) { return content; }
	//date_tag parse_date(std::string_view content, int32_t line, error_handler& err);


	//
	// CSV file parsing
	// Note: this does not parse CSV files according to the standard
	// instead it parses them according to the "paradox standard"
	//

	struct separator_scan_result {
		char const* new_position = nullptr;
		bool found = false;
	};

	char const* csv_advance(char const* start, char const* end, char seperator);
	char const* csv_advance_n(uint32_t n, char const* start, char const* end, char seperator);
	char const* csv_advance_to_next_line(char const* start, char const* end);
	separator_scan_result csv_find_separator_token(char const* start, char const* end, char seperator);

	template<size_t count_values, typename T>
	char const* parse_fixed_amount_csv_values(char const* start, char const* end, char seperator, T&& function) {
		std::string_view values[count_values];
		for(uint32_t i = 0; i < count_values; ++i) {
			auto r = csv_find_separator_token(start, end, seperator);
			values[i] = std::string_view(start, r.new_position - start);
			start = r.new_position + int32_t(r.found);
		}
		function(values);

		return csv_advance_to_next_line(start, end);
	}

	template<typename T>
	char const* parse_first_and_nth_csv_values(uint32_t nth, char const* start, char const* end, char seperator, T&& function) {
		auto first_separator = csv_find_separator_token(start, end, seperator);

		std::string_view first_value = std::string_view(start, first_separator.new_position - start);

		start = csv_advance_n(nth - 2ui32, first_separator.new_position + int32_t(first_separator.found), end, seperator);

		auto second_end = csv_find_separator_token(start, end, seperator);

		function(first_value, std::string_view(start, second_end.new_position - start));
		return csv_advance_to_next_line(second_end.new_position + int32_t(second_end.found), end);
	}
}
