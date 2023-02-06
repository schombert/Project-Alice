#include <cstdint>
#include <memory>
#include <vector>
#include <string_view>
#include <string>
#include <locale>
#include <fstream>
#include <functional>
#include <cstdarg>
#include <iostream>
#include <optional>
#include <sstream>
#include <cassert>

// Objects
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

// Diagnostics
struct location_info {
	location_info() = default;
	location_info(int _row, int _column) : row{ _row }, column{ _column } {}
	int row = 0;
	int column = 0;
};

// Tokenizer and lexer stage
enum token_type {
	none,
	group_ident,
	group_item_ident,
	ident,
	lparen,
	rparen,
	newline
};
struct token {
	token_type type = token_type::none;
	std::string data;
	location_info loc_info;

static std::string_view get_type_name(token_type const& type) {
	switch(type) {
	case token_type::none: return "none";
	case token_type::group_ident: return "group_ident";
	case token_type::group_item_ident: return "group_item_ident";
	case token_type::ident: return "ident";
	case token_type::lparen: return "(";
	case token_type::rparen: return ")";
	case token_type::newline: return "newline";
	}
	std::abort();
}
};

struct parser_state {
	std::ctype<char> const* char_facet = nullptr;
	std::vector<token> tokens;
	location_info loc_info;
	std::vector<group_contents> groups;
	int error_count = 0;
	int warning_count = 0;
	std::stringstream console_stream;
	std::string_view file_name;

parser_state(std::string_view const _file_name)
	: file_name{ _file_name }
{
	char_facet = &std::use_facet<std::ctype<char>>(std::locale("C"));
}

void report_any(std::string_view const severity, int code, location_info local_loc_info, std::string_view const fmt) {
	console_stream << file_name;
	if(local_loc_info.row > 0) {
		if(local_loc_info.column > 0)
			console_stream << "(" << std::to_string(local_loc_info.row) << "," << std::to_string(local_loc_info.column) << ")";
		else
			console_stream << "(" << std::to_string(local_loc_info.row) << ")";
	}
	console_stream << ": " << severity << " " << std::to_string(code) << ": " << fmt.data() << "\n";
}

void report_error(int code, location_info local_loc_info, std::string_view const fmt) {
	report_any("error", code, local_loc_info, fmt);
	error_count++;
}

size_t get_column(std::string_view const s, std::string_view::iterator const it) {
	return size_t(std::distance(s.begin(), it));
}

bool is_ident(char c) {
	return std::isalnum(c) || c == '#' || c == '@' || c == '_';
}

void tokenize_line(std::string_view const line) {
	if(line.empty())
		return;
	
	auto it = line.begin();
	
	while(it != line.cend()) {
		token tok{};
		// If a line starts with a space - it's a group identifier
		if(it == line.begin() && (std::isspace(*it) || *it == ',')) {
			tok.type = token_type::group_item_ident;
			++it;
		} else if(it == line.begin()) {
			tok.type = token_type::group_ident;
		}

		// Skip a single spacing character
		if(std::isspace(*it) || *it == ',')
			++it;
		if(it == line.cend())
			break;
		
		// Otherwise skip the rest...
		while(std::isspace(*it) || *it == ',')
			++it;
		if(it == line.cend()) {
			report_error(101, location_info(loc_info.row, int(get_column(line, it))), "Trailing spaces\n");
			break;
		}

		if(*it == '(') {
			tok.type = token_type::lparen;
			++it;
		} else if(*it == ')') {
			tok.type = token_type::rparen;
			++it;
		} else if(is_ident(*it)) {
			if(tok.type != token_type::group_ident && tok.type != token_type::group_item_ident)
				tok.type = token_type::ident;
			auto start_ident = it;
			while(it != line.cend() && is_ident(*it))
				++it;
			auto start_idx = std::distance(line.begin(), start_ident);
			auto end_idx = std::distance(line.begin(), it);
			tok.data = line.substr(start_idx, end_idx - start_idx);

			// Verify naming constraints for all identifiers
			bool violated_casing = false;
			for(auto const c : tok.data)
				if(std::isalpha(c) && !std::islower(c)) {
					violated_casing = true;
					break;
				}
			
			if(violated_casing)
				report_error(120, location_info(loc_info.row, int(get_column(line, it))), "Naming constraints violated '" + tok.data + "\n");
		} else {
			report_error(100, location_info(loc_info.row, int(get_column(line, it))), std::string() + "Unexpected token '" + *it + "'\n");
			break;
		}
		tok.loc_info = loc_info;
		tokens.push_back(tok);
	}
}

void tokenize_file(std::stringstream& stream) {
	for(std::string line; std::getline(stream, line); ) {
		tokenize_line(line);
		
		token tok{};
		tok.loc_info = loc_info;
		tok.type = token_type::newline;
		tokens.push_back(tok);

		++loc_info.row;
	}
}

std::optional<token> get_token(auto& it) {
	return std::optional(*it++);
}

token get_specific_token(auto& it, bool& err_cond, token_type const& type) {
	auto o = get_token(it);
	if(!o.has_value()) {
		// TODO: This is risky - but allows for accurate-ish reporting
		report_error(109, it[-1].loc_info, std::string() + "Expected a '" + token::get_type_name(type).data() + "' token\n");
		err_cond = true;
		return token{};
	}
	token tok{o.value()};
	if(tok.type != type) {
		report_error(110, tok.loc_info, std::string() + "Expected a '" + token::get_type_name(type).data() + "' token, but found '" + token::get_type_name(tok.type).data() + "'\n");
		err_cond = true;
		return token{};
	}
	return tok;
}

void parse() {
	auto it = tokens.begin();
	while(it != tokens.end()) {
		auto const key = get_token(it).value();
		if(key.type == token_type::group_ident) {
			groups.emplace_back();
			groups.back().group_object_type = key.data;
		} else if(key.type == token_type::group_item_ident) {
			if(groups.empty()) {
				report_error(120, key.loc_info, "Item '" + key.data + "' defined before first group\n");
				continue;
			}

			if(key.data == "#free") {
				auto err_cond = false;
				auto const type = get_specific_token(it, err_cond, token_type::ident);
				auto const opt = get_specific_token(it, err_cond, token_type::ident);
				auto const handler_type = get_specific_token(it, err_cond, token_type::ident);
				
				auto const tmp = get_token(it).value_or(token{});
				auto const handler_opt = tmp.type == token_type::lparen
					? get_specific_token(it, err_cond, token_type::ident)
					: token{};
				if(tmp.type == token_type::lparen)
					get_specific_token(it, err_cond, token_type::rparen);
				
				if(err_cond)
					continue;

				if(type.data == "parser" || type.data == "group") {
					groups.back().set_handler = group_association{ key.data, opt.data, value_and_optional{handler_type.data, handler_opt.data}, false };
				} else if(type.data == "extern") {
					groups.back().set_handler = group_association{ key.data, opt.data, value_and_optional{handler_type.data, handler_opt.data}, true };
				} else if(type.data == "value") {
					groups.back().single_value_handler_type = opt.data;
					groups.back().single_value_handler_result = value_and_optional{ handler_type.data, handler_opt.data };
				} else {
					report_error(102, type.loc_info, "Invalid #free type '" + type.data + "'\n");
				}
			} else if(key.data == "#base") {
				auto err_cond = false;
				auto const base_class_name = get_specific_token(it, err_cond, token_type::ident);
				if(err_cond)
					continue;
				
				for(auto& g : groups) {
					if(g.group_object_type == base_class_name.data) {
						for(auto& v : g.groups)
							groups.back().groups.push_back(v);
						for(auto& v : g.values)
							groups.back().values.push_back(v);
						groups.back().any_group_handler = g.any_group_handler;
						groups.back().any_value_handler = g.any_value_handler;
						groups.back().single_value_handler_result = g.single_value_handler_result;
						groups.back().single_value_handler_type = g.single_value_handler_type;
						groups.back().any_value_handler = g.any_value_handler;
						groups.back().set_handler = g.set_handler;
					}
				}
			} else if(key.data == "#any") {
				/* #any: type, opt, handler_type (handler_opt) */
				auto err_cond = false;
				auto const type = get_specific_token(it, err_cond, token_type::ident);
				auto const opt = get_specific_token(it, err_cond, token_type::ident);
				auto const handler_type = get_specific_token(it, err_cond, token_type::ident);
				
				auto const tmp = get_token(it).value_or(token{});
				auto const handler_opt = tmp.type == token_type::lparen
					? get_specific_token(it, err_cond, token_type::ident)
					: token{};
				if(tmp.type == token_type::lparen)
					get_specific_token(it, err_cond, token_type::rparen);
				
				if(err_cond)
					continue;

				static std::vector<std::string_view> const valid_group_handler_values = {
					"discard", "member", "member_fn", "function"
				};

				if(type.data == "parser" || type.data == "group") {
					groups.back().any_group_handler = group_association{ "", opt.data, value_and_optional{handler_type.data, handler_opt.data}, false };
				} else if(type.data == "value") {
					groups.back().any_value_handler = value_association{ "", opt.data, value_and_optional{handler_type.data, handler_opt.data} };
				} else if(type.data == "extern") {
					groups.back().any_group_handler = group_association{ "", opt.data, value_and_optional{handler_type.data, handler_opt.data}, true };
					
					if(std::find(valid_group_handler_values.begin(), valid_group_handler_values.end(), groups.back().any_group_handler.handler.value) == valid_group_handler_values.end())
						report_error(104, type.loc_info, "Unhandled #any group '" + type.data + "' with invalid handler_type '" + handler_type.data + "'\n");
				} else {
					report_error(103, type.loc_info, "Invalid #any type '" + type.data + "'\n");
				}
			} else {
				/* key: type opt handler_type (handler_opt) */
				auto err_cond = false;
				auto const type = get_specific_token(it, err_cond, token_type::ident);
				auto const opt = get_specific_token(it, err_cond, token_type::ident);
				auto const handler_type = get_specific_token(it, err_cond, token_type::ident);

				auto const tmp = get_token(it).value_or(token{});
				auto const handler_opt = tmp.type == token_type::lparen
					? get_specific_token(it, err_cond, token_type::ident)
					: token{};
				if(tmp.type == token_type::lparen)
					get_specific_token(it, err_cond, token_type::rparen);

				if(std::find_if(groups.back().groups.begin(), groups.back().groups.end(), [&](auto const& g) { return g.key == key.data; }) != groups.back().groups.end()) {
					report_error(116, type.loc_info, "Duplicate key '" + key.data + "' in group '" + groups.back().group_object_type + "'\n");
					err_cond = true;
				}

				if(err_cond)
					continue;
				
				if(type.data == "parser" || type.data == "group") {
					groups.back().groups.push_back(group_association{ key.data, opt.data, value_and_optional{handler_type.data, handler_opt.data}, false });
				} else if(type.data == "value") {
					groups.back().values.push_back(value_association{ key.data, opt.data, value_and_optional{handler_type.data, handler_opt.data} });
				} else if(type.data == "extern") {
					groups.back().groups.push_back(group_association{ key.data, opt.data, value_and_optional{handler_type.data, handler_opt.data}, true });
				} else {
					report_error(104, type.loc_info, "Invalid #free type '" + type.data + "'\n");
				}
			}
		} else if(key.type == token_type::newline) {
			// ignore newline
		} else {
			report_error(120, key.loc_info, std::string() + "Unexpected token '" + token::get_type_name(key.type).data() + "'\n");
		}
	}
}
};

