#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "province.hpp"
#include "text.hpp"
#include "unit_tooltip.hpp"

namespace ui {

enum class outline_color {
	gray = 0, gold = 1, blue = 2, cyan = 3, red = 4
};

using unit_var = std::variant<std::monostate, dcon::army_id, dcon::navy_id>;

class unit_frame_bg : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	// TODO: select unit
};

class unit_org_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		float avg = 0;
		if(std::holds_alternative<dcon::army_id>(u)) {
			float total = 0;
			float count = 0;
			auto a = std::get<dcon::army_id>(u);
			for(auto r : state.world.army_get_army_membership(a)) {
				total += r.get_regiment().get_org();
				++count;
			}
			if(count > 0) {
				avg = total / count;
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			float total = 0;
			float count = 0;
			auto a = std::get<dcon::navy_id>(u);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				total += r.get_ship().get_org();
				++count;
			}
			if(count > 0) {
				avg = total / count;
			}
		}
		progress = avg;
	}
};

class unit_status_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);

		

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);

			auto is_understr = [&]() {
				for(auto m : state.world.army_get_army_membership(a)) {
					if(m.get_regiment().get_strength() < 1.0f)
						return true;
				}
				return false;
			};
			if(state.world.army_get_black_flag(a)) {
				frame = 1;
			} else if(state.world.army_get_is_retreating(a)) {
				frame = 2;
			} else if(state.world.army_get_battle_from_army_battle_participation(a)) {
				frame = 6;
			} else if(state.world.army_get_navy_from_army_transport(a)) {
				frame = 5;
			} else if(military::will_recieve_attrition(state, a)) {
				frame = 3;
			} else if(is_understr()) {
				frame = 4;
			} else {
				frame = 0;
			}
			
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			
			auto a = std::get<dcon::navy_id>(u);
			auto trange = state.world.navy_get_army_transport(a);

			auto is_understr = [&]() {
				for(auto m : state.world.navy_get_navy_membership(a)) {
					if(m.get_ship().get_strength() < 1.0f)
						return true;
				}
				return false;
			};

			auto nb_level = state.world.province_get_naval_base_level(state.world.navy_get_location_from_navy_location(a));
			

			if(state.world.navy_get_is_retreating(a)) {
				frame = 2;
			} else if(state.world.navy_get_battle_from_navy_battle_participation(a)) {
				frame = 6;
			} else if(military::will_recieve_attrition(state, a)) {
				frame = 3;
			} else if(trange.begin() != trange.end()) {
				frame = 5;
			} else if(!(state.world.navy_get_arrival_time(a)) && nb_level > 0 && is_understr()) {
				frame = 4;
			} else{
				frame = 0;
			}
			
		}
		
	}
};

class unit_strength : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		color = text::text_color::gold;
		float total = 0.0f;

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);

			for(auto m : state.world.army_get_army_membership(a)) {
				total += m.get_regiment().get_strength();
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			
			for(auto m : state.world.navy_get_navy_membership(a)) {
				total += m.get_ship().get_strength();
			}
		}

		set_text(state, text::format_float(total, 1));
	}
};

class unit_dig_in_pips_image : public image_element_base {
public:
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			auto dig_in = state.world.army_get_dig_in(a);
			frame = dig_in;
			visible = true;
		} else {
			visible = false;
		}

	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible) {
			image_element_base::render(state, x, y);
		}
	}
};

class unit_most_prevalent : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);

			for(auto m : state.world.navy_get_navy_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		}

		int32_t max_index = 0;
		for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
			if(by_icon_count[i] > by_icon_count[max_index])
				max_index = int32_t(i);
		}

		frame = max_index;
	}
};

