#include <vector>
#include "gui_unit_panel_subwindow.hpp"
#include "gui_element_types.hpp"
#include "gui_listbox_templates.hpp"
#include "gui_templates.hpp"

namespace ui {

//======================================================================================================================================
//	REORGANISATION WINDOW
//======================================================================================================================================

template<class T>
class subunit_organisation_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto fat = dcon::fatten(state.world, retrieve<T>(state, parent));
		progress = fat.get_org();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("curr_comb_org"));
			auto fat = dcon::fatten(state.world, retrieve<T>(state, parent));
			auto color = fat.get_org() >= 0.9f ? text::text_color::green
				: fat.get_org() < 0.5f ? text::text_color::red
				: text::text_color::yellow;
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ fat.get_org() }, color);
			text::close_layout_box(contents, box);
		}
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::land_organisation, true);
		} else {
			ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::naval_organisation, true);
		}
	}
};

template<class T>
class subunit_strength_progress_bar : public vertical_progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		auto fat = dcon::fatten(state.world, retrieve<T>(state, parent));
		progress = fat.get_strength();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, std::string_view("curr_comb_str"));
			auto fat = dcon::fatten(state.world, retrieve<T>(state, parent));
			auto color = fat.get_strength() >= 0.9f ? text::text_color::green
				: fat.get_strength() < 0.5f ? text::text_color::red
				: text::text_color::yellow;
			text::add_to_layout_box(state, contents, box, text::fp_percentage{ fat.get_strength() }, color);
			text::close_layout_box(contents, box);
		}

		auto fat_id = dcon::fatten(state.world, retrieve<T>(state, parent));
		float total_cost = 0.f;

		// Reinforcement cost as % of construction cost
		// Shows how many goods will it take to fully reinforce the unit
		auto& build_cost = state.military_definitions.unit_base_definitions[fat_id.get_type()].build_cost;
		auto hadheader = false;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(build_cost.commodity_type[i]) {
				float cost = state.world.commodity_get_cost(build_cost.commodity_type[i]);
				float curstr = military::unit_get_strength(state, fat_id);
				if(curstr >= 1.0f) {
					continue;
				}
				auto box = text::open_layout_box(contents, 0);

				if(!hadheader) {
					hadheader = true;

					text::localised_format_box(state, contents, box, "unit_reinforcement_needs");
					text::close_layout_box(contents, box);
					box = text::open_layout_box(contents, 0);
				}

				float amount = build_cost.commodity_amounts[i] * (1.0f - curstr);
				text::substitution_map m;
				text::add_to_substitution_map(m, text::variable_type::name, state.world.commodity_get_name(build_cost.commodity_type[i]));
				text::add_to_substitution_map(m, text::variable_type::val, text::fp_currency{ cost });
				text::add_to_substitution_map(m, text::variable_type::need, text::fp_four_places{ amount });
				text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ cost * amount });

				auto cid = build_cost.commodity_type[i];
				std::string padding = cid.index() < 10 ? "0" : "";
				std::string description = "@$" + padding + std::to_string(cid.index());
				text::add_unparsed_text_to_layout_box(state, contents, box, description);

				text::localised_format_box(state, contents, box, "alice_spending_commodity", m);
				text::close_layout_box(contents, box);
				total_cost += cost * amount;
			} else {
				break;
			}
		}
		// Everyday supply costs
		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "unit_supply_needs");
			text::close_layout_box(contents, box);
		}
		auto o_sc_mod = std::max(0.01f, state.world.nation_get_modifier_values(state.local_player_nation, sys::national_mod_offsets::supply_consumption) + 1.0f);
		auto& supply_cost = state.military_definitions.unit_base_definitions[fat_id.get_type()].supply_cost;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			if(supply_cost.commodity_type[i]) {
				float cost = state.world.commodity_get_cost(supply_cost.commodity_type[i]);
				float curstr = military::unit_get_strength(state, fat_id);
				float amount = supply_cost.commodity_amounts[i] * state.world.nation_get_unit_stats(state.local_player_nation, fat_id.get_type()).supply_consumption * o_sc_mod * curstr;
				text::substitution_map m;
				text::add_to_substitution_map(m, text::variable_type::name, state.world.commodity_get_name(supply_cost.commodity_type[i]));
				text::add_to_substitution_map(m, text::variable_type::val, text::fp_currency{ cost });
				text::add_to_substitution_map(m, text::variable_type::need, text::fp_four_places{ amount });
				text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ cost * amount });
				auto box = text::open_layout_box(contents, 0);

				auto cid = supply_cost.commodity_type[i];
				std::string padding = cid.index() < 10 ? "0" : "";
				std::string description = "@$" + padding + std::to_string(cid.index());
				text::add_unparsed_text_to_layout_box(state, contents, box, description);

				text::localised_format_box(state, contents, box, "alice_spending_commodity", m);
				text::close_layout_box(contents, box);
				total_cost += cost * amount;
			} else {
				break;
			}
		}
		text::substitution_map m;
		text::add_to_substitution_map(m, text::variable_type::cost, text::fp_currency{ total_cost });
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, "alice_spending_unit_1", m);
		text::close_layout_box(contents, box);
	}
};

