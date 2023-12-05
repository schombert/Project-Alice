#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "text.hpp"

namespace ui {

class map_legend_title : public simple_text_element_base {
	std::string_view get_title_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "mapmode_8";
		case map_mode::mode::civilization_level:
			return "mapmode_19";
		case map_mode::mode::colonial:
			return "mapmode_7";
		case map_mode::mode::crisis:
			return "mapmode_21";
		case map_mode::mode::migration:
			return "mapmode_18";
		case map_mode::mode::naval:
			return "mapmode_22";
		case map_mode::mode::nationality:
			return "mapmode_13";
		case map_mode::mode::national_focus:
			return "mapmode_10";
		case map_mode::mode::party_loyalty:
			return "mapmode_16";
		case map_mode::mode::political:
			return "mapmode_2";
		case map_mode::mode::population:
			return "mapmode_12";
		case map_mode::mode::rank:
			return "mapmode_17";
		case map_mode::mode::recruitment:
			return "mapmode_9";
		case map_mode::mode::region:
			return "mapmode_5";
		case map_mode::mode::relation:
			return "mapmode_20";
		case map_mode::mode::revolt:
			return "mapmode_3";
		case map_mode::mode::rgo_output:
			return "mapmode_11";
		case map_mode::mode::supply:
			return "mapmode_15";
		case map_mode::mode::terrain:
			return "mapmode_1";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_title_from_mode(state.map_state.active_map_mode)));
	}
};
class map_legend_gradient_image : public image_element_base {
	int16_t get_frame_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::migration:
		case map_mode::mode::rank:
		case map_mode::mode::relation:
		case map_mode::mode::rgo_output:
		case map_mode::mode::supply:
		case map_mode::mode::admin:
		case map_mode::mode::civilization_level:
		case map_mode::mode::colonial:
			return 1;
		case map_mode::mode::population:
		case map_mode::mode::crisis:
		case map_mode::mode::revolt:
			return 3;
		default:
			return 0;
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		frame = get_frame_from_mode(state.map_state.active_map_mode);
	}
};
class map_legend_gradient_max : public simple_text_element_base {
	std::string_view get_gradient_max_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "lg_max_mapmode_8";
		case map_mode::mode::civilization_level:
			return "lg_max_mapmode_19";
		case map_mode::mode::colonial:
			return "lg_max_mapmode_7";
		case map_mode::mode::crisis:
			return "lg_max_mapmode_21";
		case map_mode::mode::migration:
			return "lg_max_mapmode_18";
		case map_mode::mode::population:
			return "lg_max_mapmode_12";
		case map_mode::mode::rank:
			return "lg_max_mapmode_17";
		case map_mode::mode::relation:
			return "lg_max_mapmode_20";
		case map_mode::mode::revolt:
			return "lg_max_mapmode_3";
		case map_mode::mode::rgo_output:
			return "lg_max_mapmode_11";
		case map_mode::mode::supply:
			return "lg_max_mapmode_15";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_gradient_max_from_mode(state.map_state.active_map_mode)));
	}
};
class map_legend_gradient_min : public simple_text_element_base {
	std::string_view get_gradient_min_from_mode(map_mode::mode v) {
		switch(v) {
		case map_mode::mode::admin:
			return "lg_min_mapmode_8";
		case map_mode::mode::civilization_level:
			return "lg_min_mapmode_19";
		case map_mode::mode::colonial:
			return "lg_min_mapmode_7";
		case map_mode::mode::crisis:
			return "lg_min_mapmode_21";
		case map_mode::mode::migration:
			return "lg_min_mapmode_18";
		case map_mode::mode::population:
			return "lg_min_mapmode_12";
		case map_mode::mode::rank:
			return "lg_min_mapmode_17";
		case map_mode::mode::relation:
			return "lg_min_mapmode_20";
		case map_mode::mode::revolt:
			return "lg_min_mapmode_3";
		case map_mode::mode::rgo_output:
			return "lg_min_mapmode_11";
		case map_mode::mode::supply:
			return "lg_min_mapmode_15";
		default:
			return "???";
		}
	}
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::produce_simple_string(state, get_gradient_min_from_mode(state.map_state.active_map_mode)));
	}
};

