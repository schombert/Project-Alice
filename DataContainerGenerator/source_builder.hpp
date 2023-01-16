#pragma once

#include <string>
#include <vector>
#include <functional>
#include <utility>

class raw_lines {
public:
	std::vector<char const*> lines;

	raw_lines& operator+(char const* l) {
		lines.push_back(l);
		return *this;
	}
}; 

class substitute {
public:
	std::string sub_key;
	std::string sub_text;

	substitute(char const* key, std::string text) : sub_key(key), sub_text(text) {

	}
};

struct append {
	char const* value;
};

struct trailing_semicolon {};

struct line_break {};

struct heading {
	char const* str;
};

class basic_builder;

struct trailing_semicolon_state {
	basic_builder& ref;

	template<typename T>
	auto operator+(T&& func)->std::enable_if_t<std::is_invocable_v<T, basic_builder&>, basic_builder&>;
};

class basic_builder {
public:
	std::vector<substitute> substitutes;
	std::vector<std::string> lines;
	int32_t extra_tabs = 0;
	bool declaration_mode = false;

	basic_builder& operator+(substitute const& sub) & {
		for(auto& s : substitutes) {
			if(s.sub_key == sub.sub_key) {
				s.sub_text = sub.sub_text;
				return *this;
			}
		}
		substitutes.push_back(sub);
		return *this;
	}
	basic_builder& operator+(char const* line) & {
		add_line(line);
		return *this;
	}
	basic_builder& operator+(append const& a) & {
		if(lines.size() > 0)
			lines.back() += (std::string(" ") + a.value);
		else
			add_line(std::string(" ") + a.value);
		return *this;
	}
	basic_builder& operator+(std::string const& line) & {
		add_line(line);
		return *this;
	}
	trailing_semicolon_state operator+(trailing_semicolon const&) {
		return trailing_semicolon_state{ *this };
	}
	basic_builder&  operator+(line_break const&) {
		lines.push_back(std::string(extra_tabs, '\t'));
		return *this;
	}
	
	basic_builder&  operator+(heading const& h) {
		lines.push_back(std::string(extra_tabs, '\t') + "//");
		add_line(std::string(extra_tabs, '\t') + "// " + h.str);
		lines.push_back(std::string(extra_tabs, '\t') + "//");
		return *this;
	}
	template<typename T>
	auto operator+(T&& func) ->std::enable_if_t<std::is_invocable_v<T, basic_builder&>, basic_builder&> {
		if(!declaration_mode) {
			if(lines.size() > 0)
				lines.back() += " {";
			else 
				add_line("{");

			++extra_tabs;
			func(*this);
			--extra_tabs;

			add_line("}");
		} else if(lines.size() > 0) {
			lines.back() += ';';
		}
		return *this;
	}
	void add_line(std::string const& line) {
		std::string built_string;
		bool in_sub_name = false;
		for(size_t pos = 0; pos < line.length(); ) {
			size_t end_pos = line.find('@', pos);
			if(end_pos > line.length()) {
				end_pos = line.length();
			}

			std::string segment = line.substr(pos, end_pos - pos);

			if(in_sub_name) {
				bool found = false;
				for(auto s : substitutes) {
					if(s.sub_key == segment) {
						built_string += s.sub_text;
						found = true;
					}
				}
				if(!found) {
					built_string += (std::string("$") + segment + "$");
				}
			} else {
				built_string += segment;
			}
			in_sub_name = !in_sub_name;
			pos = end_pos + 1;
		}

		lines.push_back(std::string(extra_tabs, '\t') + built_string);
	}
	[[nodiscard]] std::string to_string(int tab_level) {
		std::string output;
		std::string tabs = std::string(tab_level, '\t');

		for(auto& l : lines) {
			output += (tabs + l + "\n");
		}

		extra_tabs = 0;
		lines.clear();
		substitutes.clear();
		declaration_mode = false;

		return output;
	}
};

template<typename T>
auto trailing_semicolon_state::operator+(T&& func)->std::enable_if_t<std::is_invocable_v<T, basic_builder&>, basic_builder&> {
	ref + func;
	if(!ref.declaration_mode && ref.lines.size() > 0) {
		ref.lines.back() += ';';
	}
	return ref;
}

#define block [&](basic_builder& o)
#define class_block trailing_semicolon{} + [&](basic_builder& o) 
#define inline_block line_break{} + [&](basic_builder& o)


