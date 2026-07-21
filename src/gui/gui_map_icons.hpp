#pragma once

#include "dcon_generated_ids.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "province.hpp"
#include "text.hpp"
#include "unit_tooltip.hpp"
#include "gui_combat.hpp"
#include "gui_units.hpp"
#include "gui_lookup.hpp"

namespace ui {

inline constexpr float big_counter_cutoff = 15.0f;
inline constexpr float prov_details_cutoff = 18.0f;

struct toggle_unit_grid {
	bool with_shift;
};

class map_image_element_base : public image_element_base {
	
};

class map_button_element_base : public button_element_base {
public:
	bool hover;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};


template<bool IsNear>
class port_ex_bg : public button_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) > 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			return button_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	void button_shift_action(sys::state& state) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			send(state, parent, toggle_unit_grid{ true });
	}
	void button_action(sys::state& state) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			send(state, parent, toggle_unit_grid{ false });
	}
};

template<bool IsNear>
class port_sm_bg : public image_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) == 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && (state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			image_element_base::render(state, x, y);
	}
};

class port_level_bar : public image_element_base {
public:
	bool visible = false;
	int32_t level = 1;

	void on_update(sys::state& state) noexcept override {
		auto prov = retrieve<dcon::province_id>(state, parent);
		auto port_level = state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::naval_base));
		visible = port_level >= level;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible && state.map_state.get_zoom() >= big_counter_cutoff)
			image_element_base::render(state, x, y);
	}
};

template<bool IsNear>
class port_ship_count : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t count = retrieve<int32_t>(state, parent);
		color = text::text_color::white;
		if(count <= 0) {
			set_text(state, "");
		} else {
			set_text(state, std::to_string(count));
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if((state.map_state.get_zoom() >= big_counter_cutoff) == IsNear)
			color_text_element::render(state, x, y);
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
			return make_element_by_type<port_ship_count<true>>(state, id);
		} else if(name == "port_minimized") {
			return make_element_by_type<port_sm_bg<true>>(state, id);
		} else if(name == "port_expanded") {
			return make_element_by_type<port_ex_bg<true>>(state, id);
		} else if(name == "port_collapsed_small") {
			return make_element_by_type<port_ex_bg<false>>(state, id);
		} else if(name == "port_collapsed_small_icon") {
			return make_element_by_type<port_sm_bg<false>>(state, id);
		} else if(name == "collapsed_ship_count") {
			return make_element_by_type<port_ship_count<false>>(state, id);
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
		auto& vertex = state.map_state.map_data.border_vertices[border.start_index + border.count / 4];

		map_x = vertex.position.x;
		map_y = vertex.position.y;
	}

	void on_update(sys::state& state) noexcept override {

		auto navies = state.world.province_get_navy_location(port_for);
		if(state.world.province_get_building_level(port_for, uint8_t(economy::province_building_type::naval_base)) == 0 && navies.begin() == navies.end()) {
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
			map_space::point_normalized_inverted_y map_pos {{map_x, 1.0f - map_y}};
			auto screen_size =
				glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			screen_space::point_ui screen_pos;
			if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f })) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(port_for)]) {
				visible = false;
				return;
			}
			visible = true;

			auto new_position = xy_pair{ int16_t(screen_pos.data.x), int16_t(screen_pos.data.y) };
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
					//if(n.get_navy().get_controller_from_navy_control() == state.local_player_nation) {
						state.select(n.get_navy().id);
						// Hide province window when navy is clicked.
						if(state.ui_state.province_window) {
							state.ui_state.province_window->set_visible(state, false);
							state.set_selected_province(dcon::province_id{});
						}
					//}
				}

				auto location = get_absolute_non_mirror_location(state, *this);
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

struct top_display_parameters {
	float top_left_value = 0.0f;
	float top_right_value = 0.0f;
	float top_left_org_value = 0.0f;
	float top_right_org_value = 0.0f;
	float battle_progress = 0.0f;
	dcon::nation_id top_left_nation;
	dcon::nation_id top_right_nation;
	dcon::rebel_faction_id top_left_rebel;
	dcon::rebel_faction_id top_right_rebel;
	int8_t top_left_status = 0;
	int8_t top_dig_in = -1;
	int8_t top_right_dig_in = -1;
	int8_t right_frames = 0;
	int8_t colors_used = 0;
	int8_t common_unit_1 = -1;
	int8_t common_unit_2 = -1;
	std::array<outline_color, 5> colors;
	outline_color top_right_color;
	bool is_army = false;
	float attacker_casualties = 0.0f;
	float defender_casualties = 0.0f;
	bool player_involved_battle = false;
	bool player_is_attacker = false;
	dcon::army_id top_army_id;
};

class map_siege : public window_element_base {
public:
	progress_bar progress;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			auto& item = progress;
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, item.base_data);
			item.on_create(state);
			item.parent = this;
		}

		return nullptr;		
	}

};

constexpr int siege_progress_shift_y_small = -23;

class prov_map_battle_bar : public progress_bar {
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
};

class prov_map_br_overlay : public image_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		image_element_base::on_create(state);
		frame = 1;
	}
 };

constexpr int casualties_shift_big_y = -80;
constexpr int casualties_shift_small_y = -38;
constexpr int casualties_attacker_shift_x = -14;
constexpr int casualties_defender_shift_x = 55;
constexpr int battle_progress_shift_small_y = -22;

