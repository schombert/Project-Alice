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
	ignore_rtl = 25,
	draggable = 26,
	table_highlight_color = 27,
	ascending_sort_icon = 28,
	descending_sort_icon = 29,
	row_background_a = 30,
	row_background_b = 31,
	row_height = 32,
	table_insert = 33,
	table_display_column_data = 34,
	table_internal_column_data = 35,
	table_divider_color = 36,
	table_has_per_section_headers = 37,
	animation_type = 38,
	datapoints = 39,
	other_color = 40,
	hover_activation = 41,
	hotkey = 42,
	share_table_highlight = 43,
	page_button_textures = 44,
	layout_information = 45,
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
	std::string_view page_left_texture;
	std::string_view page_right_texture;
	char const* layout_data;
	uint16_t layout_data_size;
	int16_t x_pos;
	int16_t y_pos;
	int16_t x_size;
	int16_t y_size;
	int16_t border_size;
	ui::orientation orientation;
	text::text_color page_text_color = text::text_color::black;
};

namespace ogl {
struct color4f {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;

	bool operator==(color4f const& o) const noexcept {
		return r == o.r && g == o.g && b == o.b && a == o.a;
	}
	bool operator!=(color4f const& o) const noexcept {
		return !(*this == o);
	}
	color4f operator*(float v) const noexcept {
		return color4f{ r * v, b * v, g * v, a };
	}
};
}

struct table_display_column {
	std::string_view header_key;
	std::string_view header_tooltip_key;
	std::string_view header_texture;
	std::string_view cell_tooltip_key;
	int16_t width = 0;
	text::text_color cell_text_color = text::text_color::black;
	text::text_color header_text_color = text::text_color::black;
	text::alignment text_alignment = text::alignment::center;
};

struct aui_element_data {
	std::vector<table_display_column> table_columns;
	std::string_view name;
	std::string_view texture;
	std::string_view alt_texture;
	std::string_view tooltip_text_key;
	std::string_view text_key;
	std::string_view ascending_sort_icon;
	std::string_view descending_sort_icon;
	std::string_view row_background_a;
	std::string_view row_background_b;
	ogl::color4f table_highlight_color{ 0.0f, 0.0f, 0.0f, 0.0f };
	ogl::color3f table_divider_color{ 0.0f, 0.0f, 0.0f };
	float text_scale = 1.0f;
	float row_height = 2.0f;
	int16_t x_pos = 0;
	int16_t y_pos = 0;
	int16_t x_size = 0;
	int16_t y_size = 0;
	int16_t border_size = 0;
	text::text_color text_color = text::text_color::black;
	aui_text_type text_type = aui_text_type::body;
	text::alignment text_alignment = text::alignment::left;
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
		} else if(ptype == aui_property::page_button_textures) {
			result.page_left_texture = essential_window_section.read<std::string_view>();
			result.page_right_texture = essential_window_section.read<std::string_view>();
			essential_window_section.read(result.page_text_color);
		} else if(ptype == aui_property::layout_information) {
			auto lsection = essential_window_section.read_section();
			result.layout_data = lsection.view_data() + lsection.view_read_position();
			result.layout_data_size = uint16_t(lsection.view_size() - lsection.view_read_position());
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
		} else if(ptype == aui_property::table_highlight_color) {
			essential_child_section.read(result.table_highlight_color);
		} else if(ptype == aui_property::other_color) {
			essential_child_section.read(result.table_highlight_color);
		} else if(ptype == aui_property::ascending_sort_icon) {
			result.ascending_sort_icon =essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::descending_sort_icon) {
			result.descending_sort_icon = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::row_background_a) {
			result.row_background_a = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::row_background_b) {
			result.row_background_b = essential_child_section.read<std::string_view>();
		} else if(ptype == aui_property::row_height) {
			essential_child_section.read(result.row_height);
		} else if(ptype == aui_property::table_divider_color) {
			essential_child_section.read(result.table_divider_color);
		} else if(ptype == aui_property::table_display_column_data) {
			table_display_column tc;
			tc.header_key = essential_child_section.read<std::string_view>();
			tc.header_tooltip_key = essential_child_section.read<std::string_view>();
			tc.header_texture = essential_child_section.read<std::string_view>();
			tc.cell_tooltip_key = essential_child_section.read<std::string_view>();
			essential_child_section.read(tc.width);
			essential_child_section.read(tc.cell_text_color);
			essential_child_section.read(tc.header_text_color);
			essential_child_section.read(tc.text_alignment);
			result.table_columns.emplace_back(tc);
		} else {
			abort();
		}
	}

	return result;
}
