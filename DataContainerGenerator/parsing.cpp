#include "parsing.hpp"
#include <assert.h>
 
char const * advance_to_closing_bracket(char const * pos, char const * end) {
	int32_t depth_count = 0;
	bool in_quote = false;

	while(pos < end) {
		if(*pos == '{' && !in_quote)
			++depth_count;
		else if(*pos == '}' && !in_quote)
			--depth_count;
		else if(*pos == '\"')
			in_quote = !in_quote;
		else if(*pos == '\\' && in_quote) {
			++pos;
		}
		if(depth_count < 0)
			return pos;
		++pos;
	}
	return pos;
}

char const * advance_to_closing_parenthesis(char const * pos, char const * end) {
	int32_t depth_count = 0;
	bool in_quote = false;

	while(pos < end) {
		if(*pos == '(' && !in_quote)
			++depth_count;
		else if(*pos == ')' && !in_quote)
			--depth_count;
		else if(*pos == '\"')
			in_quote = !in_quote;
		else if(*pos == '\\' && in_quote) {
			++pos;
		}
		if(depth_count < 0)
			return pos;
		++pos;
	}
	return pos;
}

char const * advance_to_closing_square_bracket(char const * pos, char const * end) {
	int32_t depth_count = 0;
	bool in_quote = false;

	while(pos < end) {
		if(*pos == '[' && !in_quote)
			++depth_count;
		else if(*pos == ']' && !in_quote)
			--depth_count;
		else if(*pos == '\"')
			in_quote = !in_quote;
		else if(*pos == '\\' && in_quote) {
			++pos;
		}
		if(depth_count < 0)
			return pos;
		++pos;
	}
	return pos;
}

bool is_ascii_ws(char v) {
	return v == ' ' || v == '\t' || v == '\n' || v == '\r';
}

char const * advance_to_non_whitespace(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos != ' ' && *pos != '\t' && *pos != '\n' && *pos != '\r')
			return pos;
		++pos;
	}
	return pos;
}

char const * advance_to_non_whitespace_non_comma(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos != ' ' && *pos != '\t' && *pos != '\n' && *pos != '\r' && *pos != ',')
			return pos;
		++pos;
	}
	return pos;
}

char const * advance_to_identifier_end(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r'
			|| *pos == ',' || *pos == '(' || *pos == ')' || *pos == '[' || *pos == ']' || *pos == '{'
			|| *pos == '}' || *pos == ';' || *pos == '.' || *pos == '?' || *pos == '/' || *pos == '<'
			|| *pos == '>' || *pos == '\'' || *pos == '\"' || *pos == '|' || *pos == '\\'
			|| *pos == '=' || *pos == '+' || *pos == '-' || *pos == '~' || *pos == '!' || *pos == '#'
			|| *pos == '$' || *pos == '%' || *pos == '^' || *pos == '&' || *pos == '*')
			return pos;
		++pos;
	}
	return pos;
}


char const * advance_to_at(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos == '@')
			return pos;
		++pos;
	}
	return pos;
}

std::string remove_ats(char const * pos, char const * end) {
	std::string result;

	while(pos < end) {
		if(*pos != '@')
			result += *pos;
		++pos;
	}

	return result;
}

char const * advance_to_whitespace(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r')
			return pos;
		++pos;
	}
	return pos;
}

char const * advance_to_whitespace_or_brace(char const * pos, char const * end) {
	while(pos < end) {
		if(*pos == ' ' || *pos == '\t' || *pos == '\n' || *pos == '\r' || *pos == '{')
			return pos;
		++pos;
	}
	return pos;
}

bool check_for_identifier(char const* identifier, char const * start, char const * end) {
	char const* pos = advance_to_non_whitespace_non_comma(start, end);
	char const* id_end = advance_to_identifier_end(pos, end);
	while(pos < id_end) {
		if(*identifier == 0)
			return pos == id_end;
		if(*identifier != *pos)
			return false;
		++identifier;
		++pos;
	}
	return *identifier == 0;
}

char const * reverse_to_non_whitespace(char const * start, char const * pos) {
	while(pos > start) {
		if(*(pos - 1) != ' ' && *(pos - 1) != '\t' && *(pos - 1) != '\n' && *(pos - 1) != '\r')
			return pos;
		--pos;
	}
	return pos;
}

std::vector<selection_item> parse_all_selection_items(char const* &start, char const * end, char const * global_start, error_record & err) {
	std::vector<selection_item> result;

	while(start < end) {
		if(check_for_identifier("from", start, end)) {
			start = advance_to_identifier_end(advance_to_non_whitespace_non_comma(start, end), end);
			return result;
		}
		start = advance_to_non_whitespace_non_comma(start, end);
		if(start != end)
			result.push_back(parse_selection_item(start, end, global_start, err));
	}
	
	return result;
}

std::vector<from_item> parse_all_from_items(char const* &start, char const * end, char const * global_start, error_record & err) {
	std::vector<from_item> result;

	auto start_copy = start;
	while(start < end) {
		if(check_for_identifier("where", start, end) || check_for_identifier("group", start, end)) {
			return result;
		}
		start = advance_to_non_whitespace_non_comma(start, end);
		if(start != end)
			result.push_back(parse_from_item(start, end, global_start, err));
	}

	if(result.size() == 0) {
		err.add(calculate_line_from_position(global_start, start_copy), 1, std::string("a query must draw upon at least one data source"));
	}

	return result;

}

select_statement_definition parse_select_statement(char const* &start, char const * end, char const * global_start, error_record & err) {
	select_statement_definition result;

	result.returned_values = parse_all_selection_items(start, end, global_start, err);
	result.from = parse_all_from_items(start, end, global_start, err);

	if(check_for_identifier("where", start, end)) {
		start = advance_to_identifier_end(advance_to_non_whitespace_non_comma(start, end), end);
		while(start < end) {
			start = advance_to_non_whitespace(start, end);
			if(start < end && *start == '(') {
				char const* block_end = advance_to_closing_parenthesis(start + 1, end);
				if(block_end < end)
					++block_end;
				if(result.where_clause.length() > 0) result.where_clause += " ";
				result.where_clause += std::string(start, block_end);
				start = block_end;
			} else if(check_for_identifier("group", start, end)) {
				break;
			} else {
				char const* block_end = advance_to_whitespace(start, end);
				if(result.where_clause.length() > 0) result.where_clause += " ";
				result.where_clause += std::string(start, block_end);
				start = block_end;
			}
		}
	}
	if(check_for_identifier("group", start, end)) {
		start = advance_to_identifier_end(advance_to_non_whitespace_non_comma(start, end), end);
		result.group_by = std::string(advance_to_non_whitespace_non_comma(start, end), end);
		start = end;
	}
	return result;
}