class regiment_pop_icon : public tinted_image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto reg_id = retrieve<dcon::regiment_id>(state, parent);
		auto base_pop = state.world.regiment_get_pop_from_regiment_source(reg_id);
		if(!base_pop) {
			frame = state.world.pop_type_get_sprite(state.culture_definitions.soldiers) - 1;
			color = sys::pack_color(255, 75, 75);
		} else {
			frame = state.world.pop_type_get_sprite(state.world.pop_get_poptype(base_pop)) - 1;
			if(state.world.pop_get_size(base_pop) < state.defines.pop_min_size_for_regiment) {
				color = sys::pack_color(220, 75, 75);
			} else if(state.world.pop_get_size(base_pop) < state.defines.pop_size_per_regiment) {
				color = sys::pack_color(200, 200, 0);
			} else {
				color = sys::pack_color(255, 255, 255);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto reg_id = retrieve<dcon::regiment_id>(state, parent);
		auto base_pop = state.world.regiment_get_pop_from_regiment_source(reg_id);

		if(!base_pop) {
			text::add_line(state, contents, "reinforce_rate_none");
		} else {
			// Added culture name to the tooltip
			text::add_line(state, contents, "x_from_y", text::variable_type::x, state.world.pop_get_poptype(base_pop).get_name(), text::variable_type::y, state.world.pop_get_province_from_pop_location(base_pop), text::variable_type::culture, state.world.pop_get_culture(base_pop).get_name());
			text::add_line_break_to_layout(state, contents);

			auto reg_range = state.world.pop_get_regiment_source(base_pop);
			text::add_line(state, contents, "pop_size_unitview",
				text::variable_type::val, text::pretty_integer{ int64_t(state.world.pop_get_size(base_pop)) },
				text::variable_type::allowed, military::regiments_possible_from_pop(state, base_pop),
				text::variable_type::current, int64_t(reg_range.end() - reg_range.begin())
			);

			//auto a = state.world.regiment_get_army_from_army_membership(reg_id);
			auto reinf = state.defines.pop_size_per_regiment * military::unit_calculate_reinforcement<military::reinforcement_estimation_type::monthly>(state, reg_id, true);
			if(reinf >= 2.0f) {
				text::add_line(state, contents, "reinforce_rate", text::variable_type::x, int64_t(reinf));
			} else {
				text::add_line(state, contents, "reinforce_rate_none");
			}
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
class reorg_unit_transfer_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if(content)
			send(state, parent, element_selection_wrapper<T>{content});
		
	}
	void button_shift_action(sys::state& state) noexcept override {
		auto content = retrieve<T>(state, parent);
		if constexpr(std::is_same_v<T, dcon::regiment_id>) {
			auto army = state.world.regiment_get_army_from_army_membership(content);
			auto type = state.world.regiment_get_type(content);
			for(auto r : state.world.army_get_army_membership(army)) {
				if(r.get_regiment().get_type() == type) {
					send(state, parent, element_selection_wrapper<T>{r.get_regiment().id});
				}
			}
		} else {
			auto n = state.world.ship_get_navy_from_navy_membership(content);
			auto type = state.world.ship_get_type(content);
			for(auto r : state.world.navy_get_navy_membership(n)) {
				if(r.get_ship().get_type() == type) {
					send(state, parent, element_selection_wrapper<T>{r.get_ship().id});
				}
			}
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

class subunit_details_regiment_amount : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::regiment_id{};
			parent->impl_get(state, payload);
			dcon::regiment_id content = any_cast<dcon::regiment_id>(payload);
			if(content)
				set_text(state, text::format_wholenum(int32_t(state.world.regiment_get_strength(content) * state.defines.pop_size_per_regiment)));
			else
				set_text(state, "");
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
			if(content)
				set_text(state, text::format_percentage(state.world.ship_get_strength(content), 0));
			else
				set_text(state, "");
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
			std::string str{ state.to_string_view(dcon::fatten(state.world, content).get_name()) };
			set_text(state, str);
		}
	}
};

template<class T>
class reorg_unit_listbox_row : public listbox_row_element_base<T> {
using listrow = listbox_row_element_base<T>;

public:
	void on_create(sys::state& state) noexcept override {
		listrow::base_data.position.y += 128;	// Nudge
		listrow::base_data.position.x += 256;
		listrow::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "band_reorg_regiment") {
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);

		} else if(name == "band_reorg_naval") {
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);

		} else if(name == "unit_icon") {
			return make_element_by_type<subunit_details_type_icon<T>>(state, id);
		} else if(name == "subunit_name") {
			return make_element_by_type<subunit_details_name<T>>(state, id);
		} else if(name == "subunit_type") {
			return make_element_by_type<subunit_details_type_text<T>>(state, id);
		} else if(name == "subunit_strength") {
			return make_element_by_type<subunit_details_regiment_amount>(state, id);
		} else if(name == "subunit_strength_naval") {
			return make_element_by_type<subunit_details_ship_amount>(state, id);
		} else if(name == "connected_pop") {
			if constexpr(std::is_same_v<T, dcon::regiment_id>) {
				return make_element_by_type<regiment_pop_icon>(state, id);
			} else {
				return make_element_by_type<invisible_element>(state, id);
			}
		} else if(name == "rebel_faction") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "org_bar") {
			return make_element_by_type<subunit_organisation_progress_bar<T>>(state, id);
		} else if(name == "str_bar") {
			return make_element_by_type<subunit_strength_progress_bar<T>>(state, id);
		} else if(name == "transferbutton") {
			return make_element_by_type<reorg_unit_transfer_button<T>>(state, id);
		} else {
			return nullptr;
		}
	}
};