std::string char_to_hex(char c) {
	static std::string_view hexmap = "0123456789ABCDEF";
	uint32_t v = c | 0x20;
	std::string temp{};
	temp.push_back(hexmap[(v / 16) % 16]);
	temp.push_back(hexmap[v % 16]);
	return temp;
}

std::string string_to_hex(std::string_view const s, int32_t start, int32_t count) {
	std::string res = "0x";
	for(int32_t i = count - 1; i >= 0; --i)
		res += char_to_hex(s[start + i]);
	return res;
}

std::string final_match_condition_internal(std::string_view const key, int32_t starting_position, int32_t ending_position) {
	if(starting_position >= ending_position)
		return "";

	if(ending_position - starting_position >= 8) {
		return
			" && (*(uint64_t*)(&cur.content[" + std::to_string(starting_position) + "]) | uint64_t(0x2020202020202020) ) == uint64_t(" + string_to_hex(key, starting_position, 8) + ")"
			+ final_match_condition_internal(key, starting_position + 8, ending_position);
	} else if(ending_position - starting_position >= 4) {
		return
			" && (*(uint32_t*)(&cur.content[" + std::to_string(starting_position) + "]) | uint32_t(0x20202020) ) == uint32_t(" + string_to_hex(key, starting_position, 4) + ")"
			+ final_match_condition_internal(key, starting_position + 4, ending_position);
	} else if(ending_position - starting_position >= 2) {
		return
			" && (*(uint16_t*)(&cur.content[" + std::to_string(starting_position) + "]) | 0x2020 ) == " + string_to_hex(key, starting_position, 2)
			+ final_match_condition_internal(key, starting_position + 2, ending_position);
	} else { /// single char
		return " && (cur.content[" + std::to_string(starting_position) + "] | 0x20 ) == " + string_to_hex(key, starting_position, 1);
	}
}