from_item parse_from_item(char const* &start, char const * end, char const * global_start, error_record & err) {
	from_item result;

	start = advance_to_non_whitespace_non_comma(start, end);

	if(start < end && *start == '@') {
		result.type = from_type::parameter;
		++start;

		result.table_identifier = parse_qual_name(start, end, global_start, err);
		return result;
	}

	const char* ident_end = advance_to_identifier_end(start, end);
	if(char const* non_ws = advance_to_non_whitespace(ident_end, end); non_ws < end && *non_ws == '.') {
		result.left_of_join = std::string(start, ident_end);
		start = non_ws + 1;
	}

	if(check_for_identifier("join", start, end)) {
		start = advance_to_identifier_end(advance_to_non_whitespace(start, end), end);
		if(start < end && *start == '+') {
			result.type = from_type::join_plus;
			++start;
		} else {
			result.type = from_type::join;
		}
	}

	start = advance_to_non_whitespace(start, end);
	if(check_for_identifier("on", start, end)) {
		start = advance_to_non_whitespace(advance_to_identifier_end(start, end), end);
		char const* id_end = advance_to_identifier_end(start, end);
		result.join_on = std::string(start, id_end);
		start = advance_to_non_whitespace(id_end, end);
	}

	result.table_identifier = parse_qual_name(start, end, global_start, err);

	return result;
}

type_name_pair parse_type_and_name(char const* start, char const * end) {
	type_name_pair result;

	char const* first_id_end = advance_to_identifier_end(start, end);

	if(first_id_end < end && *first_id_end == '.') {
		char const* member_end = advance_to_identifier_end(first_id_end + 1, end);

		result.type = std::string(start, first_id_end);
		result.name = std::string(first_id_end + 1, member_end);
	} else {
		result.name = std::string(start, first_id_end);
	}

	return result;
}

qualified_name parse_qual_name(char const* &start, char const * end, char const * , error_record & ) {
	qualified_name result;

	char const* non_ws_start = advance_to_non_whitespace_non_comma(start, end);
	char const* first_id_end = advance_to_identifier_end(non_ws_start, end);
	char const* trailing_first_id = advance_to_non_whitespace(first_id_end, end);

	if(trailing_first_id < end && *trailing_first_id == '.') {
		char const* member_non_ws_start = advance_to_non_whitespace(trailing_first_id + 1, end);
		char const* member_end = advance_to_identifier_end(member_non_ws_start, end);

		result.type_name = std::string(non_ws_start, first_id_end);
		result.member_name = std::string(member_non_ws_start, member_end);

		char const* trailing_second_id = advance_to_non_whitespace(member_end, end);

		if(trailing_second_id < end + 2 && *trailing_second_id == 'a' && *(trailing_second_id + 1) == 's'
			&& is_ascii_ws(*(trailing_second_id + 2))) {

			char const* as_non_ws_start = advance_to_non_whitespace(trailing_second_id + 2, end);
			char const* as_id_end = advance_to_identifier_end(as_non_ws_start, end);

			result.as_name = std::string(as_non_ws_start, as_id_end);
			start = as_id_end;
		} else {

			result.as_name = result.type_name + "_" + result.member_name;
			start = trailing_second_id;
		}

	} else if(trailing_first_id < end + 2 && *trailing_first_id == 'a' && *(trailing_first_id + 1) == 's'
		&& is_ascii_ws(*(trailing_first_id + 2))) {

		char const* as_non_ws_start = advance_to_non_whitespace(trailing_first_id + 2, end);
		char const* as_id_end = advance_to_identifier_end(as_non_ws_start, end);

		result.member_name = std::string(non_ws_start, first_id_end);
		result.as_name = std::string(as_non_ws_start, as_id_end);

		start = as_id_end;
	} else {
		result.member_name = std::string(non_ws_start, first_id_end);
		result.as_name = result.member_name;

		start = trailing_first_id;
	}

	return result;
}

selection_item parse_selection_item(char const* &start, char const * end, char const * global_start, error_record & err) {
	selection_item result;

	char const* non_ws_start = advance_to_non_whitespace(start, end);
	char const* first_id_end = advance_to_identifier_end(non_ws_start, end);
	char const* trailing_first_id = advance_to_non_whitespace(first_id_end, end);

	if(trailing_first_id < end && *trailing_first_id == '(') { // is aggregate
		result.is_aggregate = true;
		result.aggregate_name = std::string(non_ws_start, first_id_end);

		char const* inner_start = trailing_first_id + 1;
		char const* inner_end = advance_to_closing_parenthesis(inner_start, end);

		result.property = parse_qual_name(inner_start, inner_end, global_start, err);
		start = inner_end + 1;
	} else {
		result.property = parse_qual_name(start, end, global_start, err);
	}

	return result;
}

row_col_pair calculate_line_from_position(char const * start, char const * pos) {
	row_col_pair result;
	result.row = 1;
	result.column = 1;

	char const* t = start;
	while(t < pos) {
		if(*t == '\n') {
			result.column = 0;
			++result.row;
		}
		++result.column;
		++t;
	}

	return result;
}

parsed_item extract_item(char const * input, char const * end, char const * global_start, error_record & err) {
	parsed_item result;

	result.key.start = advance_to_non_whitespace(input, end);
	result.key.end = advance_to_whitespace_or_brace(result.key.start, end);

	char const* position = advance_to_non_whitespace(result.key.end, end);
	result.terminal = position;

	while(position < end) {
		if(*position != '{')
			break;
		position = advance_to_non_whitespace(position + 1, end);
		char const* value_close = advance_to_closing_bracket(position, end);
		if(value_close == end) {
			err.add(calculate_line_from_position(global_start, position), 2, "Open bracket was not properly closed.");
		}
		result.values.push_back(char_range{ position, reverse_to_non_whitespace(position, value_close) });
		position = advance_to_non_whitespace(value_close + 1, end);
		result.terminal = std::min(end, position);
	}

	return result;
}

