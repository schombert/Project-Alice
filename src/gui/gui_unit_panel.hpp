#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_unit_panel_subwindow.hpp"
#include "text.hpp"
#include "prng.hpp"
#include "gui_leader_tooltip.hpp"
#include "gui_leader_select.hpp"
#include "gui_unit_grid_box.hpp"

namespace ui {

enum class unitpanel_action : uint8_t { close, reorg, split, disband, changeleader, temp, upgrade };

class unit_selection_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<unitpanel_action>{unitpanel_action{ unitpanel_action::close }});
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "deselect_unit");
	}
};

template<class T>
class unit_selection_new_unit_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		// No selected Regiments or Ships
		if(!state.selected_regiments[0] && !state.selected_ships[0]) {
			send(state, parent, element_selection_wrapper<unitpanel_action>{unitpanel_action::reorg});
		}
		else {
			auto content = retrieve<T>(state, parent);
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				std::array<dcon::regiment_id, command::num_packed_units> tosplit{};
				for(size_t i = 0; i < state.selected_regiments.size(); i++) {
					if(state.selected_regiments[i]) {
						tosplit[i % command::num_packed_units] = state.selected_regiments[i];
					} else {
						break;
					}
					if(i % command::num_packed_units == command::num_packed_units - 1) {
						command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
						tosplit = {};
					}
				}

				command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
				command::split_army(state, state.local_player_nation, content);
				sys::selected_regiments_clear(state);
			} else if constexpr(std::is_same_v<T, dcon::navy_id>) {
				std::array<dcon::ship_id, command::num_packed_units> tosplit{};
				for(size_t i = 0; i < state.selected_ships.size(); i++) {
					if(state.selected_ships[i]) {
						tosplit[i % command::num_packed_units] = state.selected_ships[i];
					}
					else {
						break;
					}
					if(i % command::num_packed_units == command::num_packed_units - 1) {
						command::mark_ships_to_split(state, state.local_player_nation, tosplit);
						tosplit = {};
					}
				}

				command::mark_ships_to_split(state, state.local_player_nation, tosplit);
				command::split_navy(state, state.local_player_nation, content);
				sys::selected_ships_clear(state);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			disabled = !command::can_split_army(state, state.local_player_nation, content);
		} else if constexpr(std::is_same_v<T, dcon::navy_id>) {
			disabled = !command::can_split_navy(state, state.local_player_nation, content);
		}
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "new_unit");
	}
};

template<class T>
class unit_selection_split_in_half_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			command::evenly_split_army(state, state.local_player_nation, content);
		} else {
			command::evenly_split_navy(state, state.local_player_nation, content);
		}
		state.select(content); //deselect original
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			disabled = !command::can_evenly_split_army(state, state.local_player_nation, content);
		} else {
			disabled = !command::can_evenly_split_navy(state, state.local_player_nation, content);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "splitinhalf");
	}
};


class disband_units_title : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto count = retrieve<size_t>(state, parent);
		if(count == 1) {
			set_text(state, text::produce_simple_string(state, "CONFIRM_DISBAND_TITLE"));
		}
		else if(count > 1) {
			set_text(state, text::produce_simple_string(state, "DISBAND_ALL_TITLE"));
		}
	}
};

class disband_units_desc : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto count = retrieve<size_t>(state, parent);
		auto contents = text::create_endless_layout(state, internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::white, true });
		auto box = text::open_layout_box(contents);
		if(count == 1) {
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "CONFIRM_DISBAND_DESC"), text::text_color::white);
		}
		else if(count > 1) {
			text::substitution_map m;
			text::add_to_substitution_map(m, text::variable_type::num, int64_t(count));
			text::add_to_layout_box(state, contents, box, text::resolve_string_substitution(state, "DISBAND_ALL_DESC", m), text::text_color::white);
		}
		text::close_layout_box(contents, box);
	}
};

template<typename T>
struct disband_unit_wrapper {
	std::vector<T> to_be_deleted;
};

class disband_agree_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		const auto& armies = retrieve<std::vector<dcon::army_id>>(state, parent);
		const auto& navies = retrieve<std::vector<dcon::navy_id>>(state, parent);
		for(auto unit : armies) {
			if(!command::can_delete_army(state, state.local_player_nation, unit)) {
				disabled = true;
			}
		}
		disabled = false;

		for(auto unit : navies) {
			if(!command::can_delete_navy(state, state.local_player_nation, unit)) {
				disabled = true;
			}
		}
		disabled = false;
		
	}

	void button_action(sys::state& state) noexcept override {

		const auto& armies = retrieve<std::vector<dcon::army_id>>(state, parent);
		const auto& navies = retrieve<std::vector<dcon::navy_id>>(state, parent);
		for(auto unit : armies) {
			command::delete_army(state, state.local_player_nation, unit);
		}
		for(auto unit : navies) {
			command::delete_navy(state, state.local_player_nation, unit);
		}
		
		parent->set_visible(state, false); // Close parent window automatically
		//send(state, parent, element_selection_wrapper<unitpanel_action>{unitpanel_action{ unitpanel_action::close }});

	}
};






class disband_unit_confirmation : public window_element_base {

	std::vector<dcon::army_id> armies;
	std::vector<dcon::navy_id> navies;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
		base_data.position.x = base_data.position.y = 0;
	}

	void set_units_to_be_disbanded(sys::state& state, const std::vector<dcon::army_id>& units_to_be_deleted) {
		armies.clear();
		navies.clear();
		for(auto unit : units_to_be_deleted) {
			armies.push_back(unit);
		}
	}
	void set_units_to_be_disbanded(sys::state& state, const std::vector<dcon::navy_id>& units_to_be_deleted) {
		armies.clear();
		navies.clear();
		for(auto unit : units_to_be_deleted) {
			navies.push_back(unit);
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "default_popup_banner")
			return make_element_by_type<image_element_base>(state, id); // Not used in Vic2?
		else if(name == "title")
			return make_element_by_type<disband_units_title>(state, id);
		else if(name == "description")
			return make_element_by_type<disband_units_desc>(state, id);
		else if(name == "agreebutton")
			return make_element_by_type<disband_agree_button>(state, id);
		else if(name == "declinebutton")
			return make_element_by_type<generic_close_button>(state, id);
		else
			return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<std::vector<dcon::army_id>>()) {
			payload = armies;
			return message_result::consumed;
		}
		else if(payload.holds_type<std::vector<dcon::navy_id>>()) {
			payload = navies;
			return message_result::consumed;
		}
		else if(payload.holds_type<size_t>()) {
			payload = armies.size() + navies.size();
			return message_result::consumed;
		}
		else if(payload.holds_type<disband_unit_wrapper<dcon::army_id>>()) {
			disband_unit_wrapper<dcon::army_id> item = any_cast<disband_unit_wrapper<dcon::army_id>>(payload);
			set_units_to_be_disbanded(state, item.to_be_deleted);
			set_visible(state, true);
			return message_result::consumed;
		}
		else if(payload.holds_type<disband_unit_wrapper<dcon::navy_id>>()) {
			disband_unit_wrapper<dcon::navy_id> item = any_cast<disband_unit_wrapper<dcon::navy_id>>(payload);
			set_units_to_be_disbanded(state, item.to_be_deleted);
			set_visible(state, true);
			return message_result::consumed;
		}



		return message_result::unseen;
	}

};



template<class T>
class unit_selection_disband_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if(state.user_settings.unit_disband_confirmation) {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				std::vector<dcon::army_id> units;
				units.push_back(content);
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::army_id> {units});
			} else {
				std::vector<dcon::navy_id> units;
				units.push_back(content);
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::navy_id> {units});
			}
		}
		else {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				command::delete_army(state, state.local_player_nation, content);
			} else {
				command::delete_navy(state, state.local_player_nation, content);
			}
		}
		
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			disabled = !command::can_delete_army(state, state.local_player_nation, content);
		} else {
			disabled = !command::can_delete_navy(state, state.local_player_nation, content);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "disband_unit");
	}
};

class unit_selection_disband_too_small_button : public tinted_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_disband_undermanned_regiments(state, state.local_player_nation, retrieve<dcon::army_id>(state, parent));
		color = sys::pack_color(255, 196, 196);
		if(state.user_settings.color_blind_mode == sys::color_blind_mode::deutan || state.user_settings.color_blind_mode == sys::color_blind_mode::protan) {
			color = sys::pack_color(255, 255, 255); //remap to blue
		}
	}
	void button_action(sys::state& state) noexcept override {
		 command::disband_undermanned_regiments(state, state.local_player_nation, retrieve<dcon::army_id>(state, parent));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "disband_too_small_unit");
	}
};

class unit_selection_unit_upgrade_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		disabled = !(state.selected_regiments[0] || state.selected_ships[0]);
	}
	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<unitpanel_action>{unitpanel_action{ unitpanel_action::upgrade }});
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "unit_upgrade_button");
	}
};

template<typename T>
class unit_selection_unit_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		set_text(state, std::string(state.to_string_view(dcon::fatten(state.world, content).get_name())));
	}
};

template<typename T>
class unit_selection_leader_name : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			auto fat = dcon::fatten(state.world, content);
			auto name = fat.get_general_from_army_leadership().get_name();
			if(bool(name)) {
				// if its a actual leader
				set_text(state, std::string(state.to_string_view(name)));
			}
			else {
				// if its no leader
				set_text(state, text::produce_simple_string(state, "no_leader"));
			}
		}
		else {
			auto fat = dcon::fatten(state.world, content);
			auto name = fat.get_admiral_from_navy_leadership().get_name();
			if(bool(name)) {
				// if its a actual leader
				set_text(state, std::string(state.to_string_view(name)));
			}
			else {
				// if its no leader
				set_text(state, text::produce_simple_string(state, "no_leader"));
			}
		}
	}
};

template<class T>
class unit_selection_change_leader_button : public button_element_base {
public:

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return  tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto unit = retrieve<T>(state, parent);
		dcon::leader_id lid;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			lid = state.world.army_get_general_from_army_leadership(unit);
		} else {
			lid = state.world.navy_get_admiral_from_navy_leadership(unit);
		}
		display_leader_full(state, lid, contents, 0);
	}
	

	void button_action(sys::state& state) noexcept override {
		auto unit = retrieve<T>(state, parent);
		auto location = get_absolute_non_mirror_location(state, *this);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			if(command::can_change_general(state, state.local_player_nation, unit, dcon::leader_id{}))
				open_leader_selection(state, unit, dcon::navy_id{}, location.x, location.y);
		} else {
			if(command::can_change_admiral(state, state.local_player_nation, unit, dcon::leader_id{}))
				open_leader_selection(state, dcon::army_id{}, unit, location.x, location.y);
		}
	}
};

template<class T>
class unit_selection_leader_image : public button_element_base {
public:
	dcon::gfx_object_id default_img;

	void on_update(sys::state& state) noexcept override {
		if(!default_img) {
			if(base_data.get_element_type() == ui::element_type::image)
				default_img = base_data.data.image.gfx_object;
			else if(base_data.get_element_type() == ui::element_type::button)
				default_img = base_data.data.button.button_image;
		}

		auto unit = retrieve<T>(state, parent);
		dcon::leader_id lid;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			lid = state.world.army_get_general_from_army_leadership(unit);
			disabled = !command::can_change_general(state, state.local_player_nation, unit, dcon::leader_id{});
		} else {
			lid = state.world.navy_get_admiral_from_navy_leadership(unit);
			disabled = !command::can_change_admiral(state, state.local_player_nation, unit, dcon::leader_id{});
		}

