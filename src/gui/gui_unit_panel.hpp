#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_unit_panel_subwindow.hpp"
#include "text.hpp"
#include "prng.hpp"
#include "gui_leader_tooltip.hpp"
#include "gui_leader_select.hpp"

namespace ui {

enum class unitpanel_action : uint8_t { close, reorg, split, disband, changeleader, temp };

class unit_selection_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<unitpanel_action>{unitpanel_action{unitpanel_action::close}};
			parent->impl_get(state, payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("deselect_unit"));
		text::close_layout_box(contents, box);
	}
};

class unit_selection_new_unit_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = element_selection_wrapper<unitpanel_action>{unitpanel_action::reorg};
		parent->impl_get(state, payload);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("new_unit"));
		text::close_layout_box(contents, box);
	}
};

template<class T>
class unit_selection_split_in_half_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			command::split_army(state, state.local_player_nation, content);
		} else {
			command::split_navy(state, state.local_player_nation, content);
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			disabled = !command::can_split_army(state, state.local_player_nation, content);
		} else {
			disabled = !command::can_split_navy(state, state.local_player_nation, content);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("splitinhalf"));
		text::close_layout_box(contents, box);
	}
};

template<class T>
class unit_selection_disband_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				command::delete_army(state, state.local_player_nation, content);
			} else {
				command::delete_navy(state, state.local_player_nation, content);
			}
			Cyto::Any payload2 = element_selection_wrapper<unitpanel_action>{unitpanel_action{unitpanel_action::close}};
			parent->impl_get(state, payload2);
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
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("disband_unit"));
		text::close_layout_box(contents, box);
	}
};

class unit_selection_disband_too_small_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("disband_too_small_unit"));
		text::close_layout_box(contents, box);
	}
};

class unit_selection_unit_name_text : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("uw_unitnames_iro"));
		text::close_layout_box(contents, box);
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
		if(lid)
			display_leader_full(state, lid, contents, 0);
	}
	

	void button_action(sys::state& state) noexcept override {
		auto unit = retrieve<T>(state, parent);
		auto location = get_absolute_location(*this);
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
		if(lid)
			display_leader_full(state, lid, contents, 0);
	}


	void button_action(sys::state& state) noexcept override {
		auto unit = retrieve<T>(state, parent);
		auto location = get_absolute_location(*this);
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			open_leader_selection(state, unit, dcon::navy_id{}, location.x + base_data.size.x, location.y);
		} else {
			open_leader_selection(state, dcon::army_id{}, unit, location.x + base_data.size.x, location.y);
		}
	}
};

class unit_selection_unit_location_text : public simple_text_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("uw_loc_iro"));
		text::close_layout_box(contents, box);
	}
};

template<class T>
class unit_selection_str_bar : public vertical_progress_bar {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);

			float total_strenght = 0.0f;
			std::uint16_t unit_count = 0u;
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id nmid) {
					auto regiment = dcon::fatten(state.world, state.world.army_membership_get_regiment(nmid));
					total_strenght += regiment.get_strength();
					++unit_count;
				});
			} else {
				state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
					auto ship = dcon::fatten(state.world, state.world.navy_membership_get_ship(nmid));
					total_strenght += ship.get_strength();
					++unit_count;
				});
			}
			total_strenght /= static_cast<float>(unit_count);

			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("curr_comb_str"));
			text::add_to_layout_box(state, contents, box, text::fp_percentage{total_strenght}, text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
	}
};