template<class T, class T2>
class reorg_unit_listbox_left : public listbox_element_base<reorg_unit_listbox_row<T2>, T2> {
using listbox_left = listbox_element_base<reorg_unit_listbox_row<T2>, T2>;
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(listbox_left::parent) {
			auto content = retrieve<T>(state, listbox_left::parent);

			Cyto::Any vpayload = std::vector<T2>{};
			listbox_left::parent->impl_get(state, vpayload);
			std::vector<T2> selectedunits = any_cast<std::vector<T2>>(vpayload);

			auto fat = dcon::fatten(state.world, content);

			listbox_left::row_contents.clear();

			if constexpr(std::is_same_v<T, dcon::army_id>) {
				for(auto regi : fat.get_army_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_regiment().id); result == std::end(selectedunits)) {
						listbox_left::row_contents.push_back(regi.get_regiment().id);
					}
				}
				std::sort(listbox_left::row_contents.begin(), listbox_left::row_contents.end(), [&](dcon::regiment_id a, dcon::regiment_id b) {
					auto av = state.world.regiment_get_type(a).index();
					auto bv = state.world.regiment_get_type(b).index();
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
			} else {
				for(auto regi : fat.get_navy_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_ship().id); result == std::end(selectedunits)) {
						listbox_left::row_contents.push_back(regi.get_ship().id);
					}
				}
				std::sort(listbox_left::row_contents.begin(), listbox_left::row_contents.end(), [&](dcon::ship_id a, dcon::ship_id b) {
					auto av = state.world.ship_get_type(a).index();
					auto bv = state.world.ship_get_type(b).index();
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
			}

			listbox_left::update(state);
		}
	}
};