std::string final_match_condition(std::string_view const key, size_t starting_position, size_t ending_position) {
	if(!ending_position)
		ending_position = key.length();
	assert(ending_position <= key.length());
	assert(starting_position <= ending_position);
	return std::string("(true") + final_match_condition_internal(key, int32_t(starting_position), int32_t(ending_position)) + ")";
}

template<typename V, typename F>
void enum_with_prefix(V const& vector, std::string_view const prefix, int32_t length, F const& fn) {
	for(int32_t i = 0; i < int32_t(vector.size()); ++i) {
		if(int32_t(vector[i].key.length()) == length) {
			bool match = true;
			for(int32_t j = 0; j < int32_t(prefix.length()); ++j) {
				if((vector[i].key[j] | 0x20) != (prefix[j] | 0x20)) {
					match = false;
					break;
				}
			}

			if(match)
				fn(vector[i]);
		}
	}
}

template<typename V>
int32_t count_with_prefix(V const& vector, std::string_view const prefix, int32_t length) {
	int32_t total = 0;
	enum_with_prefix(vector, prefix, length, [&](auto const&) {
		++total;
	});
	return total;
}

template<typename V>
int32_t max_length(V const& vector) {
	int32_t mx = 0;
	for(auto const& e : vector)
		mx = mx > int32_t(e.key.length()) ? mx : int32_t(e.key.length());
	return mx;
}