		auto pculture = state.world.nation_get_primary_culture(state.local_player_nation);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype && lid) {
			auto admiral = state.world.leader_get_is_admiral(lid);
			if(admiral) {
				auto arange = ltype.get_admirals();
				if(arange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), arange.size());
					if(base_data.get_element_type() == ui::element_type::image)
						base_data.data.image.gfx_object = arange[in_range];
					else if(base_data.get_element_type() == ui::element_type::button)
						base_data.data.button.button_image = arange[in_range];
				}
			} else {
				auto grange = ltype.get_generals();
				if(grange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), grange.size());
					if(base_data.get_element_type() == ui::element_type::image)
						base_data.data.image.gfx_object = grange[in_range];
					else if(base_data.get_element_type() == ui::element_type::button)
						base_data.data.button.button_image = grange[in_range];
				}
			}
		} else {
			if(base_data.get_element_type() == ui::element_type::image)
				base_data.data.image.gfx_object = default_img;
			else if(base_data.get_element_type() == ui::element_type::button)
				base_data.data.button.button_image = default_img;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return  tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto unit = retrieve<T>(state, parent);
		dcon::leader_id lid;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			lid = state.world.army_get_general_from_army_leadership(unit);
		} else {
			lid = state.world.navy_get_admiral_from_navy_leadership(unit);
		}
		display_leader_full(state, lid, contents, 0);
	}


	void button_action(sys::state& state) noexcept override {
		auto unit = retrieve<T>(state, parent);
		auto location = get_absolute_non_mirror_location(state, *this);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			open_leader_selection(state, unit, dcon::navy_id{}, location.x + base_data.size.x, location.y);
		} else {
			open_leader_selection(state, dcon::army_id{}, unit, location.x + base_data.size.x, location.y);
		}
	}
};

template<class T>
class unit_selection_unit_location_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_location_from_army_location().get_name()));
		} else {
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_location_from_navy_location().get_name()));
		}
	}
};
template<class T>
class unit_selection_unit_location_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "uw_loc_iro");
	}
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			auto p = dcon::fatten(state.world, content).get_location_from_army_location();
			state.map_state.center_map_on_province(state, p);
		} else {
			auto p = dcon::fatten(state.world, content).get_location_from_navy_location();
			state.map_state.center_map_on_province(state, p);
		}
	}
};

template<class T>
class unit_selection_str_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);

		float total_strength = 0.0f;
		int32_t unit_count = 0;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
				auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
				total_strength += regiment.get_strength();
				++unit_count;
			});
		} else {
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
				total_strength += ship.get_strength();
				++unit_count;
			});
		}
		total_strength /= static_cast<float>(unit_count);
		progress = total_strength;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<T>(state, parent);
		float total_strength = 0.0f;
		int32_t unit_count = 0;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
				auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
				total_strength += regiment.get_strength();
				++unit_count;
			});
		} else {
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
				total_strength += ship.get_strength();
				++unit_count;
			});
		}
		total_strength /= static_cast<float>(unit_count);

		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("curr_comb_str"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ total_strength }, text::text_color::yellow);
		text::close_layout_box(contents, box);
	}
};

template<class T>
class unit_selection_org_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);

		float total_org = 0.0f;
		int32_t unit_count = 0;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
				auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
				total_org += regiment.get_org();
				++unit_count;
			});
		} else {
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
				total_org += ship.get_org();
				++unit_count;
			});
		}
		total_org /= static_cast<float>(unit_count);
		progress = total_org;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<T>(state, parent);
		float total_org = 0.0f;
		int32_t unit_count = 0;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
				auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
				total_org += regiment.get_org();
				++unit_count;
			});
		} else {
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
				total_org += ship.get_org();
				++unit_count;
			});
		}
		total_org /= static_cast<float>(unit_count);

		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("curr_comb_org"));
		text::add_to_layout_box(state, contents, box, text::fp_percentage{ total_org }, text::text_color::yellow);
		text::close_layout_box(contents, box);

		if constexpr(std::is_same_v<T, dcon::army_id>) {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::land_organisation, true);
		} else {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::naval_organisation, true);
		}
	}
};

template<class T>
class unit_experience_bar : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			auto regiment = retrieve<dcon::regiment_id>(state, parent);
			frame = int(state.world.regiment_get_experience(regiment) * 10);
		} else {
			auto ship = retrieve<dcon::ship_id>(state, parent);
			frame = int(state.world.ship_get_experience(ship) * 10);
		}
	}

	void on_create(sys::state& state) noexcept override {
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			auto regiment = retrieve<dcon::regiment_id>(state, parent);
			frame = int(state.world.regiment_get_experience(regiment) * 10);
		} else {
			auto ship = retrieve<dcon::ship_id>(state, parent);
			frame = int(state.world.ship_get_experience(ship) * 10);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			auto regiment = retrieve<dcon::regiment_id>(state, parent);
			auto exp = state.world.regiment_get_experience(regiment);
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "unit_experience");
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ exp }, text::text_color::green);
			text::close_layout_box(contents, box);
		} else {
			auto ship = retrieve<dcon::ship_id>(state, parent);
			auto exp = state.world.ship_get_experience(ship);
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "unit_experience");
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ exp }, text::text_color::green);
			text::close_layout_box(contents, box);
		}
	}
};

template<class T>
class unit_selection_total_str_text : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);

		int32_t total_strength = 0;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
				auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
				total_strength += int32_t(regiment.get_strength() * state.defines.pop_size_per_regiment);
			});
		} else {
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
				++total_strength;
			});
		}

		set_text(state, text::prettify(total_strength));
	}
};

template<class T>
class unit_selection_attrition_icon : public image_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		visible = military::will_recieve_attrition(state, content);
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

template<class T>
class unit_selection_attrition_amount : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		auto amount = military::attrition_amount(state, content);
		if(amount > 0) {
			set_text(state, text::format_percentage(amount, 1));
			color = text::text_color::red;
		}
		else {
			set_text(state, "");
		}
	}
};

template<class T>
class unit_panel_dynamic_tinted_bg : public opaque_element_base {
public:
	uint32_t color = 0;

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			if(state.world.army_get_controller_from_army_control(content) == state.local_player_nation) {
				color = sys::pack_color(210, 255, 210);
			} else {
				color = sys::pack_color(170, 190, 170);
			}
		} else {
			if(state.world.navy_get_controller_from_navy_control(content) == state.local_player_nation) {
				color = sys::pack_color(210, 210, 255);
			} else {
				color = sys::pack_color(170, 170, 190);
			}
		}
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid) {
			auto const& gfx_def = state.ui_defs.gfx[gid];
			if(gfx_def.primary_texture_handle) {
				if(gfx_def.number_of_frames > 1) {
					ogl::render_tinted_subsprite(state, frame,
						gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped(),
						state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
				} else {
					ogl::render_tinted_textured_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped(),
						state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
				}
			}
		}
	}
};

template<class T>
class unit_selection_panel : public window_element_base {
	dcon::gfx_object_id disband_gfx{};
	unit_selection_disband_too_small_button* disband_too_small_btn = nullptr;
public:
	window_element_base* reorg_window = nullptr;
	window_element_base* combat_window = nullptr;

	void on_create(sys::state& state) noexcept override {
		{
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto win2 = make_element_by_type<unit_reorg_window<T, dcon::regiment_id>>(state, state.ui_state.defs_by_name.find(state.lookup_key("reorg_window"))->second.definition);
				win2->base_data.position.y = base_data.position.y - 29;
				win2->set_visible(state, false);
				reorg_window = win2.get();
				add_child_to_front(std::move(win2));
			} else {
				auto win2 = make_element_by_type<unit_reorg_window<T, dcon::ship_id>>(state, state.ui_state.defs_by_name.find(state.lookup_key("reorg_window"))->second.definition);
				win2->base_data.position.y = base_data.position.y - 29;
				win2->set_visible(state, false);
				reorg_window = win2.get();
				add_child_to_front(std::move(win2));
			}
		}
		window_element_base::on_create(state);
		if(disband_too_small_btn && disband_gfx) {
			disband_too_small_btn->base_data.data.button.button_image = disband_gfx;
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unitpanel_bg") {
			return make_element_by_type<unit_panel_dynamic_tinted_bg<T>>(state, id);
		} else if(name == "leader_prestige_icon") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "leader_prestige_bar") {
			return make_element_by_type<leader_prestige_progress_bar<T>>(state, id);
		} else if(name == "prestige_bar_frame") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "unitname") {
			auto ptr = make_element_by_type<unit_selection_unit_name_text<T>>(state, id);
			ptr->base_data.position.x += 9;
			ptr->base_data.position.y += 4;
			return ptr;
		} else if(name == "only_unit_from_selection_button") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "remove_unit_from_selection_button") {
			return make_element_by_type<unit_selection_close_button>(state, id);
		} else if(name == "newunitbutton") {
			return make_element_by_type<unit_selection_new_unit_button<T>>(state, id);
		} else if(name == "splitinhalf") {
			return make_element_by_type<unit_selection_split_in_half_button<T>>(state, id);
		} else if(name == "disbandbutton") {
			auto ptr = make_element_by_type<unit_selection_disband_button<T>>(state, id);
			disband_gfx = ptr->base_data.data.button.button_image;
			return ptr;
		} else if(name == "disbandtoosmallbutton") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<unit_selection_disband_too_small_button>(state, id);
				disband_too_small_btn = ptr.get();
				return ptr;
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else if(name == "str_bar") {
			return make_element_by_type<unit_selection_str_bar<T>>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<unit_selection_org_bar<T>>(state, id);
		} else if(name == "unitattrition_icon") {
			return make_element_by_type<unit_selection_attrition_icon<T>>(state, id);
		} else if(name == "unitattrition") {
			return make_element_by_type<unit_selection_attrition_amount<T>>(state, id);
		} else if(name == "unitstrength") {
			return make_element_by_type<unit_selection_total_str_text<T>>(state, id);
		} else if(name == "unitlocation") {
			return make_element_by_type<unit_selection_unit_location_text<T>>(state, id);
		} else if(name == "unit_location_button") {
			return make_element_by_type<unit_selection_unit_location_button<T>>(state, id);
		} else if(name == "unitleader") {
			return make_element_by_type<unit_selection_leader_name<T>>(state, id);
		} else if(name == "leader_button") {
			return make_element_by_type<unit_selection_change_leader_button<T>>(state, id);
		} else if(name == "unit_activity") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leader_photo") {
			return make_element_by_type<unit_selection_leader_image<T>>(state, id);
		} else if(name == "unit_upgrade_button") {
			auto ptr = make_element_by_type<unit_selection_unit_upgrade_button>(state, id);
			move_child_to_front(ptr.get());
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<unitpanel_action>>()) {
			auto action = any_cast<element_selection_wrapper<unitpanel_action>>(payload).data;
			switch(action) {
				case unitpanel_action::close: {
					// Bucket Carry, we dont handle this, but the parent does
					parent->impl_get(state, payload);
					// Tell reorg window to clean up after itself
					Cyto::Any cpayload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::close}};
					reorg_window->impl_get(state, cpayload);
					break;
				} case unitpanel_action::reorg: {
					reorg_window->is_visible() ? reorg_window->set_visible(state, false) : reorg_window->set_visible(state, true);
					reorg_window->impl_on_update(state);
					break;
				} case unitpanel_action::changeleader: {
					break;
				} case unitpanel_action::upgrade:
				{
					return message_result::unseen;
				}
				default: {
					break;
				}
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