class map_legend_gradient : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "legend_title") {
			return make_element_by_type<map_legend_title>(state, id);
		} else if(name == "gradient_icon") {
			return make_element_by_type<map_legend_gradient_image>(state, id);
		} else if(name == "gradient_min") {
			return make_element_by_type<map_legend_gradient_min>(state, id);
		} else if(name == "gradient_max") {
			return make_element_by_type<map_legend_gradient_max>(state, id);
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_civ_level : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(53, 196, 53));
		} else if(name == "s5_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(53, 196, 53));
		} else if(name == "s5_ico_b") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(53, 53, 250));
		} else if(name == "s2_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(250, 5, 5));
		} else if(name == "s3_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(250, 250, 5));
		} else if(name == "s4_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(64, 64, 64));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_col : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(46, 247, 15));
		} else if(name == "s2_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(247, 15, 15));
		} else if(name == "s3_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(140, 247, 15));
		} else if(name == "s4_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(250, 250, 5));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_dip : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") { // Viewpoint
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FF00));
		} else if(name == "s2_ico") { // Ally
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0xFFAA00));
		} else if(name == "s3_ico") { // Enemy
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x0000FF));
		} else if(name == "s4_ico") { // Puppet
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x009900));
		} else if(name == "s5_ico") { // Sphere
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x55AA55));
		} else if(name == "s6_ico") { // Other
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x222222));
		} else if(name == "s7_ico") { // Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x222222));
		} else if(name == "s8_ico") { // Non Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FF00));
		} else if(name == "s9_ico") { // Union Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x222222));
		} else if(name == "s7_ico_b") { // Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FF00));
		} else if(name == "s8_ico_b") { // Non Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0xFFFFFF));
		} else if(name == "s9_ico_b") { // Union Core
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FFFF));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_rr : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") { // Level 0
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(41, 5, 245));
		} else if(name == "s2_ico") { // Level 5
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(14, 240, 44));
		} else if(name == "s3_ico") { // Upgrading
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(222, 7, 46));
		} else if(name == "s3_ico_b") { // Upgrading
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(232, 228, 111));
		} else if(name == "s4_ico") { // Max level
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(232, 228, 111));
		} else if(name == "s5_ico") { // Blocked
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(222, 7, 46));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_nav : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") { // No base
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x222222));
		} else if(name == "s2_ico") { // Max level
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x005500));
		} else if(name == "s3_ico") { // Upgrading
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x005500));
		} else if(name == "s3_ico_b") { // Upgrading
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FF00));
		} else if(name == "s4_ico") { // Upgradeable
			return make_element_by_type<tinted_image_element_base>(state, id, uint32_t(0x00FF00));
		} else if(name == "s5_ico") { // Constructible
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(50, 150, 200));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

class map_legend_rank : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "s1_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(int32_t(48 * 0.65f), int32_t(242 * 0.65f), int32_t(51 * 0.65f)));
		} else if(name == "s2_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(int32_t(36 * 0.65f), int32_t(47 * 0.65f), int32_t(255 * 0.65f)));
		} else if(name == "s3_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(int32_t(238 * 0.65f), int32_t(252 * 0.65f), int32_t(38 * 0.65f)));
		} else if(name == "s4_ico") {
			return make_element_by_type<tinted_image_element_base>(state, id, sys::pack_color(int32_t(250 * 0.65f), int32_t(5 * 0.65f), int32_t(5 * 0.65f)));
		} else {
			return nullptr;
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto new_position = xy_pair{ int16_t((state.ui_state.root->base_data.size.x - base_data.size.x) / 2), int16_t((state.ui_state.root->base_data.size.y - base_data.size.y) - (state.ui_state.msg_log_window->is_visible() ? state.ui_state.msg_log_window->base_data.size.y + 54 : 12)) };
		base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}
};

}