class map_battle : public window_element_base {
public:
	progress_bar progress;
	prov_map_br_overlay overlay_right;
	remote_text_element_base defender_casualties;
	remote_text_element_base attacker_casualties;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "progress") {
			auto& item = progress;
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, item.base_data);
			item.on_create(state);
			item.parent = this;
		} else if(name == "overlay_right") {
			auto& item = overlay_right;
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, item.base_data);
			item.on_create(state);
			item.parent = this;
		} else if(name == "defender_casualties") {
			auto& item = defender_casualties;
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, item.base_data);
			item.parent = this;
		} else if(name == "attacker_casualties") {
			auto& item = attacker_casualties;
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(item.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, item.base_data);
			item.parent = this;
		}

		return nullptr;		
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(state.map_state.get_zoom() >= big_counter_cutoff)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return window_element_base::impl_probe_mouse(state, x, y + 23, type);
	}
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		int shift_bar = 0;
		int shift_cas = casualties_shift_big_y;
		if(state.map_state.get_zoom() < big_counter_cutoff) {
			shift_bar = battle_progress_shift_small_y;
			shift_cas = casualties_shift_small_y;
		}

		{
			auto& item = progress;
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x, y + shift.y + shift_bar);
		}
		{
			auto& item = overlay_right;
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x, y + shift.y + shift_bar);
		}
		{
			auto& item = defender_casualties;
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x + casualties_defender_shift_x, y + shift.y + shift_cas + shift_bar);
		}
		{
			auto& item = attacker_casualties;
			auto shift = child_relative_location(state, *this, item);
			item.impl_render(state, x + shift.x + casualties_attacker_shift_x, y + shift.y + shift_cas + shift_bar);
		}
	}
};