template<std::size_t N>
class selected_army_group_land_details_item : public window_element_base {
private:
	simple_text_element_base* unitamount_text = nullptr;
	simple_text_element_base* unitstrength_text = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitamount_text = ptr.get();
			return ptr;
		} else if(name == "unit_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitstrength_text = ptr.get();
			return ptr;
		} else if(name == "unit_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = N;
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			uint16_t totalunits = 0;
			uint32_t totalpops = 0;

			if(state.selected_army_group) {
				auto group = fatten(state.world, state.selected_army_group);
				for(auto regiment_membership : group.get_automated_army_group_membership_regiment()) {
					auto fat = regiment_membership.get_regiment().get_regiment_from_automation();
					auto strenght = fat.get_strength() * state.defines.pop_size_per_regiment;
					unitstrength_text->set_visible(state, true);
				
					dcon::unit_type_id utid = fat.get_type();
					auto result = utid ? state.military_definitions.unit_base_definitions[utid].type : military::unit_type::infantry;
					if constexpr(N == 0) {
						if(result == military::unit_type::infantry) {
							totalunits++;
							totalpops += uint32_t(strenght);
						}
					} else if constexpr(N == 1) {
						if(result == military::unit_type::cavalry) {
							totalunits++;
							totalpops += uint32_t(strenght);
						}
					} else if constexpr(N == 2) {
						if(result == military::unit_type::support || result == military::unit_type::special) {
							totalunits++;
							totalpops += uint32_t(strenght);
						}
					}
				}
			}
			unitamount_text->set_text(state, text::format_float(totalunits, 0));
			unitstrength_text->set_text(state, text::format_wholenum(totalpops));
		}
	}
};

template<std::size_t N>
class selected_army_group_sea_details_item : public window_element_base {
private:
	simple_text_element_base* unitamount_text = nullptr;
	simple_text_element_base* unitstrength_text = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitamount_text = ptr.get();
			return ptr;
		} else if(name == "unit_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitstrength_text = ptr.get();
			return ptr;
		} else if(name == "unit_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = 3 + N;
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			unitstrength_text->set_visible(state, false);
			uint16_t total = 0;

			if(state.selected_army_group) {
				auto army_group = fatten(state.world, state.selected_army_group);

				for(auto navy_membership : army_group.get_automated_army_group_membership_navy()) {
					auto fat = navy_membership.get_navy();

					for(auto n : fat.get_navy_membership()) {
						dcon::unit_type_id utid = n.get_ship().get_type();
						auto result = utid ? state.military_definitions.unit_base_definitions[utid].type : military::unit_type::infantry;
						if constexpr(N == 0) {
							if(result == military::unit_type::big_ship) {
								total++;
							}
						} else if constexpr(N == 1) {
							if(result == military::unit_type::light_ship) {
								total++;
							}
						} else if constexpr(N == 2) {
							if(result == military::unit_type::transport) {
								total++;
							}
						}
					}
				}
			}

			unitamount_text->set_text(state, text::format_float(total, 0));
		}
	}
};

template<class T, std::size_t N>
class unit_details_type_item : public window_element_base {
private:
	simple_text_element_base* unitamount_text = nullptr;
	simple_text_element_base* unitstrength_text = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitamount_text = ptr.get();
			return ptr;
		} else if(name == "unit_strength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitstrength_text = ptr.get();
			return ptr;
		} else if(name == "unit_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				ptr->frame = N;
			} else {
				ptr->frame = 3 + N;
			}
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);
			auto fat = dcon::fatten(state.world, content);

			if constexpr(std::is_same_v<T, dcon::army_id>) {
				unitstrength_text->set_visible(state, true);

				uint16_t totalunits = 0;
				uint32_t totalpops = 0;
				for(auto n : fat.get_army_membership()) {
					dcon::unit_type_id utid = n.get_regiment().get_type();
					auto result = utid ? state.military_definitions.unit_base_definitions[utid].type : military::unit_type::infantry;
					if constexpr(N == 0) {
						if(result == military::unit_type::infantry) {
							totalunits++;
							totalpops += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
						}
					} else if constexpr(N == 1) {
						if(result == military::unit_type::cavalry) {
							totalunits++;
							totalpops += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
						}
					} else if constexpr(N == 2) {
						if(result == military::unit_type::support || result == military::unit_type::special) {
							totalunits++;
							totalpops += uint32_t(state.world.regiment_get_strength(n.get_regiment().id) * state.defines.pop_size_per_regiment);
						}
					}
				}
				unitamount_text->set_text(state, text::format_float(totalunits, 0));
				unitstrength_text->set_text(state, text::format_wholenum(totalpops));
			} else {
				unitstrength_text->set_visible(state, false);
				uint16_t total = 0;
				for(auto n : fat.get_navy_membership()) {
					dcon::unit_type_id utid = n.get_ship().get_type();
					auto result = utid ? state.military_definitions.unit_base_definitions[utid].type : military::unit_type::infantry;
					if constexpr(N == 0) {
						if(result == military::unit_type::big_ship) {
							total++;
						}
					} else if constexpr(N == 1) {
						if(result == military::unit_type::light_ship) {
							total++;
						}
					} else if constexpr(N == 2) {
						if(result == military::unit_type::transport) {
							total++;
						}
					}
				}
				unitamount_text->set_text(state, text::format_float(total, 0));
			}
		}
	}
};

template<class T>
class subunit_entry_bg : public tinted_button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			dcon::unit_type_id utid = dcon::fatten(state.world, content).get_type();
			if(utid)
				frame = state.military_definitions.unit_base_definitions[utid].icon - 1;

			if constexpr(std::is_same_v<T, dcon::regiment_id>) {
				dcon::regiment_id reg = any_cast<dcon::regiment_id>(payload);

				if(std::find(state.selected_regiments.begin(), state.selected_regiments.end(), reg) != state.selected_regiments.end()) {
					color = sys::pack_color(255, 200, 200);
				} else {
					color = sys::pack_color(255, 255, 255);
				}
			}
			else if constexpr(std::is_same_v<T, dcon::ship_id>) {
				dcon::ship_id sh = any_cast<dcon::ship_id>(payload);

				if(std::find(state.selected_ships.begin(), state.selected_ships.end(), sh) != state.selected_ships.end()) {
					color = sys::pack_color(255, 200, 200);
				} else {
					color = sys::pack_color(255, 255, 255);
				}
			}
		}
	}

	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			if(parent) {
				Cyto::Any payload = dcon::regiment_id{};
				parent->impl_get(state, payload);
				dcon::regiment_id reg = any_cast<dcon::regiment_id>(payload);

				if(mods == sys::key_modifiers::modifiers_shift) {
					sys::selected_ships_clear(state);
					sys::selected_regiments_add(state, reg);
					parent->impl_on_update(state);
				} else {
					sys::selected_regiments_clear(state);
					sys::selected_ships_clear(state);
					sys::selected_regiments_add(state, reg);
					parent->impl_on_update(state);
				}
			}
		}
		else if constexpr(std::is_same_v<T, dcon::ship_id>) {
			if(parent) {
				Cyto::Any payload = dcon::ship_id{};
				parent->impl_get(state, payload);
				dcon::ship_id reg = any_cast<dcon::ship_id>(payload);

				if(mods == sys::key_modifiers::modifiers_shift) {
					sys::selected_regiments_clear(state);
					sys::selected_ships_add(state, reg);
					parent->impl_on_update(state);
				} else {
					sys::selected_ships_clear(state);
					sys::selected_regiments_clear(state);
					sys::selected_ships_add(state, reg);
					parent->impl_on_update(state);
				}
			}
		}

		return message_result::consumed;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid) {
			auto const& gfx_def = state.ui_defs.gfx[gid];
			if(gfx_def.primary_texture_handle) {
				if(gfx_def.number_of_frames > 1) {
					ogl::render_tinted_subsprite(state, frame,
						gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped(),
						state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
				} else {
					ogl::render_tinted_textured_rect(state, float(x), float(y), float(base_data.size.x), float(base_data.size.y),
						sys::red_from_int(color), sys::green_from_int(color), sys::blue_from_int(color),
						ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
						base_data.get_rotation(), gfx_def.is_vertically_flipped(),
						state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
				}
			}
		}
	}
};

class subunit_details_entry_regiment : public listbox_row_element_base<dcon::regiment_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<subunit_entry_bg<dcon::regiment_id>>(state, id);
		} else if(name == "select_naval") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "sunit_icon") {
			return make_element_by_type<subunit_details_type_icon<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_name") {
			return make_element_by_type<subunit_details_name<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_type") {
			return make_element_by_type<subunit_details_type_text<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_amount") {
			return make_element_by_type<subunit_details_regiment_amount>(state, id);
		} else if(name == "subunit_amount_naval") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "connected_pop") {
			return make_element_by_type<regiment_pop_icon>(state, id);
		} else if(name == "rebel_faction") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "unit_experience") {
			return make_element_by_type<unit_experience_bar<dcon::regiment_id>>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<subunit_organisation_progress_bar<dcon::regiment_id>>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<subunit_strength_progress_bar<dcon::regiment_id>>(state, id);
		} else {
			return nullptr;
		}
	}
};

class subunit_details_entry_ship : public listbox_row_element_base<dcon::ship_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "select_naval") {
			return make_element_by_type<subunit_entry_bg<dcon::ship_id>>(state, id);
		} else if(name == "sunit_icon") {
			return make_element_by_type<subunit_details_type_icon<dcon::ship_id>>(state, id);
		} else if(name == "subunit_name") {
			return make_element_by_type<subunit_details_name<dcon::ship_id>>(state, id);
		} else if(name == "subunit_type") {
			return make_element_by_type<subunit_details_type_text<dcon::ship_id>>(state, id);
		} else if(name == "subunit_amount") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "subunit_amount_naval") {
			return make_element_by_type<subunit_details_ship_amount>(state, id);
		} else if(name == "connected_pop") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "rebel_faction") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "unit_experience") {
			return make_element_by_type<unit_experience_bar<dcon::ship_id>>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<subunit_organisation_progress_bar<dcon::ship_id>>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<subunit_strength_progress_bar<dcon::ship_id>>(state, id);
		} else {
			return nullptr;
		}
	}
};

class unit_details_army_listbox : public listbox_element_base<subunit_details_entry_regiment, dcon::regiment_id> {
protected:
	std::string_view get_row_element_name() override {
		return "subunit_entry";
	}

	dcon::army_id cached_id;

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.y += state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("subunit_entry"))->second.definition].size.y; //nudge - allows for the extra element in the lb
		listbox_element_base<subunit_details_entry_regiment, dcon::regiment_id>::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::army_id>(state, parent);

		if(content != cached_id) {
			cached_id = content;
			sys::selected_regiments_clear(state);
		}

		row_contents.clear();
		state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id amid) {
			auto rid = state.world.army_membership_get_regiment(amid);
			row_contents.push_back(rid);
		});
		std::sort(row_contents.begin(), row_contents.end(), [&](dcon::regiment_id a, dcon::regiment_id b) {
			auto av = state.world.regiment_get_type(a).index();
			auto bv = state.world.regiment_get_type(b).index();
			if(av != bv)
				return av > bv;
			else
				return a.index() < b.index();
		});
		update(state);
	}
};
class unit_details_navy_listbox : public listbox_element_base<subunit_details_entry_ship, dcon::ship_id> {
protected:
	std::string_view get_row_element_name() override {
		return "subunit_entry";
	}

public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.y += state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("subunit_entry"))->second.definition].size.y; //nudge - allows for the extra element in the lb
		listbox_element_base<subunit_details_entry_ship, dcon::ship_id>::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::navy_id>(state, parent);
		row_contents.clear();
		state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
			auto sid = state.world.navy_membership_get_ship(nmid);
			row_contents.push_back(sid);
		});
		std::sort(row_contents.begin(), row_contents.end(), [&](dcon::ship_id a, dcon::ship_id b) {
			auto av = state.world.ship_get_type(a).index();
			auto bv = state.world.ship_get_type(b).index();
			if(av != bv)
				return av > bv;
			else
				return a.index() < b.index();
		});
		update(state);
	}
};

class unit_details_load_army_button : public button_element_base {
public:
	bool visible = false;

