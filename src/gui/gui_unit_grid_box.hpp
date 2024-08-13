#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>
#include "dcon_generated.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"
#include "gui_element_types.hpp"
#include "opengl_wrapper.hpp"
#include "sound.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include "texture.hpp"

namespace ui {

enum class outline_color : uint8_t {
	gray = 0, gold = 1, blue = 2, cyan = 3, red = 4
};

using unit_var = std::variant<std::monostate, dcon::army_id, dcon::navy_id>;

class unit_frame_bg : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		frame = int32_t(retrieve<outline_color>(state, parent));
	}
	void button_action(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				state.selected_armies.clear();
				state.selected_navies.clear();
				state.select(a);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				state.selected_armies.clear();
				state.selected_navies.clear();
				state.select(a);
			}
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				if(!state.is_selected(a))
					state.select(a);
				else
					state.deselect(a);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				if(!state.is_selected(a))
					state.select(a);
				else
					state.deselect(a);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
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

inline int32_t status_frame(sys::state& state, dcon::army_id a) {
	auto is_understr = [&]() {
		for(auto m : state.world.army_get_army_membership(a)) {
			if(m.get_regiment().get_strength() < 1.0f)
				return true;
		}
		return false;
		};
	if(state.world.army_get_black_flag(a)) {
		return 1;
	} else if(state.world.army_get_is_retreating(a)) {
		return 2;
	} else if(state.world.army_get_battle_from_army_battle_participation(a)) {
		return 6;
	} else if(state.world.army_get_navy_from_army_transport(a)) {
		return 5;
	} else if(state.world.army_get_arrival_time(a)) {
		return 7;
	} else if(military::will_recieve_attrition(state, a)) {
		return 3;
	} else if(is_understr()) {
		return 4;
	} else {
		return 0;
	}
}
inline int32_t status_frame(sys::state& state, dcon::navy_id a) {
	auto trange = state.world.navy_get_army_transport(a);

	auto is_understr = [&]() {
		for(auto m : state.world.navy_get_navy_membership(a)) {
			if(m.get_ship().get_strength() < 1.0f)
				return true;
		}
		return false;
		};

	auto nb_level = state.world.province_get_building_level(state.world.navy_get_location_from_navy_location(a), uint8_t(economy::province_building_type::naval_base));
	if(state.world.navy_get_is_retreating(a)) {
		return 2;
	} else if(state.world.navy_get_battle_from_navy_battle_participation(a)) {
		return 6;
	} else if(military::will_recieve_attrition(state, a)) {
		return 3;
	} else if(trange.begin() != trange.end()) {
		return 5;
	} else if(!(state.world.navy_get_arrival_time(a)) && nb_level > 0 && is_understr()) {
		return 4;
	} else if(state.world.navy_get_arrival_time(a)) {
		return 7;
	} else {
		return 0;
	}
}

class unit_status_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			frame = status_frame(state, a);
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			frame = status_frame(state, a);
		}
	}
};

class unit_strength : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto u = retrieve< unit_var>(state, parent);
		color = text::text_color::gold;
		if(std::holds_alternative<dcon::army_id>(u)) {
			int64_t total = 0;
			auto a = std::get<dcon::army_id>(u);
			for(auto m : state.world.army_get_army_membership(a)) {
				total += int64_t(floor(m.get_regiment().get_strength() * state.defines.pop_size_per_regiment));
			}
			set_text(state, text::prettify(total));
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			float total = 0.0f;
			auto a = std::get<dcon::navy_id>(u);
			for(auto m : state.world.navy_get_navy_membership(a)) {
				total += m.get_ship().get_strength();
			}
			set_text(state, text::format_float(total, 1));
		}

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
	bool visible = true;

	void on_update(sys::state& state) noexcept override {
		static std::vector<int32_t> by_icon_count;
		for(auto& i : by_icon_count)
			i = 0;

		auto u = retrieve< unit_var>(state, parent);

		visible = true;
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

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
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
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

		visible = true;
		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

			for(auto m : state.world.army_get_army_membership(a)) {
				auto icon = state.military_definitions.unit_base_definitions[m.get_regiment().get_type()].icon - 1;
				if(uint32_t(icon) >= by_icon_count.size()) {
					by_icon_count.resize(icon + 1, 0);
				}
				++(by_icon_count[icon]);
			}
		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) != state.local_player_nation) {
				visible = false;
				return;
			}

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

