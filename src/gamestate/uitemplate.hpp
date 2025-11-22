#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <variant>
#include <string_view>
#include "asvg.hpp"
#include "stools.hpp"
#include "unordered_dense.h"
#include "container_types.hpp"

namespace template_project {

enum class aui_text_alignment : uint8_t {
	left, right, center
};

struct background_definition {
	std::string file_name;

	int32_t base_x = 1000;
	int32_t base_y = 1000;

	// not to save -- rendering info
	asvg::svg renders;
};

struct icon_definition {
	std::string file_name;

	// not to save -- rendering info
	asvg::simple_svg renders;
};

struct color_definition {
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	float a = 1.0f;

	operator ogl::color3f() {
		return ogl::color3f{ r, g, b };
	}
};

enum class dimension_relative : uint8_t {
	height, width, smaller, larger, diagonal, pixel
};


struct affine_transform {
	float scale = 1.0f;
	float offset = 0.0f;
	dimension_relative dimension = dimension_relative::width;

	inline float resolve(float x_dim, float y_dim, float grid_size) {
		switch(dimension) {
			case dimension_relative::height: return y_dim * scale + offset * grid_size;
			case dimension_relative::width: return x_dim * scale + offset * grid_size;
			case dimension_relative::smaller: return std::min(y_dim, x_dim) * scale + offset * grid_size;
			case dimension_relative::larger: return std::max(y_dim, x_dim) * scale + offset * grid_size;
			case dimension_relative::diagonal: return std::sqrt(y_dim * y_dim + x_dim * x_dim) * scale + offset * grid_size;
			default: return 1.0f;
		}
	}
};

struct text_region_template {
	int32_t bg = -1;
	int32_t text_color = 0;
	int32_t font_choice = 0;
	float font_scale = 1.0f;
	float h_text_margins = 0.0f;
	float v_text_margins = 0.0f;
	aui_text_alignment h_text_alignment = aui_text_alignment::left;
	aui_text_alignment v_text_alignment = aui_text_alignment::left;
};

struct icon_region_template {
	int32_t bg = -1;
	int32_t icon_color = 0;
	affine_transform icon_top = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
	affine_transform icon_left = affine_transform{ 0.0f, 0.0f, dimension_relative::width };
	affine_transform icon_bottom = affine_transform{ 1.0f, 0.0f, dimension_relative::height };
	affine_transform icon_right = affine_transform{ 1.0f, 0.0f, dimension_relative::width };
};

struct layout_region_template {
	text_region_template page_number_text;
	int32_t bg = -1;
	int32_t left_button = -1;
	int32_t left_button_icon = -1;
	int32_t right_button = -1;
	int32_t right_button_icon = -1;
};

struct window_template {
	int32_t bg = -1;
	int32_t layout_region_definition = -1;
	int32_t close_button_definition = -1;
	int32_t close_button_icon = -1;
	float h_close_button_margin = 0.0f;
	float v_close_button_margin = 0.0f;
};

struct label_template {
	text_region_template primary;
};

struct button_template {
	text_region_template primary;
	text_region_template active;
	text_region_template disabled;
	bool animate_active_transition = false;
};
struct iconic_button_template {
	icon_region_template primary;
	icon_region_template active;
	icon_region_template disabled;
	bool animate_active_transition = false;
};
struct mixed_region_template {
	int32_t bg = -1;
	int32_t shared_color = 0;
	int32_t font_choice = 0;
	float font_scale = 1.0f;
	float h_text_margins = 0.0f;
	float v_text_margins = 0.0f;
	aui_text_alignment h_text_alignment = aui_text_alignment::left;
	aui_text_alignment v_text_alignment = aui_text_alignment::left;
	affine_transform icon_top = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
	affine_transform icon_left = affine_transform{ 0.0f, 0.5f, dimension_relative::height };
	affine_transform icon_bottom = affine_transform{ 1.0f, 0.0f, dimension_relative::height };
	affine_transform icon_right = affine_transform{ 1.0f, 0.5f, dimension_relative::height };
};
struct mixed_template {
	mixed_region_template primary;
	mixed_region_template active;
	mixed_region_template disabled;
	bool animate_active_transition = false;
};
struct progress_bar_template {
	int32_t bg_a = -1;
	int32_t bg_b = -1;