	void button_action(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		auto p = state.world.army_get_location_from_army_location(a);
		int32_t max_cap = 0;
		for(auto n : state.world.province_get_navy_location(p)) {
			if(n.get_navy().get_controller_from_navy_control() == state.local_player_nation &&
				!bool(n.get_navy().get_battle_from_navy_battle_participation())) {
				max_cap = std::max(military::free_transport_capacity(state, n.get_navy()), max_cap);
			}
		}
		if(!military::can_embark_onto_sea_tile(state, state.local_player_nation, p, a)
			&& max_cap > 0) { //require splitting
			auto regs = state.world.army_get_army_membership(a);
			int32_t army_cap = int32_t(regs.end() - regs.begin());
			int32_t to_split = army_cap - max_cap;
			//can mark 10 regiments to be split at a time
			std::array<dcon::regiment_id, command::num_packed_units> data;
			int32_t i = 0;
			data.fill(dcon::regiment_id{});
			for(auto reg : state.world.army_get_army_membership(a)) {
				if(to_split == 0)
					break;
				//
				data[i] = reg.get_regiment();
				++i;
				if(i >= int32_t(command::num_packed_units)) { //reached max allowed
					command::mark_regiments_to_split(state, state.local_player_nation, data);
					data.fill(dcon::regiment_id{});
					i = 0;
				}
				//
				--to_split;
			}
			if(i > 0) { //leftovers
				command::mark_regiments_to_split(state, state.local_player_nation, data);
			}
			command::split_army(state, state.local_player_nation, a);
			command::embark_army(state, state.local_player_nation, a);
		} else { //no split
			command::embark_army(state, state.local_player_nation, a);
		}
	}
	void on_update(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		auto p = state.world.army_get_location_from_army_location(a);
		visible = !bool(state.world.army_get_navy_from_army_transport(a)); //not already in ship
		disabled = true;
		frame = 0;
		if(visible) {
			int32_t max_cap = 0;
			for(auto n : state.world.province_get_navy_location(p)) {
				if(n.get_navy().get_controller_from_navy_control() == state.local_player_nation &&
					!bool(n.get_navy().get_battle_from_navy_battle_participation())) {
					max_cap = std::max(military::free_transport_capacity(state, n.get_navy()), max_cap);
				}
			}
			disabled = max_cap <= 0;
			//require splitting
			if(!military::can_embark_onto_sea_tile(state, state.local_player_nation, p, a)
				&& max_cap > 0) {
				frame = 1;
			}
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::army_id>(state, parent);
		auto loc = state.world.army_get_location_from_army_location(n);

		text::add_line(state, contents, "uw_load_is_valid");
		text::add_line_with_condition(state, contents, "alice_load_unload_1", military::can_embark_onto_sea_tile(state, state.local_player_nation, loc, n));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type t) noexcept override {
		if(visible)
			return button_element_base::test_mouse(state, x, y, t);
		return message_result::unseen;
	}
};
class unit_details_unload_army_button : public button_element_base {
public:
	bool visible = false;

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::army_id>(state, parent);
		command::embark_army(state, state.local_player_nation, n);
	}
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::army_id>(state, parent);
		auto tprted = state.world.army_get_navy_from_army_transport(n);
		auto loc = state.world.army_get_location_from_army_location(n);

		visible = bool(tprted);

		if(loc.index() >= state.province_definitions.first_sea_province.index()) {
			disabled = true;
		} else {
			disabled = false;
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::army_id>(state, parent);
		auto tprted = state.world.army_get_navy_from_army_transport(n);
		auto loc = state.world.army_get_location_from_army_location(n);

		text::add_line(state, contents, "uw_unload_valid");
		text::add_line_with_condition(state, contents, "alice_load_unload_2", bool(tprted));
		text::add_line_with_condition(state, contents, "alice_load_unload_3", loc.index() < state.province_definitions.first_sea_province.index());
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type t) noexcept override {
		if(visible)
			return button_element_base::test_mouse(state, x, y, t);
		return message_result::unseen;
	}
};
class unit_details_unload_navy_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::navy_id>(state, parent);
		auto tprted = state.world.navy_get_army_transport(n);
		std::vector<dcon::army_id> tmp;
		for(auto t : tprted)
			tmp.push_back(t.get_army());
		for(auto a : tmp)
			command::embark_army(state, state.local_player_nation, a);
	}

	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::navy_id>(state, parent);
		auto tprted = state.world.navy_get_army_transport(n);
		auto loc = state.world.navy_get_location_from_navy_location(n);

		if(tprted.begin() == tprted.end() || loc.index() >= state.province_definitions.first_sea_province.index()) {
			disabled = true;
		} else {
			disabled = false;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = retrieve<dcon::navy_id>(state, parent);
		auto tprted = state.world.navy_get_army_transport(n);
		auto loc = state.world.navy_get_location_from_navy_location(n);

		text::add_line(state, contents, "alice_unload_fleet");
		text::add_line_with_condition(state, contents, "alice_load_unload_4", tprted.begin() != tprted.end());
		text::add_line_with_condition(state, contents, "alice_load_unload_5", loc.index() < state.province_definitions.first_sea_province.index());
	}
};

class navy_transport_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.position.x -= int16_t(50);
		simple_text_element_base::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::navy_id>(state, parent);

		auto tcap = military::transport_capacity(state, n);
		auto tfree = military::free_transport_capacity(state, n);
		text::substitution_map sub;
		if(tcap != tfree) {
			auto txt = std::to_string(tfree) + " (" + std::to_string(tcap) + ")";
			text::add_to_substitution_map(sub, text::variable_type::num, std::string_view(txt));
			set_text(state, text::resolve_string_substitution(state, std::string_view("load_capacity_label"), sub));
		} else {
			text::add_to_substitution_map(sub, text::variable_type::num, tcap);
			set_text(state, text::resolve_string_substitution(state, std::string_view("load_capacity_label"), sub));
		}
	}

};

class unit_details_hunt_rebels : public button_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		visible = !state.world.army_get_is_rebel_hunter(a) && state.world.army_get_controller_from_army_control(a) == state.local_player_nation;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return button_element_base::test_mouse(state, x, y, type);
		return message_result::unseen;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "unit_enable_rebel_hunt");
	}
	void button_action(sys::state& state) noexcept override {
		command::toggle_rebel_hunting(state, state.local_player_nation, retrieve<dcon::army_id>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
};
class unit_details_dont_hunt_rebels : public button_element_base {
public:
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		visible = state.world.army_get_is_rebel_hunter(a) && state.world.army_get_controller_from_army_control(a) == state.local_player_nation;
	}
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		if(visible)
			return button_element_base::test_mouse(state, x, y, type);
		return message_result::unseen;
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_unit_disable_rebel_hunt");
	}
	void button_action(sys::state& state) noexcept override {
		command::toggle_rebel_hunting(state, state.local_player_nation, retrieve<dcon::army_id>(state, parent));
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			button_element_base::render(state, x, y);
	}
};
class unit_details_ai_controlled : public checkbox_button {
public:
	bool is_active(sys::state& state) noexcept override {
		return state.world.army_get_is_ai_controlled(retrieve<dcon::army_id>(state, parent));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_ai_controlled_unit");
	}
	void button_action(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		command::toggle_unit_ai_control(state, state.local_player_nation, a);
	}
	void on_update(sys::state& state) noexcept override {
		auto a = retrieve<dcon::army_id>(state, parent);
		disabled = state.world.army_get_controller_from_army_control(a) != state.local_player_nation;
	}
};

class unit_supply_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto army = retrieve<dcon::army_id>(state, parent);
		auto navy = retrieve<dcon::navy_id>(state, parent);

		economy::commodity_set commodities;

		auto owner = state.local_player_nation;
		auto capital = state.world.nation_get_capital(owner);
		auto s = state.world.province_get_state_membership(capital);
		auto m = state.world.state_instance_get_market_from_local_market(s);

		float spending_level = .0f;

		if(army) {
			commodities = military::get_required_supply(state, state.local_player_nation, army);
			spending_level = float(state.world.nation_get_land_spending(owner)) / 100.0f;
		} else if(navy) {
			commodities = military::get_required_supply(state, state.local_player_nation, navy);
			spending_level = float(state.world.nation_get_naval_spending(owner)) / 100.0f;
		}

		
		uint32_t total_commodities = state.world.commodity_size();

		float max_supply = 0.0f;
		float actual_supply = 0.0f;


		auto nations_commodity_spending = state.world.nation_get_spending_level(owner);

		for(uint32_t i = 0; i < total_commodities; ++i) {
			if(!commodities.commodity_type[i]) {
				break;
			}

			dcon::commodity_id c = commodities.commodity_type[i];

			auto satisfaction = state.world.market_get_demand_satisfaction(m, c);
			auto val = commodities.commodity_type[i];

			max_supply += commodities.commodity_amounts[i];
			actual_supply += commodities.commodity_amounts[i] * satisfaction * nations_commodity_spending * spending_level;
		}

		float median_supply = max_supply > 0.0f ? actual_supply / max_supply : 0.0f;

		progress = median_supply;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto army = retrieve<dcon::army_id>(state, parent);
		auto navy = retrieve<dcon::navy_id>(state, parent);
		economy::commodity_set commodities;

		float spending_level = .0f;
		auto owner = state.local_player_nation;
		auto capital = state.world.nation_get_capital(owner);
		auto s = state.world.province_get_state_membership(capital);
		auto m = state.world.state_instance_get_market_from_local_market(s);

		if(army) {
			commodities = military::get_required_supply(state, state.local_player_nation, army);
			spending_level = float(state.world.nation_get_land_spending(owner)) / 100.0f;
		} else if(navy) {
			commodities = military::get_required_supply(state, state.local_player_nation, navy);
			spending_level = float(state.world.nation_get_naval_spending(owner)) / 100.0f;
		}
		
		
		uint32_t total_commodities = state.world.commodity_size();

		float max_supply = 0.0f;
		float actual_supply = 0.0f;

		auto nations_commodity_spending = state.world.nation_get_spending_level(owner);
		for(uint32_t i = 0; i < total_commodities; ++i) {
			if(!commodities.commodity_type[i]) {
				break;
			}
			dcon::commodity_id c = commodities.commodity_type[i];

			auto satisfaction = state.world.market_get_demand_satisfaction(m, c);
			auto val = commodities.commodity_type[i];

			max_supply += commodities.commodity_amounts[i];
			actual_supply += commodities.commodity_amounts[i] * satisfaction * nations_commodity_spending * spending_level;			
		}

		float median_supply = max_supply > 0.0f ? actual_supply / max_supply : 0.0f;
		text::add_line(state, contents, "unit_current_supply", text::variable_type::val, int16_t(median_supply * 100.f));
		text::add_line_break_to_layout(state, contents);
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(commodities.commodity_type[i] && commodities.commodity_amounts[i] > 0) {
				dcon::commodity_id c = commodities.commodity_type[i];
				float satisfaction = state.world.market_get_demand_satisfaction(m, c);
				float wanted_commodity = commodities.commodity_amounts[i];
				float actual_commodity = commodities.commodity_amounts[i] * satisfaction * nations_commodity_spending * spending_level;

				int32_t display_satisfaction = int32_t(satisfaction * 100);

				if(satisfaction == 1 || satisfaction >= 0.95) {
					text::add_line(state, contents, "unit_current_supply_high", text::variable_type::what, state.world.commodity_get_name(commodities.commodity_type[i]), text::variable_type::val, text::fp_three_places{ actual_commodity }, text::variable_type::value, text::fp_three_places{ wanted_commodity }, text::variable_type::total, display_satisfaction);
				} else if (satisfaction < 0.95 && satisfaction >= 0.5) {
					text::add_line(state, contents, "unit_current_supply_mid", text::variable_type::what, state.world.commodity_get_name(commodities.commodity_type[i]), text::variable_type::val, text::fp_three_places{ actual_commodity }, text::variable_type::value, text::fp_three_places{ wanted_commodity }, text::variable_type::total, display_satisfaction);
				} else {
					text::add_line(state, contents, "unit_current_supply_low", text::variable_type::what, state.world.commodity_get_name(commodities.commodity_type[i]), text::variable_type::val, text::fp_three_places{ actual_commodity }, text::variable_type::value, text::fp_three_places{ wanted_commodity }, text::variable_type::total, display_satisfaction);
				}
			}
		}
	}
};