class unit_controller_flag : public flag_button2 {
public:
	bool visible = true;

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			auto u = retrieve< unit_var>(state, parent);
			if(std::holds_alternative<dcon::army_id>(u)) {
				auto a = std::get<dcon::army_id>(u);
				payload.emplace<dcon::nation_id>(state.world.army_get_controller_from_army_control(a));
			} else if(std::holds_alternative<dcon::navy_id>(u)) {
				auto a = std::get<dcon::navy_id>(u);
				payload.emplace<dcon::nation_id>(state.world.navy_get_controller_from_navy_control(a));
			} else {
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::rebel_faction_id>()) {
			auto u = retrieve< unit_var>(state, parent);
			if(std::holds_alternative<dcon::army_id>(u)) {
				auto a = std::get<dcon::army_id>(u);
				payload.emplace<dcon::rebel_faction_id>(state.world.army_get_controller_from_army_rebel_control(a));
			} else {
				payload.emplace<dcon::rebel_faction_id>(dcon::rebel_faction_id{});
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override {

		visible = true;
		auto u = retrieve< unit_var>(state, parent);

		if(std::holds_alternative<dcon::army_id>(u)) {
			auto a = std::get<dcon::army_id>(u);
			if(state.world.army_get_controller_from_army_control(a) == state.local_player_nation) {
				visible = false;
				return;
			}

		} else if(std::holds_alternative<dcon::navy_id>(u)) {
			auto a = std::get<dcon::navy_id>(u);
			if(state.world.navy_get_controller_from_navy_control(a) == state.local_player_nation) {
				visible = false;
				return;
			}
		} else {
			visible = false;
			return;
		}

		flag_button2::on_update(state);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			flag_button2::render(state, x, y);
	}
	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return flag_button2::impl_probe_mouse(state, x, y, type);
		else
			return mouse_probe{ nullptr, ui::xy_pair{} };
	}
};

inline outline_color to_color(sys::state& state, unit_var display_unit) {
	dcon::nation_id controller;
	bool selected = false;
	if(std::holds_alternative<dcon::army_id>(display_unit)) {
		controller = state.world.army_get_controller_from_army_control(std::get<dcon::army_id>(display_unit));
		selected = state.is_selected(std::get<dcon::army_id>(display_unit));
	} else if(std::holds_alternative<dcon::navy_id>(display_unit)) {
		controller = state.world.navy_get_controller_from_navy_control(std::get<dcon::navy_id>(display_unit));
		selected = state.is_selected(std::get<dcon::navy_id>(display_unit));
	} else {
		return outline_color::gray;
	}

	if(selected) {
		return outline_color::gold;
	} else if(controller == state.local_player_nation) {
		return outline_color::blue;
	} else if(!controller || military::are_at_war(state, controller, state.local_player_nation)) {
		return outline_color::red;
	} else if(military::are_allied_in_war(state, controller, state.local_player_nation)) {
		return outline_color::cyan;
	} else {
		return outline_color::gray;
	}
}

inline bool color_equivalent(outline_color a, outline_color b) {
	switch(a) {
	case outline_color::blue:
	case outline_color::gold:
		return b == outline_color::gold || b == outline_color::blue;
	default:
		return a == b;
	}
}
inline bool color_less(outline_color a, outline_color b) {
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
		} else if(name == "controller_flag") {
			return make_element_by_type<unit_controller_flag>(state, id);
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
		if(!std::holds_alternative<std::monostate>(display_unit)) {
			return window_element_base::impl_probe_mouse(state, x, y, type);
		} else {
			return mouse_probe{ nullptr, ui::xy_pair{} };
		}
	}
};

using grid_row = std::array<unit_var, 4>;

