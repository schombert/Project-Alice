#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "province.hpp"
#include "text.hpp"
#include "unit_tooltip.hpp"
#include "gui_land_combat.hpp"
#include "gui_naval_combat.hpp"

namespace ui {


struct toggle_unit_grid {
	bool with_shift;
};

class port_ex_bg : public shift_button_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) > 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return button_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	void button_shift_action(sys::state& state) noexcept override {
		if(visible)
			send(state, parent, toggle_unit_grid{ true });
	}
	void button_action(sys::state& state) noexcept override {
		if(visible)
			send(state, parent, toggle_unit_grid{ false });
	}
};

class port_sm_bg : public image_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) == 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class port_level_bar : public image_element_base {
public:
	bool visible = false;
	int32_t level = 1;

	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto port_level = state.world.province_get_building_level(prov, economy::province_building_type::naval_base);
		visible = port_level >= level;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class port_ship_count : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t count = retrieve<int32_t>(state, parent);
		color = text::text_color::gold;
		if(count <= 0) {
			set_text(state, "");
		} else {
			set_text(state, std::to_string(count));
		}
	}
};

class port_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;
	float map_x = 0;
	float map_y = 0;
	dcon::province_id port_for;
	outline_color color = outline_color::gray;
	int32_t displayed_count = 0;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "level1") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 1;
			return ptr;
		} else if(name == "level2") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 2;
			return ptr;
		} else if(name == "level3") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 3;
			return ptr;
		} else if(name == "level4") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 4;
			return ptr;
		} else if(name == "level5") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 5;
			return ptr;
		} else if(name == "level6") {
			auto ptr = make_element_by_type<port_level_bar>(state, id);
			ptr->level = 6;
			return ptr;
		} else if(name == "ship_count") {
			return make_element_by_type<port_ship_count>(state, id);
		} else if(name == "port_minimized") {
			return make_element_by_type<port_sm_bg>(state, id);
		} else if(name == "port_expanded") {
			return make_element_by_type<port_ex_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	void set_province(sys::state& state, dcon::province_id p) {
		port_for = p;

		auto adj = state.world.get_province_adjacency_by_province_pair(p, state.world.province_get_port_to(p));
		assert(adj);
		auto id = adj.index();
		auto& border = state.map_state.map_data.borders[id];
		auto& vertex = state.map_state.map_data.border_vertices[border.start_index + border.count / 2];

		map_x = vertex.position_.x;
		map_y = vertex.position_.y;
	}

	void on_update(sys::state& state) noexcept override {

		auto navies = state.world.province_get_navy_location(port_for);
		if(state.world.province_get_building_level(port_for, economy::province_building_type::naval_base) == 0 && navies.begin() == navies.end()) {
			populated = false;
			return;
		}

		populated = true;
		displayed_count = 0;

		if(navies.begin() == navies.end()) {
			auto controller = state.world.province_get_nation_from_province_control(port_for);
			if(controller == state.local_player_nation) {
				color = outline_color::blue;
			} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
				color = outline_color::red;
			} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
				color = outline_color::cyan;
			} else {
				color = outline_color::gray;
			}
		} else {
			bool player_navy = false;
			bool allied_navy = false;
			bool enemy_navy = false;
			bool selected_navy = false;
			for(auto n : navies) {
				auto controller = n.get_navy().get_controller_from_navy_control();
				if(state.is_selected(n.get_navy())) {
					selected_navy = true;
				} else if(controller == state.local_player_nation) {
					player_navy = true;
				} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
					enemy_navy = true;
				} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
					allied_navy = true;;
				}

				auto srange = n.get_navy().get_navy_membership();
				int32_t num_ships = int32_t(srange.end() - srange.begin());
				displayed_count += num_ships;
			}

			if(selected_navy) {
				color = outline_color::gold;
			} else if(player_navy) {
				color = outline_color::blue;
			} else if(enemy_navy) {
				color = outline_color::red;
			} else if(allied_navy) {
				color = outline_color::cyan;
			} else {
				color = outline_color::gray;
			}
		}

	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			glm::vec2 map_pos(map_x, 1.0f - map_y);
			auto screen_size =
				glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			visible = true;

			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			window_element_base::base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(port_for);
			return message_result::consumed;
		} else if(payload.holds_type<outline_color>()) {
			payload.emplace<outline_color>(color);
			return message_result::consumed;
		} else if(payload.holds_type<int32_t>()) {
			payload.emplace<int32_t>(displayed_count);
			return message_result::consumed;
		} else if(payload.holds_type< toggle_unit_grid>()) {
			auto with_shift = any_cast<toggle_unit_grid>(payload).with_shift;

			if(state.ui_state.unit_details_box->for_province == port_for && state.ui_state.unit_details_box->is_visible() == true) {
				state.ui_state.unit_details_box->set_visible(state, false);
				if(!with_shift) {
					state.selected_armies.clear();
					state.selected_navies.clear();
				} else {
					for(auto n : state.world.province_get_navy_location(port_for)) {
						state.deselect(n.get_navy().id);
					}
				}
			} else {
				if(!with_shift) {
					state.selected_armies.clear();
					state.selected_navies.clear();
				}

				for(auto n : state.world.province_get_navy_location(port_for)) {
					if(n.get_navy().get_controller_from_navy_control() == state.local_player_nation) {
						state.select(n.get_navy().id);
						// Hide province window when navy is clicked.
						if(state.ui_state.province_window)
							state.ui_state.province_window->set_visible(state, false);
					}
				}

				auto location = get_absolute_location(*this);
				location.x -= 18;
				location.y -= 18;
				state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(63), int16_t(36)}, port_for, true);
			}

			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