load_save_def parse_load_save_def(char const* start, char const* end, char const* global_start, error_record& err_out) {
	load_save_def result;
	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 3, 
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.name = extracted.values[0].to_string();
				}
			} else if(kstr == "only_objects") {
				if(extracted.values.size() == 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 4,
						std::string("wrong number of parameters for \"only_objects\""));
				} else if(result.objects_filter == filter_type::default_exclude) {
					result.objects_filter = filter_type::include;
					for(uint32_t i = 0; i < extracted.values.size(); ++ i)
						result.obj_tags.push_back(extracted.values[0].to_string());
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 5,
						std::string("illegal setting of the object filter a second time"));
				}
			} else if(kstr == "exclude_objects") {
				if(extracted.values.size() == 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 6,
						std::string("wrong number of parameters for \"exclude_objects\""));
				} else if(result.objects_filter == filter_type::default_exclude) {
					result.objects_filter = filter_type::exclude;
					for(uint32_t i = 0; i < extracted.values.size(); ++i)
						result.obj_tags.push_back(extracted.values[0].to_string());
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 5,
						std::string("illegal setting of the object filter a second time"));
				}
			} else if(kstr == "only_properties") {
				if(extracted.values.size() == 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 7, 
						std::string("wrong number of parameters for \"only_properties\""));
				} else if(result.properties_filter == filter_type::default_exclude) {
					result.properties_filter = filter_type::include;
					for(uint32_t i = 0; i < extracted.values.size(); ++i)
						result.property_tags.push_back(extracted.values[0].to_string());
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 8, 
						std::string("illegal setting of the properties filter a second time"));
				}
			} else if(kstr == "exclude_properties") {
				if(extracted.values.size() == 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 9,
						std::string("wrong number of parameters for \"exclude_properties\""));
				} else if(result.properties_filter == filter_type::default_exclude) {
					result.properties_filter = filter_type::exclude;
					for(uint32_t i = 0; i < extracted.values.size(); ++i)
						result.property_tags.push_back(extracted.values[0].to_string());
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 8,
						"illegal setting of the properties filter a second time on line ");
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 10,
					std::string("unexpected token \"") + kstr + "\" while parsing loading/saving routine defintion");
			}
		}
	}
	return result;
}

related_object parse_link_def(char const * start, char const * end, char const * global_start, error_record & err_out) {
	related_object result;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 11,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.property_name = extracted.values[0].to_string();
				}
			} else if(kstr == "type") {
				if(extracted.values.size() == 0 || extracted.values.size() > 2) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 12,
						std::string("wrong number of parameters for \"type\""));
				} else if(extracted.values[0].to_string() == "unique") {
					result.index = index_type::at_most_one;
					if(extracted.values.size() > 1) {
						if(extracted.values[1].to_string() == "optional") {
							result.is_optional = true;
						} else {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 13,
								std::string("unknown parameter \"") + extracted.values[1].to_string() + "\" passed to type");
						}
					}
				} else if(extracted.values[0].to_string() == "many") {
					result.index = index_type::many;
					if(extracted.values.size() > 1) {
						if(extracted.values[1].to_string() == "optional") {
							result.is_optional = true;
						} else {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 13,
								std::string("unknown parameter \"") + extracted.values[1].to_string() + "\" passed to type");
						}
					}
				} else if(extracted.values[0].to_string() == "unindexed") {
					result.index = index_type::none;
					if(extracted.values.size() > 1) {
						if(extracted.values[1].to_string() == "optional") {
							result.is_optional = true;
						} else {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 13,
								std::string("unknown parameter \"") + extracted.values[1].to_string() + "\" passed to type");
						}
					}
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 14,
						std::string("unknown parameter \"") + extracted.values[0].to_string() + "\" passed to type");
				}
			} else if(kstr == "object") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 15, 
						std::string("wrong number of parameters for \"object\""));
				} else {
					result.type_name = extracted.values[0].to_string();
				}
			} else if(kstr == "index_storage") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 16,
						std::string("wrong number of parameters for \"index_storage\""));
				} else if(extracted.values[0].to_string() == "std_vector") {
					result.ltype = list_type::std_vector;
				} else if(extracted.values[0].to_string() == "list") {
					result.ltype = list_type::list;
				} else if(extracted.values[0].to_string() == "array") {
					result.ltype = list_type::array;
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 17,
						std::string("unknown parameter \"") + extracted.values[0].to_string() + "\" passed to index_storage");
				}
			} else if(kstr == "multiple") {
				if(extracted.values.size() != 1 && extracted.values.size() != 2) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 18, 
						std::string("wrong number of parameters for \"multiple\""));
				} else {
					result.multiplicity = std::stoi(extracted.values[0].to_string());
					if(extracted.values.size() == 2) {
						if(extracted.values[1].to_string() == "distinct") {
							result.is_distinct = true;
						} else {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 18,
								std::string("unknown parameter \"") + extracted.values[1].to_string() + "\" passed to multiple");
						}
					}
				}
			} else if(kstr == "private") {
				if(extracted.values.size() != 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 19,
						std::string("wrong number of parameters for \"private\""));
				} else if(result.protection != protection_type::none) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 20,
						std::string("redefintion of access restriction"));
				} else {
					result.protection = protection_type::hidden;
				}
			} else if(kstr == "protected") {
				if(extracted.values.size() != 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 21,
						std::string("wrong number of parameters for \"protected\""));
				} else if(result.protection != protection_type::none) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 20,
						std::string("redefintion of access restriction"));
				} else {
					result.protection = protection_type::read_only;
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 22,
					std::string("unexpected token \"") + kstr + "\" while parsing link defintion");
			}
		}
	}
	return result;
}

composite_index_def parse_composite_key(char const * start, char const * end, char const * global_start, error_record & err_out) {
	composite_index_def result;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 23,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.name = extracted.values[0].to_string();
				}
			} else if(kstr == "index") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 24,
						std::string("wrong number of parameters for \"index\""));
				} else {
					result.component_indexes.push_back(key_component{ extracted.values[0].to_string(), std::string(), 0, 0 });
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 25, 
					std::string("unexpected token \"") + kstr + "\" while parsing composite key");
			}
		}
	}
	return result;
}