class unit_second_most_prevalent : public image_element_base {
public:
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);

			for(auto m : state.world.navy_get_navy_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_ship().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		}

		if(by_icon_count.size() == 0) {
			visible = false;
			return;
		}

		{
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			by_icon_count[max_index] = 0;
		}
		{
			int32_t max_index = 0;
			for(uint32_t i = 1; i < by_icon_count.size(); ++i) {
				if(by_icon_count[i] > by_icon_count[max_index])
					max_index = int32_t(i);
			}
			if(by_icon_count[max_index] == 0) {
				visible = false;
			} else {
				visible = true;
				frame = max_index;
			}
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

outline_color to_color(sys::state& state, unit_var display_unit) {
	dcon::nation_id controller;
	if(std::holds_alternative<dcon::army_id>(display_unit)) {
		controller = state.world.army_get_controller_from_army_control(std::get<dcon::army_id>(display_unit));
	} else if(std::holds_alternative<dcon::navy_id>(display_unit)) {
		controller = state.world.navy_get_controller_from_navy_control(std::get<dcon::navy_id>(display_unit));
	} else {
		return outline_color::gray;
	}

	// TODO: selected case
	if(controller == state.local_player_nation) {
		return outline_color::blue;
	} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
		return outline_color::red;
	} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
		return outline_color::cyan;
	} else {
		return outline_color::gray;
	}
}

bool color_equivalent(outline_color a, outline_color b) {
	switch(a) {
		case outline_color::blue:
		case outline_color::gold:
			return b == outline_color::gold || b == outline_color::blue;
		default:
			return a == b;
	}
}
bool color_less(outline_color a, outline_color b) {
	if(a == outline_color::gray)
		return false;
	if(b == outline_color::gray)
		return true;
	return int32_t(a) < int32_t(b);
}

class base_unit_container : public window_element_base {
public:
	unit_var display_unit;
	outline_color color = outline_color::gray;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "frame_bg") {
			return make_element_by_type<unit_frame_bg>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<unit_org_bar>(state, id);
		} else if(name == "status") {
			return make_element_by_type<unit_status_image>(state, id);
		} else if(name == "dig_in") {
			return make_element_by_type<unit_dig_in_pips_image>(state, id);
		} else if(name == "strength") {
			return make_element_by_type<unit_strength>(state, id);
		} else if(name == "unit_2") {
			return make_element_by_type<unit_second_most_prevalent>(state, id);
		} else if(name == "unit_1") {
			return make_element_by_type<unit_most_prevalent>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		color = to_color(state, display_unit);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<unit_var>()) {
			payload.emplace<unit_var>(display_unit);
			return message_result::consumed;
		} else if(payload.holds_type<outline_color>()) {
			payload.emplace<outline_color>(color);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(!std::holds_alternative<std::monostate>(display_unit)) {
			window_element_base::impl_render(state, x, y);
		}
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(!std::holds_alternative<std::monostate>(display_unit))
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

using grid_row = std::array<unit_var, 4>;

bool unit_var_ordering(sys::state& state, unit_var a, unit_var b) {
	if(std::holds_alternative<std::monostate>(a))
		return false;
	if(std::holds_alternative<std::monostate>(b))
		return true;
	if(std::holds_alternative<dcon::navy_id>(a) && std::holds_alternative<dcon::army_id>(b))
		return true;
	if(std::holds_alternative<dcon::army_id>(a) && std::holds_alternative<dcon::navy_id>(b))
		return false;

	auto a_color = to_color(state, a);
	auto b_color = to_color(state, b);

	if(!color_equivalent(a_color, b_color)) {
		return color_less(a_color, b_color);
	}

	if(std::holds_alternative<dcon::army_id>(a)) {
		auto aa = std::get<dcon::army_id>(a);
		auto ba = std::get<dcon::army_id>(b);
		auto aar = state.world.army_get_army_membership(aa);
		auto bar = state.world.army_get_army_membership(ba);
		auto acount = int32_t(aar.end() - aar.begin());
		auto bcount = int32_t(bar.end() - bar.begin());
		if(acount != bcount)
			return acount > bcount;
		return aa.index() < ba.index();
	} else {
		auto aa = std::get<dcon::navy_id>(a);
		auto ba = std::get<dcon::navy_id>(b);
		auto aar = state.world.navy_get_navy_membership(aa);
		auto bar = state.world.navy_get_navy_membership(ba);
		auto acount = int32_t(aar.end() - aar.begin());
		auto bcount = int32_t(bar.end() - bar.begin());
		if(acount != bcount)
			return acount > bcount;
		return aa.index() < ba.index();
	}

}

class unit_port_grid_row : public listbox_row_element_base<grid_row> {
public:
	std::array< base_unit_container*, 4> grid_items;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<grid_row>::on_create(state);

		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 0);
			win->base_data.position.y = int16_t(3);
			grid_items[0] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 1);
			win->base_data.position.y = int16_t(3);
			grid_items[1] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 2);
			win->base_data.position.y = int16_t(3);
			grid_items[2] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find("alice_base_unit_icon")->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 3);
			win->base_data.position.y = int16_t(3);
			grid_items[3] = win.get();
			add_child_to_front(std::move(win));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		return nullptr;
	}

	void on_update(sys::state& state) noexcept override {
		grid_items[0]->display_unit = content[0];
		grid_items[1]->display_unit = content[1];
		grid_items[2]->display_unit = content[2];
		grid_items[3]->display_unit = content[3];
	}
};