inline constexpr float big_counter_cutoff = 12.0f;
inline constexpr float prov_details_cutoff = 18.0f;

struct top_display_parameters {
	float top_left_value = 0.0f;
	float top_right_value = 0.0f;
	float top_left_org_value = 0.0f;
	float top_right_org_value = 0.0f;
	float battle_progress = 0.0f;
	dcon::nation_id top_left_nation;
	dcon::nation_id top_right_nation;
	int8_t top_left_status = 0;
	int8_t top_dig_in = -1;
	int8_t top_right_dig_in = -1;
	int8_t right_frames = 0;
	int8_t colors_used = 0;
	int8_t common_unit_1 = -1;
	int8_t common_unit_2 = -1;
	std::array<outline_color, 5> colors;
};

class prov_map_siege_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		progress = state.world.province_get_siege_progress(prov);
	}
};

class map_siege : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			return make_element_by_type<prov_map_siege_bar>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return window_element_base::impl_probe_mouse(state, x, y + 23, type);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
		else
			window_element_base::impl_render(state, x, y - 23);
	}
};

class prov_map_battle_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->battle_progress;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		
		sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume * state.user_settings.master_volume);

		auto prov = retrieve<dcon::province_id>(state, parent);
		for(auto b : state.world.province_get_land_battle_location(prov)) {
			auto w = b.get_battle().get_war_from_land_battle_in_war();
			if(!w) {
				land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
				win->battle = b.get_battle();
				if(state.ui_state.army_combat_window->is_visible()) {
					state.ui_state.army_combat_window->impl_on_update(state);
				} else {
					state.ui_state.army_combat_window->set_visible(state, true);
				}
				return message_result::consumed;
			} else if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
				land_combat_window* win = static_cast<land_combat_window*>(state.ui_state.army_combat_window);
				win->battle = b.get_battle();
				if(state.ui_state.army_combat_window->is_visible()) {
					state.ui_state.army_combat_window->impl_on_update(state);
				} else {
					state.ui_state.army_combat_window->set_visible(state, true);
				}
				return message_result::consumed;
			}
		}
		for(auto b : state.world.province_get_naval_battle_location(prov)) {
			auto w = b.get_battle().get_war_from_naval_battle_in_war();

			 if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
				naval_combat_window* win = static_cast<naval_combat_window*>(state.ui_state.naval_combat_window);
				win->battle = b.get_battle();
				if(state.ui_state.naval_combat_window->is_visible()) {
					state.ui_state.naval_combat_window->impl_on_update(state);
				} else {
					state.ui_state.naval_combat_window->set_visible(state, true);
				}
				return message_result::consumed;
			}
		}
		
		return message_result::consumed;
	}
};

class prov_map_br_overlay : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = 1;
	}
 };

class map_battle : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			return make_element_by_type<prov_map_battle_bar>(state, id);
		} else if(name == "overlay_right") {
			return make_element_by_type<prov_map_br_overlay>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return window_element_base::impl_probe_mouse(state, x, y + 23, type);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
		else
			window_element_base::impl_render(state, x, y - 23);
	}
};

class tr_frame_bg : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = int32_t(outline_color::red);
	}
};

class tr_edge : public image_element_base {
public:
	int32_t number = 0;
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		visible = params->right_frames > number;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tr_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->top_right_org_value;
	}
};

class tr_status : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = 6;
	}
};

class tr_dig_in : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->top_right_dig_in >= 0) {
			frame = params->top_right_dig_in;
			visible = true;
		} else {
			visible = false;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tr_strength : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		color = text::text_color::gold;
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		set_text(state, text::format_float(params->top_right_value, 1));
	}
};

class tr_controller_flag : public flag_button {
public:
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params)
			return state.world.nation_get_identity_from_identity_holder(params->top_right_nation);
		return dcon::national_identity_id{};
	}
};