template<class T, class T2>
class reorg_unit_listbox_right : public listbox_element_base<reorg_unit_listbox_row<T2>, T2> {
using listbox_right = listbox_element_base<reorg_unit_listbox_row<T2>, T2>;
protected:
	std::string_view get_row_element_name() override {
		return "reorg_entry_right";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(listbox_right::parent) {
			auto content = retrieve<T>(state, listbox_right::parent);

			Cyto::Any vpayload = std::vector<T2>{};
			listbox_right::parent->impl_get(state, vpayload);
			std::vector<T2> selectedunits = any_cast<std::vector<T2>>(vpayload);

			auto fat = dcon::fatten(state.world, content);

			listbox_right::row_contents.clear();

			if constexpr(std::is_same_v<T, dcon::army_id>) {
				for(auto regi : fat.get_army_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_regiment().id); result != std::end(selectedunits)) {
						listbox_right::row_contents.push_back(regi.get_regiment().id);
					}
				}
				std::sort(listbox_right::row_contents.begin(), listbox_right::row_contents.end(), [&](dcon::regiment_id a, dcon::regiment_id b) {
					auto av = state.world.regiment_get_type(a).index();
					auto bv = state.world.regiment_get_type(b).index();
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
			} else {
				for(auto regi : fat.get_navy_membership()) {
					if(auto result = std::find(begin(selectedunits), end(selectedunits), regi.get_ship().id); result != std::end(selectedunits)) {
						listbox_right::row_contents.push_back(regi.get_ship().id);
					}
				}
				std::sort(listbox_right::row_contents.begin(), listbox_right::row_contents.end(), [&](dcon::ship_id a, dcon::ship_id b) {
					auto av = state.world.ship_get_type(a).index();
					auto bv = state.world.ship_get_type(b).index();
					if(av != bv)
						return av > bv;
					else
						return a.index() < b.index();
				});
			}

			listbox_right::update(state);
		}
	}
};


class unit_reorg_balance_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::balance}};
			parent->impl_get(state, payload);
		}
	}
};

class reorg_win_close_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<reorg_win_action>{reorg_win_action{reorg_win_action::close}};
			parent->impl_get(state, payload);
		}
	}
};