class main_template_composition_label : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto contents = text::create_endless_layout(
			state,
			internal_layout,
			text::layout_parameters{
				0,
				0,
				static_cast<int16_t>(base_data.size.x),
				static_cast<int16_t>(base_data.size.y),
				base_data.data.text.font_handle,
				0,
				text::alignment::left,
				text::text_color::white,
				true
			});

		auto box = text::open_layout_box(contents);

		for(dcon::unit_type_id::value_base_t i = 0; i < state.military_definitions.unit_base_definitions.size(); i++) {
			auto amount = state.ui_state.current_template == -1 ? uint8_t(0) : state.ui_state.templates[state.ui_state.current_template].amounts[i];

			if(amount < 1) {
				continue;
			}

			auto const utid = dcon::unit_type_id(i);

			std::string padding = i < 10 ? "0" : "";

			text::add_to_layout_box(state, contents, box, text::int_wholenum{ amount });

			std::string description = "@*" + padding + std::to_string(i);

			text::add_unparsed_text_to_layout_box(state, contents, box, description);
		}

		text::close_layout_box(contents, box);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "unit_macro_template_tooltip");
	}
};

class apply_template_to_army_location_button : public button_element_base {
	std::vector<dcon::province_id> provinces;
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		disabled = false;
	}
	void button_action(sys::state& state) noexcept override {
		if(state.ui_state.current_template == -1)
			return;

		auto army = retrieve<dcon::army_id>(state, parent);
		auto army_location = state.world.army_get_location_from_army_location(army);

		state.fill_vector_of_connected_provinces(army_location, true, provinces);
		if(provinces.empty())
			return;

		std::array<uint8_t, sys::macro_builder_template::max_types> current_distribution;
		current_distribution.fill(0);

		for(auto reg : state.world.army_get_army_membership(army)) {
			current_distribution[reg.get_regiment().get_type().index()] += 1;
		}

		state.build_up_to_template_land(
			state.ui_state.templates[state.ui_state.current_template],
			army_location,
			provinces,
			current_distribution
		);
	}
};

class apply_template_container : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "composition") {
			return make_element_by_type<main_template_composition_label>(state, id);
		} else if(name == "background") {
			return make_element_by_type< apply_template_to_army_location_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<class T>
class unit_details_buttons : public window_element_base {
private:
	simple_text_element_base* navytransport_text = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			auto ptr = make_element_by_type<unit_details_ai_controlled>(state, "alice_enable_ai_controlled");
			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "load_button" && std::is_same_v<T, dcon::army_id>) {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_load_army_button>(state, id);
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else if(name == "unload_button") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_unload_army_button>(state, id);
			} else {
				return make_element_by_type<unit_details_unload_navy_button>(state, id);
			}
		} else if(name == "enable_rebel_button") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_hunt_rebels>(state, id);
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else if(name == "disable_rebel_button") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_dont_hunt_rebels>(state, id);
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else if(name == "attach_unit_button"
			|| name == "detach_unit_button"
			|| name == "select_land") {

			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "header") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<invisible_element>(state, id);
			} else {
				return make_element_by_type< navy_transport_text>(state, id);
			}
		} else if(name == "alice_build_up_to_template_window") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<apply_template_container>(state, id);
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else {
			return nullptr;
		}
	}
};

class unit_type_listbox_entry_image : public image_element_base {
	void on_update(sys::state& state) noexcept override {
		auto regiment_type = retrieve<dcon::unit_type_id>(state, parent);

		if(regiment_type) {
			frame = state.military_definitions.unit_base_definitions[regiment_type].icon - 1;
		}
	}
};

class unit_type_listbox_entry_label : public button_element_base {
	void on_update(sys::state& state) noexcept override {
		auto regiment_type = retrieve<dcon::unit_type_id>(state, parent);

		if(regiment_type) {
			auto name_string_def = state.military_definitions.unit_base_definitions[regiment_type].name;
			set_button_text(state, text::produce_simple_string(state, name_string_def));
		}

		auto const& ut = state.military_definitions.unit_base_definitions[regiment_type];
		if(!ut.active && !state.world.nation_get_active_unit(state.local_player_nation, regiment_type)) {
			disabled = true;
			return;
		}

		dcon::regiment_id regs[command::num_packed_units];
		dcon::ship_id ships[command::num_packed_units];
		auto allowed_transition = true;

		for(unsigned i = 0; i < state.selected_regiments.size(); i++) {
			if(state.selected_regiments[i]) {
				regs[i % command::num_packed_units] = state.selected_regiments[i];
			}
			if(state.selected_ships[i]) {
				ships[i % command::num_packed_units] = state.selected_ships[i];
			}

			if(i % command::num_packed_units == 0) {
				allowed_transition = command::can_change_unit_type(state, state.local_player_nation, regs, ships, regiment_type);
				if(!allowed_transition) {
					disabled = true; return;
				}
			}

			if(!state.selected_regiments[i] && !state.selected_ships[i]) {
				break;
			}
		}

		allowed_transition = command::can_change_unit_type(state, state.local_player_nation, regs, ships, regiment_type);
		if(!allowed_transition) {
			disabled = true;
			return;
		}

		disabled = false;
	}

	void button_action(sys::state& state) noexcept override {
		auto regiment_type = retrieve<dcon::unit_type_id>(state, parent);
		dcon::regiment_id regs[command::num_packed_units];
		dcon::ship_id ships[command::num_packed_units];

		for(unsigned i = 0; i < state.selected_regiments.size(); i++) {
			if(state.selected_regiments[i]) {
				regs[i % command::num_packed_units] = state.selected_regiments[i];
			}
			if(state.selected_ships[i]) {
				ships[i % command::num_packed_units] = state.selected_ships[i];
			}

			if(i % command::num_packed_units == 0) {
				command::change_unit_type(state, state.local_player_nation, regs, ships, regiment_type);
			}

			if(!state.selected_regiments[i] && !state.selected_ships[i]) {
				break;
			}
		}

		command::change_unit_type(state, state.local_player_nation, regs, ships, regiment_type);
		sys::selected_regiments_clear(state);
		sys::selected_ships_clear(state);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "unit_upgrade_desc", text::variable_type::days, int64_t(state.defines.discredit_days));
		text::add_line_break_to_layout(state, contents);

		auto new_type = retrieve<dcon::unit_type_id>(state, parent);
		auto const& ut = state.military_definitions.unit_base_definitions[new_type];

		if(ut.is_land) {
			if(state.world.nation_get_unit_stats(state.local_player_nation, new_type).reconnaissance_or_fire_range > 0) {
				text::add_line(state, contents, "unit_recon", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).reconnaissance_or_fire_range, 2));
			}
			if(state.world.nation_get_unit_stats(state.local_player_nation, new_type).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "unit_siege", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).siege_or_torpedo_attack, 2));
			}

			text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).attack_or_gun_power, 2));
			text::add_line(state, contents, "unit_defence", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).defence_or_hull, 2));
			text::add_line(state, contents, "unit_discipline", text::variable_type::x, text::format_percentage(ut.discipline_or_evasion, 0));
			if(ut.support > 0) {
				text::add_line(state, contents, "unit_support", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).support, 0));
			}
			text::add_line(state, contents, "unit_maneuver", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).maneuver, 0));
			text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).maximum_speed, 2));
			text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).supply_consumption * 100, 0));
			text::add_line(state, contents, "unit_supply_load", text::variable_type::x, ut.supply_consumption_score);
		}
		else {
			text::add_line(state, contents, "unit_max_speed", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).maximum_speed, 2));
			text::add_line(state, contents, "unit_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).attack_or_gun_power, 2));
			if(state.world.nation_get_unit_stats(state.local_player_nation, new_type).siege_or_torpedo_attack > 0) {
				text::add_line(state, contents, "unit_torpedo_attack", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).siege_or_torpedo_attack, 2));
			}
			text::add_line(state, contents, "unit_hull", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).defence_or_hull, 2));
			text::add_line(state, contents, "unit_fire_range", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).reconnaissance_or_fire_range, 2));
			if(ut.discipline_or_evasion > 0) {
				text::add_line(state, contents, "unit_evasion", text::variable_type::x, text::format_percentage(ut.discipline_or_evasion, 0));
			}
			text::add_line(state, contents, "unit_supply_consumption", text::variable_type::x, text::format_float(state.world.nation_get_unit_stats(state.local_player_nation, new_type).supply_consumption * 100, 0));
			text::add_line(state, contents, "unit_supply_load", text::variable_type::x, ut.supply_consumption_score);
		}

		text::add_line_break_to_layout(state, contents);

		text::add_line_with_condition(state, contents, "unit_upgrade_explain_1", !state.selected_regiments[0] || !state.selected_ships[0]);
		text::add_line_with_condition(state, contents, "unit_upgrade_explain_2", ut.active || state.world.nation_get_active_unit(state.local_player_nation, new_type));

		if(!ut.is_land && ut.type == military::unit_type::big_ship) {
			auto any_non_big_ship = false;
			for(unsigned i = 0; i < state.selected_ships.size(); i++) {
				if(!state.selected_ships[i]) {
					break;
				}
				auto shiptype = state.world.ship_get_type(state.selected_ships[i]);
				auto st = state.military_definitions.unit_base_definitions[shiptype];
				if(st.type != military::unit_type::big_ship) {
					any_non_big_ship = true;
				}
			}

			text::add_line_with_condition(state, contents, "unit_upgrade_explain_3", !any_non_big_ship);
		}

		if(ut.is_land) {
			auto any_breaking_army_check = false;
			for(unsigned i = 0; i < state.selected_regiments.size(); i++) {
				if(!state.selected_regiments[i]) {
					break;
				}
				auto a = state.world.regiment_get_army_from_army_membership(state.selected_regiments[i]);

				if(state.world.army_get_controller_from_army_control(a) != state.local_player_nation || state.world.army_get_is_retreating(a) || state.world.army_get_navy_from_army_transport(a) ||
				bool(state.world.army_get_battle_from_army_battle_participation(a))) {
					any_breaking_army_check = true;
				}
			}

			text::add_line_with_condition(state, contents, "unit_upgrade_explain_4", !any_breaking_army_check);
		}

		if(!ut.is_land) {
			auto any_breaking_navy_check = false;
			auto any_breaking_navy_base_check = false;
			// Navy-level checks
			for(unsigned i = 0; i < state.selected_ships.size(); i++) {
				if(!state.selected_ships[i]) {
					break;
				}
				auto n = state.world.ship_get_navy_from_navy_membership(state.selected_ships[i]);
				auto embarked = state.world.navy_get_army_transport(n);
				if(state.world.navy_get_controller_from_navy_control(n) != state.local_player_nation || state.world.navy_get_is_retreating(n) ||
					bool(state.world.navy_get_battle_from_navy_battle_participation(n)) || embarked.begin() != embarked.end()) {
					any_breaking_navy_check = true;
				}

				if(ut.min_port_level) {
					auto fnid = dcon::fatten(state.world, n);

					auto loc = fnid.get_location_from_navy_location();

					// Ship requires naval base level for construction but province location doesn't have one
					if(loc.get_building_level(uint8_t(economy::province_building_type::naval_base)) < ut.min_port_level) {
						any_breaking_navy_base_check = true;
					}
				}
			}

			text::add_line_with_condition(state, contents, "unit_upgrade_explain_5", !any_breaking_navy_check);
			text::add_line_with_condition(state, contents, "unit_upgrade_explain_6", !any_breaking_navy_base_check);
		}
	}
};