class top_right_unit_icon : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tr_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tr_strength>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<tr_dig_in>(state, id);
		} else if(name == "status") {
			return make_element_by_type<tr_status>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<tr_org_bar>(state, id);
		} else if(name == "edge1") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 1;
			return ptr;
		} else if(name == "edge2") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 2;
			return ptr;
		} else if(name == "edge3") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 3;
			return ptr;
		} else if(name == "edge4") {
			auto ptr = make_element_by_type<tr_edge>(state, id);
			ptr->number = 4;
			return ptr;
		} else if(name == "frame_bg") {
			return make_element_by_type<tr_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class small_top_right_unit_icon : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tr_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tr_strength>(state, id);
		} else if(name == "frame_bg") {
			return make_element_by_type<tr_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0}};
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};


class tl_edge : public image_element_base {
public:
	int32_t number = 0;
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(number >= params->colors_used) {
			visible = false;
		} else {
			frame = int32_t(params->colors[number]);
			visible = true;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_frame_bg : public shift_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		frame = int32_t(params->colors[0]);
	}
	void button_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ false });
	}
	void button_shift_action(sys::state& state) noexcept override {
		send(state, parent, toggle_unit_grid{ true });
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		populate_unit_tooltip(state, contents, retrieve<dcon::province_id>(state, parent));
	}
};

class tl_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		progress = params->top_left_org_value;
	}
};

class tl_status : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);

		if(params->top_left_status >= 0) {
			frame = params->top_left_status;
		} else {
			frame = 0;
		}
	}
};

class tl_strength : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		auto strength = params->top_left_value;
		//bool is_ship = float(int32_t(strength)) == strength;
		auto layout = text::create_endless_layout(internal_layout,
		text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::gold, false});
		auto box = text::open_layout_box(layout, 0);


		text::add_to_layout_box(state, layout, box, text::format_float(strength, 1), text::text_color::gold);
		//if(!is_ship)
			//text::add_to_layout_box(state, layout, box, std::string("k"), text::text_color::gold);

		text::close_layout_box(layout, box);

	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::unseen;
	}
};

class tl_dig_in : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->top_dig_in >= 0) {
			frame = params->top_dig_in;
			visible = true;
		} else {
			visible = false;
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_unit_1 : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->common_unit_1 >= 0) {
			frame = params->common_unit_1;
			visible = true;
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_unit_2 : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params->common_unit_2 >= 0) {
			frame = params->common_unit_2;
			visible = true;
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class tl_controller_flag : public flag_button {
public:
	bool visible = true;

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params)
			return state.world.nation_get_identity_from_identity_holder(params->top_left_nation);
		return dcon::national_identity_id{};
	}
	void on_update(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);

		visible =  params->top_left_nation != state.local_player_nation;
		if(visible)
			flag_button::on_update(state);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			flag_button::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return flag_button::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class tl_sm_controller_flag : public flag_button {
public:

	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		top_display_parameters* params = retrieve<top_display_parameters*>(state, parent);
		if(params)
			return state.world.nation_get_identity_from_identity_holder(params->top_left_nation);
		return dcon::national_identity_id{};
	}
};

class top_unit_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tl_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tl_strength>(state, id);
		} else if(name == "unit_1") {
			return make_element_by_type<tl_unit_1>(state, id);
		} else if(name == "unit_2") {
			return make_element_by_type<tl_unit_2>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<tl_dig_in>(state, id);
		} else if(name == "status") {
			return make_element_by_type<tl_status>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<tl_org_bar>(state, id);
		} else if(name == "edge1") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 1;
			return ptr;
		} else if(name == "edge2") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 2;
			return ptr;
		} else if(name == "edge3") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 3;
			return ptr;
		} else if(name == "edge4") {
			auto ptr = make_element_by_type<tl_edge>(state, id);
			ptr->number = 4;
			return ptr;
		} else if(name == "frame_bg") {
			return make_element_by_type<tl_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class small_top_unit_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "controller_flag") {
			return make_element_by_type<tl_sm_controller_flag>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<tl_strength>(state, id);
		} else if(name == "frame_bg") {
			return make_element_by_type<tl_frame_bg>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() < big_counter_cutoff)
			window_element_base::impl_render(state, x, y);
	}
};

class map_pv_rail : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(x < -32 || y < -16 || x > state.ui_state.root->base_data.size.x || y > state.ui_state.root->base_data.size.y)
			return;

		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::railroad);
			last_update = state.ui_date;
		}

		image_element_base::render(state, x, y);
		ogl::render_character(state, cached_level, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), 14.0f, state.font_collection.fonts[1]);
	}
};
class map_pv_fort : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(x < -32 || y < -16 || x > state.ui_state.root->base_data.size.x || y > state.ui_state.root->base_data.size.y)
			return;

		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::fort);
			last_update = state.ui_date;
		}

		image_element_base::render(state, x, y);
		ogl::render_character(state, cached_level, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), 14.0f, state.font_collection.fonts[1]);
	}
};

