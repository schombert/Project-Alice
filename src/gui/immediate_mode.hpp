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
};


namespace iui {

struct rect {
	float x, y, w, h;
};

void shrink(rect& rectangle, float value);
void move_to(rect& rectangle, float x, float y);

enum iui_tab {
	none, factory_types, markets, commodities_markets, commodities_nations
};

struct iui_state {
	// immediate mode UI state storage
	uint16_t current_font;
	int32_t active_element;
	int32_t hovered_element;

	iui_tab tab;

	bool over_ui = false;
	bool mouse_pressed = false;

	int page_production_methods = 0;

	std::vector<float> per_market_data;

	ankerl::unordered_dense::map<int32_t, std::vector<text::text_chunk>> gui_text_chunks_map;
	ankerl::unordered_dense::map<int32_t, std::string> gui_text_map;
	ankerl::unordered_dense::map<int32_t, float> gui_float_map;
	ankerl::unordered_dense::map<int32_t, int32_t> gui_int_map;

	void panel(
		sys::state& state,
		rect& r
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
		float x, float y
	);
	void localized_string(
		sys::state& state, int32_t identifier,
		rect& r, std::string_view key
	);
	void localized_string_r(
		sys::state& state, int32_t identifier,
		rect r, std::string_view key
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
};

}