class unit_type_listbox_entry : public listbox_row_element_base<dcon::unit_type_id> {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_type_icon") {
			return make_element_by_type<unit_type_listbox_entry_image>(state, id);
		} else if(name == "unit_type_select") {
			return make_element_by_type<unit_type_listbox_entry_label>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<class T>
class unit_type_listbox : public listbox_element_base<unit_type_listbox_entry, dcon::unit_type_id> {
protected:
	std::string_view get_row_element_name() override {
		return "unit_type_item";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		auto is_land = true;

		if constexpr(std::is_same_v<T, dcon::army_id>) {
			if(parent) {
				
			}
		} else if constexpr(std::is_same_v<T, dcon::navy_id>) {
			if(parent) {
				is_land = false;
			}
		}

		for(dcon::unit_type_id::value_base_t i = 2; i < state.military_definitions.unit_base_definitions.size(); i++) {
			auto const utid = dcon::unit_type_id(i);
			auto const& ut = state.military_definitions.unit_base_definitions[utid];
			if(ut.is_land == is_land) {
				row_contents.push_back(utid);
			}
		}

		update(state);
	}
};

template<class T>
class unit_upgrade_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_type_list") {
			return make_element_by_type<unit_type_listbox<T>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(!state.selected_regiments.at(0) && !state.selected_ships.at(0)) {
			set_visible(state, false);
			return;
		}
	}
};


class dug_in_icon : public image_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto army = retrieve<dcon::army_id>(state, parent);
		uint8_t current_dig_in = state.world.army_get_dig_in(army);
		auto controller = military::tech_nation_for_army(state, army);
		uint8_t max_dig_in = uint8_t(state.world.nation_get_modifier_values(controller, sys::national_mod_offsets::dig_in_cap));
		text::add_line(state, contents, "DIG_IN_TOOLTIP_1", text::variable_type::lev, text::format_wholenum(current_dig_in), text::variable_type::max, text::format_wholenum(max_dig_in));
		text::add_line(state, contents, "DIG_IN_TOOLTIP_2", text::variable_type::days, text::format_wholenum(uint32_t(state.defines.dig_in_increase_each_days)), text::variable_type::val, text::format_wholenum(1));

	}
};


template<class T>
class unit_details_window : public window_element_base {
	simple_text_element_base* unitspeed_text = nullptr;
	image_element_base* unitrecon_icon = nullptr;
	simple_text_element_base* unitrecon_text = nullptr;
	image_element_base* unitengineer_icon = nullptr;
	simple_text_element_base* unitengineer_text = nullptr;
	progress_bar* unitsupply_bar = nullptr;
	dug_in_icon* unitdugin_icon = nullptr;
	unit_selection_panel<T>* unit_selection_win = nullptr;

	unit_upgrade_window<T>* unit_upgrade_win = nullptr;

public:
	T unit_id;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.y = 250;

		xy_pair base_position = {20,
				0}; // state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item_start")->second.definition].position;
		xy_pair base_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("unittype_item_offset"))->second.definition].position;

		{
			auto win = make_element_by_type<unit_details_type_item<T, 0>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (0 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (0 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_details_type_item<T, 1>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (1 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (1 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_details_type_item<T, 2>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (2 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (2 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}

		const xy_pair item_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition].position;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			auto ptr = make_element_by_type<unit_details_army_listbox>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("sup_subunits"))->second.definition);
			ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
			ptr->base_data.size.y += 32;
			add_child_to_front(std::move(ptr));
		} else {
			auto ptr = make_element_by_type<unit_details_navy_listbox>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("sup_subunits"))->second.definition);
			ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
			ptr->base_data.size.y += 32;
			add_child_to_front(std::move(ptr));
		}

		{
			auto ptr = make_element_by_type<unit_details_buttons<T>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("sup_buttons_window"))->second.definition);
			ptr->base_data.position.y = base_data.size.y; // Nudge
			add_child_to_front(std::move(ptr));
		}

		{
			auto ptr =
					make_element_by_type<unit_selection_panel<T>>(state, state.ui_state.defs_by_name.find(state.lookup_key("unitpanel"))->second.definition);
			unit_selection_win = ptr.get();
			ptr->base_data.position.y = -80;
			add_child_to_front(std::move(ptr));
		}

		{
			auto ptr =
				make_element_by_type<unit_upgrade_window<T>>(state, state.ui_state.defs_by_name.find(state.lookup_key("unit_upgrade_window"))->second.definition);
			unit_upgrade_win = ptr.get();
			add_child_to_front(std::move(ptr));
			unit_upgrade_win->set_visible(state, false);
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_bottom_bg") {
			return make_element_by_type<unit_panel_dynamic_tinted_bg<T>>(state, id);
		} else if(name == "icon_speed") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "speed") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitspeed_text = ptr.get();
			return ptr;
		} else if(name == "icon_recon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unitrecon_icon = ptr.get();
			return ptr;
		} else if(name == "recon") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitrecon_text = ptr.get();
			return ptr;
		} else if(name == "icon_engineer") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			unitengineer_icon = ptr.get();
			return ptr;
		} else if(name == "engineer") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitengineer_text = ptr.get();
			return ptr;
		} else if(name == "icon_supplies_small") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "supply_status") {
			auto ptr = make_element_by_type<unit_supply_bar>(state, id);
			unitsupply_bar = ptr.get();
			return ptr;
		} else if(name == "unitstatus_dugin") {
			auto ptr = make_element_by_type<dug_in_icon>(state, id);
			unitdugin_icon = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if constexpr(std::is_same_v<T, dcon::navy_id>) {
			unitengineer_icon->set_visible(state, false);
			unitengineer_text->set_visible(state, false);
			unitrecon_icon->set_visible(state, false);
			unitrecon_text->set_visible(state, false);

			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::val, uint16_t(military::effective_navy_speed(state, unit_id)));
			unitspeed_text->set_text(state, text::resolve_string_substitution(state, std::string_view("speed"), sub));
		} else if constexpr(std::is_same_v<T, dcon::army_id>) {
			unitengineer_icon->set_visible(state, true);
			unitengineer_text->set_visible(state, true);
			unitengineer_text->set_text(state, text::format_percentage(military::get_army_siege_eff(state, unit_id), 0));
			unitrecon_icon->set_visible(state, true);
			unitrecon_text->set_visible(state, true);
			unitrecon_text->set_text(state, text::format_percentage( military::get_army_recon_eff(state, unit_id), 0));

			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::val, uint16_t(military::effective_army_speed(state, unit_id)));
			unitspeed_text->set_text(state, text::resolve_string_substitution(state, std::string_view("speed"), sub));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<T>()) {
			payload.emplace<T>(unit_id);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<T>>()) {
			unit_id = any_cast<element_selection_wrapper<T>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::leader_id>>()) {
			auto content = any_cast<element_selection_wrapper<dcon::leader_id>>(payload).data;
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				command::change_general(state, state.local_player_nation, unit_id, content);
			} else {
				command::change_admiral(state, state.local_player_nation, unit_id, content);
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<unitpanel_action>>()) {
			auto content = any_cast<element_selection_wrapper<unitpanel_action>>(payload).data;
			switch(content) {
			case unitpanel_action::close:
			{
				Cyto::Any cpayload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::close}};
				unit_selection_win->reorg_window->impl_get(state, cpayload);
				state.selected_armies.clear();
				state.selected_navies.clear();
				set_visible(state, false);
				state.game_state_updated.store(true, std::memory_order_release);
				break;
			}
			case unitpanel_action::upgrade: {
				unit_upgrade_win->is_visible() ? unit_upgrade_win->set_visible(state, false) : unit_upgrade_win->set_visible(state, true);
				unit_upgrade_win->impl_on_update(state);
				break;
			}
			default: 
				break;
			};
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class units_selected_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::x, int64_t(state.selected_armies.size() + state.selected_navies.size()));
		set_text(state, text::resolve_string_substitution(state, "multiunit_header", m));
	}
};

class merge_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(state.selected_armies.size() > 0) {
			auto first = state.selected_armies[0];
			for(uint32_t i = 1; i < state.selected_armies.size(); ++i) {
				command::merge_armies(state, state.local_player_nation, first, state.selected_armies[i]);
			}
		}
		if(state.selected_navies.size() > 0) {
			auto first = state.selected_navies[0];
			for(uint32_t i = 1; i < state.selected_navies.size(); ++i) {
				command::merge_navies(state, state.local_player_nation, first, state.selected_navies[i]);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		disabled = false;

		if(state.selected_armies.size() > 0) {

			auto first = state.selected_armies[0];

			for(uint32_t i = 1; i < state.selected_armies.size(); ++i) {
				if(!command::can_merge_armies(state, state.local_player_nation, first, state.selected_armies[i])) {
					disabled = true;
					return;
				}
			}
		}
		if(state.selected_navies.size() > 0) {
			auto first = state.selected_navies[0];

			for(uint32_t i = 1; i < state.selected_navies.size(); ++i) {
				if(!command::can_merge_navies(state, state.local_player_nation, first, state.selected_navies[i])) {
					disabled = true;
					return;
				}
			}
		}
	}
};

class deselect_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		state.selected_armies.clear();
		state.selected_navies.clear();
		state.game_state_updated.store(true, std::memory_order_release);
	}
};

class disband_all_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(state.user_settings.unit_disband_confirmation) {
			if(!state.selected_armies.empty()) {
				std::vector<dcon::army_id> units = state.selected_armies;
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::army_id> {units});
			} else if(!state.selected_navies.empty()) {
				std::vector<dcon::navy_id> units = state.selected_navies;
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::navy_id> {units});
			}
		}
		else {
			for(auto army : state.selected_armies) {
				command::delete_army(state, state.local_player_nation, army);
			}
			for(auto navy : state.selected_navies) {
				command::delete_navy(state, state.local_player_nation, navy);
			}
		}

	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "disband_all");
	}
};

class whole_panel_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			state.selected_armies.clear();
			state.selected_navies.clear();
			state.select(std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			state.selected_armies.clear();
			state.selected_navies.clear();
			state.select(std::get<dcon::navy_id>(foru));
		}
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			state.deselect(std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			state.deselect(std::get<dcon::navy_id>(foru));
		}
	}
};

class u_row_strength : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		float total = 0.0f;
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				total += r.get_regiment().get_strength() * state.defines.pop_size_per_regiment;
			}
			set_text(state, text::format_wholenum(int32_t(total)));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				total += r.get_ship().get_strength();
			}
			set_text(state, text::format_float(total, 1));
		}
	}
};
class u_row_attrition_text : public color_text_element {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<unit_var>(state, parent);
		float amount = 0.0;
		if(std::holds_alternative<dcon::army_id>(content)) {
			amount = military::attrition_amount(state, std::get<dcon::army_id>(content));
		}
		else if(std::holds_alternative<dcon::navy_id>(content)) {
			amount = military::attrition_amount(state, std::get<dcon::navy_id>(content));
		}
		if(amount > 0.0f) {
			set_text(state, text::format_percentage(amount, 1));
			color = text::text_color::red;
		} else {
			set_text(state, "");
		}
	}
};

class u_row_attrit_icon : public image_element_base {
	bool visible = false;
	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			visible = military::will_recieve_attrition(state, a);
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			visible = military::will_recieve_attrition(state, a);
		} else {
			visible = false;
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			image_element_base::render(state, x, y);
	}
};

class u_row_org_bar : public vertical_progress_bar {
	void on_update(sys::state& state) noexcept override {
		float current = 0.0f;
		float total = 0.0f;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				current += r.get_regiment().get_org();
				total += 1.0f;
			}
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				current += r.get_ship().get_org();
				total += 1.0f;
			}
		}
		progress = total > 0.0f ? current / total : 0.0f;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, text::format_percentage(progress));
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::land_organisation, true);
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::naval_organisation, true);
		}
	}
};

class u_row_str_bar : public vertical_progress_bar {
	void on_update(sys::state& state) noexcept override {
		float current = 0.0f;
		float total = 0.0f;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				current += r.get_regiment().get_strength();
				total += 1.0f;
			}
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				current += r.get_ship().get_strength();
				total += 1.0f;
			}
		}
		progress = total > 0.0f ? current / total : 0.0f;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, text::format_percentage(progress));
	}
};