class map_pv_bank : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(x < -32 || y < -16 || x > state.ui_state.root->base_data.size.x || y > state.ui_state.root->base_data.size.y)
			return;

		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::bank);
			last_update = state.ui_date;
		}

		image_element_base::render(state, x, y);
		ogl::render_character(state, cached_level, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), 14.0f, state.font_collection.fonts[1]);
	}
};

class map_pv_university : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(x < -32 || y < -16 || x > state.ui_state.root->base_data.size.x || y > state.ui_state.root->base_data.size.y)
			return;

		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), economy::province_building_type::university);
			last_update = state.ui_date;
		}

		image_element_base::render(state, x, y);
		ogl::render_character(state, cached_level, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), 14.0f, state.font_collection.fonts[1]);
	}
};

class map_pv_capital : public image_element_base {
public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		if(state.world.nation_get_capital(state.world.province_get_nation_from_province_ownership(p)) == p)
			image_element_base::render(state, x, y);
	}
};

class map_province_values : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "rail_icon") {
			return make_element_by_type<map_pv_rail>(state, id);
		} else if(name == "fort_icon") {
			return make_element_by_type<map_pv_fort>(state, id);
		} else if(name == "bank_icon") {
			if(state.economy_definitions.building_definitions[int32_t(economy::province_building_type::bank)].defined) {
				return make_element_by_type<map_pv_bank>(state, id);
			}
			return nullptr;
		} else if(name == "university_icon") {
			if(state.economy_definitions.building_definitions[int32_t(economy::province_building_type::university)].defined) {
				return make_element_by_type<map_pv_university>(state, id);
			}
			return nullptr;
		} else if(name == "capital_icon") {
			return make_element_by_type<map_pv_capital>(state, id);
		} else {
			return nullptr;
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return mouse_probe{ nullptr, ui::xy_pair{0,0} };
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(state.map_state.get_zoom() >= prov_details_cutoff && retrieve<dcon::province_id>(state, parent).index() < state.province_definitions.first_sea_province.index()) {
			window_element_base::impl_render(state, x, y);
		}
	}
};

class unit_counter_window : public window_element_base {
public:
	bool visible = true;
	bool populated = false;

	dcon::province_id prov;
	element_base* top_icon = nullptr;
	element_base* top_right_icon = nullptr;
	element_base* small_top_icon = nullptr;
	element_base* small_top_right_icon = nullptr;
	element_base* siege = nullptr;
	element_base* battle = nullptr;
	element_base* map_pv = nullptr;