property_def parse_property_def(char const * start, char const * end, char const * global_start, error_record & err_out) {
	property_def result;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 26,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.name = extracted.values[0].to_string();
				}
			} else if(kstr == "type") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 27,
						std::string("wrong number of parameters for \"type\""));
				} else {
					auto inner_extracted = extract_item(extracted.values[0].start, extracted.values[0].end, global_start, err_out);
					std::string ikstr = inner_extracted.key.to_string();

					if(ikstr == "bitfield") {
						result.type = property_type::bitfield;
					} else if(ikstr == "object") {
						if(inner_extracted.values.size() != 1) {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 28,
								std::string("wrong number of parameters for \"object\""));
						} else {
							result.type = property_type::object;
							result.data_type = inner_extracted.values[0].to_string();
						}
					} else if(ikstr == "derived") {
						if(inner_extracted.values.size() != 1) {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 29,
								std::string("wrong number of parameters for \"derived\""));
						} else {
							result.is_derived = true;
							if(inner_extracted.values[0].to_string() == "bitfield")
								result.type = property_type::bitfield;
							else
								result.type = property_type::other;
							result.data_type = inner_extracted.values[0].to_string();
						}
					} else if(ikstr == "vector_pool") {
						if(inner_extracted.values.size() != 2) {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 30,
								std::string("wrong number of parameters for \"vector_pool\""));
						} else {
							result.type = property_type::special_vector;
							result.special_pool_size = std::stoi(inner_extracted.values[0].to_string());
							result.data_type = inner_extracted.values[1].to_string();
						}
					} else if(ikstr == "array") {
						if(inner_extracted.values.size() > 2) {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 31,
								std::string("wrong number of parameters for \"array\""));
						} else if(inner_extracted.values.size() == 2) {
							result.array_index_type = inner_extracted.values[0].to_string();
							if(inner_extracted.values[1].to_string() == "bitfield") {
								result.type = property_type::array_bitfield;
							} else {
								result.type = property_type::array_other;
								result.data_type = inner_extracted.values[1].to_string();
							}
						} else {
							result.array_index_type = "uint32_t";
							if(inner_extracted.values[0].to_string() == "bitfield") {
								result.type = property_type::array_bitfield;
							} else {
								result.type = property_type::array_other;
								result.data_type = inner_extracted.values[0].to_string();
							}
						}
					} else {
						if(inner_extracted.values.size() != 0) {
							err_out.add(calculate_line_from_position(global_start, extracted.key.start), 32,
								std::string("unexpected key \"") + ikstr + "\"");
						} else {
							result.type = property_type::other;
							result.data_type = inner_extracted.key.to_string();
						}
					}
				}
			} else if(kstr == "hook") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 33,
						std::string("wrong number of parameters for \"hook\""));
				} else if(extracted.values[0].to_string() == "get") {
					result.hook_get = true;
				} else if(extracted.values[0].to_string() == "set") {
					result.hook_set = true;
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 34,
						std::string("unknown parameter \"") + extracted.values[0].to_string() + "\" passed to hook");
				}
			} else if(kstr == "tag") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 35,
						std::string("wrong number of parameters for \"tag\""));
				} else {
					result.property_tags.push_back(extracted.values[0].to_string());
				}
			} else if(kstr == "private") {
				if(extracted.values.size() != 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 36,
						std::string("wrong number of parameters for \"private\""));
				} else if(result.protection != protection_type::none) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 37,
						std::string("redefintion of access restriction"));
				} else {
					result.protection = protection_type::hidden;
				}
			} else if(kstr == "protected") {
				if(extracted.values.size() != 0) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 38,
						std::string("wrong number of parameters for \"protected\""));
				} else if(result.protection != protection_type::none) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 39,
						std::string("redefintion of access restriction"));
				} else {
					result.protection = protection_type::read_only;
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 40,
					std::string("unexpected token \"") + kstr + "\" while parsing property defintion");
			}
		}
	}
	return result;
}

relationship_object_def parse_relationship(char const * start, char const * end, char const * global_start, error_record & err_out) {
	relationship_object_def result;

	result.is_relationship = true;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 41,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.name = extracted.values[0].to_string();
				}
			} else if(kstr == "primary_key") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 42,
						std::string("wrong number of parameters for \"primary_key\""));
				} else {
					result.force_pk = extracted.values[0].to_string();
				}
			} else if(kstr == "storage_type") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 43,
						std::string("wrong number of parameters for \"storage_type\""));
				} else {
					if(extracted.values[0].to_string() == "contiguous") {
						result.store_type = storage_type::contiguous;
					} else if(extracted.values[0].to_string() == "erasable") {
						result.store_type = storage_type::erasable;
					} else if(extracted.values[0].to_string() == "compactable") {
						result.store_type = storage_type::compactable;
					} else {
						err_out.add(calculate_line_from_position(global_start, extracted.key.start), 44,
							std::string("unknown parameter \"") + kstr + "\" passed to storage_type");
					}
				}
			} else if(kstr == "size") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 45,
						std::string("wrong number of parameters for \"size\""));
				} else {
					if(extracted.values[0].to_string() == "expandable") {
						result.is_expandable = true;
						result.size = 0;
					} else {
						result.is_expandable = false;
						result.size = std::stoi(extracted.values[0].to_string());
					}
				}
			} else if(kstr == "tag") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 46,
						std::string("wrong number of parameters for \"tag\""));
				} else {
					result.obj_tags.push_back(extracted.values[0].to_string());
				}
			} else if(kstr == "property") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 47,
						std::string("wrong number of parameters for \"property\""));
				} else {
					result.properties.push_back(
						parse_property_def(extracted.values[0].start, extracted.values[0].end, global_start, err_out));
				}
			} else if(kstr == "link") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 48,
						std::string("wrong number of parameters for \"link\""));
				} else {
					result.indexed_objects.push_back(
						parse_link_def(extracted.values[0].start, extracted.values[0].end, global_start, err_out));
				}
			} else if(kstr == "composite_key") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 49,
						std::string("wrong number of parameters for \"composite_key\""));
				} else {
					result.composite_indexes.push_back(
						parse_composite_key(extracted.values[0].start, extracted.values[0].end, global_start, err_out));
				}
			} else if(kstr == "function") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 50,
						std::string("wrong number of parameters for \"function\""));
				} else {
					member_function_spec ns;
					ns.signature = remove_ats(extracted.values[0].start, extracted.values[0].end);
					ns.is_const = false;
					char const* local_pos = advance_to_at(extracted.values[0].start, extracted.values[0].end) + 1;
					char const* id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
					ns.name = std::string(local_pos, id_end);
					local_pos = id_end;
					while(local_pos < extracted.values[0].end) {
						local_pos = advance_to_at(local_pos, extracted.values[0].end) + 1;
						id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
						if(id_end != local_pos)
							ns.parameter_names.push_back(std::string(local_pos, id_end));
						local_pos = id_end;
					}
					result.member_functions.push_back(ns);
				}
			} else if(kstr == "const_function") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 51,
						std::string("wrong number of parameters for \"const_function\""));
				} else {
					member_function_spec ns;
					ns.signature = remove_ats(extracted.values[0].start, extracted.values[0].end);
					ns.is_const = true;
					char const* local_pos = advance_to_at(extracted.values[0].start, extracted.values[0].end) + 1;
					char const* id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
					ns.name = std::string(local_pos, id_end);
					local_pos = id_end;
					while(local_pos < extracted.values[0].end) {
						local_pos = advance_to_at(local_pos, extracted.values[0].end) + 1;
						id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
						if(id_end != local_pos)
							ns.parameter_names.push_back(std::string(local_pos, id_end));
						local_pos = id_end;
					}
					result.member_functions.push_back(ns);
				}
			} else if(kstr == "hook") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 52,
						std::string("wrong number of parameters for \"hook\""));
				} else if(extracted.values[0].to_string() == "create") {
					result.hook_create = true;
				} else if(extracted.values[0].to_string() == "delete") {
					result.hook_delete = true;
				} else if(extracted.values[0].to_string() == "move") {
					result.hook_move = true;
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 53,
						std::string("unknown parameter \"") + extracted.values[0].to_string() + "\" passed to hook");
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 54,
					std::string("unexpected token \"") + kstr + "\" while parsing relationship defintion");
			}
		}
	}
	return result;
}