class u_row_disband : public button_element_base {
public:

	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			disabled = !command::can_delete_army(state, state.local_player_nation, std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			disabled = !command::can_delete_navy(state, state.local_player_nation, std::get<dcon::navy_id>(foru));
		}
	}
	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(state.user_settings.unit_disband_confirmation) {
			if(std::holds_alternative<dcon::army_id>(foru)) {
				std::vector<dcon::army_id> units;
				units.push_back(std::get<dcon::army_id>(foru));
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::army_id> {units});
			} else if(std::holds_alternative<dcon::navy_id>(foru)) {
				std::vector<dcon::navy_id> units;
				units.push_back(std::get<dcon::navy_id>(foru));
				send(state, state.ui_state.disband_unit_window, disband_unit_wrapper<dcon::navy_id> {units});
			}
		}
		else {
			if(std::holds_alternative<dcon::army_id>(foru)) {
				command::delete_army(state, state.local_player_nation, std::get<dcon::army_id>(foru));
			} else if(std::holds_alternative<dcon::navy_id>(foru)) {
				command::delete_navy(state, state.local_player_nation, std::get<dcon::navy_id>(foru));
			}
		}
		
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "disband_unit");
	}
};

class u_row_remove : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			state.deselect(std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			state.deselect(std::get<dcon::navy_id>(foru));
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "deselect_unit");
	}
};

class u_row_split : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			command::evenly_split_army(state, state.local_player_nation, std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			command::evenly_split_navy(state, state.local_player_nation, std::get<dcon::navy_id>(foru));
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			disabled = !command::can_evenly_split_army(state, state.local_player_nation, std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			disabled = !command::can_evenly_split_navy(state, state.local_player_nation, std::get<dcon::navy_id>(foru));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "splitinhalf");
	}
};

class u_row_new : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		// TODO
	}
};

class u_row_inf : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			frame = 0;
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			frame = 3;
		}
	}
};

class u_row_cav : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			frame = 1;
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			frame = 4;
		}
	}
};

class u_row_art : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			frame = 2;
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			frame = 5;
		}
	}
};

class u_row_inf_count : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t total = 0;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_regiment().get_type()].type == military::unit_type::infantry) {
					++total;
				}
			}
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_ship().get_type()].type == military::unit_type::big_ship) {
					++total;
				}
			}
		}

		set_text(state, std::to_string(total));
	}
};

class u_row_cav_count : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t total = 0;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_regiment().get_type()].type == military::unit_type::cavalry) {
					++total;
				}
			}
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_ship().get_type()].type == military::unit_type::light_ship) {
					++total;
				}
			}
		}

		set_text(state, std::to_string(total));
	}
};

class u_row_art_count : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		int32_t total = 0;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			for(auto r : state.world.army_get_army_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_regiment().get_type()].type == military::unit_type::support || state.military_definitions.unit_base_definitions[r.get_regiment().get_type()].type == military::unit_type::special) {
					++total;
				}
			}
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			for(auto r : state.world.navy_get_navy_membership(a)) {
				if(state.military_definitions.unit_base_definitions[r.get_ship().get_type()].type == military::unit_type::transport) {
					++total;
				}
			}
		}

		set_text(state, std::to_string(total));
	}
};

class u_row_location : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::province_id loc;
		auto foru = retrieve<unit_var>(state, parent);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			auto a = std::get<dcon::army_id>(foru);
			loc = state.world.army_get_location_from_army_location(a);
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			auto a = std::get<dcon::navy_id>(foru);
			loc = state.world.navy_get_location_from_navy_location(a);
		}

		set_text(state, text::produce_simple_string(state, state.world.province_get_name(loc)));
	}
};


class multi_selection_leader_image : public button_element_base {
public:
	dcon::gfx_object_id default_img;

	void on_update(sys::state& state) noexcept override {
		if(!default_img) {
			if(base_data.get_element_type() == ui::element_type::image)
				default_img = base_data.data.image.gfx_object;
			else if(base_data.get_element_type() == ui::element_type::button)
				default_img = base_data.data.button.button_image;
		}

		auto foru = retrieve<unit_var>(state, parent);
		dcon::leader_id lid;
		if(std::holds_alternative<dcon::army_id>(foru)) {
			lid = state.world.army_get_general_from_army_leadership(std::get<dcon::army_id>(foru));
			disabled = !command::can_change_general(state, state.local_player_nation, std::get<dcon::army_id>(foru), dcon::leader_id{});
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			lid = state.world.navy_get_admiral_from_navy_leadership(std::get<dcon::navy_id>(foru));
			disabled = !command::can_change_admiral(state, state.local_player_nation, std::get<dcon::navy_id>(foru), dcon::leader_id{});
		}

		auto pculture = state.world.nation_get_primary_culture(state.local_player_nation);
		auto ltype = pculture.get_group_from_culture_group_membership().get_leader();

		if(ltype && lid) {
			auto admiral = state.world.leader_get_is_admiral(lid);
			if(admiral) {
				auto arange = ltype.get_admirals();
				if(arange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), arange.size());

					if(base_data.get_element_type() == ui::element_type::image)
						base_data.data.image.gfx_object = arange[in_range];
					else if(base_data.get_element_type() == ui::element_type::button)
						base_data.data.button.button_image = arange[in_range];
				}
			} else {
				auto grange = ltype.get_generals();
				if(grange.size() > 0) {
					auto rval = rng::get_random(state, uint32_t(state.world.leader_get_since(lid).value), uint32_t(lid.value));
					auto in_range = rng::reduce(uint32_t(rval), grange.size());

					if(base_data.get_element_type() == ui::element_type::image)
						base_data.data.image.gfx_object = grange[in_range];
					else if(base_data.get_element_type() == ui::element_type::button)
						base_data.data.button.button_image = grange[in_range];
				}
			}
		} else {
			if(base_data.get_element_type() == ui::element_type::image)
				base_data.data.image.gfx_object = default_img;
			else if(base_data.get_element_type() == ui::element_type::button)
				base_data.data.button.button_image = default_img;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return  tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		dcon::leader_id lid;
		if(std::holds_alternative<dcon::army_id>(foru)) {
			lid = state.world.army_get_general_from_army_leadership(std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			lid = state.world.navy_get_admiral_from_navy_leadership(std::get<dcon::navy_id>(foru));
		}
		display_leader_full(state, lid, contents, 0);
	}


	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		auto location = get_absolute_non_mirror_location(state, *this);
		if(std::holds_alternative<dcon::army_id>(foru)) {
			open_leader_selection(state, std::get<dcon::army_id>(foru), dcon::navy_id{}, location.x + base_data.size.x, location.y);
		} else {
			open_leader_selection(state, dcon::army_id{}, std::get<dcon::navy_id>(foru), location.x + base_data.size.x, location.y);
		}
	}
};

class selected_unit_item : public listbox_row_element_base<unit_var> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unitpanel_bg") {
			return make_element_by_type<whole_panel_button>(state, id);
		} else if(name == "leader_photo") {
			return make_element_by_type<multi_selection_leader_image>(state, id);
		} else if(name == "unitstrength") {
			return make_element_by_type <u_row_strength> (state, id);
		} else if(name == "unitattrition_icon") {
			return make_element_by_type<u_row_attrit_icon>(state, id);
		} else if(name == "unitattrition") {
			return make_element_by_type<u_row_attrition_text>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<u_row_org_bar>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<u_row_str_bar>(state, id);
		} else if(name == "disbandbutton") {
			return make_element_by_type<u_row_disband>(state, id);
		} else if(name == "splitinhalf") {
			return make_element_by_type<u_row_split>(state, id);
		} else if(name == "newunitbutton") {
			return make_element_by_type<u_row_new>(state, id);
		} else if(name == "remove_unit_from_selection_button") {
			return make_element_by_type<u_row_remove>(state, id);
		} else if(name == "unit_inf") {
			return make_element_by_type<u_row_inf>(state, id);
		} else if(name == "unit_inf_count") {
			return make_element_by_type<u_row_inf_count>(state, id);
		} else if(name == "unit_cav") {
			return make_element_by_type<u_row_cav>(state, id);
		} else if(name == "unit_cav_count") {
			return make_element_by_type<u_row_cav_count>(state, id);
		} else if(name == "unit_art") {
			return make_element_by_type<u_row_art>(state, id);
		} else if(name == "unit_art_count") {
			return make_element_by_type<u_row_art_count>(state, id);
		} else if(name == "location") {
			return make_element_by_type<u_row_location>(state, id);
		} else {
			return nullptr;
		}
	}
};

class selected_unit_list : public listbox_element_base<selected_unit_item, unit_var> {
public:
	std::string_view get_row_element_name() override {
		return "alice_unit_row";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto i : state.selected_armies)
			row_contents.push_back(i);
		for(auto i : state.selected_navies)
			row_contents.push_back(i);
		update(state);
	}
};

class multi_unit_details_ai_controlled : public checkbox_button {
	bool visible = true;
public:
	bool is_active(sys::state& state) noexcept override {
		for(auto i : state.selected_armies) {
			if(state.world.army_get_is_ai_controlled(i) == false)
				return false;
		}
		return true;
	}
	void on_update(sys::state& state) noexcept override {
		checkbox_button::on_update(state);
		visible = state.selected_navies.empty();
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_ai_controlled_unit");
	}
	void button_action(sys::state& state) noexcept override {
		bool all_on = true;
		for(auto i : state.selected_armies) {
			if(state.world.army_get_is_ai_controlled(i) == false) {
				all_on = false;
				break;
			}
		}
		for(auto a : state.selected_armies) {
			if(all_on) { //all on -> turn all off
				command::toggle_unit_ai_control(state, state.local_player_nation, a);
			} else { //some on -> turn all that are off into on, all off -> turn all on
				if(!state.world.army_get_is_ai_controlled(a)) {
					command::toggle_unit_ai_control(state, state.local_player_nation, a);
				}
			}
		}
	}
	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		if(visible)
			checkbox_button::render(state, x, y);
	}
};

class mulit_unit_selection_panel : public window_element_base {



public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto ptr = make_element_by_type<multi_unit_details_ai_controlled>(state, "alice_enable_ai_controlled_multi");
		add_child_to_front(std::move(ptr));


	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "desc") {
			return make_element_by_type<units_selected_text>(state, id);
		} else if(name == "mergebutton") {
			return make_element_by_type<merge_all_button>(state, id);
		} else if(name == "close_multiunit") {
			return make_element_by_type<deselect_all_button> (state, id);
		} else if(name == "disband_multiunit") {
			return make_element_by_type<disband_all_button>(state, id);
		} else if(name == "unit_listbox") {
			return make_element_by_type<selected_unit_list>(state, id);
		} else {
			return nullptr;
		}
	}


};

struct army_group_unit_type_info {
	dcon::unit_type_id id;
	uint32_t amount;

	bool operator==(army_group_unit_type_info const& o) const {
		return id == o.id && amount == o.amount;
	}
	bool operator!=(army_group_unit_type_info const& o) const {
		return !(*this == o);
	}
};

using army_group_unit_type_info_optional = std::variant<std::monostate, army_group_unit_type_info>;

using army_group_unit_type_info_grid_row = std::array<army_group_unit_type_info_optional, 3>;

class unit_type_row_image : public image_element_base {
	void on_update(sys::state& state) noexcept override {
		auto regiment_type_data = retrieve<army_group_unit_type_info_optional>(state, parent);
		if(!std::holds_alternative<std::monostate>(regiment_type_data)) {
			frame = state.military_definitions.unit_base_definitions[std::get<army_group_unit_type_info>(regiment_type_data).id].icon - 1;
		}
	}
};

class unit_type_row_label : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto regiment_type_data = retrieve<army_group_unit_type_info_optional>(state, parent);
		if(!std::holds_alternative<std::monostate>(regiment_type_data)) {
			auto name_string_def = state.military_definitions.unit_base_definitions[std::get<army_group_unit_type_info>(regiment_type_data).id].name;
			set_text(state, text::produce_simple_string(state, name_string_def));
		}
	}
};