class unit_port_grid_lb : public listbox_element_base<unit_port_grid_row, grid_row> {
protected:
	std::string_view get_row_element_name() override {
		return "alice_grid_row";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		static std::vector<unit_var> base_array;
		base_array.clear();

		auto prov = retrieve<dcon::province_id>(state, parent);
		for(auto n : state.world.province_get_navy_location(prov)) {
			base_array.emplace_back(n.get_navy().id);
		}
		for(auto a : state.world.province_get_army_location(prov)) {
			if(a.get_army().get_navy_from_army_transport()) {
				base_array.emplace_back(a.get_army().id);
			}
		}
		for(uint32_t i = 0; i < base_array.size(); i += 4) {
			grid_row new_row;
			for(uint32_t j = 0; j < 4 && (i + j) < base_array.size(); ++j) {
				new_row[j] = base_array[i + j];
			}
			row_contents.push_back(new_row);
		}

		update(state);
	}
};

class port_grid_box : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "grid_listbox") {
			return make_element_by_type<unit_port_grid_lb>(state, id);
		} else {
			return nullptr;
		}
	}
};

struct toggle_unit_grid { };

class port_ex_bg : public button_element_base {
	bool visible = false;

	void on_update(sys::state& state) noexcept override {
		visible = retrieve<int32_t>(state, parent) > 0;
		frame = int32_t(retrieve<outline_color>(state, parent));
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y,
			mouse_probe_type type) noexcept override {
		if(visible)
			return button_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
	void button_action(sys::state& state) noexcept override {
		if(visible)
			send(state, parent, toggle_unit_grid{});
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
		auto port_level = state.world.province_get_naval_base_level(prov);

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
	port_grid_box* details;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		{
			auto win = make_element_by_type<port_grid_box>(state, state.ui_state.defs_by_name.find("alice_grid_panel")->second.definition);
			details = win.get();
			details->set_visible(state, false);
			add_child_to_front(std::move(win));
		}
	}

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
		if(state.world.province_get_naval_base_level(port_for) == 0 && navies.begin() == navies.end()) {
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
			// bool selected_navy = false;
			for(auto n : navies) {
				auto controller = n.get_navy().get_controller_from_navy_control();
				if(controller == state.local_player_nation) {
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

			if(player_navy) {
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
			if(details->is_visible() == true) {
				details->set_visible(state, false);
			} else {
				// TODO: select all

				auto location = get_absolute_location(*this);
				details->set_visible(state, true);

				auto mx = int32_t(state.mouse_x_position / state.user_settings.ui_scale);
				auto my = int32_t(state.mouse_y_position / state.user_settings.ui_scale);
				if(state.ui_state.mouse_sensitive_target)
					state.ui_state.mouse_sensitive_target->set_visible(state, false);

				if(location.y + 24 + details->base_data.size.y < state.ui_state.root->base_data.size.y) { // position below
					auto desired_x = location.x + 30 - 18 - details->base_data.size.x / 2;
					auto actual_x = std::clamp(desired_x, 0, state.ui_state.root->base_data.size.x - details->base_data.size.x);
					auto relative_x = actual_x - location.x;
					details->base_data.position.x = int16_t(relative_x);
					details->base_data.position.y = 24;

					state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, actual_x));
					state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, 24 + location.y));
					state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, actual_x + details->base_data.size.x));
					state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, 24 + location.y + details->base_data.size.y));

				} else if(location.x + 49 + details->base_data.size.x < state.ui_state.root->base_data.size.x) { // position right
					auto desired_y = location.y - details->base_data.size.y / 2;
					auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - details->base_data.size.y);
					auto relative_y = actual_y - location.y;
					details->base_data.position.x = 49;
					details->base_data.position.y = int16_t(relative_y);

					state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, 49 + location.x));
					state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
					state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, 49 + location.x + details->base_data.size.x));
					state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + details->base_data.size.y));
				} else { //position left
					auto desired_y = location.y - details->base_data.size.y / 2;
					auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - details->base_data.size.y);
					auto relative_y = actual_y - location.y;
					details->base_data.position.x = int16_t(-details->base_data.size.x - 22);
					details->base_data.position.y = int16_t(relative_y);

					state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, location.x - details->base_data.size.x - 22));
					state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
					state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, location.x - 22));
					state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + details->base_data.size.y));
				}

				
				state.ui_state.mouse_sensitive_target = details;
			}
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y,
			mouse_probe_type type) noexcept override {
		if(visible)
			return window_element_base::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}

};

