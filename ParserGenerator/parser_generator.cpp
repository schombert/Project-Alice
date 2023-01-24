#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

struct row_col_pair {
	int32_t row;
	int32_t column;
};

struct error_record {
	std::string accumulated;
	std::string file_name;

	error_record(std::string const& fn) : file_name(fn) {
	}

	void add(row_col_pair const& rc, int32_t code, std::string const& s) {
		//<origin>[(position)]: [category] <kind> <code>: <description>

		accumulated += file_name;
		if(rc.row > 0) {
			if(rc.column > 0) {
				accumulated += "(" + std::to_string(rc.row) + "," + std::to_string(rc.column) + ")";
			} else {
				accumulated += "(" + std::to_string(rc.row) + ")";
			}
		} else {

		}
		accumulated += ": error ";
		accumulated += std::to_string(code);
		accumulated += ": ";
		accumulated += s;
		accumulated += "\n";
	}
};


std::string extract_string(char const* & input, char const* end) {
	while(input < end && (*input == ' ' || *input == '(' || *input == ',' || *input == '-' || *input == '\t' ||  *input == '\r' || *input == '\n'))
		++input;

	if(input >= end)
		return std::string();

	std::string result;
	bool in_quote = false;

	while(input < end && (in_quote || (*input != ' '  && *input != '(' && *input != ')' && *input != '\t' && *input != ',' && *input != '-' && *input != '\r' && *input != '\n'))) {
		if(*input == '\"')
			in_quote = !in_quote;
		else
			result += *input;
		++input;
	}

	while(input < end && (*input == ' ' || *input == ',' || *input == ')' || *input == '-' || *input == '\r' || *input == '\n'))
		++input;

	return result;
}

std::string extract_optional(char const* & input, char const* end) {
	while(input < end && (*input == ' ' || *input == ',' || *input == '-'  || *input == '\r' || *input == '\n'))
		++input;

	if(input >= end)
		return std::string();

	std::string result;
	bool in_quote = false;

	if(*input == '(') {
		++input;

		while(input < end && (in_quote || *input != ')')) {
			if(*input == '\"')
				in_quote = !in_quote;
			else
				result += *input;
			++input;
		}

		if(input < end)
			++input;
	}
	
	while(input < end && (*input == ' ' || *input == ',' || *input == '-' || *input == '\r' || *input == '\n'))
		++input;

	return result;
}

struct value_and_optional {
	std::string value;
	std::string opt;
};

struct value_association {
	std::string key;
	std::string type;
	value_and_optional handler;
};

struct group_association {
	std::string key;
	std::string type_or_function;
	value_and_optional handler;
	bool is_extern;
};

struct extern_group {
	std::string function;
};

struct group_contents {
	std::string group_object_type;

	std::string single_value_handler_type;
	value_and_optional single_value_handler_result;

	group_association set_handler;

	std::vector<group_association> groups;
	std::vector<value_association> values;

	value_association any_value_handler;
	group_association any_group_handler;
};

struct file_contents {
	std::vector<group_contents> groups;
};

const char* value_to_digit(uint32_t val) {
	switch(val) {
		case 0:  return "0";
		case 1:  return "1";
		case 2:  return "2";
		case 3:  return "3";
		case 4:  return "4";
		case 5:  return "5";
		case 6:  return "6";
		case 7:  return "7";
		case 8:  return "8";
		case 9:  return "9";
		case 10:  return "A";
		case 11:  return "B";
		case 12:  return "C";
		case 13:  return "D";
		case 14:  return "E";
		case 15:  return "F";
		default: return "X";
	}
}

std::string char_to_hex(char c) {
	uint32_t v = c | 0x20;
	if(v < 16) {
		return std::string("0") + value_to_digit(v);
	} else {
		return std::string("") + value_to_digit(v / 16) + value_to_digit(v % 16);
	}
}

std::string string_to_hex(std::string const& str, int32_t start, int32_t count) {
	std::string res = "0x";
	for(int32_t i = count - 1; i >= 0; --i) {
		res += char_to_hex(str[start + i]);
	}
	return res;
}

