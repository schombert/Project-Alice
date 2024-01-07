#pragma once

#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

enum class movements_sort_order { size, radicalism, name };

class movements_sort_order_button : public button_element_base {
public:
	movements_sort_order order = movements_sort_order::size;

	movements_sort_order_button(movements_sort_order order) : order(order) { }
	void button_action(sys::state& state) noexcept override {
		send(state, parent, order);
	}
};

class movement_suppress_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		dcon::nation_id nation_id = retrieve<dcon::nation_id>(state, parent);
		dcon::movement_id movement_id = retrieve<dcon::movement_id>(state, parent);
		disabled = !command::can_suppress_movement(state, nation_id, movement_id);
	}

	void button_action(sys::state& state) noexcept override {
		dcon::nation_id nation_id = retrieve<dcon::nation_id>(state, parent);
		dcon::movement_id movement_id = retrieve<dcon::movement_id>(state, parent);
		command::suppress_movement(state, nation_id, movement_id);
	}
};

class movements_option : public listbox_row_element_base<dcon::movement_id> {
private:
	flag_button* nationalist_flag = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			return make_element_by_type<movement_issue_name_text>(state, id);
		} else if(name == "nationalist_name") {
			return make_element_by_type<movement_nationalist_name_text>(state, id);
		} else if(name == "flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			nationalist_flag = ptr.get();
			return ptr;
		} else if(name == "size") {
			return make_element_by_type<movement_size_text>(state, id);
		} else if(name == "radical_val") {
			return make_element_by_type<movement_radicalism_text>(state, id);
		} else if(name == "suppress_button") {
			return make_element_by_type<movement_suppress_button>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.world.movement_get_associated_independence(content)) {
			nationalist_flag->set_visible(state, true);
		} else {
			nationalist_flag->set_visible(state, false);
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			payload.emplace<dcon::national_identity_id>(state.world.movement_get_associated_independence(content));
			return message_result::consumed;
		} else {
			return listbox_row_element_base<dcon::movement_id>::get(state, payload);
		}
	}
};

class movements_list : public listbox_element_base<movements_option, dcon::movement_id> {
protected:
	std::string_view get_row_element_name() override {
		return "movement_entry";
	}

public:
	movements_sort_order order = movements_sort_order::size;

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto movement : state.world.nation_get_movement_within(state.local_player_nation)) {
			if (movement.get_movement().get_pop_support() >= 1.0f) {
				row_contents.push_back(movement.get_movement().id);
			}
		}

		switch(order) {
		case movements_sort_order::size:
			std::sort(row_contents.begin(), row_contents.end(),
					[&](dcon::movement_id a, dcon::movement_id b) {
					auto a_size = state.world.movement_get_pop_support(a);
					auto b_size = state.world.movement_get_pop_support(b);
					return a_size > b_size;
					});
			break;
		case movements_sort_order::radicalism:
			std::sort(row_contents.begin(), row_contents.end(),
					[&](dcon::movement_id a, dcon::movement_id b) {
					auto a_rad = state.world.movement_get_radicalism(a);
					auto b_rad = state.world.movement_get_radicalism(b);
					return a_rad > b_rad;
					});
			break;
		case movements_sort_order::name:
			std::sort(row_contents.begin(), row_contents.end(),
					[&](dcon::movement_id a, dcon::movement_id b) {
					auto fat_a = dcon::fatten(state.world, a);
					std::string a_name;
					if(fat_a.get_associated_independence()) {
						a_name = text::get_name_as_string(state, fat_a.get_associated_independence());
					} else {
						a_name = text::produce_simple_string(state, fat_a.get_associated_issue_option().get_movement_name());
					}
					auto fat_b = dcon::fatten(state.world, b);
					std::string b_name;
					if(fat_b.get_associated_independence()) {
						b_name = text::get_name_as_string(state, fat_b.get_associated_independence());
					} else {
						b_name = text::produce_simple_string(state, fat_b.get_associated_issue_option().get_movement_name());
					}
					return a_name < b_name;
					});
			break;
		}
		update(state);
	}
};

class rebel_faction_size_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept override {
		float total = 0.f;
		for(auto member : state.world.rebel_faction_get_pop_rebellion_membership(rebel_faction_id)) {
			total += member.get_pop().get_size();
		}
		return text::prettify(int64_t(total));
	}
};

class rebel_faction_active_brigade_count_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept override {
		auto count = rebel::get_faction_brigades_active(state, rebel_faction_id);
		return text::prettify(count);
	}
};

class rebel_faction_ready_brigade_count_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept override {
		auto count = rebel::get_faction_brigades_ready(state, rebel_faction_id);
		return text::prettify(count);
	}
};