struct update_position { };

class unit_icon_color : public image_element_base {
	int32_t get_frame(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			dcon::nation_id content = any_cast<dcon::nation_id>(payload);

			if(content == state.local_player_nation)
				return 0; // green, ourselves
			auto drid = state.world.get_diplomatic_relation_by_diplomatic_pair(state.local_player_nation, content);
			if(state.world.diplomatic_relation_get_are_allied(drid))
				return 0; // green, allies, puppets and us
			else {
				auto iid = state.world.nation_get_identity_from_identity_holder(content);
				for(auto wa : state.world.nation_get_war_participant(state.local_player_nation))
					for(auto o : wa.get_war().get_war_participant())
						if(o.get_is_attacker() != wa.get_is_attacker() && o.get_nation().get_identity_from_identity_holder().id == iid)
							return 1; // red, enemy
			}
		}
		return 2; // gray, i.e not involved or irrelevant for us
	}

public:
	void on_update(sys::state& state) noexcept override {
		frame = get_frame(state);
	}
};

class unit_strength_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = float{0.f};
			parent->impl_get(state, payload);
			float content = any_cast<float>(payload);
			set_text(state, text::prettify(int32_t(content)));
		}
	}
};

class unit_icon_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id content = any_cast<dcon::province_id>(payload);
			state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) {
				// Force the Reorg window to be cleaned up
				Cyto::Any cpayload = element_selection_wrapper<unitpanel_action>{unitpanel_action{unitpanel_action::close}};
				state.ui_state.army_status_window->impl_get(state, cpayload);
				state.ui_state.navy_status_window->impl_get(state, cpayload);

				state.ui_state.army_status_window->set_visible(state, true);
				state.ui_state.navy_status_window->set_visible(state, false);

				auto aid = state.world.army_location_get_army(id);
				Cyto::Any d_payload = element_selection_wrapper<dcon::army_id>{aid};
				state.ui_state.army_status_window->impl_get(state, d_payload);
			});
			state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) {
				// Force the Reorg window to be cleaned up
				Cyto::Any cpayload = element_selection_wrapper<unitpanel_action>{unitpanel_action{unitpanel_action::close}};
				state.ui_state.army_status_window->impl_get(state, cpayload);
				state.ui_state.navy_status_window->impl_get(state, cpayload);

				state.ui_state.army_status_window->set_visible(state, false);
				state.ui_state.navy_status_window->set_visible(state, true);

				state.ui_state.army_combat_window->set_visible(state, false);
				state.ui_state.naval_combat_window->set_visible(state, true);

				auto nid = state.world.navy_location_get_navy(id);
				Cyto::Any d_payload = element_selection_wrapper<dcon::navy_id>{nid};
				state.ui_state.navy_status_window->impl_get(state, d_payload);
				auto fat = dcon::fatten(state.world, any_cast<element_selection_wrapper<dcon::navy_id>>(d_payload).data);
				if(!fat.get_battle_from_navy_battle_participation().is_valid()) {
					state.ui_state.naval_combat_window->set_visible(state, false);
				} else {
					state.ui_state.naval_combat_window->impl_get(state, d_payload);
					state.ui_state.naval_combat_window->impl_on_update(state);
				}
			});
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::province_id{};
			parent->impl_get(state, payload);
			dcon::province_id content = any_cast<dcon::province_id>(payload);
			// TODO - hacky, better solution probably exists
			state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) {
				if(dcon::fatten(state.world, id).get_army().get_army_control().get_controller().id == state.local_player_nation) {
					frame = 0;
				} else {
					frame = 2;
				}
			});
			state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) {
				frame = 1;
			});
		}
	}


	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		populate_unit_tooltip(state, contents, retrieve<dcon::province_id>(state, parent));
	}
};

