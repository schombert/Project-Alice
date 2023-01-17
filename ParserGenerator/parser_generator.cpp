#include "pch.h"
#include <Windows.h>
#include <string>
#include <vector>

#undef max
#undef min

std::string extract_string(char const* & input, char const* end) {
	while(input < end && (*input == ' ' || *input == '(' || *input == ',' || *input == ':' || *input == '-' || *input == '>' || *input == '\r' || *input == '\n'))
		++input;

	if(input >= end)
		return std::string();

	std::string result;
	bool in_quote = false;

	while(input < end && (in_quote || (*input != ' '  && *input != '(' && *input != ')' && *input != '\t' && *input != ',' && *input != '-' && *input != '>' && *input != ':' && *input != '\r' && *input != '\n'))) {
		if(*input == '\"')
			in_quote = !in_quote;
		else
			result += *input;
		++input;
	}

	while(input < end && (*input == ' ' || *input == ',' || *input == ')' || *input == ':' || *input == '-' || *input == '>' || *input == '\r' || *input == '\n'))
		++input;

	return result;
}

std::string extract_optional(char const* & input, char const* end) {
	while(input < end && (*input == ' ' || *input == ',' || *input == ':' || *input == '-' || *input == '>' || *input == '\r' || *input == '\n'))
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
	
	while(input < end && (*input == ' ' || *input == ',' || *input == ':' || *input == '-' || *input == '>' || *input == '\r' || *input == '\n'))
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
	std::string namespace_name;

	std::vector<group_contents> groups;
};