inline bool unit_var_ordering(sys::state& state, unit_var a, unit_var b) {
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

class unit_grid_row : public listbox_row_element_base<grid_row> {
public:
	std::array< base_unit_container*, 4> grid_items;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<grid_row>::on_create(state);
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_base_unit_icon"))->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 0);
			win->base_data.position.y = int16_t(3);
			grid_items[0] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_base_unit_icon"))->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 1);
			win->base_data.position.y = int16_t(3);
			grid_items[1] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_base_unit_icon"))->second.definition);
			win->base_data.position.x = int16_t(3 + 67 * 2);
			win->base_data.position.y = int16_t(3);
			grid_items[2] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<base_unit_container>(state, state.ui_state.defs_by_name.find(state.lookup_key("alice_base_unit_icon"))->second.definition);
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


class unit_grid_lb : public listbox_element_base<unit_grid_row, grid_row> {
public:
	std::string_view get_row_element_name() override {
		return "alice_grid_row";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		static std::vector<unit_var> base_array;
		base_array.clear();

		auto prov = retrieve<dcon::province_id>(state, parent);
		bool as_port = retrieve<bool>(state, parent);

		if(as_port || (prov.index() >= state.province_definitions.first_sea_province.index())) {
			for(auto n : state.world.province_get_navy_location(prov)) {
				base_array.emplace_back(n.get_navy().id);
			}
		}
		if(as_port) {
			for(auto a : state.world.province_get_army_location(prov)) {
				if(a.get_army().get_navy_from_army_transport()) {
					base_array.emplace_back(a.get_army().id);
				}
			}
		} else if(prov.index() >= state.province_definitions.first_sea_province.index()) {
			for(auto a : state.world.province_get_army_location(prov)) {
				base_array.emplace_back(a.get_army().id);
			}
		} else {
			for(auto a : state.world.province_get_army_location(prov)) {
				if(!(a.get_army().get_navy_from_army_transport())) {
					base_array.emplace_back(a.get_army().id);
				}
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

class grid_box : public window_element_base {
public:
	bool as_port = false;
	dcon::province_id for_province;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "grid_listbox") {
			return make_element_by_type<unit_grid_lb>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::province_id>()) {
			payload.emplace<dcon::province_id>(for_province);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(as_port);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	void open(sys::state& state, ui::xy_pair location, ui::xy_pair source_size, dcon::province_id p, bool port) {
		if(!as_port) {
			if(p.index() < state.province_definitions.first_sea_province.index()) {
				auto arange = state.world.province_get_army_location(p);
				if(int32_t(arange.end() - arange.begin()) <= 1)
					return;
			} else {
				auto arange = state.world.province_get_army_location(p);
				auto nrange = state.world.province_get_navy_location(p);
				if(int32_t(nrange.end() - nrange.begin()) <= 1 && arange.begin() == arange.end())
					return;
			}
		}

		as_port = port;
		for_province = p;

		auto mx = int32_t(state.mouse_x_position / state.user_settings.ui_scale);
		auto my = int32_t(state.mouse_y_position / state.user_settings.ui_scale);
		if(state.ui_state.mouse_sensitive_target)
			state.ui_state.mouse_sensitive_target->set_visible(state, false);

		if(location.y + source_size.y + 3 + base_data.size.y < state.ui_state.root->base_data.size.y) { // position below
			auto desired_x = location.x + source_size.x / 2 - base_data.size.x / 2;
			auto actual_x = std::clamp(desired_x, 0, state.ui_state.root->base_data.size.x - base_data.size.x);
			base_data.position.x = int16_t(actual_x);
			base_data.position.y = int16_t(location.y + source_size.y + 3);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, actual_x));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, location.y + source_size.y + 3));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, actual_x + base_data.size.x));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, location.y + source_size.y + 3 + base_data.size.y));

		} else if(location.x + source_size.x + 3 + base_data.size.x < state.ui_state.root->base_data.size.x) { // position right
			auto desired_y = location.y + source_size.y / 2 - base_data.size.y / 2;

			auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - base_data.size.y);
			base_data.position.x = int16_t(location.x + source_size.x + 3);
			base_data.position.y = int16_t(actual_y);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, location.x + source_size.x + 3));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, location.x + source_size.x + 3 + base_data.size.x));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + base_data.size.y));
		} else { //position left
			auto desired_y = location.y + source_size.y / 2 - base_data.size.y / 2;

			auto actual_y = std::clamp(desired_y, 0, state.ui_state.root->base_data.size.y - base_data.size.y);
			base_data.position.x = int16_t(location.x - base_data.size.x - 3);
			base_data.position.y = int16_t(actual_y);

			state.ui_state.target_ul_bounds.x = int16_t(std::min(mx - 4, location.x - base_data.size.x - 3));
			state.ui_state.target_ul_bounds.y = int16_t(std::min(my - 4, actual_y));
			state.ui_state.target_lr_bounds.x = int16_t(std::max(mx + 4, location.x - base_data.size.x - 3));
			state.ui_state.target_lr_bounds.y = int16_t(std::max(my + 4, actual_y + base_data.size.y));
		}

		state.ui_state.mouse_sensitive_target = this;
		set_visible(state, true);
	}
};

} //namespace ui