template<class T>
class unit_selection_panel : public window_element_base {
private:
	simple_text_element_base* unitlocation_text = nullptr;
	simple_text_element_base* unitname_text = nullptr;
	simple_text_element_base* unitleader_text = nullptr;
	simple_text_element_base* totalunitstrength_text = nullptr;

public:
	window_element_base* reorg_window = nullptr;
	window_element_base* combat_window = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		{
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto win2 = make_element_by_type<unit_reorg_window<T, dcon::regiment_id>>(state, state.ui_state.defs_by_name.find("reorg_window")->second.definition);
				win2->base_data.position.y = base_data.position.y - 29;
				win2->set_visible(state, false);
				reorg_window = win2.get();
				add_child_to_front(std::move(win2));
			} else {
				auto win2 = make_element_by_type<unit_reorg_window<T, dcon::ship_id>>(state, state.ui_state.defs_by_name.find("reorg_window")->second.definition);
				win2->base_data.position.y = base_data.position.y - 29;
				win2->set_visible(state, false);
				reorg_window = win2.get();
				add_child_to_front(std::move(win2));
			}
		}
		window_element_base::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unitpanel_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leader_prestige_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leader_prestige_bar") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "prestige_bar_frame") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "unitname") {
			auto ptr = make_element_by_type<unit_selection_unit_name_text>(state, id);
			ptr->base_data.position.x += 9;
			ptr->base_data.position.y += 4;
			unitname_text = ptr.get();
			return ptr;
		} else if(name == "only_unit_from_selection_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "remove_unit_from_selection_button") {
			return make_element_by_type<unit_selection_close_button>(state, id);
		} else if(name == "newunitbutton") {
			return make_element_by_type<unit_selection_new_unit_button>(state, id);
		} else if(name == "splitinhalf") {
			return make_element_by_type<unit_selection_split_in_half_button<T>>(state, id);
		} else if(name == "disbandbutton") {
			return make_element_by_type<unit_selection_disband_button<T>>(state, id);
		} else if(name == "disbandtoosmallbutton") {
			return make_element_by_type<unit_selection_disband_too_small_button>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<unit_selection_str_bar<T>>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<vertical_progress_bar>(state, id);
		} else if(name == "unitattrition_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "unitattrition") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "unitstrength") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			totalunitstrength_text = ptr.get();
			return ptr;
		} else if(name == "unitlocation") {
			auto ptr = make_element_by_type<unit_selection_unit_location_text>(state, id);
			unitlocation_text = ptr.get();
			return ptr;
		} else if(name == "unit_location_button") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "unitleader") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			unitleader_text = ptr.get();
			return ptr;
		} else if(name == "leader_button") {
			return make_element_by_type<unit_selection_change_leader_button<T>>(state, id);
		} else if(name == "unit_activity") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "leader_photo") {
			return make_element_by_type<unit_selection_leader_image<T>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto content = any_cast<T>(payload);
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto fat = dcon::fatten(state.world, content);
				unitleader_text->set_text(state, std::string(state.to_string_view(fat.get_general_from_army_leadership().get_name())));
				unitlocation_text->set_text(state,
						text::produce_simple_string(state, dcon::fatten(state.world, content).get_location_from_army_location().get_name()));
				uint16_t total = 0;
				for(auto n : fat.get_army_membership()) {
					total++;
				}
				totalunitstrength_text->set_text(state, text::format_wholenum(total * 1000));
			} else {
				auto fat = dcon::fatten(state.world, content);
				unitleader_text->set_text(state, std::string(state.to_string_view(fat.get_admiral_from_navy_leadership().get_name())));
				unitlocation_text->set_text(state,
						text::produce_simple_string(state, dcon::fatten(state.world, content).get_location_from_navy_location().get_name()));
				uint16_t total = 0;
				for(auto n : fat.get_navy_membership()) {
					total++;
				}
				totalunitstrength_text->set_text(state, text::format_wholenum(total * 1000));
			}
			unitname_text->set_text(state, std::string(state.to_string_view(dcon::fatten(state.world, content).get_name())));
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
			} default: {
				break;
			}
			}
			return message_result::consumed;
		}
		return message_result::unseen;
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
class subunit_details_name : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			std::string str{state.to_string_view(dcon::fatten(state.world, content).get_name())};
			set_text(state, str);
		}
	}
};

template<class T>
class subunit_details_type_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			dcon::unit_type_id utid = dcon::fatten(state.world, content).get_type();
			if(utid)
				set_text(state, text::produce_simple_string(state, state.military_definitions.unit_base_definitions[utid].name));
			else
				set_text(state, "");
		}
	}
};

template<class T>
class subunit_details_type_icon : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			T content = any_cast<T>(payload);
			dcon::unit_type_id utid = dcon::fatten(state.world, content).get_type();
			if(utid)
				frame = state.military_definitions.unit_base_definitions[utid].icon - 1;
		}
	}
};

template<class T>
class subunit_organisation_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<T>(payload));
			progress = fat.get_org();
		}
	}
};

template<class T>
class subunit_strength_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = T{};
			parent->impl_get(state, payload);
			auto fat = dcon::fatten(state.world, any_cast<T>(payload));
			progress = fat.get_strength();
		}
	}
};

