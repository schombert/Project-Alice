#pragma once
#include <vector>
#include <string>
#include <string_view>
#include "stools.hpp"
#include "gui/gui_graphics.hpp"
#include "text/text.hpp"
#include "unordered_dense.h"
#include "simple_fs.hpp"

enum class aui_background_type : uint8_t {
	none, texture, bordered_texture, existing_gfx
};
enum class aui_text_type : uint8_t {
	body,
	header
};
enum class aui_container_type : uint8_t {
	none,
	list,
	grid
};

enum class aui_property : uint8_t {
	texture = 1,
	tooltip_text_key = 2,
	text_key = 3,
	rectangle_color = 4,
	text_color = 5,
	background = 6,
	no_grid = 7,
	text_align = 8,
	dynamic_element = 9,
	dynamic_tooltip = 10,
	can_disable = 11,
	can_hide = 12,
	left_click_action = 13,
	right_click_action = 14,
	shift_click_action = 15,
	dynamic_text = 16,
	border_size = 17,
	text_scale = 18,
	text_type = 19,
	container_type = 20,
	child_window = 21,
	list_content = 22,
	data_member = 23,
	has_alternate_bg = 24,
	alternate_bg = 25,
};

inline void bytes_to_windows(char const* bytes, size_t size, std::string const& project_name, ankerl::unordered_dense::map<std::string, sys::aui_pending_bytes>& map) {
	serialization::in_buffer buffer(bytes, size);
	auto header_section = buffer.read_section();

	while(buffer) {
		auto window_section = buffer.read_section(); // essential section
		auto window_offset = window_section.view_read_position();
		auto window_size = window_section.view_size() - window_offset;

		auto essential_window_section = window_section.read_section(); // essential section

		std::string name;
		essential_window_section.read(name);

		map.insert_or_assign(project_name + "::" + name, sys::aui_pending_bytes{ bytes + window_offset, window_size });
	}
}

struct aui_window_data {
	std::string_view texture;
	std::string_view alt_texture;
	int16_t x_pos;
	int16_t y_pos;
	int16_t x_size;
	int16_t y_size;
	int16_t border_size;
	ui::orientation orientation;
};

struct aui_element_data {
	std::string_view name;
	std::string_view texture;
	std::string_view alt_texture;
	std::string_view tooltip_text_key;
	std::string_view text_key;
	float text_scale = 1.0f;
	int16_t x_pos;
	int16_t y_pos;
	int16_t x_size;
	int16_t y_size;
	int16_t border_size;
	text::text_color text_color;
	aui_text_type text_type;
	text::alignment text_alignment;
};

inline aui_window_data read_window_bytes(char const* bytes, size_t size, std::vector<sys::aui_pending_bytes>& children_out) {
	aui_window_data result;

	serialization::in_buffer window_section(bytes, size);

	auto essential_window_section = window_section.read_section(); // essential section

	essential_window_section.read<std::string_view>(); // toss name
	essential_window_section.read(result.x_pos);
	essential_window_section.read(result.y_pos);
	essential_window_section.read(result.x_size);
	essential_window_section.read(result.y_size);
	essential_window_section.read(result.orientation);
	while(essential_window_section) {
		auto ptype = essential_window_section.read<aui_property>();
		if(ptype == aui_property::border_size) {
			essential_window_section.read(result.border_size);
		} else if(ptype == aui_property::texture) {
			result.texture = essential_window_section.read<std::string_view>();
		} else if(ptype == aui_property::alternate_bg) {
			result.alt_texture = essential_window_section.read<std::string_view>();
		} else {
			abort();
		}
	}

	auto optional_section = window_section.read_section(); // toss section

	while(window_section) {
		auto child_start = window_section.view_read_position();

		auto essential_child_section = window_section.read_section(); // toss
		auto optional_child_section = window_section.read_section(); // toss

		auto child_size = window_section.view_read_position() - child_start;
		children_out.push_back(sys::aui_pending_bytes{ bytes  + child_start, child_size});
	}
	return result;
}

inline aui_element_data read_child_bytes(char const* bytes, size_t size) {
	aui_element_data result;
	serialization::in_buffer window_section(bytes, size);
	auto essential_child_section = window_section.read_section();

	result.name = essential_child_section.read<std::string_view>();
	essential_child_section.read(result.x_pos);
	essential_child_section.read(result.y_pos);
	essential_child_section.read(result.x_size);
	essential_child_section.read(result.y_size);

	while(essential_child_section) {
		auto ptype = essential_child_section.read< aui_property>();
		if(ptype == aui_property::text_color) {
			essential_child_section.read(result.text_color);
		} else if(ptype == aui_property::text_align) {
			essential_child_section.read(result.text_alignment);
		} else if(ptype == aui_property::tooltip_text_key) {
			result.tooltip_text_key = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::text_key) {
			result.text_key = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::text_type) {
			essential_child_section.read(result.text_type);
		} else if(ptype == aui_property::text_scale) {
			essential_child_section.read(result.text_scale);
		} else if(ptype == aui_property::border_size) {
			essential_child_section.read(result.border_size);
		} else if(ptype == aui_property::texture) {
			result.texture = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::alternate_bg) {
			result.alt_texture = essential_child_section.read<std::string_view>();
		} else {
			abort();
		}
	}

	return result;
}