	top_display_parameters display;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "top_unit_icon") {
			auto ptr = make_element_by_type<top_unit_icon>(state, id);
			top_icon = ptr.get();
			return ptr;
		} else if(name == "top_right_unit_icon") {
			auto ptr = make_element_by_type<top_right_unit_icon>(state, id);
			top_right_icon = ptr.get();
			return ptr;
		} else if(name == "small_top_unit_icon") {
			auto ptr = make_element_by_type<small_top_unit_icon>(state, id);
			small_top_icon = ptr.get();
			return ptr;
		} else if(name == "small_top_right_unit_icon") {
			auto ptr = make_element_by_type<small_top_right_unit_icon>(state, id);
			small_top_right_icon = ptr.get();
			return ptr;
		} else if(name == "siege") {
			auto ptr = make_element_by_type<map_siege>(state, id);
			siege = ptr.get();
			return ptr;
		} else if(name == "battle") {
			auto ptr = make_element_by_type<map_battle>(state, id);
			battle = ptr.get();
			return ptr;
		} else if(name == "province_values") {
			auto ptr = make_element_by_type<map_province_values>(state, id);
			map_pv = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
		if(!populated)
			return;

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {

		bool found_selected = false;
		bool all_selected = true;
		bool found_ally = false;
		bool found_enemy = false;
		bool found_other = false;

		if(prov.index() < state.province_definitions.first_sea_province.index()) {
			populated = false;
			for(auto a : state.world.province_get_army_location(prov)) {
				if(!(a.get_army().get_navy_from_army_transport())) {

					auto controller = a.get_army().get_controller_from_army_control().id;

					if(state.is_selected(a.get_army()))
						found_selected = true;
					else if(controller == state.local_player_nation)
						all_selected = false;
					else if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						found_ally = true;
					else
						found_other = true;

					populated = true;
				}
			}
			if(!populated) {
				map_pv->base_data.position.y = -8;
				return;
			}
		} else {
			populated = true;
			auto navies = state.world.province_get_navy_location(prov);
			if(navies.begin() == navies.end()) {
				populated = false;
				return;
			} else {
				for(auto n : navies) {
					auto controller = n.get_navy().get_controller_from_navy_control().id;
					if(state.is_selected(n.get_navy()))
						found_selected = true;
					else if(controller == state.local_player_nation)
						all_selected = false;
					else if(!controller || military::are_at_war(state, controller, state.local_player_nation))
						found_enemy = true;
					else if(military::are_allied_in_war(state, controller, state.local_player_nation))
						found_ally = true;
					else
						found_other = true;
				}
			}
			if(!populated) {
				map_pv->base_data.position.y = -8;
				return;
			}
		}

		map_pv->base_data.position.y = -48;

		bool player_involved_battle = false;
		dcon::land_battle_id lbattle;
		dcon::naval_battle_id nbattle;
		for(auto b : state.world.province_get_land_battle_location(prov)) {
			auto w = b.get_battle().get_war_from_land_battle_in_war();
			if(!w) {
				player_involved_battle = true;
				lbattle = b.get_battle();
				break;
			} else if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
				player_involved_battle = true;
				lbattle = b.get_battle();
				break;
			}
		}
		if(!player_involved_battle) {
			for(auto b : state.world.province_get_naval_battle_location(prov)) {
				auto w = b.get_battle().get_war_from_naval_battle_in_war();
				if(military::get_role(state, w, state.local_player_nation) != military::war_role::none) {
					player_involved_battle = true;
					nbattle = b.get_battle();
					break;
				}
			}
		}

		display.colors_used = 0;

		if(found_selected) {
			display.colors[display.colors_used] = outline_color::gold;
			++display.colors_used;
		}
		if(!all_selected) {
			display.colors[display.colors_used] = outline_color::blue;
			++display.colors_used;
		}
		if(!player_involved_battle) {
			if(found_enemy) {
				display.colors[display.colors_used] = outline_color::red;
				++display.colors_used;
			}
		}
		if(found_ally) {
			display.colors[display.colors_used] = outline_color::cyan;
			++display.colors_used;
		}
		if(found_other) {
			display.colors[display.colors_used] = outline_color::gray;
			++display.colors_used;
		}

		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;


		display.top_left_status = -1;
		display.top_dig_in = -1;
		display.top_right_dig_in = -1;
		display.right_frames = 0;
		display.common_unit_1 = -1;
		display.common_unit_2 = -1;
		display.top_left_value = 0.0f;
		display.top_right_value = 0.0f;
		display.top_left_org_value = 0.0f;
		display.top_right_org_value = 0.0f;
		display.top_right_org_value = 0.0f;

		if(lbattle) {
			float max_str = 0.0f;
			float max_opp_str = 0.0f;
			int32_t total_count = 0;
			int32_t total_opp_count = 0;

			auto w = state.world.land_battle_get_war_from_land_battle_in_war(lbattle);
			bool player_is_attacker = w ? military::is_attacker(state, w, state.local_player_nation) : false;

			display.top_left_status = 6;

			for(auto ar : state.world.land_battle_get_army_battle_participation(lbattle)) {
				auto controller = ar.get_army().get_controller_from_army_control();
				if(!controller || military::is_attacker(state, w, controller) != player_is_attacker) { // opposed
					++display.right_frames;

					float str = 0.0f;
					for(auto m : state.world.army_get_army_membership(ar.get_army())) {
						++total_opp_count;

						str += m.get_regiment().get_strength();
						display.top_right_value += m.get_regiment().get_strength();
						display.top_right_org_value += m.get_regiment().get_org();
					}

					auto dig_in = ar.get_army().get_dig_in();
					if(display.top_right_dig_in == -1) {
						display.top_right_dig_in = int8_t(dig_in);
					} else {
						display.top_right_dig_in = std::min(display.top_right_dig_in, int8_t(dig_in));
					}

					if(str > max_opp_str) {
						max_opp_str = str;
						display.top_right_nation = controller;
					}
				} else { // same side
					float str = 0.0f;
					for(auto m : state.world.army_get_army_membership(ar.get_army())) {
						auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_regiment().get_strength();
						display.top_left_value += m.get_regiment().get_strength();
						display.top_left_org_value += m.get_regiment().get_org();
					}

					auto dig_in = ar.get_army().get_dig_in();
					if(display.top_dig_in == -1) {
						display.top_dig_in = int8_t(dig_in);
					} else {
						display.top_dig_in = std::min(display.top_dig_in, int8_t(dig_in));
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = controller;
					}
				}
			}

			display.top_left_org_value /= float(total_count);
			display.top_right_org_value /= float(total_opp_count);

			int32_t areg = 0;
			int32_t breg = 0;

			for(auto reg : state.world.land_battle_get_reserves(lbattle)) {
				if((reg.flags & military::reserve_regiment::is_attacking) != 0)
					++areg;
				else
					++breg;
			}

			auto& att_back = state.world.land_battle_get_attacker_back_line(lbattle);
			auto& def_back = state.world.land_battle_get_defender_back_line(lbattle);
			auto& att_front = state.world.land_battle_get_attacker_front_line(lbattle);
			auto& def_front = state.world.land_battle_get_defender_front_line(lbattle);
			for(auto r : att_back) {
				if(r)
					++areg;
			}
			for(auto r : att_front) {
				if(r)
					++areg;
			}
			for(auto r : def_back) {
				if(r)
					++breg;
			}
			for(auto r : def_front) {
				if(r)
					++breg;
			}
			if(state.world.land_battle_get_war_attacker_is_attacker(lbattle) == player_is_attacker) {
				display.battle_progress = float(areg) / float(areg + breg);
			} else {
				display.battle_progress = float(breg) / float(areg + breg);
			}

			battle->set_visible(state, true);
			siege->set_visible(state, false);
			top_icon->base_data.position.x = -68;
			top_right_icon->set_visible(state, true);
			small_top_icon->base_data.position.x = -62;
			small_top_right_icon->set_visible(state, true);
		} else if(nbattle) {
			float max_str = 0.0f;
			float max_opp_str = 0.0f;
			int32_t total_count = 0;
			int32_t total_opp_count = 0;

			auto w = state.world.naval_battle_get_war_from_naval_battle_in_war(nbattle);
			bool player_is_attacker = w ? military::is_attacker(state, w, state.local_player_nation) : false;

			display.top_left_status = 6;

			for(auto ar : state.world.naval_battle_get_navy_battle_participation(nbattle)) {
				auto controller = ar.get_navy().get_controller_from_navy_control();
				if(!controller || military::is_attacker(state, w, controller) != player_is_attacker) { // opposed
					++display.right_frames;

					float str = 0.0f;
					for(auto m : state.world.navy_get_navy_membership(ar.get_navy())) {
						++total_opp_count;

						str += m.get_ship().get_strength();
						display.top_right_value += m.get_ship().get_strength();
						display.top_right_org_value += m.get_ship().get_org();
					}

					if(str > max_opp_str) {
						max_opp_str = str;
						display.top_right_nation = controller;
					}
				} else { // same side
					float str = 0.0f;
					for(auto m : state.world.navy_get_navy_membership(ar.get_navy())) {
						auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_ship().get_strength();
						display.top_left_value += m.get_ship().get_strength();
						display.top_left_org_value += m.get_ship().get_org();
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = controller;
					}
				}
			}

			display.top_left_org_value /= float(total_count);
			display.top_right_org_value /= float(total_opp_count);

			int32_t attacker_ships = 0;
			int32_t defender_ships = 0;

			auto slots = state.world.naval_battle_get_slots(nbattle);

			for(uint32_t j = slots.size(); j-- > 0;) {
				switch(slots[j].flags & military::ship_in_battle::mode_mask) {
					case military::ship_in_battle::mode_seeking:
					case military::ship_in_battle::mode_approaching:
					case military::ship_in_battle::mode_retreating:
					case military::ship_in_battle::mode_engaged:
						if((slots[j].flags & military::ship_in_battle::is_attacking) != 0)
							++attacker_ships;
						else
							++defender_ships;
						break;
					default:
						break;
				}
			}

			if(state.world.land_battle_get_war_attacker_is_attacker(lbattle) == player_is_attacker) {
				display.battle_progress = float(attacker_ships) / float(attacker_ships + defender_ships);
			} else {
				display.battle_progress = float(defender_ships) / float(attacker_ships + defender_ships);
			}

			battle->set_visible(state, true);
			siege->set_visible(state, false);
			top_icon->base_data.position.x = -68;
			top_right_icon->set_visible(state, true);
			small_top_icon->base_data.position.x = -62;
			small_top_right_icon->set_visible(state, true);
		} else if(prov.index() < state.province_definitions.first_sea_province.index()) {
			std::function<bool(dcon::army_id)> filter;

			if(display.colors[0] == outline_color::gold) {
				display.top_left_nation = state.local_player_nation;
				filter = [&](dcon::army_id a) { return state.world.army_get_controller_from_army_control(a) == state.local_player_nation && state.is_selected(a); };
			} else if(display.colors[0] == outline_color::blue) {
				display.top_left_nation = state.local_player_nation;
				filter = [&](dcon::army_id a) { return state.world.army_get_controller_from_army_control(a) == state.local_player_nation && !state.is_selected(a); };
			} else if(display.colors[0] == outline_color::cyan) {
				filter = [&](dcon::army_id a) {
					auto n = state.world.army_get_controller_from_army_control(a);
					return n != state.local_player_nation && military::are_allied_in_war(state, n, state.local_player_nation);
				};
			} else if(display.colors[0] == outline_color::red) {
				filter = [&](dcon::army_id a) {
					auto n = state.world.army_get_controller_from_army_control(a);
					return !n || military::are_at_war(state, n, state.local_player_nation);
				};
			} else if(display.colors[0] == outline_color::gray) {
				filter = [&](dcon::army_id a) {
					auto n = state.world.army_get_controller_from_army_control(a);
					return n != state.local_player_nation && !military::are_allied_in_war(state, n, state.local_player_nation) && !military::are_at_war(state, n, state.local_player_nation);
				};
			}

			float max_str = 0.0f;

			int32_t total_count = 0;
			int32_t ucount = 0;

			for(auto a : state.world.province_get_army_location(prov)) {
				if(!(a.get_army().get_navy_from_army_transport()) && filter(a.get_army())) {
					++ucount;
					float str = 0.0f;
					for(auto m : state.world.army_get_army_membership(a.get_army())) {
						auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_regiment().get_strength();
						display.top_left_value += m.get_regiment().get_strength();
						display.top_left_org_value += m.get_regiment().get_org();
					}

					auto frame = status_frame(state, a.get_army());
					if(display.top_left_status == -1) {
						display.top_left_status = int8_t(frame);
					} else if(display.top_left_status == int8_t(frame)) {

					} else {
						display.top_left_status = 0;
					}

					auto dig_in = a.get_army().get_dig_in();
					if(display.top_dig_in == -1) {
						display.top_dig_in = int8_t(dig_in);
					} else {
						display.top_dig_in = std::min(display.top_dig_in, int8_t(dig_in));
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = state.world.army_get_controller_from_army_control(a.get_army());
					}
				}
			}

			if(ucount > 1 && display.colors_used == 1) {
				display.colors_used = 2;
				display.colors[1] = display.colors[0];
			}

			display.top_left_org_value /= float(total_count);

			battle->set_visible(state, false);
			siege->set_visible(state, state.world.province_get_siege_progress(prov) > 0.f);
			top_icon->base_data.position.x = -30;
			top_right_icon->set_visible(state, false);
			small_top_icon->base_data.position.x = -30;
			small_top_right_icon->set_visible(state, false);
		} else {
			std::function<bool(dcon::navy_id)> filter;

			if(display.colors[0] == outline_color::gold) {
				display.top_left_nation = state.local_player_nation;
				filter = [&](dcon::navy_id a) { return state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation && state.is_selected(a); };
			} else if(display.colors[0] == outline_color::blue) {
				display.top_left_nation = state.local_player_nation;
				filter = [&](dcon::navy_id a) { return state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation && !state.is_selected(a); };
			} else if(display.colors[0] == outline_color::cyan) {
				filter = [&](dcon::navy_id a) {
					auto n = state.world.navy_get_controller_from_navy_control(a);
					return n != state.local_player_nation && military::are_allied_in_war(state, n, state.local_player_nation);
				};
			} else if(display.colors[0] == outline_color::red) {
				filter = [&](dcon::navy_id a) {
					auto n = state.world.navy_get_controller_from_navy_control(a);
					return military::are_at_war(state, n, state.local_player_nation);
				};
			} else if(display.colors[0] == outline_color::gray) {
				filter = [&](dcon::navy_id a) {
					auto n = state.world.navy_get_controller_from_navy_control(a);
					return n != state.local_player_nation && !military::are_allied_in_war(state, n, state.local_player_nation) && !military::are_at_war(state, n, state.local_player_nation);
				};
			}

			float max_str = 0.0f;
			int32_t total_count = 0;
			int32_t ucount = 0;

			for(auto a : state.world.province_get_navy_location(prov)) {
				if(filter(a.get_navy())) {
					++ucount;
					float str = 0.0f;
					for(auto m : state.world.navy_get_navy_membership(a.get_navy())) {
						auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
						if(uint32_t(icon) >= by_icon_count.size()) {
							by_icon_count.resize(icon + 1, 0);
						}
						++(by_icon_count[icon]);
						++total_count;

						str += m.get_ship().get_strength();
						display.top_left_value += m.get_ship().get_strength();
						display.top_left_org_value += m.get_ship().get_org();
					}

					auto frame = status_frame(state, a.get_navy());
					if(display.top_left_status == -1) {
						display.top_left_status = int8_t(frame);
					} else if(display.top_left_status == int8_t(frame)) {

					} else {
						display.top_left_status = 0;
					}

					if(str > max_str) {
						max_str = str;
						display.top_left_nation = state.world.navy_get_controller_from_navy_control(a.get_navy());
					}
				}
			}

			if(ucount > 1 && display.colors_used == 1) {
				display.colors_used = 2;
				display.colors[1] = display.colors[0];
			}

			display.top_left_org_value /= float(total_count);

			battle->set_visible(state, false);
			siege->set_visible(state, state.world.province_get_siege_progress(prov) > 0);
			top_icon->base_data.position.x = -30;
			top_right_icon->set_visible(state, false);
			small_top_icon->base_data.position.x = -30;
			small_top_right_icon->set_visible(state, false);
		}

		if(display.top_left_nation == state.local_player_nation) {
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			display.common_unit_1 = int8_t(max_index);

			if(by_icon_count.size() > 0) {
				by_icon_count[max_index] = 0;
				max_index = 0;
				for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
					if(by_icon_count[i] > by_icon_count[max_index])
						max_index = int32_t(i);
				}
				if(by_icon_count[max_index] > 0)
					display.common_unit_2 = int8_t(max_index);
			}
		}

	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size =
				glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			visible = true;
		
			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			window_element_base::base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		} else if(prov.index() < state.province_definitions.first_sea_province.index()) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size =
				glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			glm::vec2 screen_pos;
			if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
				visible = false;
				return;
			}
			visible = true;

			auto new_position = xy_pair{ int16_t(screen_pos.x), int16_t(screen_pos.y) };
			window_element_base::base_data.position = new_position;

			auto relative_location = child_relative_location(*this, *map_pv);
			map_pv->impl_render(state, new_position.x + relative_location.x, new_position.y + relative_location.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		} else if(payload.holds_type<top_display_parameters*>()) {
			payload.emplace<top_display_parameters*>(&display);
			return message_result::consumed;
		} else if(payload.holds_type< toggle_unit_grid>()) {
			auto with_shift = any_cast<toggle_unit_grid>(payload).with_shift;

			if(state.ui_state.unit_details_box->for_province == prov && state.ui_state.unit_details_box->is_visible() == true) {
				state.ui_state.unit_details_box->set_visible(state, false);
				if(!with_shift) {
					state.selected_armies.clear();
					state.selected_navies.clear();
				} else {
					if(prov.index() >= state.province_definitions.first_sea_province.index()) {
						for(auto n : state.world.province_get_navy_location(prov)) {
							state.deselect(n.get_navy().id);
						}
					} else {
						for(auto n : state.world.province_get_army_location(prov)) {
							state.deselect(n.get_army().id);
						}
					}
				}
			} else {
				if(!with_shift) {
					state.selected_armies.clear();
					state.selected_navies.clear();
				}

				if(prov.index() >= state.province_definitions.first_sea_province.index()) {
					for(auto n : state.world.province_get_navy_location(prov)) {
						if(state.world.navy_get_controller_from_navy_control(n.get_navy()) == state.local_player_nation) {
							state.select(n.get_navy().id);
							// Hide province window when navy is clicked.
							if(state.ui_state.province_window)
								state.ui_state.province_window->set_visible(state, false);
						}
					}
				} else {
					for(auto n : state.world.province_get_army_location(prov)) {
						if(!(n.get_army().get_navy_from_army_transport()) && n.get_army().get_controller_from_army_control() == state.local_player_nation) {
							state.select(n.get_army().id);
							// Hide province window when army is clicked.
							if(state.ui_state.province_window)
								state.ui_state.province_window->set_visible(state, false);
						}
					}
				}



				auto location = get_absolute_location(*this);

				if(state.map_state.get_zoom() >= big_counter_cutoff) {
					int32_t height = 60;
					int32_t left = -30;
					int32_t right = 31;
					if(siege->is_visible()) {
						height = 80;
						right = 48;
						left = -49;
					} else if(battle->is_visible()) {
						height = 80;
						right = 62;
						left = -62;
					}
					location.x += int16_t(left);
					location.y -= 30;
					state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(right - left), int16_t(height)}, prov, false);
				} else {
					if(siege->is_visible()) {
						location.x -= 49;
						location.y -= 10;
						state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(97), int16_t(39)}, prov, false);
					} else if(battle->is_visible()) {
						location.x -= 62;
						location.y -= 10;
						state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(124), int16_t(39)}, prov, false);
					} else {
						location.x -= 30;
						location.y -= 10;
						state.ui_state.unit_details_box->open(state, location, ui::xy_pair{int16_t(60), int16_t(19)}, prov, false);
					}
				}
			}

			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && populated)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

class rgo_icon : public image_element_base {
public:
	dcon::province_id content{};
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto mid_point = state.world.province_get_mid_point(content);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size =
				glm::vec2{float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale)};
		glm::vec2 screen_pos;
		if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos))
			return;
		auto new_position = xy_pair{int16_t(screen_pos.x - base_data.size.x / 2), int16_t(screen_pos.y - base_data.size.y / 2)};
		image_element_base::base_data.position = new_position;
		image_element_base::impl_render(state, new_position.x, new_position.y);
	}
	void on_update(sys::state& state) noexcept override {
		auto cid = state.world.province_get_rgo(content).id;
		frame = int32_t(state.world.commodity_get_icon(cid));
	}
};
} // namespace ui