relationship_object_def parse_object(char const * start, char const * end, char const * global_start, error_record & err_out) {
	relationship_object_def result;

	result.is_relationship = false;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 55,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.name = extracted.values[0].to_string();
				}
			} else if(kstr == "storage_type") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 56,
						std::string("wrong number of parameters for \"storage_type\""));
				} else {
					if(extracted.values[0].to_string() == "contiguous") {
						result.store_type = storage_type::contiguous;
					} else if(extracted.values[0].to_string() == "erasable") {
						result.store_type = storage_type::erasable;
					} else if(extracted.values[0].to_string() == "compactable") {
						result.store_type = storage_type::compactable;
					} else {
						err_out.add(calculate_line_from_position(global_start, extracted.key.start), 57,
							std::string("unknown parameter \"") + kstr + "\" passed to storage_type");
					}
				}
			} else if(kstr == "size") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 58,
						std::string("wrong number of parameters for \"size\""));
				} else {
					if(extracted.values[0].to_string() == "expandable") {
						result.is_expandable = true;
						result.size = 0;
					} else {
						result.is_expandable = false;
						result.size = std::stoi(extracted.values[0].to_string());
					}
				}
			} else if(kstr == "tag") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 59,
						std::string("wrong number of parameters for \"tag\""));
				} else {
					result.obj_tags.push_back(extracted.values[0].to_string());
				}
			} else if(kstr == "hook") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 60,
						std::string("wrong number of parameters for \"hook\""));
				} else if(extracted.values[0].to_string() == "create") {
					result.hook_create = true;
				} else if(extracted.values[0].to_string() == "delete") {
					result.hook_delete = true;
				} else if(extracted.values[0].to_string() == "move") {
					result.hook_move = true;
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 61,
						std::string("unknown parameter \"") + extracted.values[0].to_string() + "\" passed to hook");
				}
			} else if(kstr == "property") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 62,
						std::string("wrong number of parameters for \"property\""));
				} else {
					result.properties.push_back(
						parse_property_def(extracted.values[0].start, extracted.values[0].end, global_start, err_out));
				}
			} else if(kstr == "function") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 63,
						std::string("wrong number of parameters for \"function\""));
				} else {
					member_function_spec ns;
					ns.signature = remove_ats(extracted.values[0].start, extracted.values[0].end);
					ns.is_const = false;
					char const* local_pos = advance_to_at(extracted.values[0].start, extracted.values[0].end) + 1;
					char const* id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
					ns.name = std::string(local_pos, id_end);
					local_pos = id_end;
					while(local_pos < extracted.values[0].end) {
						local_pos = advance_to_at(local_pos, extracted.values[0].end) + 1;
						id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
						if(id_end != local_pos)
							ns.parameter_names.push_back(std::string(local_pos, id_end));
						local_pos = id_end;
					}
					result.member_functions.push_back(ns);
				}
			} else if(kstr == "const_function") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 64,
						std::string("wrong number of parameters for \"const_function\""));
				} else {
					member_function_spec ns;
					ns.signature = remove_ats(extracted.values[0].start, extracted.values[0].end);
					ns.is_const = true;
					char const* local_pos = advance_to_at(extracted.values[0].start, extracted.values[0].end) + 1;
					char const* id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
					ns.name = std::string(local_pos, id_end);
					local_pos = id_end;
					while(local_pos < extracted.values[0].end) {
						local_pos = advance_to_at(local_pos, extracted.values[0].end) + 1;
						id_end = advance_to_identifier_end(local_pos, extracted.values[0].end);
						if(id_end != local_pos)
							ns.parameter_names.push_back(std::string(local_pos, id_end));
						local_pos = id_end;
					}
					result.member_functions.push_back(ns);
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 65,
					std::string("unexpected token \"") + kstr + "\" while parsing relationship defintion");
			}
		}
	}
	return result;
}

std::vector<std::string> parse_legacy_types(char const * start, char const * end, char const * global_start, error_record & err_out) {
	std::vector<std::string> result;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 66,
						std::string("wrong number of parameters for \"name\""));
				} else {
					result.push_back(extracted.values[0].to_string());
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 67,
					std::string("unexpected token \"") + kstr + "\" while parsing legacy types list");
			}
		}
	}
	return result;
}