template<class T, class T2>
class unit_reorg_window : public window_element_base {
private:
	std::vector<T2> selectedsubunits;
	T unitToReorg{};
	simple_text_element_base* orginialunit_text = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "reorg_bg") {
			return make_element_by_type<opaque_element_base>(state, id);

		} else if(name == "reorg_label") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "unitname_1") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			orginialunit_text = ptr.get();
			return ptr;

		} else if(name == "unitname_2") {
			return make_element_by_type<simple_text_element_base>(state, id);

		} else if(name == "cell_window_left" || name == "cell_window_right") {
			return make_element_by_type<invisible_element>(state, id);

		} else if(name == "balancebutton") {
			return make_element_by_type<unit_reorg_balance_button>(state, id);

		} else if(name == "closebutton") {
			return make_element_by_type<reorg_win_close_button>(state, id);

		} else if(name == "left_list") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<reorg_unit_listbox_left<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			} else {
				auto ptr = make_element_by_type<reorg_unit_listbox_left<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			}

		} else if(name == "right_list") {
			if constexpr(std::is_same_v<T, dcon::army_id>) {
				auto ptr = make_element_by_type<reorg_unit_listbox_right<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			} else {
				auto ptr = make_element_by_type<reorg_unit_listbox_right<T, T2>>(state, id);
				//ptr->base_data.position.x += 255;	// Nudge
				//ptr->list_scrollbar->base_data.position.x += 256;
				ptr->base_data.size.x += 256;
				return ptr;
			}

		} else if(name == "external_scroll_slider_left" || name == "external_scroll_slider_right") {
			return make_element_by_type<invisible_element>(state, id);

		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		unitToReorg = retrieve<T>(state, parent);
		auto fat = dcon::fatten(state.world, unitToReorg);
		orginialunit_text->set_text(state, std::string(state.to_string_view(fat.get_name())));
	}

	void on_visible(sys::state& state) noexcept override {
		selectedsubunits.clear();
	}
	void on_hide(sys::state& state) noexcept override {
		selectedsubunits.clear();
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<element_selection_wrapper<T>>()) {
			unitToReorg = any_cast<element_selection_wrapper<T>>(payload).data;
			return message_result::consumed;
		} else if(payload.holds_type<T>()) {
			payload.emplace<T>(unitToReorg);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<T2>>()) {
			auto content = any_cast<element_selection_wrapper<T2>>(payload).data;
			if(!selectedsubunits.empty()) {
				if(auto result = std::find(selectedsubunits.begin(), selectedsubunits.end(), content); result != selectedsubunits.end()) {
					selectedsubunits.erase(result);
				} else {
					selectedsubunits.push_back(content);
				}
			} else {
				selectedsubunits.push_back(content);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<std::vector<T2>>()) {
			payload.emplace<std::vector<T2>>(selectedsubunits);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<reorg_win_action>>()) {
			auto content = any_cast<element_selection_wrapper<reorg_win_action>>(payload).data;
			switch(content) {
				case reorg_win_action::close:
					if(selectedsubunits.size() <= command::num_packed_units && !selectedsubunits.empty()) {
						if constexpr(std::is_same_v<T2, dcon::regiment_id>) {
							std::array<dcon::regiment_id, command::num_packed_units> tosplit{};
							for(size_t i = 0; i < selectedsubunits.size(); i++) {
								tosplit[i] = selectedsubunits[i];
							}
							command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
							command::split_army(state, state.local_player_nation, unitToReorg);
						} else {
							std::array<dcon::ship_id, command::num_packed_units> tosplit{};
							for(size_t i = 0; i < selectedsubunits.size(); i++) {
								tosplit[i] = selectedsubunits[i];
							}
							command::mark_ships_to_split( state, state.local_player_nation, tosplit);
							command::split_navy(state, state.local_player_nation, unitToReorg);
						}
					} else if(selectedsubunits.size() > command::num_packed_units){
						if constexpr(std::is_same_v<T2, dcon::regiment_id>) {
							std::array<dcon::regiment_id, command::num_packed_units> tosplit{};
							//while(selectedsubunits.size() > command::num_packed_units) {
							while(selectedsubunits.size() > 0) {
								tosplit.fill(dcon::regiment_id{});
								for(size_t i = 0; i < command::num_packed_units && i < selectedsubunits.size(); i++) {
									tosplit[i] = selectedsubunits[i];
								}
								command::mark_regiments_to_split(state, state.local_player_nation, tosplit);
								(selectedsubunits.size() > command::num_packed_units) ? selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.begin() + command::num_packed_units)
																						: selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
							}
							command::split_army(state, state.local_player_nation, unitToReorg);
						} else {
							std::array<dcon::ship_id, command::num_packed_units> tosplit{};
							//while(selectedsubunits.size() > command::num_packed_units) {
							while(selectedsubunits.size() > 0) {
								tosplit.fill(dcon::ship_id{});
								for(size_t i = 0; i < command::num_packed_units && i < selectedsubunits.size(); i++) {
									tosplit[i] = selectedsubunits[i];
								}
								command::mark_ships_to_split(state, state.local_player_nation, tosplit);
								(selectedsubunits.size() > command::num_packed_units) ? selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.begin() + command::num_packed_units)
																						: selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
							}
							command::split_navy(state, state.local_player_nation, unitToReorg);
						}
					}
					if(selectedsubunits.empty()) { selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end()); }
					set_visible(state, false);
					break;
				case reorg_win_action::balance:
					// Disregard any of the units the player already selected, because its our way or the hi(fi)-way
					selectedsubunits.erase(selectedsubunits.begin(), selectedsubunits.end());
					if constexpr(std::is_same_v<T, dcon::army_id>) {
						for(auto reg : dcon::fatten(state.world, unitToReorg).get_army_membership()) {
							selectedsubunits.push_back(reg.get_regiment().id);
						}
					} else {
						for(auto reg : dcon::fatten(state.world, unitToReorg).get_navy_membership()) {
							selectedsubunits.push_back(reg.get_ship().id);
						}
					}
					std::sort(selectedsubunits.begin(), selectedsubunits.end(), [&](auto a, auto b){return dcon::fatten(state.world,a).get_type().value > dcon::fatten(state.world,b).get_type().value;});
					//for(size_t i = selectedsubunits.size(); i > 0; i--) {
					for(size_t i = selectedsubunits.size(); i-->0;) {
						if((i % 2) == 0) {
							selectedsubunits.erase(selectedsubunits.begin() + i);
						}
					}
					impl_on_update(state);
					break;
				default:
					break;
			}
			return message_result::consumed;
		}	
		return message_result::unseen;
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
			} else if constexpr(std::is_same_v<T, dcon::ship_id>) {
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
		} else if constexpr(std::is_same_v<T, dcon::ship_id>) {
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

std::unique_ptr<element_base> make_navy_details_listbox(sys::state& state, dcon::gui_def_id def) {
	return make_element_by_type<unit_details_navy_listbox>(state, def);
}
std::unique_ptr<element_base> make_army_details_listbox(sys::state& state, dcon::gui_def_id def) {
	return make_element_by_type<unit_details_army_listbox>(state, def);
}

std::unique_ptr<window_element_base> make_regiment_reorg_window(sys::state& state, dcon::gui_def_id def) {
	return make_element_by_type<unit_reorg_window<dcon::army_id, dcon::regiment_id>>(state, def);
}
std::unique_ptr<window_element_base> make_ship_reorg_window(sys::state& state, dcon::gui_def_id def) {
	return make_element_by_type<unit_reorg_window<dcon::navy_id, dcon::ship_id>>(state, def);
}

} // namespace ui