class subunit_details_regiment_amount : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::regiment_id{};
			parent->impl_get(state, payload);
			dcon::regiment_id content = any_cast<dcon::regiment_id>(payload);
			set_text(state, text::format_wholenum(int32_t(state.world.regiment_get_strength(content) * state.defines.pop_size_per_regiment)));
		}
	}
};
class subunit_details_ship_amount : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::ship_id{};
			parent->impl_get(state, payload);
			dcon::ship_id content = any_cast<dcon::ship_id>(payload);
			set_text(state, text::format_percentage(state.world.ship_get_strength(content), 0));
		}
	}
};

class subunit_details_entry_regiment : public listbox_row_element_base<dcon::regiment_id> {
private:
	button_element_base* connectedpop_icon = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "select_naval") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;	
		} else if(name == "sunit_icon") {
			return make_element_by_type<subunit_details_type_icon<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_name") {
			return make_element_by_type<subunit_details_name<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_type") {
			return make_element_by_type<subunit_details_type_text<dcon::regiment_id>>(state, id);
		} else if(name == "subunit_amount") {
			return make_element_by_type<subunit_details_regiment_amount>(state, id);
		} else if(name == "subunit_amount_naval") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "connected_pop") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			connectedpop_icon = ptr.get();
			return ptr;
		} else if(name == "rebel_faction") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "unit_experience") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<subunit_organisation_progress_bar<dcon::regiment_id>>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<subunit_strength_progress_bar<dcon::regiment_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override { 
		auto fat = dcon::fatten(state.world, content);
		connectedpop_icon->set_visible(state, true);
		connectedpop_icon->frame = fat.get_pop_from_regiment_source().get_poptype().get_sprite() - 1;
	}
};

class subunit_details_entry_ship : public listbox_row_element_base<dcon::ship_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "select") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;	
		} else if(name == "select_naval") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "sunit_icon") {
			return make_element_by_type<subunit_details_type_icon<dcon::ship_id>>(state, id);
		} else if(name == "subunit_name") {
			return make_element_by_type<subunit_details_name<dcon::ship_id>>(state, id);
		} else if(name == "subunit_type") {
			return make_element_by_type<subunit_details_type_text<dcon::ship_id>>(state, id);
		} else if(name == "subunit_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "subunit_amount_naval") {
			return make_element_by_type<subunit_details_ship_amount>(state, id);
		} else if(name == "connected_pop") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "rebel_faction") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "unit_experience") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<subunit_organisation_progress_bar<dcon::ship_id>>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<subunit_strength_progress_bar<dcon::ship_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override { 
	}
};

class unit_details_army_listbox : public listbox_element_base<subunit_details_entry_regiment, dcon::regiment_id> {
protected:
	std::string_view get_row_element_name() override {
		return "subunit_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::army_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::army_id>(payload);
			state.world.army_for_each_army_membership_as_army(content, [&](dcon::army_membership_id amid) {
				auto rid = state.world.army_membership_get_regiment(amid);
				row_contents.push_back(rid);
			});
		}
		update(state);
	}
};
class unit_details_navy_listbox : public listbox_element_base<subunit_details_entry_ship, dcon::ship_id> {
protected:
	std::string_view get_row_element_name() override {
		return "subunit_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = dcon::navy_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::navy_id>(payload);
			state.world.navy_for_each_navy_membership_as_navy(content, [&](dcon::navy_membership_id nmid) {
				auto sid = state.world.navy_membership_get_ship(nmid);
				row_contents.push_back(sid);
			});
		}
		update(state);
	}
};

class unit_details_load_army_button : public button_element_base {
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

		visible = !bool(tprted);

		disabled = !military::can_embark_onto_sea_tile(state, state.local_player_nation, loc, n);
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
		else
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
		else
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

template<class T>
class unit_details_buttons : public window_element_base {
private:
	simple_text_element_base* navytransport_text = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "load_button" && std::is_same_v<T, dcon::army_id>) {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_load_army_button>(state, id);
			} else {
				auto ptr = make_element_by_type<element_base>(state, id);
				ptr->set_visible(state, false);
				return ptr;
			}
		} else if(name == "unload_button") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				return make_element_by_type<unit_details_unload_army_button>(state, id);
			} else {
				return make_element_by_type<unit_details_unload_navy_button>(state, id);
			}
		} else if(name == "enable_rebel_button"
			|| name == "disable_rebel_button"
			|| name == "attach_unit_button"
			|| name == "detach_unit_button"
			|| name == "select_land") {

			auto ptr = make_element_by_type<element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;

	

		} else if(name == "header") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<element_base>(state, id);
				ptr->set_visible(state, false);
				return ptr;
			} else {
				return make_element_by_type< navy_transport_text>(state, id);
			}
		} else {
			return nullptr;
		}
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
	image_element_base* unitdugin_icon = nullptr;
	unit_selection_panel<T>* unit_selection_win = nullptr;

	

