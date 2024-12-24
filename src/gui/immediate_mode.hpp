#pragma once

namespace sys {
struct state;
};

namespace ui {
void render_text_chunk(
	sys::state& state,
	text::text_chunk t,
	float x,
	float baseline_y,
	uint16_t font_id,
	ogl::color3f text_color,
	ogl::color_modification cmod
);
ogl::color3f get_text_color(sys::state& state, text::text_color text_color);
};


namespace iui {

enum class alignment_vertical : uint8_t {
	top, center, horizontal
};
enum class alignment_horizontal : uint8_t {
	left, center, right
};

struct rect {
	float x, y, w, h;
};

struct element_description {
	float w;
	float h;
	GLuint texture_handle;
};

void shrink(rect& rectangle, float value);
void move_to(rect& rectangle, float x, float y);
rect subrect(rect& rectangle, float w, float h, alignment_horizontal align_hor, alignment_vertical align_vert);

enum iui_tab {
	none, factory_types, markets, commodities_markets, commodities_nations
};

struct iui_state {
	// immediate mode UI state storage
	uint16_t current_font = 0;
	int32_t active_element = 0;
	int32_t hovered_element = 0;

	iui_tab tab = iui_tab::none;

	bool over_ui = false;
	bool mouse_pressed = false;

	int page_production_methods = 0;

	std::vector<float> per_market_data;
	std::vector<dcon::nation_id> input_efficiency_leaders;
	std::vector<text::text_chunk> input_efficiency_leaders_string;

	bool loaded_descriptions = false;
	element_description priority_button;
	element_description page_button;
	element_description page_selector_bg;
	element_description item_selector_bg;
	element_description factory_type_bg;
	element_description commodity_bg;
	element_description factory_type_name_bg;
	element_description factory_type_priority_bg;
	element_description close_button;
	element_description top_bar_button;
	element_description map_label;

	ankerl::unordered_dense::map<int32_t, std::vector<text::text_chunk>> gui_text_chunks_map;
	ankerl::unordered_dense::map<int32_t, std::string> gui_text_map;
	ankerl::unordered_dense::map<int32_t, float> gui_float_map;
	ankerl::unordered_dense::map<int32_t, int32_t> gui_int_map;

	void panel(
		sys::state& state,
		rect& r
	);

	void panel_subsprite(
		sys::state& state,
		rect& r,
		int frame, int total_frames,
		GLuint texture_handle
	);

	void panel_textured(
		sys::state& state,
		rect& r,
		GLuint texture_handle
	);

	void panel_colored(
		sys::state& state,
		rect& r,
		float red, float green, float blue
	);

	bool button(
		sys::state& state, int32_t identifier,
		rect& r,
		bool active
	);

	bool button_textured(
		sys::state& state, int32_t identifier,
		rect& r,
		int total_frames,
		GLuint texture_handle,
		bool active
	);

	text::endless_layout init_layout(
		sys::state& state,
		text::layout& layout,
		uint16_t font_handle,
		text::alignment align, float w, float h
	);
	void float_2(
		sys::state& state, int32_t identifier,
		rect& r, float value
	);
	void int_whole(
		sys::state& state, int32_t identifier,
		rect& r,
		int32_t value
	);
	void price(
		sys::state& state, int32_t identifier,
		rect& r,
		float value
	);
	void render_text(
		sys::state& state,
		std::vector<text::text_chunk>& content,
		rect& r, ogl::color3f color
	);
	void localized_string(
		sys::state& state, int32_t identifier,
		rect& r, std::string_view key, ogl::color3f color = { 0.f, 0.f, 0.f }
	);
	void localized_string_r(
		sys::state& state, int32_t identifier,
		rect r, std::string_view key, ogl::color3f color = { 0.f, 0.f, 0.f }
	);

	void frame_start() {
		over_ui = false;
	}

	void frame_end() {
		if(mouse_pressed && (active_element == 0)) {
			mouse_pressed = false;
		}
		if(!mouse_pressed) {
			active_element = 0;
		}
	}

	void load_description(sys::state& state, std::string_view name, element_description& local_desc);
};

}