conversion_def parse_conversion_def(char const * start, char const * end, char const * global_start, error_record & err_out) {
	conversion_def result;
	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "from") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 68,
						std::string("wrong number of parameters for \"from\""));
				} else if(result.from.length() == 0) {
					result.from = extracted.values[0].to_string();
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 69,
						std::string("multiple defintion of \"from\" while parsing conversion defintion"));
				}
			} else if(kstr == "to") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 70,
						std::string("wrong number of parameters for \"to\""));
				} else if(result.to.length() == 0) {
					result.to = extracted.values[0].to_string();
				} else {
					err_out.add(calculate_line_from_position(global_start, extracted.key.start), 71,
						std::string("multiple defintion of \"to\" while parsing conversion defintion"));
				}
			} else {
				err_out.add(calculate_line_from_position(global_start, extracted.key.start), 72,
					std::string("unexpected token \"") + kstr + "\" while parsing conversion defintion");
			}
		}
	}
	return result;
}

std::vector<type_name_pair> parse_query_parameters_list(char const* &start, char const * end, char const * global_start, error_record & err) {
	std::vector<type_name_pair> result;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			
			if(extracted.values.size() == 0) {
				err.add(calculate_line_from_position(global_start, extracted.key.start), 73,
					std::string("missing type declaration"));
			} else if(extracted.values.size() > 1) {
				err.add(calculate_line_from_position(global_start, extracted.key.start), 74,
					std::string("too many type declarations"));
			} else {
				result.push_back(type_name_pair{ kstr, extracted.values[0].to_string() });
			}
		}
	}

	return result;
}

query_definition parse_query_definition(char const* &start, char const * end, char const * global_start, error_record & err) {
	query_definition result;
	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, global_start, err);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr = extracted.key.to_string();
			if(kstr == "name") {
				if(extracted.values.size() != 1) {
					err.add(calculate_line_from_position(global_start, extracted.key.start), 75,
						std::string("wrong number of parameters for \"name\""));
				} else if(result.name.length() == 0) {
					result.name = extracted.values[0].to_string();
				} else {
					err.add(calculate_line_from_position(global_start, extracted.key.start), 76,
						std::string("multiple defintion of \"name\" while parsing query defintion"));
				}
			} else if(kstr == "select") {
				if(extracted.values.size() != 1) {
					err.add(calculate_line_from_position(global_start, extracted.key.start), 77,
						std::string("wrong number of parameters for \"select\""));
				} else {
					result.line = calculate_line_from_position(global_start, extracted.values[0].start);
					result.select = parse_select_statement(extracted.values[0].start, extracted.values[0].end, global_start, err);
				}
			} else if(kstr == "parameters") {
				if(extracted.values.size() != 1) {
					err.add(calculate_line_from_position(global_start, extracted.key.start), 78,
						std::string("wrong number of parameters for \"parameters\""));
				} else {
					result.parameters = parse_query_parameters_list(extracted.values[0].start, extracted.values[0].end, global_start, err);
				}
			} else {
				err.add(calculate_line_from_position(global_start, extracted.key.start), 79,
					std::string("unexpected token \"") + kstr + "\" while parsing query defintion");
			}
		}
	}
	return result;
}

file_def parse_file(char const * start, char const * end, error_record & err_out) {
	file_def parsed_file;

	char const* pos = start;
	while(pos < end) {
		auto extracted = extract_item(pos, end, start, err_out);
		pos = extracted.terminal;

		if(extracted.key.start != extracted.key.end) {
			std::string kstr(extracted.key.start, extracted.key.end);
			if(kstr == "namespace") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 80,
						std::string("wrong number of parameters for \"namespace\""));
				} else {
					parsed_file.namspace = extracted.values[0].to_string();
				}
			} else if(kstr == "include") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 81,
						std::string("wrong number of parameters for \"include\""));
				} else {
					parsed_file.includes.push_back(extracted.values[0].to_string());
				}
			} else if(kstr == "convert") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 82,
						std::string("wrong number of parameters for \"convert\""));
				} else {
					parsed_file.conversion_list.push_back(
						parse_conversion_def(extracted.values[0].start, extracted.values[0].end, start, err_out));
				}
			} else if(kstr == "legacy_types") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 83,
						std::string("wrong number of parameters for \"legacy_types\""));
				} else {
					auto nlist = parse_legacy_types(extracted.values[0].start, extracted.values[0].end, start, err_out);
					for(auto& s : nlist) {
						parsed_file.legacy_types.push_back(s);
					}
				}
			} else if(kstr == "global") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 84,
						std::string("wrong number of parameters for \"global\""));
				} else {
					parsed_file.globals.push_back(extracted.values[0].to_string());
				}
			} else if(kstr == "make_index") {
				if(extracted.values.size() != 2) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 85,
						std::string("wrong number of parameters for \"make_index\""));
				} else {
					parsed_file.extra_ids.push_back(made_id{ extracted.values[0].to_string() , extracted.values[1].to_string() });
				}
			} else if(kstr == "load_save") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 86,
						std::string("wrong number of parameters for \"load_save\""));
				} else {
					parsed_file.load_save_routines.push_back(
						parse_load_save_def(extracted.values[0].start, extracted.values[0].end, start, err_out));
				}
			} else if(kstr == "object") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 87,
						std::string("wrong number of parameters for \"object\""));
				} else {
					parsed_file.relationship_objects.push_back(
						parse_object(extracted.values[0].start, extracted.values[0].end, start, err_out));
				}
			} else if(kstr == "relationship") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 88,
						std::string("wrong number of parameters for \"relationship\""));
				} else {
					parsed_file.relationship_objects.push_back(
						parse_relationship(extracted.values[0].start, extracted.values[0].end, start, err_out));
				}
			} else if(kstr == "query") {
				if(extracted.values.size() != 1) {
					err_out.add(calculate_line_from_position(start, extracted.key.start), 89,
						std::string("wrong number of parameters for \"query\""));
				} else {
					parsed_file.unprepared_queries.push_back(
						parse_query_definition(extracted.values[0].start, extracted.values[0].end, start, err_out));
				}
			} else {
				err_out.add(calculate_line_from_position(start, extracted.key.start), 90,
					std::string("unexpetected top level key: ") + kstr);
			}
		}
	}

	return parsed_file;
}