class tl_edge : public image_element_base {
public:
	bool visible = false;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class tl_unit_1 : public image_element_base {
public:
	bool visible = true;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class tl_unit_2 : public image_element_base {
public:
	bool visible = true;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

// A component for a singular standing army
class top_unit_icon : public window_element_base {
public:
	map_button_element_base frame;
	tl_edge edge[4];
	tl_unit_1 unit1;
	tl_unit_2 unit2;
	bool show_flag;
	external_flag_button flag;
	remote_text_element_base strength_label;
	image_element_base dig_in;
	progress_bar org_bar;
	image_element_base status;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void on_create(sys::state& state) noexcept override;
};


class small_top_unit_icon : public window_element_base {
public:
	remote_text_element_base strength_label;
	external_flag_button flag;
	map_button_element_base frame;
	bool has_attrition;
	image_element_base attrition_icon;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;
};


class army_group_icon : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frame_bg") {
			return make_element_by_type<select_army_group_button>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::automated_army_group_id>()) {
			payload.emplace<dcon::automated_army_group_id>(retrieve<dcon::automated_army_group_id>(state, parent));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class army_group_counter_window : public window_element_base {
public:
	bool populated = false;
	bool visible = true;

	dcon::automated_army_group_id info {};
	
	element_base* main_icon = nullptr;
	dcon::province_id prov;
	outline_color color;

	xy_pair base_size;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_size = window_element_base::base_data.size;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "main_army_group_icon") {
			auto ptr = make_element_by_type<army_group_icon>(state, id);
			main_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		populated = false;

		state.world.for_each_automated_army_group([&](dcon::automated_army_group_id item) {
			auto hq = state.world.automated_army_group_get_hq(item);
			if(hq == prov) {
				info = item;
				populated = true;
			}
		});
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(populated) {
			auto mid_point = state.world.province_get_mid_point(prov);
			auto map_pos = state.map_state.normalize_map_coord(mid_point);
			auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			screen_space::point_ui screen_pos;
			if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f })) {
				visible = false;
				return;
			}
			if(!state.map_state.visible_provinces[province::to_map_id(prov)]) {
				visible = false;
				return;
			}
			visible = true;

			auto new_position = xy_pair{ int16_t(screen_pos.data.x), int16_t(screen_pos.data.y) };
			window_element_base::base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::automated_army_group_id>()) {
			if(populated) payload.emplace<dcon::automated_army_group_id>(info);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

enum unit_counter_part {
	flag,
	frame,
	right_flag,
	right_frame,
	battle,
	siege
};

inline bool test_rect(int x, int y, xy_pair shift, element_data base_data) {
	x -= shift.x;
	y -= shift.y;
	return x >= 0 && y >= 0 && x <= base_data.size.x && y <= base_data.size.y;
}

inline xy_pair add_shift(xy_pair A, xy_pair B) {
	return { (int16_t)(A.x + B.x), (int16_t)(A.y + B.y) };
}

constexpr int battle_shift_big = -38;
constexpr int battle_shift_small = -48;

class unit_counter_window : public window_element_base {
public:
	dcon::province_id current_province;

	dcon::province_id hovered_icon;
	unit_counter_part hovered_part;

	top_unit_icon top_icon {}; // Attacker
	top_unit_icon top_right_icon {}; // Defender; Displayed exclusively in land and naval battles
	small_top_unit_icon small_top_icon{};  // Attacker
	small_top_unit_icon small_top_right_icon{}; // Defender;Displayed exclusively in land and naval battles

	map_siege siege{};
	map_battle battle{};

	std::vector<top_display_parameters> display_cache;

	std::vector<float> strength_cache { };
	std::vector<text::layout> strength_layout_cache{ };
	std::vector<text::layout> strength_layout_small_cache{ };

	std::vector<float> strength_right_cache{ };
	std::vector<text::layout> strength_right_layout_cache{ };
	std::vector<text::layout> strength_right_layout_small_cache{ };

	std::vector<float> casualties_cache{ };
	std::vector<text::layout> casualties_layout_cache{ };

	std::vector<float> casualties_right_cache{ };
	std::vector<text::layout> casualties_right_layout_cache{ };

	std::vector<uint8_t> visible_counters { };
	std::vector<uint8_t> populated_counters{ };
	std::vector<uint8_t> has_attrition{ };
	std::vector<uint8_t> active_battles{ };
	std::vector<uint8_t> active_siege{ };

	std::vector<GLuint> flag_texture_handles{ };
	std::vector<GLuint> flag_right_texture_handles{ };

	std::vector<glm::vec2> last_screen_coordinates { };
	bool lack_of_movement_hypothesis = false;
	float decaying_max_shift = 0.f;

	void on_update(sys::state& state) noexcept override;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "top_unit_icon") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(top_icon.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, top_icon.base_data);
			top_icon.on_create(state);
			top_icon.parent = this;
		} else if(name == "top_right_unit_icon") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(top_right_icon.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, top_right_icon.base_data);
			top_right_icon.on_create(state);
			top_right_icon.parent = this;
		} else if(name == "small_top_unit_icon") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(small_top_icon.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, small_top_icon.base_data);
			small_top_icon.on_create(state);
			small_top_icon.parent = this;
		} else if(name == "small_top_right_unit_icon") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(small_top_right_icon.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, small_top_right_icon.base_data);
			small_top_right_icon.on_create(state);
			small_top_right_icon.parent = this;
		} else if(name == "siege") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(siege.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, siege.base_data);
			siege.on_create(state);
			siege.parent = this;
		} else if(name == "battle") {
			auto& def = ui::get_element_data(state, id);
			std::memcpy(&(battle.base_data), &def, sizeof(ui::element_data));
			make_size_from_graphics(state, battle.base_data);
			battle.on_create(state);
			battle.parent = this;
		}
		return nullptr;
	}

	void impl_on_update(sys::state& state) noexcept override {
		on_update(state);
	}
	template<class T, bool Is_Top_Left>
	void set_found_flags(T ar) {
		if constexpr(Is_Top_Left) {

		}
	}

	void on_create(sys::state& state) noexcept override {		
		visible_counters.resize(state.world.province_size());
		populated_counters.resize(state.world.province_size());
		display_cache.resize(state.world.province_size());

		strength_cache.resize(state.world.province_size());
		strength_layout_cache.resize(state.world.province_size());
		strength_layout_small_cache.resize(state.world.province_size());
		strength_right_cache.resize(state.world.province_size());
		strength_right_layout_cache.resize(state.world.province_size());
		strength_right_layout_small_cache.resize(state.world.province_size());

		casualties_cache.resize(state.world.province_size());
		casualties_layout_cache.resize(state.world.province_size());

		casualties_right_cache.resize(state.world.province_size());
		casualties_right_layout_cache.resize(state.world.province_size());

		flag_texture_handles.resize(state.world.province_size());
		flag_right_texture_handles.resize(state.world.province_size());

		last_screen_coordinates.resize(state.world.province_size());
		has_attrition.resize(state.world.province_size());

		active_battles.resize(state.world.province_size());
		active_siege.resize(state.world.province_size());

		//window_element_base::on_create(state);

		auto first_child = base_data.data.window.first_child;
		auto num_children = base_data.data.window.num_children;
		for(uint32_t i = num_children; i-- > 0;) {
			auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i + first_child.index()));
			make_child(state, state.to_string_view(state.ui_defs.gui[child_tag].name), child_tag);
		}
	}

	void set_province(sys::state& state, dcon::province_id prov) {
		if(!prov) {			
			top_icon.strength_label.external_layout = nullptr;
			top_right_icon.strength_label.external_layout = nullptr;
			small_top_icon.strength_label.external_layout = nullptr;
			small_top_right_icon.strength_label.external_layout = nullptr;
			return;
		}

		if(!populated_counters[prov.index()]) {
			return;
		}

		current_province = prov;		
		auto mid_point = state.world.province_get_mid_point(current_province);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
		screen_space::point_ui screen_pos;

		//if(lack_of_movement_hypothesis && visible_counters[current_province.index()] == 0) {
			//return;
		//}

		if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f })) {
			visible_counters[current_province.index()] = 0;
			return;
		}
		if(!state.map_state.visible_provinces[province::to_map_id(current_province)]) {
			visible_counters[current_province.index()] = 0;
			return;
		}

		visible_counters[current_province.index()] = 1;
		auto shift = screen_pos.data - last_screen_coordinates[current_province.index()];
		if(abs(shift.x) + abs(shift.y) > 50.f) {
			lack_of_movement_hypothesis = false;
		}
		decaying_max_shift = std::max(abs(shift.x) + abs(shift.y), decaying_max_shift * 0.99f);
		if(decaying_max_shift < 2.f) {
			lack_of_movement_hypothesis = true;
		}

		last_screen_coordinates[current_province.index()] = screen_pos.data;
		auto new_position = xy_pair{ int16_t(screen_pos.data.x), int16_t(screen_pos.data.y) };
		window_element_base::base_data.position = new_position;

		top_icon.strength_label.external_layout = &strength_layout_cache[prov.index()];
		top_right_icon.strength_label.external_layout = &strength_right_layout_cache[prov.index()];
		small_top_icon.strength_label.external_layout = &strength_layout_small_cache[prov.index()];
		small_top_right_icon.strength_label.external_layout = &strength_right_layout_small_cache[prov.index()];
		battle.attacker_casualties.external_layout = &casualties_layout_cache[prov.index()];
		battle.defender_casualties.external_layout = &casualties_right_layout_cache[prov.index()];

		auto& params = display_cache[current_province.index()];

		battle.progress.progress = params.battle_progress;
		siege.progress.progress = state.world.province_get_siege_progress(prov);

		top_icon.frame.frame = int32_t(params.colors[0]);
		top_right_icon.frame.frame = int32_t(params.top_right_color);

		for(int i = 0; i < 4; i++) {
			if ((i + 1) >= params.colors_used) {
				top_icon.edge[i].visible = false;
			} else {
				top_icon.edge[i].visible = true;
				top_icon.edge[i].frame = int32_t(params.colors[i + 1]);
			}
		}

		top_icon.flag.rebel_faction = params.top_left_rebel;
		top_right_icon.flag.rebel_faction = params.top_right_rebel;
		small_top_icon.flag.rebel_faction = params.top_left_rebel;
		small_top_right_icon.flag.rebel_faction = params.top_right_rebel;

		top_icon.flag.nation = params.top_left_nation;
		top_right_icon.flag.nation = params.top_right_nation;
		small_top_icon.flag.nation = params.top_left_nation;
		small_top_right_icon.flag.nation = params.top_right_nation;

		{
			auto related_nation = top_icon.flag.nation;
			if(!related_nation && top_icon.flag.identity) {
				related_nation = state.world.national_identity_get_nation_from_identity_holder(top_icon.flag.identity);
			}
			if(!related_nation && top_icon.flag.rebel_faction) {
				related_nation = state.world.rebel_faction_get_ruler_from_rebellion_within(top_icon.flag.rebel_faction);
			}
			top_icon.flag.related_nation = related_nation;
			small_top_icon.flag.related_nation = related_nation;
		}

		{
			auto related_nation = top_right_icon.flag.nation;
			if(!related_nation && top_right_icon.flag.identity) {
				related_nation = state.world.national_identity_get_nation_from_identity_holder(top_right_icon.flag.identity);
			}
			if(!related_nation && top_right_icon.flag.rebel_faction) {
				related_nation = state.world.rebel_faction_get_ruler_from_rebellion_within(top_right_icon.flag.rebel_faction);
			}
			top_right_icon.flag.related_nation = related_nation;
			small_top_right_icon.flag.related_nation = related_nation;
		}

		top_icon.flag.flag_texture_handle = flag_texture_handles[current_province.index()];
		small_top_icon.flag.flag_texture_handle = flag_texture_handles[current_province.index()];
		top_right_icon.flag.flag_texture_handle = flag_right_texture_handles[current_province.index()];
		small_top_right_icon.flag.flag_texture_handle = flag_right_texture_handles[current_province.index()];

		top_icon.show_flag = state.local_player_nation ? top_icon.flag.nation != state.local_player_nation : true;
		top_right_icon.show_flag = state.local_player_nation ? top_right_icon.flag.nation != state.local_player_nation : true;

		top_icon.org_bar.progress = params.top_left_org_value;
		top_right_icon.org_bar.progress = params.top_right_org_value;

		if(params.top_left_status >= 0) {
			top_icon.status.frame = params.top_left_status;
		} else {
			top_icon.status.frame = 0;
		}

		/*
		if(params.top_right_status >= 0) {
			top_right_icon.status.frame = params.top_right_status;
		} else {
			top_right_icon.status.frame = 0;
		}
		*/

		if(params.top_dig_in >= 0) {
			top_icon.dig_in.frame = params.top_dig_in;
		} else {
			top_icon.dig_in.frame = 0;
		}

		if(params.top_right_dig_in >= 0) {
			top_right_icon.dig_in.frame = params.top_right_dig_in;
		} else {
			top_right_icon.dig_in.frame = 0;
		}

		// If the army has a general - don't display unit previews
		if((state.defines.alice_render_on_map_generals == 0.f || !state.world.army_get_general_from_army_leadership(params.top_army_id))) {
			if(params.common_unit_1 >= 0) {
				top_icon.unit1.frame = params.common_unit_1;
				top_icon.unit1.visible = true;
			} else {
				top_icon.unit1.visible = false;
			}
			if(params.common_unit_2 >= 0) {
				top_icon.unit2.frame = params.common_unit_2;
				top_icon.unit2.visible = true;
			} else {
				top_icon.unit2.visible = false;
			}
		}

		small_top_icon.has_attrition = has_attrition[current_province.index()];	
	}


	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		// check if we are the hovered one

		bool hovered = state.ui_state.under_mouse == this;

		if(state.map_state.get_zoom() >= big_counter_cutoff) {
			// use big counters
			for (auto prov : state.world.in_province) {
				set_province(state, prov);
				auto populated = populated_counters[prov.id.index()];
				auto visible = visible_counters[prov.id.index()];
				if(!(populated && visible)) continue;

				top_icon.frame.hover = hovered_icon == prov.id;
				x = base_data.position.x;
				y = base_data.position.y;

				{
					auto shift = child_relative_location(state, *this, top_icon);
					if (active_battles[prov.id.index()]) {						
						top_icon.impl_render(state, x + shift.x + battle_shift_big, y + shift.y);
					} else {
						top_icon.impl_render(state, x + shift.x, y + shift.y);
					}
				}
				if(active_battles[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, top_right_icon);
					top_right_icon.impl_render(state, x + shift.x, y + shift.y);
				}
				if(active_battles[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, battle);
					battle.impl_render(state, x + shift.x, y + shift.y);
				}
				if(!active_battles[prov.id.index()] && active_siege[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, siege);
					siege.impl_render(state, x + shift.x, y + shift.y);
					auto shift2 = child_relative_location(state, siege, siege.progress);
					siege.progress.impl_render(state, x + shift.x + shift2.x, y + shift.y + shift2.y);
				}
			}
		} else {
			// use small counters
			for (auto prov : state.world.in_province) {
				set_province(state, prov);
				auto populated = populated_counters[prov.id.index()];
				auto visible = visible_counters[prov.id.index()];
				if(!(populated && visible)) continue;

				small_top_icon.frame.hover = hovered_icon == prov.id;
				x = base_data.position.x;
				y = base_data.position.y;

				{
					auto shift = child_relative_location(state, *this, small_top_icon);
					if (active_battles[prov.id.index()]) {
						small_top_icon.impl_render(state, x + shift.x + battle_shift_small, y + shift.y);
					} else {
						small_top_icon.impl_render(state, x + shift.x, y + shift.y);
					}
				}
				if(active_battles[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, small_top_right_icon);
					small_top_right_icon.impl_render(state, x + shift.x, y + shift.y);
				}
				if(active_battles[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, battle);
					battle.impl_render(state, x + shift.x, y + shift.y);
				}
				if(!active_battles[prov.id.index()] && active_siege[prov.id.index()]) {
					auto shift = child_relative_location(state, *this, siege);
					siege.impl_render(state, x + shift.x, y + shift.y + siege_progress_shift_y_small);
					auto shift2 = child_relative_location(state, siege, siege.progress);
					siege.progress.impl_render(state, x + shift.x + shift2.x, y + shift.y + shift2.y + siege_progress_shift_y_small);
				}
			}
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(current_province);
			return message_result::consumed;
		} else if(payload.holds_type<top_display_parameters*>()) {
			payload.emplace<top_display_parameters*>(&display_cache[current_province.index()]);
			return message_result::consumed;
		} else if(payload.holds_type< toggle_unit_grid>()) {
			

			return message_result::consumed;
		}
		return message_result::unseen;
	}

	bool probe_specific_province(sys::state& state, dcon::province_id prov, int32_t mx, int32_t my, mouse_probe_type type) {
		if(!prov) return false;
		auto populated = populated_counters[prov.index()];
		auto visible = visible_counters[prov.index()];
		if(!(populated && visible)) return false;

		auto coord = last_screen_coordinates[prov.index()];
		auto x = mx - (int32_t)coord.x;
		auto y = my - (int32_t)coord.y;

		if(state.map_state.get_zoom() >= big_counter_cutoff) {
			auto to_main = child_relative_location(state, *this, top_icon);
			auto to_right_main = child_relative_location(state, *this, top_right_icon);
			if(active_battles[prov.index()]) {
				to_main.x += battle_shift_big;
			}

			auto to_flag = add_shift(to_main, child_relative_location(state, top_icon, top_icon.flag));
			if(test_rect(x, y, to_flag, top_icon.flag.base_data)) {
				hovered_part = unit_counter_part::flag;
				hovered_icon = prov;
				return true;
			}

			auto to_frame = add_shift(to_main, child_relative_location(state, top_icon, top_icon.frame));
			if(test_rect(x, y, to_frame, top_icon.frame.base_data)) {
				hovered_part = unit_counter_part::frame;
				hovered_icon = prov;
				return true;
			}

			auto to_right_flag = add_shift(to_main, child_relative_location(state, top_right_icon, top_right_icon.flag));
			if(active_battles[prov.index()] && test_rect(x, y, to_right_flag, top_right_icon.flag.base_data)) {				
				hovered_part = unit_counter_part::right_flag;
				hovered_icon = prov;
				return true;
			}

			auto to_right_frame = add_shift(to_main, child_relative_location(state, top_right_icon, top_right_icon.frame));
			if(active_battles[prov.index()] && test_rect(x, y, to_right_frame, top_right_icon.frame.base_data)) {
				hovered_part = unit_counter_part::right_frame;
				hovered_icon = prov;
				return true;
			}

			auto to_battle = add_shift(child_relative_location(state, *this, battle), child_relative_location(state, battle, battle.progress));
			if(active_battles[prov.index()] && test_rect(x, y, to_battle, battle.base_data)) {
				hovered_part = unit_counter_part::battle;
				hovered_icon = prov;
				return true;
			}
			
			auto to_siege = add_shift(child_relative_location(state, *this, siege), child_relative_location(state, siege, siege.progress));
			if(!active_battles[prov.index()] && active_siege[prov.index()] && test_rect(x, y, to_siege, siege.base_data)) {
				hovered_part = unit_counter_part::siege;
				hovered_icon = prov;
				return true;
			}
		} else {
			auto to_main = child_relative_location(state, *this, small_top_icon);
			auto to_right_main = child_relative_location(state, *this, small_top_right_icon);
			if(active_battles[prov.index()]) {
				to_main.x += battle_shift_small;
			}

			auto to_flag = add_shift(to_main, child_relative_location(state, small_top_icon, small_top_icon.flag));
			if(test_rect(x, y, to_flag, small_top_icon.flag.base_data)) {
				hovered_part = unit_counter_part::flag;
				hovered_icon = prov;
				return true;
			}

			auto to_frame = add_shift(to_main, child_relative_location(state, small_top_icon, small_top_icon.frame));
			if(test_rect(x, y, to_frame, small_top_icon.frame.base_data)) {
				hovered_part = unit_counter_part::frame;
				hovered_icon = prov;
				return true;
			}

			auto to_right_flag = add_shift(to_main, child_relative_location(state, small_top_right_icon, small_top_right_icon.flag));
			if(active_battles[prov.index()] && test_rect(x, y, to_right_flag, small_top_right_icon.flag.base_data)) {
				hovered_part = unit_counter_part::right_flag;
				hovered_icon = prov;
				return true;
			}

			auto to_right_frame = add_shift(to_main, child_relative_location(state, small_top_right_icon, small_top_right_icon.frame));
			if(active_battles[prov.index()] && test_rect(x, y, to_right_frame, small_top_right_icon.frame.base_data)) {
				hovered_part = unit_counter_part::right_frame;
				hovered_icon = prov;
				return true;
			}
			
			auto to_battle = add_shift(child_relative_location(state, *this, battle), child_relative_location(state, battle, battle.progress));
			to_battle.y += battle_progress_shift_small_y;
			if(active_battles[prov.index()] && test_rect(x, y, to_battle, battle.base_data)) {
				hovered_part = unit_counter_part::battle;
				hovered_icon = prov;
				return true;
			}

			auto to_siege = add_shift(child_relative_location(state, *this, siege), child_relative_location(state, siege, siege.progress));
			to_siege.y += siege_progress_shift_y_small;
			if(!active_battles[prov.index()] && active_siege[prov.index()] && test_rect(x, y, to_siege, siege.base_data)) {
				hovered_part = unit_counter_part::siege;
				hovered_icon = prov;
				return true;
			}
		}

		return false;
	}

	void tooltip_position(sys::state& state, int32_t x, int32_t y, int32_t& ident, ui::urect& subrect) noexcept override {
		ident = hovered_icon.index() + state.world.province_size() * (int32_t)hovered_part;
		subrect.top_left = ui::get_absolute_location(state, *this);
		subrect.size = base_data.size;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t mx, int32_t my, mouse_probe_type type) noexcept override {		
		mouse_probe probe_result = mouse_probe{nullptr, xy_pair{int16_t(mx), int16_t(my)}};

		// check cached result first
		if(probe_specific_province(state, hovered_icon, mx, my, type)) {
			probe_result.under_mouse = this;
			return probe_result;
		}

		for (auto prov : state.world.in_province) {
			auto populated = populated_counters[prov.id.index()];
			auto visible = visible_counters[prov.id.index()];
			auto coord = last_screen_coordinates[prov.id.index()];
			auto x = mx - (int32_t)coord.x;
			auto y = my - (int32_t)coord.y;
			if (!(populated && visible)) continue;

			if(probe_specific_province(state, prov.id, mx, my, type)) {
				probe_result.under_mouse = this;
				return probe_result;
			}
		}


		hovered_icon = {};
		return mouse_probe{ nullptr, ui::xy_pair{} };
	}

	void button_action(sys::state& state) noexcept;
	void button_shift_action(sys::state& state) noexcept;
	tooltip_behavior has_tooltip(sys::state& state) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void toogle_grid(sys::state& state, dcon::province_id target, bool with_shift);
	void button_right_action(sys::state& state) noexcept;
	virtual void button_shift_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_shift_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual void button_ctrl_shift_right_action(sys::state& state) noexcept {
		button_action(state);
	}
	virtual sound::audio_instance& get_click_sound(sys::state& state) noexcept {
		return sound::get_click_sound(state);
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	message_result on_lbutton_up(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods, bool under_mouse) noexcept override;
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept override;
};