class rebel_faction_organization_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept override {
		auto org = rebel::get_faction_organization(state, rebel_faction_id);
		return text::format_percentage(org, 1);
	}
};

class rebel_faction_revolt_risk_text : public standard_rebel_faction_text {
public:
	std::string get_text(sys::state& state, dcon::rebel_faction_id rebel_faction_id) noexcept override {
		auto risk = rebel::get_faction_revolt_risk(state, rebel_faction_id);
		return text::format_percentage(risk, 1);
	}
};

class rebel_faction_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto reb = retrieve<dcon::rebel_faction_id>(state, parent);
		set_text(state, rebel::rebel_name(state, reb));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto n = state.local_player_nation;
		auto reb = retrieve<dcon::rebel_faction_id>(state, parent);
		auto type = state.world.rebel_faction_get_type(reb);
		if(auto k = state.world.rebel_type_get_demands_enforced_trigger(type); k) {
			text::add_line(state, contents, "alice_rf_demands_enforced_trigger");
			ui::trigger_description(state, contents, k, trigger::to_generic(n), trigger::to_generic(state.local_player_nation), -1);
		}
		if(auto k = state.world.rebel_type_get_demands_enforced_effect(type); k) {
			text::add_line(state, contents, "alice_rf_demands_enforced_effect");
			ui::effect_description(state, contents, k, trigger::to_generic(n), trigger::to_generic(state.local_player_nation), -1, uint32_t(state.current_date.value), uint32_t(n.index() ^ (reb.index() << 4)));
		}
		auto prov = state.world.nation_get_capital(n);
		if(auto k = state.world.rebel_type_get_siege_won_trigger(type); k) {
			text::add_line(state, contents, "alice_rf_siege_won_trigger", text::variable_type::x, prov);
			ui::trigger_description(state, contents, k, trigger::to_generic(prov), trigger::to_generic(prov));
		}
		if(auto k = state.world.rebel_type_get_siege_won_effect(type); k) {
			text::add_line(state, contents, "alice_rf_siege_won_effect", text::variable_type::x, prov);
			ui::effect_description(state, contents, k, trigger::to_generic(prov), trigger::to_generic(prov), trigger::to_generic(reb), uint32_t(state.current_date.value), uint32_t(reb.index() ^ (reb.index() << 4)));
		}
	}
};

class rebel_faction_type_icon : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::rebel_faction_id>(state, parent);
		frame = int32_t(state.world.rebel_type_get_icon(state.world.rebel_faction_get_type(content)) - 1);
	}
};

class movements_rebel_option : public listbox_row_element_base<dcon::rebel_faction_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "size") {
			return make_element_by_type<rebel_faction_size_text>(state, id);
		} else if(name == "name") {
			return make_element_by_type<rebel_faction_name_text>(state, id);
		} else if(name == "type") {
			return make_element_by_type<rebel_faction_type_icon>(state, id);
		} else if(name == "brigades_ready") {
			return make_element_by_type<rebel_faction_ready_brigade_count_text>(state, id);
		} else if(name == "brigades_active") {
			return make_element_by_type<rebel_faction_active_brigade_count_text>(state, id);
		} else if(name == "organization") {
			return make_element_by_type<rebel_faction_organization_text>(state, id);
		} else if(name == "revolt_risk") {
			return make_element_by_type<rebel_faction_revolt_risk_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class movements_rebel_list : public listbox_element_base<movements_rebel_option, dcon::rebel_faction_id> {
protected:
	std::string_view get_row_element_name() override {
		return "rebel_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto rebellion : state.world.nation_get_rebellion_within(state.local_player_nation)) {
			row_contents.push_back(rebellion.get_rebels().id);
		}
		update(state);
	}
};

class movements_window : public window_element_base {
private:
	movements_list* movements_listbox = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "sortby_size_button") {
			auto ptr = make_element_by_type<movements_sort_order_button>(state, id, movements_sort_order::size);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_radical_button") {
			auto ptr = make_element_by_type<movements_sort_order_button>(state, id, movements_sort_order::radicalism);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sortby_name_button") {
			return make_element_by_type<movements_sort_order_button>(state, id, movements_sort_order::name);
		} else if(name == "rebel_listbox") {
			return make_element_by_type<movements_rebel_list>(state, id);
		} else if(name == "movements_listbox") {
			auto ptr = make_element_by_type<movements_list>(state, id);
			movements_listbox = ptr.get();
			return ptr;
		} else if(name == "suppression_value") {
			return make_element_by_type<nation_suppression_points_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<movements_sort_order>()) {
			auto enum_val = any_cast<movements_sort_order>(payload);
			movements_listbox->order = enum_val;
			movements_listbox->impl_on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

} // namespace ui