std::string final_match_condition_internal(std::string const& key, int32_t starting_position) {
	if(starting_position >= int32_t(key.length()))
		return "";

	if(key.length() - starting_position >= 8) {
		return
			" && (*(uint64_t*)(cur.content.data() + " + std::to_string(starting_position) + ") | uint64_t(0x2020202020202020) ) == uint64_t(" + string_to_hex(key, starting_position, 8) + ")"
			+ final_match_condition_internal(key, starting_position + 8);
	} else if(key.length() - starting_position >= 4) {
		return
			" && (*(uint32_t*)(cur.content.data() + " + std::to_string(starting_position) + ") | uint32_t(0x20202020) ) == uint32_t(" + string_to_hex(key, starting_position, 4) + ")"
			+ final_match_condition_internal(key, starting_position + 4);
	} else if(key.length() - starting_position >= 2) {
		return
			" && (*(uint16_t*)(cur.content.data() + " + std::to_string(starting_position) + ") | 0x2020 ) == " + string_to_hex(key, starting_position, 2)
			+ final_match_condition_internal(key, starting_position + 2);
	} else { /// single char
		return " && (*(cur.content.data() + " + std::to_string(starting_position) + ") | 0x20 ) == " + string_to_hex(key, starting_position, 1);
	}
}

std::string final_match_condition(std::string const& key, int32_t starting_position) {
	return std::string("(true") + final_match_condition_internal(key, starting_position) + ")";
}


template<typename T>
int32_t count_with_prefix(T const& vector, std::string const& prefix, int32_t length) {
	int32_t const sz = int32_t(vector.size());
	int32_t total = 0;
	for(int32_t i = 0; i < sz; ++i) {
		if(int32_t(vector[i].key.length()) == length) {
			int32_t psize = int32_t(prefix.length());
			bool match = true;
			for(int32_t j = 0; j < psize; ++j) {
				if(((vector[i].key[j]) | 0x20) != (prefix[j] | 0x20)) {
					match = false;
					break;
				}
			}

			if(match)
				++total;
		}
	}

	return total;
}

template<typename T, typename F>
void enum_with_prefix(T const& vector, std::string const& prefix, int32_t length, F const& fn) {
	int32_t const sz = int32_t(vector.size());
	for(int32_t i = 0; i < sz; ++i) {
		if(int32_t(vector[i].key.length()) == length) {
			int32_t psize = int32_t(prefix.length());
			bool match = true;
			for(int32_t j = 0; j < psize; ++j) {
				if(((vector[i].key[j]) | 0x20) != (prefix[j] | 0x20)) {
					match = false;
					break;
				}
			}

			if(match)
				fn(vector[i]);
		}
	}
}

template<typename T>
int32_t max_length(T const& vector) {
	int32_t const sz = int32_t(vector.size());
	int32_t mx = 0;
	for(int32_t i = 0; i < sz; ++i) {
		mx = mx > int32_t(vector[i].key.length()) ? mx : int32_t(vector[i].key.length());
	}
	return mx;
}


template<typename T, typename F>
std::string construct_match_tree_internal(T const& vector, F const& generator_match, std::string const& no_match, std::string const& prefix, int32_t length) {
	std::string output = "\t\t\t\t\t switch(0x20 | int32_t(*(cur.content.data() + " + std::to_string(prefix.length()) + "))) {\n";

	for(int32_t c = 32; c <= 95; ++c) {
		int32_t count = count_with_prefix(vector, prefix + char(c), length);

		if(count == 0) {
			//skip
		} else if(count == 1) {
			output += "\t\t\t\t\t case 0x" + char_to_hex(char(c)) + ":\n";
			enum_with_prefix(vector, prefix + char(c), length, [&output, &generator_match, &no_match, &prefix](auto& v) {
				output += "\t\t\t\t\t\t if(" + final_match_condition(v.key, int32_t(prefix.length()) + 1) + ") {\n";
				output += "\t\t\t\t\t\t\t " + generator_match(v) + "\n";
				output += "\t\t\t\t\t\t } else {\n";
				output += "\t\t\t\t\t\t\t " + no_match + "\n";
				output += "\t\t\t\t\t\t }\n";
			});
			output += "\t\t\t\t\t\t break;\n";
		} else {
			output += "\t\t\t\t\t case 0x" + char_to_hex(char(c)) + ":\n";
			output += construct_match_tree_internal(vector, generator_match, no_match, prefix + char(c), length);
			output += "\t\t\t\t\t\t break;\n";
		}
	}

	output += "\t\t\t\t\t default:\n";
	output += "\t\t\t\t\t\t " + no_match + "\n";
	output += "\t\t\t\t\t }\n";

	return output;
}