related_object const* find_link_by_name(std::string const& name,
	relationship_object_def const& obja, relationship_object_def const& objb) {

	for(auto& link : obja.indexed_objects) {
		if(link.property_name == name)
			return &link;
	}
	for(auto& link : objb.indexed_objects) {
		if(link.property_name == name)
			return &link;
	}
	return nullptr;
}

related_object const* find_common_link(relationship_object_def const& obja, relationship_object_def const& objb) {
	related_object const* found = nullptr;
	for(auto& links_in_a : obja.indexed_objects) {
		if(links_in_a.related_to == &objb) {
			if(!found) found = &links_in_a;
			else return nullptr;
		}
	}
	for(auto& links_in_b : objb.indexed_objects) {
		if(links_in_b.related_to == &obja) {
			if(!found) found = &links_in_b;
			else return nullptr;
		}
	}
	return found;
}

prepared_query_definition make_prepared_definition(file_def const& parsed_file, query_definition const& def, error_record & err) {
	prepared_query_definition result;

	result.parameters = def.parameters;
	result.name = def.name;

	std::vector<std::string> exposed_table_names;

	result.table_slots.resize(def.select.from.size());
	for(uint32_t i = 0; i < def.select.from.size(); ++i) {
		result.table_slots[i].is_join_plus = def.select.from[i].type == from_type::join_plus;
		result.table_slots[i].is_parameter_type = def.select.from[i].type == from_type::parameter;

		if(result.table_slots[i].is_parameter_type) {
			if(i != 0) {
				err.add(def.line, 91, "Only the first target in a from clause can be a parameter; use \"where\" to filter other targets");
				return result;
			}
			result.table_slots[i].internally_named_as = std::string("m_parameters.") + def.select.from[i].table_identifier.member_name;
			result.table_slots[i].reference_name = def.select.from[i].table_identifier.as_name;
			std::string param_id;
			for(auto &p : def.parameters) {
				if(p.name == def.select.from[i].table_identifier.member_name)
					param_id = p.type;
			}
			param_id.pop_back(); param_id.pop_back(); param_id.pop_back();
			result.table_slots[i].actual_table = find_by_name(parsed_file, param_id);
			if(!result.table_slots[i].actual_table) {
				err.add(def.line, 92, std::string("Could not find an object or relationship named ")
					+ param_id + " as required by usage of parameter "
					+ def.select.from[i].table_identifier.member_name + " in a from clause");
				return result;
			}
			auto parameter = std::find_if(def.parameters.begin(), def.parameters.end(),
				[&](type_name_pair const& p) { return def.select.from[i].table_identifier.member_name == p.name; });
			if(parameter != def.parameters.end()) {
				std::string derived_type = parameter->type.substr(0, parameter->type.length() - 3);
				result.table_slots[i].actual_table = find_by_name(parsed_file, derived_type);
				if(!result.table_slots[i].actual_table) {
					err.add(def.line, 93, std::string("Could not find an object or relationship named ")
						+ derived_type + " as required by usage of parameter "
						+ def.select.from[i].table_identifier.member_name +" in a from clause");
					return result;
				}
			} else {
				err.add(def.line, 94,
					std::string("Could not find query parameter refernced as ")
					+ def.select.from[i].table_identifier.member_name + " in from clause of a query in its the parameter list");
				return result;
			}
		} else {
			//first find base name
			result.table_slots[i].reference_name = def.select.from[i].table_identifier.as_name;
			if(std::find(exposed_table_names.begin(), exposed_table_names.end(), def.select.from[i].table_identifier.as_name) == exposed_table_names.end()) {
				result.table_slots[i].expose_has_name = true;
				exposed_table_names.push_back(def.select.from[i].table_identifier.as_name);
			}
			result.table_slots[i].internally_named_as = std::string("m_tableindex") + std::to_string(i);
			result.table_slots[i].actual_table = find_by_name(parsed_file, def.select.from[i].table_identifier.member_name);
			if(!result.table_slots[i].actual_table) {
				err.add(def.line, 95,
					std::string("Could not find an object or relationship named ")
					+ def.select.from[i].table_identifier.member_name + " referenced in a from clause");
				return result;
			}
			
			if(def.select.from[i].left_of_join == "") {
				if(i != 0) {
					result.table_slots[i].joined_to = &(result.table_slots[i - 1]);
				}
			} else {
				for(uint32_t j = 0; j < i; ++j) {
					if(result.table_slots[j].reference_name == def.select.from[i].left_of_join) {
						result.table_slots[i].joined_to = &(result.table_slots[j]);
					}
				}
				if(!result.table_slots[i].joined_to) {
					err.add(def.line, 96,
						std::string("Could not find item in from cause referenced by ")
						+ def.select.from[i].left_of_join + " as the target of a join");
					return result;
				}
			}


			if(result.table_slots[i].joined_to) {
				if(def.select.from[i].join_on == "") {
					result.table_slots[i].joind_by_link = find_common_link(*(result.table_slots[i].joined_to->actual_table),
						*(result.table_slots[i].actual_table));
					if(!result.table_slots[i].joind_by_link) {
						err.add(def.line, 97,
							std::string("Could not automatically pick a link between ")
							+ result.table_slots[i].joined_to->actual_table->name
							+ " and " + result.table_slots[i].actual_table->name + " to join on");
						return result;
					}
				} else {
					result.table_slots[i].joind_by_link = find_link_by_name(def.select.from[i].join_on, *(result.table_slots[i].joined_to->actual_table),
						*(result.table_slots[i].actual_table));
					if(!result.table_slots[i].joind_by_link) {
						err.add(def.line, 98,
							std::string("Could not find a link named ")
							+ def.select.from[i].join_on + " between " + result.table_slots[i].joined_to->actual_table->name
							+ " and " + result.table_slots[i].actual_table->name);
						return result;
					}
				}
			}
		}
	} // end loop over from items

	bool any_is_min_max = false;
	for(auto& val : def.select.returned_values) {
		if(val.aggregate_name == "min" || val.aggregate_name == "max" ||
			val.aggregate_name == "std::min" || val.aggregate_name == "std::max") {
			any_is_min_max = true;
		}
	}
	if(def.select.group_by.length() > 0) {
		for(auto& s : result.table_slots) {
			if(s.reference_name == def.select.group_by) {
				result.has_group = true;
				s.is_group_slot = true;
			}
		}
	}

	for(auto& val : def.select.returned_values) {
		query_table_slot const* found = nullptr;
		bool after_group_slot = false;
		for(auto& s : result.table_slots) {
			if(s.reference_name == val.property.type_name) {
				found = &s;
				break;
			}
			if(val.property.type_name == "") {
				if(val.property.member_name == "id") {
					found = &s;
					break;
				}
				if(std::find_if(s.actual_table->properties.begin(), s.actual_table->properties.end(), 
					[&](property_def const& prop) { return prop.name == val.property.member_name; }) != s.actual_table->properties.end()) {
					found = &s;
					break;
				}
				if(std::find_if(s.actual_table->indexed_objects.begin(), s.actual_table->indexed_objects.end(),
					[&](related_object const& prop) { return prop.property_name == val.property.member_name; }) != s.actual_table->indexed_objects.end()) {
					found = &s;
					break;
				}
			}
			if(s.is_group_slot)
				after_group_slot = true;
		}
		
		if(!found) {
			if(val.property.type_name != "") {
				err.add(def.line, 99,
					std::string("From target named ")
					+ val.property.type_name
					+ " could not be found for " + val.property.as_name);
				return result;
			} else {
				err.add(def.line, 100,
					std::string("From target with property named ")
					+ val.property.member_name
					+ " could not be found for " + val.property.as_name);
				return result;
			}
		} else {
			property_def const* referred_property = nullptr;
			related_object const* referred_link = nullptr;

			for(auto& prop : found->actual_table->properties) {
				if(prop.name == val.property.member_name) {
					referred_property = &prop;
				}
			}
			for(auto& link : found->actual_table->indexed_objects) {
				if(link.property_name == val.property.member_name) {
					referred_link = &link;
				}
			}
			if(!referred_property && !referred_link && val.property.member_name != "id") {
				err.add(def.line, 101,
					std::string("No property named ")
					+ val.property.member_name
					+ " could be found in target " + val.property.type_name);
				return result;
			} else {
				if(!val.is_aggregate) {
					if(any_is_min_max && after_group_slot) {
						result.exposed_min_max_terms.push_back(perpared_selection_item{
							val.property.as_name,
							referred_property,
							referred_link,
							found
							});
					} else {
						result.exposed_values.push_back(perpared_selection_item{
							val.property.as_name,
							referred_property,
							referred_link,
							found
							});
					}
				} else {
					result.exposed_aggregates.push_back(perpared_aggregate_item{
						val.aggregate_name,
						val.property.as_name,
						referred_property,
						referred_link,
						found
						});
				}
			}
		}
	} // end loop over returned values

	char const* where_start = def.select.where_clause.data();
	char const* where_end = where_start + def.select.where_clause.length();

	while(where_start < where_end) {
		char const* next_at = advance_to_at(where_start, where_end);
		result.where_conditional += std::string(where_start, next_at);
		if(next_at == where_end)
			break;

		char const* at_end = advance_to_identifier_end(next_at + 1, where_end);
		if(at_end < where_end && *at_end == '.') {
			at_end = advance_to_identifier_end(at_end + 1, where_end);
		}

		auto at_value = parse_type_and_name(next_at + 1, at_end);

		bool found = false;
		if(at_value.type.length() > 0) {
			for(auto& slot : result.table_slots) {
				if(slot.reference_name == at_value.type) {
					if(at_value.name == "id") {
						result.where_conditional += slot.internally_named_as;
						found = true;
					}
					for(auto& prop : slot.actual_table->properties) {
						if(prop.name == at_value.name && !found) {
							if(prop.type == property_type::array_bitfield || prop.type == property_type::array_other
								|| prop.type == property_type::array_vectorizable) {

								if(at_end < where_end && *at_end == '[') {
									char const* bracket_end = advance_to_closing_square_bracket(at_end + 1, where_end);

									result.where_conditional += "m_container."
										+ slot.actual_table->name + "_get_"
										+ prop.name + "(" + slot.internally_named_as + ", "
										+ std::string(at_end + 1, bracket_end) + ")";
									at_end = bracket_end + 1;
								} else {
									err.add(def.line, 102, 
										std::string("variable of array type missing an indexing expression: ")
										+ std::string(next_at, at_end));
									return result;
								}

							} else {
								result.where_conditional += "m_container."
									+ slot.actual_table->name + "_get_"
									+ prop.name
									+ "(" + slot.internally_named_as + ")";
							}
							found = true;
						}
					}
					for(auto& prop : slot.actual_table->indexed_objects) {
						if(prop.property_name == at_value.name && !found) {
							result.where_conditional += "m_container."
								+ slot.actual_table->name + "_get_"
								+ prop.property_name
								+ "(" + slot.internally_named_as + ")";
							found = true;
						}
					}
				}
			}
		} else {
			for(auto& param : result.parameters) {
				if(param.name == at_value.name && !found) {
					result.where_conditional += "m_parameters." + at_value.name;
					found = true;
				}
			}
			for(auto& val : result.exposed_values) {
				if(val.exposed_name == at_value.name && !found) {

					if(val.from_property && val.from_property->type == property_type::array_bitfield || val.from_property->type == property_type::array_other
						|| val.from_property->type == property_type::array_vectorizable) {

						if(at_end < where_end && *at_end == '[') {
							char const* bracket_end = advance_to_closing_square_bracket(at_end + 1, where_end);

							result.where_conditional += "m_container."
								+ val.derived_from_slot->actual_table->name + "_get_"
								+ val.from_property->name
								+ "(" + val.derived_from_slot->internally_named_as + ", " 
								+ std::string(at_end + 1, bracket_end) + ")";
							at_end = bracket_end + 1;
						} else {
							err.add(def.line, 103,
								std::string("variable of array type missing an indexing expression: ")
								+ at_value.name);
							return result;
						}

					} else {
						result.where_conditional += "m_container."
							+ val.derived_from_slot->actual_table->name + "_get_"
							+ (val.from_link ? val.from_link->property_name : (val.from_property ? val.from_property->name : std::string("")))
							+ "(" + val.derived_from_slot->internally_named_as + ")";
						found = true;
					}
				}
			}
		}

		if(!found) {
			err.add(def.line, 104, 
				std::string("No replacement found for ")
				+ std::string(next_at, at_end));
			return result;
		}

		where_start = at_end;
	}

	return result;
}