	int32_t text_color = 0;
	int32_t font_choice = 0;
	float h_text_margins = 0.0f;
	float v_text_margins = 0.0f;
	aui_text_alignment h_text_alignment = aui_text_alignment::center;
	aui_text_alignment v_text_alignment = aui_text_alignment::center;

	bool display_percentage_text = false;
};
struct color_region {
	int32_t bg = -1;
	int32_t color = 0;
};
struct toggle_region {
	color_region primary;
	color_region active;
	color_region disabled;
	int32_t font_choice;
	float font_scale = 1.0f;
	aui_text_alignment h_text_alignment = aui_text_alignment::left;
	aui_text_alignment v_text_alignment = aui_text_alignment::left;
	affine_transform text_margin_left = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
	affine_transform text_margin_right = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
	affine_transform text_margin_top = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
	affine_transform text_margin_bottom = affine_transform{ 0.0f, 0.0f, dimension_relative::height };
};
struct toggle_button_template {
	toggle_region on_region;
	toggle_region off_region;
	bool animate_active_transition = false;
};

struct table_template {
	int32_t arrow_increasing = -1;
	int32_t arrow_decreasing = -1;
	int32_t table_color = 0;
	int32_t interactable_header_bg = -1;
	int32_t active_header_bg = -1;
};

struct stacked_bar_template {
	int32_t overlay_bg = -1;
	float l_margin = 0.0f;
	float t_margin = 0.0f;
	float r_margin = 0.0f;
	float b_margin = 0.0f;
};


struct drop_down_template {

	int32_t primary_bg = -1;
	int32_t active_bg = -1;
	int32_t disabled_bg = -1;

	int32_t list_button = -1;
	int32_t list_button_alt = -1;
	int32_t selection_icon = -1;

	int32_t layout_region_base = -1;

	int32_t dropdown_window_bg = -1;
	float dropdown_window_margin = 0.5;
	int32_t vertical_nudge = 0;
	bool animate_active_transition = false;
};

struct project {
	std::u16string svg_directory;
	std::vector<label_template> label_t;
	std::vector<button_template> button_t;
	std::vector<progress_bar_template> progress_bar_t;
	std::vector<window_template> window_t;
	std::vector<iconic_button_template> iconic_button_t;
	std::vector<layout_region_template> layout_region_t;
	std::vector< background_definition> backgrounds;
	std::vector<mixed_template> mixed_button_t;
	std::vector<toggle_button_template> toggle_button_t;
	std::vector<table_template> table_t;
	std::vector< stacked_bar_template> stacked_bar_t;
	std::vector< drop_down_template> drop_down_t;
	std::vector< icon_definition> icons;
	std::vector<color_definition> colors;

	ankerl::unordered_dense::map<std::string, int32_t> icons_by_name;
	ankerl::unordered_dense::map<std::string, int32_t> colors_by_name;
	ankerl::unordered_dense::map<std::string, int32_t> backgrounds_by_name;
};

inline int32_t icon_by_name(project const& p, std::string_view name) {
	if(auto it = p.icons_by_name.find(std::string(name)); it != p.icons_by_name.end()) {
		return it->second;
	}
	return -1;
}
inline int32_t color_by_name(project const& p, std::string_view name) {
	if(auto it = p.colors_by_name.find(std::string(name)); it != p.colors_by_name.end()) {
		return it->second;
	}
	return -1;
}
inline int32_t background_by_name(project const& p, std::string_view name) {
	if(auto it = p.backgrounds_by_name.find(std::string(name)); it != p.backgrounds_by_name.end()) {
		return it->second;
	}
	return -1;
}

project bytes_to_project(serialization::in_buffer& buffer);

}