class unit_icon_window : public window_element_base {
	unit_strength_text* strength_text = nullptr;
	image_element_base* attr_icon = nullptr;
public:
	bool visible = true;
	bool province_is_populated = false;

	dcon::province_id content{};

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_panel_bg") {
			return make_element_by_type<unit_icon_button>(state, id);
		} else if(name == "unit_panel_country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "unit_panel_color") {
			return make_element_by_type<unit_icon_color>(state, id);
		} else if(name == "unit_strength") {
			auto ptr = make_element_by_type<unit_strength_text>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			strength_text = ptr.get();
			return ptr;
		} else if(name == "unit_panel_org_bar") {
			return make_element_by_type<vertical_progress_bar>(state, id);
		} else if(name == "unit_panel_attr") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			attr_icon = ptr.get();
			return ptr;
		} else
			return nullptr;
	}

	void on_update(sys::state& state) noexcept override {
	 province_is_populated = false;
		state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) { province_is_populated = true; });
		state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) { province_is_populated = true; });

		bool has_attrition = false; // TODO: Attrition
		attr_icon->set_visible(state, has_attrition);
	}

	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(!province_is_populated)
			return;

		auto mid_point = state.world.province_get_mid_point(content);
		auto map_pos = state.map_state.normalize_map_coord(mid_point);
		auto screen_size =
				glm::vec2{float(state.x_size / state.user_settings.ui_scale), float(state.y_size / state.user_settings.ui_scale)};
		glm::vec2 screen_pos;
		if(!state.map_state.map_to_screen(state, map_pos, screen_size, screen_pos)) {
			visible = false;
			return;
		}
		visible = true;
		auto new_position = xy_pair{int16_t(screen_pos.x - 25), int16_t(screen_pos.y - 20)};
		window_element_base::base_data.position = new_position;
		window_element_base::impl_render(state, new_position.x, new_position.y);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(content);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			dcon::nation_id nation_id{};
			state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) {
				auto aid = state.world.army_location_get_army(id);
				nation_id = state.world.army_control_get_controller(state.world.army_get_army_control_as_army(aid));
			});
			state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) {
				auto nid = state.world.navy_location_get_navy(id);
				nation_id = state.world.navy_control_get_controller(state.world.navy_get_navy_control_as_navy(nid));
			});
			payload.emplace<dcon::nation_id>(nation_id);
			return message_result::consumed;
		} else if(payload.holds_type<float>()) {
			float strength = 0.f;
			state.world.province_for_each_army_location_as_location(content, [&](dcon::army_location_id id) {
				auto aid = state.world.army_location_get_army(id);
				state.world.army_for_each_army_membership_as_army(aid, [&](dcon::army_membership_id amid) {
					auto rid = state.world.army_membership_get_regiment(amid);
					strength += state.world.regiment_get_strength(rid);
				});
			});
			state.world.province_for_each_navy_location_as_location(content, [&](dcon::navy_location_id id) {
				auto nid = state.world.navy_location_get_navy(id);
				state.world.navy_for_each_navy_membership_as_navy(nid, [&](dcon::navy_membership_id nmid) {
					auto sid = state.world.navy_membership_get_ship(nmid);
					strength += 1.f;
				});
			});
			payload.emplace<float>(strength);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y,
			mouse_probe_type type) noexcept override {
		if(visible && province_is_populated)
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