struct match_tree_builder {
	std::string tabs;

void tabulate_increment() {
	tabs.push_back('\t');
}

void tabulate_decrement() {
	tabs.pop_back();
}

std::string tabulate(std::string_view const s) const {
	return tabs + s.data();
}

uint32_t token_string_hash(std::string_view const s) {
	uint32_t h = 5381;
	for(auto it = s.begin(); it != s.end(); it++)
		h = ((h << 5) + h) + uint8_t(*it | 0x20);
	return h;
}

template<typename V, typename F>
std::string construct_match_tree_outer(V const& vector, F const& generator_match, std::string_view const no_match) {
	if(vector.empty())
		return tabulate(std::string(no_match) + "\n");
	
	// No duplicates
	for(auto const& e1 : vector)
		assert(std::count_if(vector.begin(), vector.end(), [&](auto const& e2) {
			return e1.key == e2.key;
		}) == 1);
	
	std::string output;
	output += tabulate("switch(parsers::token_string_hash(cur.content)) {\n");
	for(auto const& e : vector) {
		output += tabulate("case " + std::to_string(token_string_hash(e.key)) + "ul:\n");
		tabulate_increment();
		output += tabulate("// " + e.key + "\n");
		output += tabulate(generator_match(e) + "\n");
		output += tabulate("break;\n");
		tabulate_decrement();
	}
	output += tabulate("default:\n");
	tabulate_increment();
	output += tabulate(std::string(no_match) + "\n");
	tabulate_decrement();
	output += tabulate("}\n");
	return output;
}

void file_write_out(std::fstream& stream, std::vector<group_contents>& groups) {
	//	process the parsed content into the generated file
	std::string output;
	output += "// parser generator 2.0 electric boogaloo\n";
	output += "#include \"parsers.hpp\"\n\n";
	// output += "#pragma warning( push )\n";
	// output += "#pragma warning( disable : 4065 )\n";
	// output += "#pragma warning( disable : 4189 )\n";
	output += "\n";
	output += "namespace parsers {\n";
	// declare fns
	for(auto& g : groups) {
		output += "template<typename C>\n";
		output += g.group_object_type + " parse_" + g.group_object_type + "(token_generator& gen, error_handler& err, C&& context);\n";
	}
	output += "\n";
	// fn bodies
	for(auto& g : groups) {
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
			tabulate_increment();
			tabulate_increment();
			tabulate_increment();
			tabulate_increment();
			output += construct_match_tree_outer(g.groups, match_handler, no_match_effect);
			tabulate_decrement();
			tabulate_decrement();
			tabulate_decrement();
			tabulate_decrement();
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
			tabulate_increment();
			tabulate_increment();
			tabulate_increment();
			tabulate_increment();
			output += construct_match_tree_outer(g.values, match_handler, no_match_effect);
			tabulate_decrement();
			tabulate_decrement();
			tabulate_decrement();
			tabulate_decrement();
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
	stream.write(output.data(), output.size());
}
};

int main(int argc, char *argv[]) {
	if(argc > 1) {
		auto const input_filename = std::string(argv[1]);
		std::string output_filename;
		if(argc > 2) {
			output_filename = std::string(argv[2]);
		} else {
			output_filename = std::string(argv[1]);
			if(output_filename.length() >= 4 && output_filename[output_filename.length() - 4] == '.') {
				output_filename.pop_back();
				output_filename.pop_back();
				output_filename.pop_back();
				output_filename.pop_back();
			}
			output_filename += "_generated.hpp";
		}

		std::fstream input_file;
		input_file.open(input_filename, std::ios::in);

		std::fstream output_file;
		output_file.open(output_filename, std::ios::out);

		std::stringstream file_contents_stream{std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>{})};
		parser_state state(input_filename);
		state.tokenize_file(file_contents_stream);
		state.parse();

		std::cout << state.console_stream.str() << std::endl;
		if(state.error_count > 0)
			std::exit(EXIT_FAILURE);
		
		match_tree_builder tree_builder{};
		tree_builder.file_write_out(output_file, state.groups);
	} else {
		fprintf(stderr, "Usage: %s <input> [output]\n", argv[0]);
	}
	return 0;
}