class map_colonization_flag : public flag_button {
	dcon::national_identity_id get_current_nation(sys::state& state) noexcept override {
		auto colonization = retrieve<dcon::colonization_id>(state, parent);
		auto national_ident = state.world.nation_get_identity_from_identity_holder( state.world.colonization_get_colonizer( colonization));
		return national_ident;
	}
};

class map_colonization_progress : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto colonization = retrieve<dcon::colonization_id>(state, parent);
		if (colonization) {
			auto state_def = retrieve<dcon::state_definition_id>(state, parent);
			auto col_range = state.world.state_definition_get_colonization(state_def);
			uint8_t level;
			// If can instantly protectorate it with no competition, displayed colonization level shall be atleast 5
			if (col_range.end() - col_range.begin() == 1 && state.world.state_definition_get_colonization_stage(state_def) >= uint8_t(3)) { // no competition and atleast stage 3
				level = std::max(state.world.colonization_get_level(colonization), uint8_t(5));
			}
			else {
				level = state.world.colonization_get_level(colonization);
			}
			set_text(state, std::to_string(level));
		}
	
	}
};


class map_colonization_icon : public window_element_base {

	dcon::colonization_id get_colonization(sys::state& state) {
		auto state_colonization = state.world.state_definition_get_colonization(state_def);
		if(colonization_index >= state_colonization.end() - state_colonization.begin()) {
			return dcon::colonization_id{ };
		} else {
			auto colonization_iterator = state_colonization.begin() + colonization_index;
			return *colonization_iterator;
		}
	}

public:
	dcon::state_definition_id state_def;
	uint32_t colonization_index;
	bool visible = false;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "colonization_panel_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "colonization_progress") {
			return make_element_by_type<map_colonization_progress>(state, id);
		} else if(name == "colonization_country_flag") {
			return make_element_by_type<map_colonization_flag>(state, id);
		} else {
			return nullptr;
		}
	}
	void set_state_def(sys::state& state, dcon::state_definition_id def) {
		state_def = def;

	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto colonization = get_colonization(state);
		dcon::province_id render_prov;
		int32_t highest_prio = -1;
		if(colonization) {
			for (auto prov : state.world.state_definition_get_abstract_state_membership(state_def)) {
				// grab first uncolonized province
				if (!prov.get_province().get_province_ownership().get_nation()) {
					render_prov = prov.get_province();
					break;
				}
			}
			auto midpoint = state.world.province_get_mid_point(render_prov);
			map_space::point_normalized_inverted_y map_pos = state.map_state.normalize_map_coord(midpoint);
			auto screen_size =
				glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
			screen_space::point_ui screen_pos;
			if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f })) {
				visible = false;
				return;
			}
			visible = true;
			// reduce coordinates by half the element's size so that they are centered on province
			screen_pos.data.x -= float(base_data.size.x / 2.0f);
			screen_pos.data.y -= float(base_data.size.y / 2.0f);
			screen_pos.data.y += float(colonization_index * base_data.size.y); // Nudge the next element downwards so they don't overlap
			auto new_position = xy_pair{ int16_t(screen_pos.data.x), int16_t(screen_pos.data.y) };
			window_element_base::base_data.position = new_position;
			window_element_base::impl_render(state, new_position.x, new_position.y);
		}
	}


	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_definition_id>()) {
			payload.emplace<dcon::state_definition_id>(state_def);
			return message_result::consumed;
		}
		else if(payload.holds_type<dcon::colonization_id>()) {
			auto colonization = get_colonization(state);
			if(colonization) {
				payload.emplace<dcon::colonization_id>(colonization);
				return message_result::consumed;
			}
			else {
				payload.emplace< dcon::colonization_id>(dcon::colonization_id{ });
				return message_result::consumed;
			}
			
		}
		return message_result::unseen;
	}
};