class unit_type_row_amount : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto regiment_type_data = retrieve<army_group_unit_type_info_optional>(state, parent);
		if(!std::holds_alternative<std::monostate>(regiment_type_data)) {
			set_text(state, std::to_string(std::get<army_group_unit_type_info>(regiment_type_data).amount));
		}
	}
};

class unit_type_grid_item : public window_element_base {
public:
	army_group_unit_type_info_optional display_unit;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_army_group_unit_type_entry_icon") {
			return make_element_by_type<unit_type_row_image>(state, id);
		} else if(name == "alice_army_group_unit_type_entry_name") {
			return make_element_by_type<unit_type_row_label>(state, id);
		} else if(name == "alice_army_group_unit_type_entry_amount") {
			return make_element_by_type<unit_type_row_amount>(state, id);
		}
		return nullptr;
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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<army_group_unit_type_info_optional>()) {
			payload.emplace<army_group_unit_type_info_optional>(display_unit);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class unit_type_row : public listbox_row_element_base<army_group_unit_type_info_grid_row> {
public:
	std::array< unit_type_grid_item*, 3> grid_items;

	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<army_group_unit_type_info_grid_row>::on_create(state);
		auto def = state.ui_state.defs_by_name.find(state.lookup_key("alice_army_group_unit_type_grid_item"))->second.definition;

		uint8_t additional_padding = 5;

		{
			auto win = make_element_by_type<unit_type_grid_item>(state, def);
			win->base_data.position.x = int16_t(additional_padding);
			win->base_data.position.y = int16_t(3);
			grid_items[0] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_type_grid_item>(state, def);
			win->base_data.position.x = int16_t(additional_padding * 2 + win->base_data.size.x);
			win->base_data.position.y = int16_t(3);
			grid_items[1] = win.get();
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_type_grid_item>(state, def);
			win->base_data.position.x = int16_t(additional_padding * 3 + win->base_data.size.x * 2);
			win->base_data.position.y = int16_t(3);
			grid_items[2] = win.get();
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
	}
};

class selected_army_group_land_units_list : public listbox_element_base<unit_type_row, army_group_unit_type_info_grid_row> {
public:
	std::string_view get_row_element_name() override {
		return "alice_army_group_unit_type_row";
	}

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		std::vector<uint32_t> regiments_by_type{ };
		regiments_by_type.resize(state.military_definitions.unit_base_definitions.size() + 2);

		if(state.selected_army_group) {
			auto group = fatten(state.world, state.selected_army_group);

			for(auto regiment_membership : group.get_automated_army_group_membership_regiment()) {
				auto regiment = regiment_membership.get_regiment().get_regiment_from_automation();
				auto type = regiment.get_type();
				if(type) {
					regiments_by_type[type.index()] += 1;
				}
			}
			for(auto navy_membership : group.get_automated_army_group_membership_navy()) {
				for(auto ship_membership : state.world.navy_get_navy_membership(navy_membership.get_navy())) {
					auto type = ship_membership.get_ship().get_type();
					if(type) {
						regiments_by_type[type.index()] += 1;
					}
				}				
			}
		}

		size_t unit_types = state.military_definitions.unit_base_definitions.size();
		for(size_t i = 2; i < unit_types; i += 3) {
			army_group_unit_type_info_grid_row content_of_grid_row;

			for(size_t j = 0; (j + i < unit_types) && (j < 3); j += 1) {
				dcon::unit_type_id type_id{ dcon::unit_type_id::value_base_t(j + i) };

				army_group_unit_type_info new_item = {
					.id = type_id,
					.amount = regiments_by_type[type_id.index()]
				};

				content_of_grid_row[j] = new_item;
			}

			row_contents.push_back({
				content_of_grid_row
			});
		}
		update(state);
	}
};

class selected_army_group_sea_units_list : public listbox_element_base<selected_unit_item, unit_var> {
public:
	std::string_view get_row_element_name() override {
		return "alice_unit_row";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(state.selected_army_group) {
			auto group = dcon::fatten(state.world, state.selected_army_group);
			for(auto navy_membership : group.get_automated_army_group_membership_navy()) {
				row_contents.push_back(navy_membership.get_navy());
			}
		}
		update(state);
	}
};

class army_group_details_window_header : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		xy_pair base_position = { 15, 0 }; 
		uint16_t base_offset = 95;

		{
			auto win = make_element_by_type<selected_army_group_land_details_item<0>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (0 * base_offset); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<selected_army_group_land_details_item<1>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (1 * base_offset); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<selected_army_group_land_details_item<2>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (2 * base_offset); // Flexnudge
			add_child_to_front(std::move(win));
		}

		{
			auto win = make_element_by_type<selected_army_group_sea_details_item<0>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (0 * base_offset); // Flexnudge
			win->base_data.position.y = 40;
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<selected_army_group_sea_details_item<1>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (1 * base_offset); // Flexnudge
			win->base_data.position.y = 40;
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<selected_army_group_sea_details_item<2>>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("unittype_item"))->second.definition);
			win->base_data.position.x = base_position.x + (2 * base_offset); // Flexnudge
			win->base_data.position.y = 40;
			add_child_to_front(std::move(win));
		}
	}
};

class army_group_details_window_body : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_army_group_unit_listbox") {
			return make_element_by_type<selected_army_group_land_units_list>(state, id);
		} 
		return nullptr;
	}
};

class toggle_defend_order_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		if(state.selected_army_group_order == sys::army_group_order::defend) {
			state.selected_army_group_order = sys::army_group_order::none;
		} else {
			state.selected_army_group_order = sys::army_group_order::defend;
		}
		on_update(state);
		state.game_state_updated.store(true, std::memory_order_release);
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group_order == sys::army_group_order::defend) {
			frame = 1;
		} else {
			frame = 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_battleplanner_defend_order");
	}
};

class toggle_enforce_control_order_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		if(state.selected_army_group_order == sys::army_group_order::siege) {
			state.selected_army_group_order = sys::army_group_order::none;
		} else {
			state.selected_army_group_order = sys::army_group_order::siege;
		}

		on_update(state);
		state.game_state_updated.store(true, std::memory_order_release);
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group_order == sys::army_group_order::siege) {
			frame = 1;
		} else {
			frame = 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_battleplanner_enforce_control_order");
	}
};

class toggle_ferry_origin_order_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		if(state.selected_army_group_order == sys::army_group_order::designate_port) {
			state.selected_army_group_order = sys::army_group_order::none;
		} else {
			state.selected_army_group_order = sys::army_group_order::designate_port;
		}
		on_update(state);
		state.game_state_updated.store(true, std::memory_order_release);
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group_order == sys::army_group_order::designate_port) {
			frame = 1;
		} else {
			frame = 0;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_battleplanner_travel_origin_order");
	}
};

class add_selected_units_to_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		for(auto item : state.selected_armies) {
			state.world.for_each_automated_army_group([&](dcon::automated_army_group_id group) {
				state.remove_army_army_group_clean(group, item);
			});			
			state.add_army_to_army_group(state.selected_army_group, item);
		}
		for(auto item : state.selected_navies) {
			state.world.for_each_automated_army_group([&](dcon::automated_army_group_id group) {
				state.remove_navy_from_army_group(group, item);
			});
			state.add_navy_to_army_group(state.selected_army_group, item);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group) {
			disabled = false;
		} else {
			disabled = true;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_armygroup_go_to_selection");
	}
};

class remove_selected_units_from_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept final {
		if(state.selected_army_group) {
			for(auto item : state.selected_armies) {
				state.remove_army_army_group_clean(state.selected_army_group, item);
			}
			for(auto item : state.selected_navies) {
				state.remove_navy_from_army_group(state.selected_army_group, item);
			}
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group) {
			disabled = false;
		} else {
			disabled = true;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "alice_battleplanner_travel_origin_order");
	}
};

class new_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		auto selected = state.map_state.selected_province;
		if(selected) {
			state.new_army_group(selected);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.map_state.selected_province) {
			disabled = false;
			return;
		}
		disabled = true;
	}
};

class delete_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		auto selected = state.selected_army_group;
		if(selected) {
			state.delete_army_group(selected);
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.selected_army_group) {
			disabled = false;
			return;
		}
		disabled = true;
	}
};

class army_group_location : public simple_text_element_base{
public:
	void on_update(sys::state & state) noexcept override {
		auto content = retrieve<dcon::automated_army_group_id>(state, parent);
		auto hq = state.world.automated_army_group_get_hq(content);
		set_text(state, text::get_name_as_string(state, dcon::fatten(state.world, hq)));
	}
};

class select_army_group_button : public button_element_base {
	void button_action(sys::state& state) noexcept override {
		auto info = retrieve<dcon::automated_army_group_id>(state, parent);
		state.select_army_group(info);
	}

	void on_update(sys::state& state) noexcept override {
		auto info = retrieve<dcon::automated_army_group_id>(state, parent);


		if(state.selected_army_group) {
			if(info) {
				auto local_hq = state.world.automated_army_group_get_hq(info);
				auto selected_hq = state.world.automated_army_group_get_hq(state.selected_army_group);
				if(local_hq == selected_hq) {
					frame = 1;
					return;
				}
			}
		}
		frame = 0;
	}
};

class army_group_entry : public listbox_row_element_base<dcon::automated_army_group_id> {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_select_army_group_button") {
			return make_element_by_type<select_army_group_button>(state, id);
		} else if(name == "alice_army_group_location") {
			return make_element_by_type<army_group_location>(state, id);
		} else {
			return nullptr;
		}
	}
};

class army_groups_list : public listbox_element_base<army_group_entry, dcon::automated_army_group_id> {
public:
	std::string_view get_row_element_name() override {
		return "alice_army_group_entry";
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_automated_army_group([&](dcon::automated_army_group_id item) {
			row_contents.push_back(item);
		});			
		update(state);
	}
};

class army_groups_list_wrapper : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_army_group_listbox") {
			return make_element_by_type<army_groups_list>(state, id);
		} else {
			return nullptr;
		}
	}
};

class battleplanner_control : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_armygroup_new_button") {
			return make_element_by_type<new_army_group_button>(state, id);
		} else if(name == "alice_armygroup_delete_button") {
			return make_element_by_type<delete_army_group_button>(state, id);
		} else if(name == "alice_battleplanner_remove_selected") {
			return make_element_by_type<remove_selected_units_from_army_group_button>(state, id);
		} else if(name == "alice_army_group_listbox_wrapper") {
			return make_element_by_type<army_groups_list_wrapper>(state, id);
		} else {
			return nullptr;
		}
	}
};

class battleplanner_selection_control : public window_element_base {
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_battleplanner_add_selected") {
			return make_element_by_type<add_selected_units_to_army_group_button>(state, id);
		} if(name == "alice_battleplanner_remove_selected") {
			return make_element_by_type<remove_selected_units_from_army_group_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class army_group_control_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_armygroup_defend_button") {
			return make_element_by_type<toggle_defend_order_button>(state, id);
		} else if(name == "alice_armygroup_enforce_control_button") {
			return make_element_by_type<toggle_enforce_control_order_button>(state, id);
		} else if(name == "alice_armygroup_naval_travel_origin_button") {
			return make_element_by_type<toggle_ferry_origin_order_button>(state, id);
		} else if(name == "alice_armygroup_go_to_selection") {
			return make_element_by_type<go_to_battleplanner_selection_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class army_group_details_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "alice_army_group_regiments_list_header") {
			return make_element_by_type<army_group_details_window_header>(state, id);
		}
		if(name == "alice_army_group_control") {
			return make_element_by_type<army_group_control_window>(state, id);
		}
		if(name == "alice_army_group_unit_listbox_wrapper") {
			return make_element_by_type<army_group_details_window_body>(state, id);
		}
		return nullptr;
	}
};

} // namespace ui