template<typename T, typename F>
std::string construct_match_tree_outer(T const& vector, F const& generator_match, std::string const& no_match) {
	auto const maxlen = max_length(vector);
	std::string output = "\t\t\t\t switch(int32_t(cur.content.length())) {\n";

	for(int32_t l = 1; l <= maxlen; ++l) {
		int32_t count = count_with_prefix(vector, "", l);

		if(count == 0) {
			// skip
		} else if(count == 1) {
			output += "\t\t\t\t case " + std::to_string(l) + ":\n";
			enum_with_prefix(vector, "", l, [&output, &generator_match, &no_match](auto& v) {
				output += "\t\t\t\t\t if(" + final_match_condition(v.key, 0) + ") {\n";
				output += "\t\t\t\t\t\t " + generator_match(v) + "\n";
				output += "\t\t\t\t\t } else {\n";
				output += "\t\t\t\t\t\t " + no_match + "\n";
				output += "\t\t\t\t\t }\n";
			});
			output += "\t\t\t\t\t break;\n";
		} else {
			output += "\t\t\t\t case " + std::to_string(l) + ":\n";
			output += construct_match_tree_internal(vector, generator_match, no_match, "", l);
			output += "\t\t\t\t\t break;\n";
		}
	}

	output += "\t\t\t\t default:\n";
	output += "\t\t\t\t\t " + no_match + "\n";
	output += "\t\t\t\t }\n";
	return output;
}

void clear_file(std::string const& file_name) {
	std::fstream fileout;
	fileout.open(file_name, std::ios::out);
	if(fileout.is_open()) {
		fileout << "";
		fileout.close();
	}
}