public:
	T unit_id;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.y = 250;

		xy_pair base_position = {20,
				0}; // state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item_start")->second.definition].position;
		xy_pair base_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item_offset")->second.definition].position;

		{
			auto win = make_element_by_type<unit_details_type_item<T, 0>>(state,
					state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (0 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (0 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_details_type_item<T, 1>>(state,
					state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (1 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (1 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}
		{
			auto win = make_element_by_type<unit_details_type_item<T, 2>>(state,
					state.ui_state.defs_by_name.find("unittype_item")->second.definition);
			win->base_data.position.x = base_position.x + (2 * base_offset.x); // Flexnudge
			win->base_data.position.y = base_position.y + (2 * base_offset.y); // Flexnudge
			add_child_to_front(std::move(win));
		}

		const xy_pair item_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("unittype_item")->second.definition].position;
		if constexpr(std::is_same_v<T, dcon::army_id>) {
			auto ptr = make_element_by_type<unit_details_army_listbox>(state,
					state.ui_state.defs_by_name.find("sup_subunits")->second.definition);
			ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
			ptr->base_data.size.y += 32;
			add_child_to_front(std::move(ptr));
		} else {
			auto ptr = make_element_by_type<unit_details_navy_listbox>(state,
					state.ui_state.defs_by_name.find("sup_subunits")->second.definition);
			ptr->base_data.position.y = base_position.y + item_offset.y + (3 * base_offset.y) + 72 - 32;
			ptr->base_data.size.y += 32;
			add_child_to_front(std::move(ptr));
		}

		{
			auto ptr = make_element_by_type<unit_details_buttons<T>>(state,
					state.ui_state.defs_by_name.find("sup_buttons_window")->second.definition);
			ptr->base_data.position.y = base_data.size.y; // Nudge
			add_child_to_front(std::move(ptr));
		}

		{
			auto ptr =
					make_element_by_type<unit_selection_panel<T>>(state, state.ui_state.defs_by_name.find("unitpanel")->second.definition);
			unit_selection_win = ptr.get();
			ptr->base_data.position.y = -80;
			add_child_to_front(std::move(ptr));
		}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "unit_bottom_bg") {
			return make_element_by_type<draggable_target>(state, id);
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
			auto ptr = make_element_by_type<progress_bar>(state, id);
			unitsupply_bar = ptr.get();
			return ptr;
		} else if(name == "unitstatus_dugin") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
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
			unitrecon_icon->set_visible(state, true);
			unitrecon_text->set_visible(state, true);

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
			case unitpanel_action::close: {
				Cyto::Any cpayload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::close}};
				unit_selection_win->reorg_window->impl_get(state, cpayload);
				state.selected_armies.clear();
				state.selected_navies.clear();
				set_visible(state, false);
				state.game_state_updated.store(true, std::memory_order_release);
				break;
			} default: {
				break;
			}
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
		for(auto a : state.selected_armies) {
			command::delete_army(state, state.local_player_nation, a);
		}
		for(auto a : state.selected_navies) {
			command::delete_navy(state, state.local_player_nation, a);
		}
		
	}
};

class whole_panel_button : public shift_button_element_base {
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
		if(std::holds_alternative<dcon::army_id>(foru)) {
			command::delete_army(state, state.local_player_nation, std::get<dcon::army_id>(foru));
		} else if(std::holds_alternative<dcon::navy_id>(foru)) {
			command::delete_navy(state, state.local_player_nation, std::get<dcon::navy_id>(foru));
		}
	}
};

class u_row_split : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		// TODO
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
		if(lid)
			display_leader_full(state, lid, contents, 0);
	}


	void button_action(sys::state& state) noexcept override {
		auto foru = retrieve<unit_var>(state, parent);
		auto location = get_absolute_location(*this);
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

class mulit_unit_selection_panel : public main_window_element_base {
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

} // namespace ui
