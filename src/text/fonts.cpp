#include "fonts.hpp"
#include "parsers.hpp"

namespace text {

constexpr uint16_t pack_font_handle(uint32_t font_index, bool black, uint32_t size) {
	return uint16_t(uint32_t(font_index << 9) | uint32_t(black ? (1 << 8) : 0) | uint32_t(size & 0xFF));
}


bool is_black_font(std::string_view txt) {
	if(parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "_bl")
		|| parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black")
		|| parsers::has_fixed_suffix_ci(txt.data(), txt.data() + txt.length(), "black_bold")
		) {
		return true;
	} else {
		return false;
	}
}

uint32_t font_size(std::string_view txt) {
	const char* first_int = txt.data();
	const char* end = txt.data() + txt.size();
	while(first_int != end) {
		if(isdigit(*first_int))
			break;
		++first_int;
	}
	const char* last_int = first_int;
	while(last_int != end) {
		if(!isdigit(*last_int))
			break;
		++last_int;
	}
	if(first_int == last_int) {
		if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "fps_font"))
			return 14;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "tooltip_font"))
			return 16;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "frangoth_bold"))
			return 18;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "impact_small"))
			return 24;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "old_english"))
			return 50;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "timefont"))
			return 24;
		else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "vic_title"))
			return 42;
		else
			return 14;
	}

	uint32_t rvalue = 0;
	std::from_chars(first_int, last_int, rvalue);
	return rvalue;
}

uint32_t font_index(std::string_view txt) {
	if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "arial"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "fps"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "main"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "tooltip"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "frangoth"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "garamond"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "impact"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "old"))
		return 2;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "timefont"))
		return 1;
	else if(parsers::has_fixed_prefix_ci(txt.data(), txt.data() + txt.size(), "vic"))
		return 2;
	else
		return 1;
}

uint16_t name_into_font_id(std::string_view txt) {
	return pack_font_handle(font_index(txt), is_black_font(txt), font_size(txt));
}

int32_t size_from_font_id(uint16_t id) {
	return int32_t(id & 0xFF);
}

bool is_black_from_font_id(uint16_t id) {
	return ((id >> 8) & 1) != 0;
}
uint32_t font_index_from_font_id(uint16_t id) {
	return uint32_t((id >> 9) & 0xFF);
}

}