class map_pv_rail_dots : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 6 - state.world.province_get_building_level(p, uint8_t(economy::province_building_type::railroad));
	}
};
class map_pv_fort_dots : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto p = retrieve<dcon::province_id>(state, parent);
		frame = 6 - state.world.province_get_building_level(p, uint8_t(economy::province_building_type::fort));
	}
};

class map_pv_bank : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), uint8_t(economy::province_building_type::bank));
			last_update = state.ui_date;
		}
		image_element_base::render(state, x, y);
		ogl::color3f color{ 0.f, 0.f, 0.f };
		//ogl::render_text(state, &cached_level, 1, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), color, 1);
	}
};

class map_pv_university : public image_element_base {
public:
	sys::date last_update;
	char cached_level = 0;

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(last_update != state.ui_date) {
			cached_level = '0' + state.world.province_get_building_level(retrieve<dcon::province_id>(state, parent), uint8_t(economy::province_building_type::university));
			last_update = state.ui_date;
		}
		image_element_base::render(state, x, y);
		ogl::color3f color{ 0.f, 0.f, 0.f };
		//ogl::render_text(state, &cached_level, 1, ogl::color_modification::none, float(x + 16 + 1.0f), float(y + 1.0f), color, 1);
	}
};

class province_details_container : public window_element_base {
public:
	