int main(int argc, char* argv[]) {
	if(argc > 1) {

		std::fstream input_file;
		std::string input_file_name(argv[1]);
		input_file.open(argv[1], std::ios::in);

		const std::string output_file_name = [otemp = std::string(argv[1])]() mutable {
			if(otemp.length() >= 4 && otemp[otemp.length() - 4] == '.') {
				otemp.pop_back();
				otemp.pop_back();
				otemp.pop_back();
				otemp.pop_back();

				return otemp + "_generated.hpp";
			}
			return otemp + "_generated.hpp";
		}();

		error_record err(input_file_name);

		if(!input_file.is_open()) {
			err.add(row_col_pair{ 0,0 }, 1000, "Could not open input file");
			clear_file(output_file_name);
			std::cout << err.accumulated;
			return -1;
		}

		//	read file
		std::string file_contents_str((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		input_file.close();


		//	parse file
		char const* input = file_contents_str.c_str();
		char const* const file_end = file_contents_str.c_str() + file_contents_str.length();

		file_contents file;

		while(input < file_end) {
			file.groups.emplace_back();

			file.groups.back().group_object_type = extract_string(input, file_end);

			while(input < file_end && *input == '\t') {
				++input;

				std::string key = extract_string(input, file_end);

				if(key == "#free") {
					std::string type = extract_string(input, file_end);
					std::string opt = extract_string(input, file_end);

					std::string handler_type = extract_string(input, file_end);
					std::string handler_opt = extract_optional(input, file_end);

					if(type == "group") {
						file.groups.back().set_handler = group_association{ key, opt, value_and_optional{handler_type, handler_opt}, false };
					} else if(type == "extern") {
						file.groups.back().set_handler = group_association{ key, opt, value_and_optional{handler_type, handler_opt}, true };
					} else if(type == "value") {
						file.groups.back().single_value_handler_type = opt;
						file.groups.back().single_value_handler_result = value_and_optional{ handler_type, handler_opt };
					}

					// value1 == "extern" | "group" | "value"
					// value2 = set_handler.type_or_function = function to call on "gen" to process it (with extern == true, wont prepend parse)
					// value3 = set_handler.handler.value = discard | member | ...
					// (value4) = set_handler.handler.opt = name of non default member target / fn name to call w/ results
					

				} else if(key == "#base") {
					std::string base_t = extract_string(input, file_end);
					for(auto& g : file.groups) {
						if(g.group_object_type == base_t) {
							for(auto& v : g.groups)
								file.groups.back().groups.push_back(v);
							for(auto& v : g.values)
								file.groups.back().values.push_back(v);
							file.groups.back().any_group_handler = g.any_group_handler;
							file.groups.back().any_value_handler = g.any_value_handler;
							file.groups.back().single_value_handler_result = g.single_value_handler_result;
							file.groups.back().single_value_handler_type = g.single_value_handler_type;
							file.groups.back().any_value_handler = g.any_value_handler;
							file.groups.back().set_handler = g.set_handler;
						}
					}
				} else {
					std::string type = extract_string(input, file_end);
					std::string opt = extract_string(input, file_end);

					std::string handler_type = extract_string(input, file_end);
					std::string handler_opt = extract_optional(input, file_end);

					if(key == "#any") {
						if(type == "parser" || type == "group") {
							file.groups.back().any_group_handler = group_association{ "", opt, value_and_optional{handler_type, handler_opt}, false };
						} else if(type == "value") {
							file.groups.back().any_value_handler = value_association{ "", opt, value_and_optional{handler_type, handler_opt} };
						} else if(type == "extern") {
							file.groups.back().any_group_handler = group_association{ "", opt, value_and_optional{handler_type, handler_opt}, true };
						}
					} else {
						if(type == "parser" || type == "group") {
							if(std::find_if(file.groups.back().groups.begin(), file.groups.back().groups.end(), [&](group_association const& g) { return g.key == key; }) == file.groups.back().groups.end()) {
								file.groups.back().groups.push_back(group_association{ key, opt, value_and_optional{handler_type, handler_opt}, false });
							} else {
								err.add(row_col_pair{ 0,0 }, 10, std::string("group key ") + key + " was repeated in " + file.groups.back().group_object_type);
							}
						} else if(type == "value") {
							if(std::find_if(file.groups.back().values.begin(), file.groups.back().values.end(), [&](value_association const& g) { return g.key == key; }) == file.groups.back().values.end()) {
								file.groups.back().values.push_back(value_association{ key, opt, value_and_optional{handler_type, handler_opt} });
							} else {
								err.add(row_col_pair{ 0,0 }, 11, std::string("group key ") + key + " was repeated in " + file.groups.back().group_object_type);
							}
						} else if(type == "extern") {
							if(std::find_if(file.groups.back().groups.begin(), file.groups.back().groups.end(), [&](group_association const& g) { return g.key == key; }) == file.groups.back().groups.end()) {
								file.groups.back().groups.push_back(group_association{ key, opt, value_and_optional{handler_type, handler_opt}, true });
							} else {
								err.add(row_col_pair{ 0,0 }, 12, std::string("group key ") + key + " was repeated in " + file.groups.back().group_object_type);
							}
						}

						/*
						groups:
						
						key: type, opt, handler_type (handler_opt)

						type = (group, extern -> groups, value -> values) 

						key = association.key
						opt = association.type_or_function
						handler_type = handler.value
						handler_opt = handler.opt

						values:

						key: type, opt, handler_type (handler_opt)

						type = (group, extern -> groups, value -> values)

						key = association.key
						opt = association.type
						handler_type = handler.value
						handler_opt = handler.opt
						*/
					}
				}
			}
		}

		//	encountered errors? if so, exit
		//  TODO currently the parser isn't sophisticated enough to report errors

		if(err.accumulated.length() > 0) {
			clear_file(output_file_name);
			std::cout << err.accumulated;
			return -1;
		}

		//	process the parsed content into the generated file

		std::string output;


		output += "#include \"parsers.hpp\"\n";
		output += "\n";

		// output += "#pragma warning( push )\n";
		// output += "#pragma warning( disable : 4065 )\n";
		// output += "#pragma warning( disable : 4189 )\n";

		output += "\n";

		output += "namespace parsers {\n";

		// declare fns

		for(auto& g : file.groups) {
			output += "template<typename C>\n";
			output += g.group_object_type + " parse_" + g.group_object_type + "(token_generator& gen, error_handler& err, C&& context);\n";
		}
		output += "\n";


		// fn bodies
		for(auto& g : file.groups) {
			output += "template<typename C>\n";
			output += g.group_object_type + " parse_" + g.group_object_type + "(token_generator& gen, error_handler& err, C&& context) {\n";
			output += "\t " + g.group_object_type + " cobj;\n";
			output += "\t for(token_and_type cur = gen.get(); cur.type != token_type::unknown && cur.type != token_type::close_brace; cur = gen.get()) {\n";

			// case: free group
			output += "\t\t if(cur.type == token_type::open_brace) { \n";
			{
				// set_handler.handler.value = discard | member | ...
				// set_handler.handler.opt = name of non default member target / fn name to call w/ results
				// set_handler.type_or_function = function to call on "gen" to process it (with extern == true, wont prepend parse)
				std::string set_effect;
				if(g.set_handler.is_extern == false) {
					if(g.set_handler.handler.value == "discard") {
						set_effect = "gen.discard_group();";
					} else if(g.set_handler.handler.value == "member") {
						set_effect = "cobj." +
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							" = parse_" + g.set_handler.type_or_function + "(gen, err, context);";
					} else if(g.set_handler.handler.value == "member_fn") {
						set_effect = "cobj." +
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							"(parse_" + g.set_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else if(g.set_handler.handler.value == "function") {
						set_effect =
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							"(cobj, parse_" + g.set_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else {
						set_effect = "err.unhandled_free_group(cur); gen.discard_group();";
					}
				} else {
					if(g.set_handler.handler.value == "discard") {
						set_effect = "gen.discard_group();";
					} else if(g.set_handler.handler.value == "member") {
						set_effect = "cobj." +
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							" = " + g.set_handler.type_or_function + "(gen, err, context);";
					} else if(g.set_handler.handler.value == "member_fn") {
						set_effect = "cobj." +
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							"(" + g.set_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else if(g.set_handler.handler.value == "function") {
						set_effect =
							(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_group")) +
							"(cobj, " + g.set_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else {
						set_effect = "err.unhandled_free_group(cur); gen.discard_group();";
					}
				}
				output += "\t\t\t " + set_effect + "\n";
				output += "\t\t\t continue;\n";
				output += "\t\t }\n";
			}

			output += "\t\t auto peek_result = gen.next();\n";
			output += "\t\t if(peek_result.type == token_type::special_identifier) {\n"; // start next token if
			output += "\t\t\t auto peek2_result = gen.next_next();\n";
			output += "\t\t\t if(peek2_result.type == token_type::open_brace) {\n";

			// match groups

			output += "\t\t\t\t gen.get(); gen.get();\n";

			{
				/*
					#any: type, opt, handler_type (handler_opt)

					type = (parser, extern -> groups, value -> values)

					key = association.key
					opt = association.type_or_function
					handler_type = handler.value
					handler_opt = handler.opt
				*/

				std::string no_match_effect;
				if(g.any_group_handler.is_extern == false) {
					if(g.any_group_handler.handler.value == "discard") {
						no_match_effect = "gen.discard_group();";
					} else if(g.any_group_handler.handler.value == "member") {
						no_match_effect = "cobj." +
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							" = parse_" + g.any_group_handler.type_or_function + "(gen, err, context);";
					} else if(g.any_group_handler.handler.value == "member_fn") {
						no_match_effect = "cobj." +
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							"(cur.content, parse_" + g.any_group_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else if(g.any_group_handler.handler.value == "function") {
						no_match_effect =
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							"(cobj, cur.content, parse_" + g.any_group_handler.type_or_function + "(gen, err, context), err, cur.line, context);";
					} else {
						no_match_effect = "err.unhandled_group_key(cur); gen.discard_group();";
					}
				} else {
					if(g.any_group_handler.handler.value == "discard") {
						no_match_effect = g.any_group_handler.type_or_function + "(cur.content, gen, err, context);";
					} else if(g.any_group_handler.handler.value == "member") {
						no_match_effect = "cobj." +
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							" = " + g.any_group_handler.type_or_function + "(cur.content, gen, err, context);";
					} else if(g.any_group_handler.handler.value == "member_fn") {
						no_match_effect = "cobj." +
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							"(cur.content, " + g.any_group_handler.type_or_function + "(cur.content, gen, err, context), err, cur.line, context);";
					} else if(g.any_group_handler.handler.value == "function") {
						no_match_effect =
							(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any_group")) +
							"(cobj, cur.content, " + g.any_group_handler.type_or_function + "(cur.content, gen, err, context), err, cur.line, context);";
					} else {
						no_match_effect = "err.unhandled_group_key(cur); gen.discard_group();";
					}
				}

				/*
					key: type, opt, handler_type (handler_opt)

					type = (group, extern -> groups, value -> values)

					key = association.key
					opt = association.type_or_function
					handler_type = handler.value
					handler_opt = handler.opt
				*/

				auto match_handler = [](group_association const& v) {
					std::string out;
					if(v.is_extern) {
						if(v.handler.value == "discard") {
							out = v.type_or_function + "(gen, err, context);";
						} else if(v.handler.value == "member") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								" = " + v.type_or_function + "(gen, err, context);";
						} else if(v.handler.value == "member_fn") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(" + v.type_or_function + "(gen, err, context), err, cur.line, context);";
						} else if(v.handler.value == "function") {
							out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(cobj, " + v.type_or_function + "(gen, err, context), err, cur.line, context);";
						} else {
							out = "err.unhandled_group_key(cur);";
						}
					} else {
						if(v.handler.value == "discard") {
							out = "gen.discard_group();";
						} else if(v.handler.value == "member") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								" = parse_" + v.type_or_function + "(gen, err, context);";
						} else if(v.handler.value == "member_fn") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(parse_" + v.type_or_function + "(gen, err, context), err, cur.line, context);";
						} else if(v.handler.value == "function") {
							out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(cobj, parse_" + v.type_or_function + "(gen, err, context), err, cur.line, context);";
						} else {
							out = "err.unhandled_group_key(cur);";
						}
					}
					return out;
				};

				output += construct_match_tree_outer(g.groups, match_handler, no_match_effect);
			}

			output += "\t\t\t } else {\n"; // next next != open brace

			output += "\t\t\t\t auto const assoc_token = gen.get();\n";
			output += "\t\t\t\t auto const assoc_type = parse_association_type(assoc_token.content, assoc_token.line, err);\n";
			output += "\t\t\t\t auto const rh_token = gen.get();\n";

			// match values

			{
				std::string no_match_effect;
				if(g.any_value_handler.handler.value == "discard") {
				} else if(g.any_value_handler.handler.value == "member") {
					no_match_effect = "cobj." +
						(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any_value")) +
						" = parse_" + g.any_value_handler.type + "(rh_token.content, rh_token.line, err);";
				} else if(g.any_value_handler.handler.value == "member_fn") {
					no_match_effect = "cobj." +
						(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any_value")) +
						"(cur.content, assoc_type, parse_" + g.any_value_handler.type + "(rh_token.content, rh_token.line, err), err, cur.line, context);";
				} else if(g.any_value_handler.handler.value == "function") {
					no_match_effect =
						(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any_value")) +
						"(cobj, cur.content, assoc_type, parse_" + g.any_value_handler.type + "(rh_token.content, rh_token.line, err), err, cur.line, context);";
				} else {
					no_match_effect = "err.unhandled_association_key(cur);";
				}

				/*
					key: type, opt, handler_type (handler_opt)

					type = (group, extern -> groups, value -> values)

					key = association.key
					opt = association.type
					handler_type = handler.value
					handler_opt = handler.opt
				*/

				auto match_handler = [](value_association const& v) {
					std::string out;
					if(v.handler.value == "discard") {

					} else if(v.handler.value == "member") {
						out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
							" = parse_" + v.type + "(rh_token.content, rh_token.line, err);";
					} else if(v.handler.value == "member_fn") {
						out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
							"(assoc_type, parse_" + v.type + "(rh_token.content, rh_token.line, err), err, cur.line, context);";
					} else if(v.handler.value == "function") {
						out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
							"(cobj, assoc_type, parse_" + v.type + "(rh_token.content, rh_token.line, err), err, cur.line, context);";
					} else {
						out = "err.unhandled_association_key(cur);";
					}
					return out;
				};

				output += construct_match_tree_outer(g.values, match_handler, no_match_effect);
			}

			output += "\t\t\t }\n"; // end next next

			output += "\t\t } else {\n"; // next != special identifier

			// case: free value;
			if(g.single_value_handler_result.value.length() > 0) {
				if(g.single_value_handler_result.value == "discard") {
					// do nothing
				} else if(g.single_value_handler_result.value == "member") {
					output += "\t\t\t cobj." +
						(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("free_value")) +
						"= parse_" + g.single_value_handler_type + "(cur.content, cur.line, err);\n";
				} else if(g.single_value_handler_result.value == "member_fn") {
					output += "\t\t\t cobj." +
						(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("free_value")) +
						"(parse_" + g.single_value_handler_type + "(cur.content, cur.line, err), err, cur.line, context);\n";
				} else if(g.single_value_handler_result.value == "function") {
					output += "\t\t\t " +
						(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("free_value")) +
						"(cobj, parse_" + g.single_value_handler_type + "(cur.content, cur.line, err), err, cur.line, context);\n";
				}
			} else {
				output += "\t\t\t err.unhandled_free_value(cur);\n"; // end next token if
			}

			output += "\t\t }\n"; // end next token if

			output += "\t }\n"; // end token loop
			output += "\t cobj.finish(context);\n";
			output += "\t return cobj;\n";
			output += "}\n"; // end fn
		}

		output += "}\n"; // end namespace

		// output += "#pragma warning( pop )\n";

		//newline at end of file
		output += "\n";

		std::fstream fileout;
		fileout.open(output_file_name, std::ios::out);
		if(fileout.is_open()) {
			fileout << output;
			fileout.close();
		} else {
			std::abort();
		}

	}
}

/*
//TREE CODE

switch([token length]) {
	case LENGTH N:
		switch(first_char | 0x20) {
			case X:
				more than one left: ... do switch again
			case Y:
				only one left: fast memcmp to correct handler OR any_match or ERROR_HANDLER
			default:
				any_match or ERROR_HANDLER
		}
	default:
		any_match or ERROR_HANDLER
}

template<typename ERROR_HANDLER, typename context = ...>
obj_type parse_#typename#(token_generator& gen, ERROR_HANDLER& err, context&& = ...) {
	obj_type cobj;

	for(token_and_type cur = gen.get(); cur.type != token_type::unknown && cur.type != token_type::close_brace; cur = gen.get()) {
		auto peek_result = gen.next();
		if(peek_result.type == token_type::special_identifier) {
			auto peek2_result = gen.next_next();
			if(peek2_result.type == token_type::open_brace) { // group type
				gen.get();
				gen.get();

				//TREE CODE TO FIND MATCH
				-- not extern --
				OR cobj.member = parse_fn(gen, ERROR_HANDLER, context);
				OR cobj.member_fn(#cur token if any#, parse_fn(gen, ERROR_HANDLER, context), context);
				OR ffunction(cobj, #cur token if any#, parse_fn(gen, ERROR_HANDLER, context), context);
				OR discard -- call discard fn
				-- extern --
				OR cobj.member = extern_fn((#cur token if any#, gen, ERROR_HANDLER, context);
				OR cobj.member_fn(#cur token if any#, extern_fn((#cur token if any#, gen, ERROR_HANDLER, context), context);
				OR ffunction(cobj, #cur token if any#, extern_fn((#cur token if any#, gen, ERROR_HANDLER, context), context);
				OR discard -- extern_fn((#cur token if any#, gen, ERROR_HANDLER, context) -- and do nothing
			} else {
				auto const assoc_type = process_assoc(gen.get());
				auto rh_token = gen.get();

				//TREE CODE TO FIND MATCH
				OR cobj.member = process_token_fn(ERROR_HANDLER, rh_token);
				OR cobj.member_fn(#cur token if any#, assoc, process_token_fn(ERROR_HANDLER, rh_token), context);
				OR ffunction(cobj, #cur token if any#, assoc, process_token_fn(ERROR_HANDLER, rh_token), context);
				OR discard -- discard do nothing
			}
		} else {
			OR cobj.member = process_token_fn(ERROR_HANDLER,cur);
			OR cobj.member_fn(process_token_fn(ERROR_HANDLER,cur), context);
			OR ffunction(cobj, process_token_fn(ERROR_HANDLER,cur), context);
			OR ... do nothing (discard)
		}
	}

	return cobj;
}

*/