std::string type_string_to_type(std::string const& t) {
	if(t.length() == 0)
		return "token_and_type";
	if(t == "token")
		return "token_and_type";
	if(t == "date")
		return "date_tag";
	return t;
}

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
	if(starting_position >= key.length())
		return "";

	if(key.length() - starting_position >= 8) {
		return
			" && (*(uint64_t*)(cur.start + " + std::to_string(starting_position) + ") | 0x2020202020202020ui64) == " + string_to_hex(key, starting_position, 8) + "ui64"
			+ final_match_condition_internal(key, starting_position + 8);
	} else if(key.length() - starting_position >= 4) {
		return
			" && (*(uint32_t*)(cur.start + " + std::to_string(starting_position) + ") | 0x20202020ui32) == " + string_to_hex(key, starting_position, 4) + "ui32"
			+ final_match_condition_internal(key, starting_position + 4);
	} else if(key.length() - starting_position >= 2) {
		return
			" && (*(uint16_t*)(cur.start + " + std::to_string(starting_position) + ") | 0x2020) == " + string_to_hex(key, starting_position, 2)
			+ final_match_condition_internal(key, starting_position + 2);
	} else { /// single char
		return " && (*(cur.start + " + std::to_string(starting_position) + ") | 0x20) == " + string_to_hex(key, starting_position, 1);
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
		if(vector[i].key.length() == length) {
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
		if(vector[i].key.length() == length) {
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
	std::string output = "\t\t\t\t\t switch(0x20 | int32_t(*(cur.start + " + std::to_string(prefix.length()) + "))) {\r\n";

	for(int32_t c = 32; c <= 95; ++c) {
		int32_t count = count_with_prefix(vector, prefix + char(c), length);

		if(count == 0) {
			//skip
		} else if(count == 1) {
			output += "\t\t\t\t\t case 0x" + char_to_hex(char(c)) + ":\r\n";
			enum_with_prefix(vector, prefix + char(c), length, [&output, &generator_match, &no_match, &prefix](auto& v) {
				output += "\t\t\t\t\t\t if(" + final_match_condition(v.key, int32_t(prefix.length()) + 1) + ") {\r\n";
				output += "\t\t\t\t\t\t\t " + generator_match(v) + "\r\n";
				output += "\t\t\t\t\t\t } else {\r\n";
				output += "\t\t\t\t\t\t\t " + no_match + "\r\n";
				output += "\t\t\t\t\t\t }\r\n";
			});
			output += "\t\t\t\t\t\t break;\r\n";
		} else {
			output += "\t\t\t\t\t case 0x" + char_to_hex(char(c)) + ":\r\n";
			output += construct_match_tree_internal(vector, generator_match, no_match, prefix + char(c), length);
			output += "\t\t\t\t\t\t break;\r\n";
		}
	}

	output += "\t\t\t\t\t default:\r\n";
	output += "\t\t\t\t\t\t " + no_match + "\r\n";
	output += "\t\t\t\t\t }\r\n";

	return output;
}

template<typename T, typename F>
std::string construct_match_tree_outer(T const& vector, F const& generator_match, std::string const& no_match) {
	auto const maxlen = max_length(vector);
	std::string output = "\t\t\t\t switch(int32_t(cur.end - cur.start)) {\r\n";

	for(int32_t l = 1; l <= maxlen; ++l) {
		int32_t count = count_with_prefix(vector, "", l);

		if(count == 0) {
			// skip
		} else if(count == 1) {
			output += "\t\t\t\t case " + std::to_string(l) + ":\r\n";
			enum_with_prefix(vector, "", l, [&output, &generator_match, &no_match](auto& v) {
				output += "\t\t\t\t\t if(" + final_match_condition(v.key, 0) + ") {\r\n";
				output += "\t\t\t\t\t\t " + generator_match(v) + "\r\n";
				output += "\t\t\t\t\t } else {\r\n";
				output += "\t\t\t\t\t\t " + no_match + "\r\n";
				output += "\t\t\t\t\t }\r\n";
			});
			output += "\t\t\t\t\t break;\r\n";
		} else {
			output += "\t\t\t\t case " + std::to_string(l) + ":\r\n";
			output += construct_match_tree_internal(vector, generator_match, no_match, "", l);
			output += "\t\t\t\t\t break;\r\n";
		}
	}

	output += "\t\t\t\t default:\r\n";
	output += "\t\t\t\t\t " + no_match + "\r\n";
	output += "\t\t\t\t }\r\n";
	return output;
}

int wmain(int argc, wchar_t *argv[]) {
	for(int32_t i = 1; i < argc; ++i) {

		if(lstrlen(argv[i]) <= 0)
			continue;
	
		const auto handle = CreateFile(argv[i], GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if(handle != INVALID_HANDLE_VALUE) {

			LARGE_INTEGER file_size;
			GetFileSizeEx(handle, &file_size);
			auto const sz = (size_t)(file_size.QuadPart);

			char* buffer = new char[sz];
			DWORD read;
			ReadFile(handle, buffer, (DWORD)sz, &read, nullptr);

			char const* input = buffer;

			// namespace_name = extract_string(input, buffer + sz);
			
			file_contents file;

			file.namespace_name = extract_string(input, buffer + sz);

			while(input < buffer + sz) {
				file.groups.emplace_back();

				file.groups.back().group_object_type = extract_string(input, buffer + sz);

				while(input < buffer + sz  && *input == '\t') {
					++input;

					std::string key = extract_string(input, buffer + sz);

					if(key == "#value") {
						std::string value_type = extract_optional(input, buffer + sz);

						std::string handler_type = extract_string(input, buffer + sz);
						std::string handler_opt = extract_optional(input, buffer + sz);

						file.groups.back().single_value_handler_type = value_type;
						file.groups.back().single_value_handler_result = value_and_optional{ handler_type, handler_opt };
					} else if(key == "#set") {
						std::string type = extract_string(input, buffer + sz);
						std::string opt = extract_optional(input, buffer + sz);

						std::string handler_type = extract_string(input, buffer + sz);
						std::string handler_opt = extract_optional(input, buffer + sz);

						if(type == "group") {
							file.groups.back().set_handler = group_association{ key, opt, value_and_optional{handler_type, handler_opt}, false };
						} else { //if(type == "extern") {
							file.groups.back().set_handler = group_association{ key, opt, value_and_optional{handler_type, handler_opt}, true };
						}
					} else if(key == "#base") {
						std::string base_t = extract_string(input, buffer + sz);
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
						std::string type = extract_string(input, buffer + sz);
						std::string opt = extract_optional(input, buffer + sz);

						std::string handler_type = extract_string(input, buffer + sz);
						std::string handler_opt = extract_optional(input, buffer + sz);

						if(key == "#any") {
							if(type == "group") {
								file.groups.back().any_group_handler = group_association{ "", opt, value_and_optional{handler_type, handler_opt}, false};
							} else if(type == "value") {
								file.groups.back().any_value_handler = value_association{ "", opt, value_and_optional{handler_type, handler_opt} };
							} else if(type == "extern") {
								file.groups.back().any_group_handler = group_association{ "", opt, value_and_optional{handler_type, handler_opt}, true };
							}
						} else {
							if(type == "group") {
								file.groups.back().groups.push_back(group_association{ key, opt, value_and_optional{handler_type, handler_opt}, false });
							} else if(type == "value") {
								file.groups.back().values.push_back(value_association{ key, opt, value_and_optional{handler_type, handler_opt} });
							} else if(type == "extern") {
								file.groups.back().groups.push_back(group_association{ key, opt, value_and_optional{handler_type, handler_opt}, true });
							}
						}
					}
				}
			}

			delete[] buffer;

			CloseHandle(handle);

			// make output
			std::string output;


			output += "#include \"common\\\\common.h\"\r\n";
			output += "#include \"Parsers\\\\parsers.hpp\"\r\n";
			output += "\r\n";

			output += "#pragma warning( push )\r\n";
			output += "#pragma warning( disable : 4065 )\r\n";
			output += "#pragma warning( disable : 4189 )\r\n";
			output += "\r\n";

			output += "namespace ";
			output += file.namespace_name;
			output += " {\r\n";

			// declare fns

			for(auto& g : file.groups) {
				output += "template<typename ERR_H, typename C = int32_t>\r\n";
				output += g.group_object_type + " parse_" + g.group_object_type + "(token_generator& gen, ERR_H& err, C&& context = 0);\r\n";
			}
			output += "\r\n";


			// fn bodies
			for(auto& g : file.groups) {
				output += "template<typename ERR_H, typename C>\r\n";
				output += g.group_object_type + " parse_" + g.group_object_type + "(token_generator& gen, ERR_H& err, C&& context) {\r\n";
				output += "\t " + g.group_object_type + " cobj;\r\n";
				output += "\t for(token_and_type cur = gen.get(); cur.type != token_type::unknown && cur.type != token_type::close_brace; cur = gen.get()) {\r\n";

				output += "\t\t if(cur.type == token_type::open_brace) { \r\n";
				{
					std::string set_effect;
					if(g.set_handler.is_extern == false) {
						if(g.set_handler.handler.value == "discard") {
							set_effect = "discard_group(gen);";
						} else if(g.set_handler.handler.value == "member") {
							set_effect = "cobj." +
								(g.set_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("free_set")) +
								" = parse_" + g.set_handler.type_or_function + "(gen, err, context);";
						} else if(g.set_handler.handler.value == "member_fn") {
							set_effect = "cobj." +
								(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_set")) +
								"(parse_" + g.set_handler.type_or_function + "(gen, err, context), context);";
						} else if(g.set_handler.handler.value == "function") {
							set_effect =
								(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_set")) +
								"(cobj, parse_" + g.set_handler.type_or_function + "(gen, err, context), context);";
						} else {
							set_effect = "err.unhandled_free_set(); discard_group(gen);";
						}
					} else {
						if(g.set_handler.handler.value == "discard") {
							set_effect = g.set_handler.type_or_function + "(cur, gen, err, context);";
						} else if(g.set_handler.handler.value == "member") {
							set_effect = "cobj." +
								(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_set")) +
								" = " + g.set_handler.type_or_function + "(gen, err, context);";
						} else if(g.set_handler.handler.value == "member_fn") {
							set_effect = "cobj." +
								(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_set")) +
								"(" + g.set_handler.type_or_function + "(gen, err, context), context);";
						} else if(g.set_handler.handler.value == "function") {
							set_effect =
								(g.set_handler.handler.opt.length() > 0 ? g.set_handler.handler.opt : std::string("free_set")) +
								"(cobj, " + g.set_handler.type_or_function + "(gen, err, context), context);";
						} else {
							set_effect = "err.unhandled_free_set(); discard_group(gen);";
						}
					}
					output += "\t\t\t " + set_effect + "\r\n";
					output += "\t\t\t continue;\r\n";
					output += "\t\t }\r\n";
				}

				output += "\t\t auto peek_result = gen.next();\r\n";
				output += "\t\t if(peek_result.type == token_type::special_identifier) {\r\n"; // start next token if
				output += "\t\t\t auto peek2_result = gen.next_next();\r\n";
				output += "\t\t\t if(peek2_result.type == token_type::open_brace) {\r\n";

				// match groups

				output += "\t\t\t\t gen.get(); gen.get();\r\n";
				
				{
					std::string no_match_effect;
					if(g.any_group_handler.is_extern == false) {
						if(g.any_group_handler.handler.value == "discard") {
							no_match_effect = "discard_group(gen);";
						} else if(g.any_group_handler.handler.value == "member") {
							no_match_effect = "cobj." +
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								" = parse_" + g.any_group_handler.type_or_function + "(gen, err, context);";
						} else if(g.any_group_handler.handler.value == "member_fn") {
							no_match_effect = "cobj." +
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								"(cur, parse_" + g.any_group_handler.type_or_function + "(gen, err, context), err, context);";
						} else if(g.any_group_handler.handler.value == "function") {
							no_match_effect =
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								"(cobj, cur, parse_" + g.any_group_handler.type_or_function + "(gen, err, context), err, context);";
						} else {
							no_match_effect = "err.unhandled_group_key(cur); discard_group(gen);";
						}
					} else {
						if(g.any_group_handler.handler.value == "discard") {
							no_match_effect = g.any_group_handler.type_or_function + "(cur, gen, err, context);";
						} else if(g.any_group_handler.handler.value == "member") {
							no_match_effect = "cobj." +
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								" = " + g.any_group_handler.type_or_function + "(cur, gen, err, context);";
						} else if(g.any_group_handler.handler.value == "member_fn") {
							no_match_effect = "cobj." +
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								"(cur, " + g.any_group_handler.type_or_function + "(cur, gen, err, context), err, context);";
						} else if(g.any_group_handler.handler.value == "function") {
							no_match_effect =
								(g.any_group_handler.handler.opt.length() > 0 ? g.any_group_handler.handler.opt : std::string("any")) +
								"(cobj, cur, " + g.any_group_handler.type_or_function + "(cur, gen, err, context), err, context);";
						} else {
							no_match_effect = "err.unhandled_group_key(cur); discard_group(gen);";
						}
					}

					auto match_handler = [](group_association const& v) {
						std::string out;
						if(v.is_extern) {
							if(v.handler.value == "discard") {
								out = v.type_or_function + "(gen, err, context);";
							} else if(v.handler.value == "member") {
								out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									" = " + type_string_to_type(v.type_or_function) + "(gen, err, context);";
							} else if(v.handler.value == "member_fn") {
								out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									"(" + type_string_to_type(v.type_or_function) + "(gen, err, context), err, context);";
							} else if(v.handler.value == "function") {
								out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									"(cobj, " + type_string_to_type(v.type_or_function) + "(gen, err, context), err, context);";
							} else {
								out = "err.unhandled_group_key(cur);";
							}
						} else {
							if(v.handler.value == "discard") {
								out = "discard_group(gen);";
							} else if(v.handler.value == "member") {
								out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									" = parse_" + type_string_to_type(v.type_or_function) + "(gen, err, context);";
							} else if(v.handler.value == "member_fn") {
								out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									"(parse_" + type_string_to_type(v.type_or_function) + "(gen, err, context), err, context);";
							} else if(v.handler.value == "function") {
								out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
									"(cobj, parse_" + type_string_to_type(v.type_or_function) + "(gen, err, context), err, context);";
							} else {
								out = "err.unhandled_group_key(cur);";
							}
						}
						return out;
					};

					output += construct_match_tree_outer(g.groups, match_handler, no_match_effect);
				}

				output += "\t\t\t } else {\r\n"; // next next != open brace

				output += "\t\t\t\t auto const assoc_token = gen.get();\r\n";
				output += "\t\t\t\t auto const assoc_type = parse_association_type_b(assoc_token.start, assoc_token.end);\r\n";
				output += "\t\t\t\t auto const rh_token = gen.get();\r\n";

				// match values

				{
					std::string no_match_effect;
					if(g.any_value_handler.handler.value == "discard") {
					} else if(g.any_value_handler.handler.value == "member") {
						no_match_effect = "cobj." +
							(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any")) +
							" = token_to<" + type_string_to_type(g.any_value_handler.type) + ">(rh_token, err);";
					} else if(g.any_value_handler.handler.value == "member_fn") {
						no_match_effect = "cobj." +
							(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any")) +
							"(cur, assoc_type, token_to<" + type_string_to_type(g.any_value_handler.type) + ">(rh_token, err), err, context);";
					} else if(g.any_value_handler.handler.value == "function") {
						no_match_effect =
							(g.any_value_handler.handler.opt.length() > 0 ? g.any_value_handler.handler.opt : std::string("any")) +
							"(cobj, cur, assoc_type, token_to<" + type_string_to_type(g.any_value_handler.type) + ">(rh_token, err), err, context);";
					} else {
						no_match_effect = "err.unhandled_association_key(cur);";
					}

					auto match_handler = [](value_association const& v) {
						std::string out;
						if(v.handler.value == "discard") {

						} else if(v.handler.value == "member") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								" = token_to<" + type_string_to_type(v.type) + ">(rh_token, err);";
						} else if(v.handler.value == "member_fn") {
							out = "cobj." + (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(assoc_type, token_to<" + type_string_to_type(v.type) + ">(rh_token, err), err, context);";
						} else if(v.handler.value == "function") {
							out = (v.handler.opt.length() > 0 ? v.handler.opt : v.key) +
								"(cobj, assoc_type, token_to<" + type_string_to_type(v.type) + ">(rh_token, err), err, context);";
						} else {
							out = "err.unhandled_association_key(cur);";
						}
						return out;
					};

					output += construct_match_tree_outer(g.values, match_handler, no_match_effect);
				}

				output += "\t\t\t }\r\n"; // end next next

				output += "\t\t } else {\r\n"; // next != special identifier

				// case: free value;
				if(g.single_value_handler_result.value.length() > 0) {
					if(g.single_value_handler_result.value == "discard") {
						// do nothing
					} else if(g.single_value_handler_result.value == "member") {
						output += "\t\t\t cobj." + 
							(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("value")) +
							"= token_to<" + type_string_to_type(g.single_value_handler_type) + ">(cur, err);\r\n";
					} else if(g.single_value_handler_result.value == "member_fn") {
						output += "\t\t\t cobj." +
							(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("value")) +
							"(token_to<" + type_string_to_type(g.single_value_handler_type) + ">(cur, err), err, context);\r\n";
					} else if(g.single_value_handler_result.value == "function") {
						output += "\t\t\t " +
							(g.single_value_handler_result.opt.length() > 0 ? g.single_value_handler_result.opt : std::string("value")) +
							"(cobj, token_to<" + type_string_to_type(g.single_value_handler_type) + ">(cur, err), err, context);\r\n";
					}
				} else {
					output += "\t\t\t err.unhandled_free_value(cur);\r\n"; // end next token if
				}

				output += "\t\t }\r\n"; // end next token if

				output += "\t }\r\n"; // end token loop
				output += "\t finish_parse(cobj);\r\n";
				output += "\t return cobj;\r\n";
				output += "}\r\n"; // end fn
			}

			output += "}\r\n"; // end namespace

			output += "#pragma warning( pop )\r\n";
			output += "\r\n";

			// write output
			std::wstring output_name;
			for(auto c : file.namespace_name) {
				output_name += wchar_t(c);
			}
			output_name += wchar_t('.');
			output_name += wchar_t('h');

			const auto handleb = CreateFile(output_name.c_str(), GENERIC_WRITE | GENERIC_READ, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
			if(handleb != INVALID_HANDLE_VALUE) {
				DWORD written;
				WriteFile(handleb, output.c_str(), DWORD(output.length()), &written, nullptr);
				CloseHandle(handleb);
			}
		} else {
			MessageBox(NULL, argv[i], L"File not found", MB_OK);
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