	dcon::province_id prov;
	sys::date last_update;
	bool visible = false;
	
	element_base* capital_icon = nullptr;
	element_base* rails_icon = nullptr;
	element_base* rails_dots = nullptr;
	element_base* fort_icon = nullptr;
	element_base* fort_dots = nullptr;
	element_base* bank_icon = nullptr;
	element_base* unv_icon = nullptr;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "capital_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			capital_icon = ptr.get();
			return ptr;
		} else if(name == "rail_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			rails_icon = ptr.get();
			return ptr;
		} else if(name == "rail_dots") {
			auto ptr = make_element_by_type<map_pv_rail_dots>(state, id);
			rails_dots = ptr.get();
			return ptr;
		} else if(name == "fort_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			fort_icon = ptr.get();
			return ptr;
		} else if(name == "fort_dots") {
			auto ptr = make_element_by_type<map_pv_fort_dots>(state, id);
			fort_dots = ptr.get();
			return ptr;
		} else if(name == "bank_icon") {
			auto ptr = make_element_by_type<map_pv_bank>(state, id);
			bank_icon = ptr.get();
			return ptr;
		} else if(name == "university_icon") {
			auto ptr = make_element_by_type<map_pv_university>(state, id);
			unv_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void impl_on_update(sys::state& state) noexcept override {
		if(!visible)
			return;
		if(last_update && state.ui_date == last_update)
			return;

		last_update = state.ui_date;
		on_update(state);

		for(auto& c : children) {
			if(c->is_visible()) {
				c->impl_on_update(state);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		int32_t rows = 0;
		if(state.world.nation_get_capital(state.world.province_get_nation_from_province_ownership(prov)) == prov) {
			capital_icon->set_visible(state, true);
			++rows;
		} else {
			capital_icon->set_visible(state, false);
		}
		if(state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::railroad)) != 0) {
			++rows;
			rails_icon->set_visible(state, true);
			rails_dots->set_visible(state, true);
		} else {
			rails_icon->set_visible(state, false);
			rails_dots->set_visible(state, false);
		}
		if(state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::fort)) != 0) {
			++rows;
			fort_icon->set_visible(state, true);
			fort_dots->set_visible(state, true);
		} else {
			fort_icon->set_visible(state, false);
			fort_dots->set_visible(state, false);
		}
		if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::university)) != 0)
			|| (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::bank)) != 0)) {
			++rows;
		} else {
			bank_icon->set_visible(state, false);
			unv_icon->set_visible(state, false);
		}

		auto top = (-16 * rows) / 2;
		if(state.world.nation_get_capital(state.world.province_get_nation_from_province_ownership(prov)) == prov) {
			capital_icon->base_data.position.y = int16_t(top - 2);
			capital_icon->base_data.position.x = int16_t(-10);
			top += 16;
		}
		if(state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::railroad)) != 0) {
			rails_icon->base_data.position.y = int16_t(top );
			rails_dots->base_data.position.y = int16_t(top);
			int32_t total_width = 18 + 2 + 3 + 4 * state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::railroad));
			rails_icon->base_data.position.x = int16_t(-total_width / 2);
			rails_dots->base_data.position.x = int16_t(20 -total_width / 2);
			top += 16;
		}
		if(state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::fort)) != 0) {
			fort_icon->base_data.position.y = int16_t(top);
			fort_dots->base_data.position.y = int16_t(top);
			int32_t total_width = 18 + 2 + 3 + 4 * state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::fort));
			fort_icon->base_data.position.x = int16_t(-total_width / 2);
			fort_dots->base_data.position.x = int16_t(20 - total_width / 2);
			top += 16;
		}
		if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::university)) != 0)
			|| (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::bank)) != 0)) {


			if((state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::university)) != 0)
			&& (state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::bank)) != 0)) {

				unv_icon->base_data.position.y = int16_t(top);
				unv_icon->base_data.position.x = int16_t(0);
				bank_icon->base_data.position.y = int16_t(top);
				bank_icon->base_data.position.x = int16_t(-32);
				bank_icon->set_visible(state, true);
				unv_icon->set_visible(state, true);
			} else if(state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::university)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::university)) != 0) {

				unv_icon->base_data.position.y = int16_t(top);
				unv_icon->base_data.position.x = int16_t(-16);
				bank_icon->set_visible(state, false);
				unv_icon->set_visible(state, true);
			} else if(state.economy_definitions.building_definitions[uint32_t(economy::province_building_type::bank)].defined && state.world.province_get_building_level(prov, uint8_t(economy::province_building_type::bank)) != 0) {

				bank_icon->base_data.position.y = int16_t(top);
				bank_icon->base_data.position.x = int16_t(-16);
				bank_icon->set_visible(state, true);
				unv_icon->set_visible(state, false);
			}
		}
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		auto mid_point = state.world.province_get_mid_point(prov);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size = glm::vec2{ float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale) };
		screen_space::point_ui screen_pos;

		if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f })) {
			visible = false;
			return;
		}

		if(screen_pos.data.x < -32 || screen_pos.data.y < -32 || screen_pos.data.x > state.ui_state.root->base_data.size.x + 32 || screen_pos.data.y > state.ui_state.root->base_data.size.y + 32) {
			visible = false;
			return;
		}
		if(visible == false) {
			visible = true;
			impl_on_update(state);
		}
			

		auto new_position = xy_pair{ int16_t(screen_pos.data.x), int16_t(screen_pos.data.y) };
		window_element_base::base_data.position = new_position;

		window_element_base::impl_render(state, new_position.x, new_position.y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(prov);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
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
		screen_space::point_ui  screen_pos;
		if(!state.map_state.map_to_screen(map_pos, screen_size, state.user_settings.map_is_globe, screen_pos, { 200.f, 200.f }))
			return;
		auto new_position = xy_pair{int16_t(screen_pos.data.x - base_data.size.x / 2), int16_t(screen_pos.data.y - base_data.size.y / 2)};
		image_element_base::base_data.position = new_position;
		image_element_base::impl_render(state, new_position.x, new_position.y);
	}
	void on_update(sys::state& state) noexcept override {
		auto cid = state.world.province_get_rgo(content).id;
		frame = int32_t(state.world.commodity_get_icon(cid));
	}
};
} // namespace ui
