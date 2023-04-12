#pragma once

#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

enum class movements_sort_order {
	size, radicalism, name
};

class movements_sort_order_button : public button_element_base {
public:
	movements_sort_order order = movements_sort_order::size;

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = order;
			parent->impl_get(state, payload);
		}
	}
};

class movements_option : public listbox_row_element_base<dcon::movement_id> {
private:
	flag_button* nationalist_flag = nullptr;
	movement_nationalist_name_text* nationalist_name = nullptr;
	movement_issue_name_text* issue_name = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			auto ptr = make_element_by_type<movement_issue_name_text>(state, id);
			issue_name = ptr.get();
			return ptr;
		} else if(name == "nationalist_name") {
			auto ptr = make_element_by_type<movement_nationalist_name_text>(state, id);
			nationalist_name = ptr.get();
			return ptr;
		} else if(name == "flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			nationalist_flag = ptr.get();
			return ptr;
		} else if(name == "size") {
			return make_element_by_type<movement_size_text>(state, id);
		} else if(name == "radical_val") {
			return make_element_by_type<movement_radicalism_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(state.world.movement_get_associated_independence(content)) {
			nationalist_flag->set_visible(state, true);
			nationalist_name->set_visible(state, true);
			issue_name->set_visible(state, false);
		} else {
			nationalist_flag->set_visible(state, false);
			nationalist_name->set_visible(state, false);
			issue_name->set_visible(state, true);
		}
		Cyto::Any payload = content;
		impl_set(state, payload);
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
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "movement_entry";
    }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto movement : state.world.nation_get_movement_within(nation_id)) {
			row_contents.push_back(movement.get_movement().id);
		}
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
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
			return make_element_by_type<rebel_type_icon>(state, id);
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

	void update(sys::state& state) noexcept override {
		Cyto::Any faction_payload = content;
		Cyto::Any rebel_type_payload = state.world.rebel_faction_get_type(content).id;
		impl_set(state, faction_payload);
		impl_set(state, rebel_type_payload);
	}
};

class movements_rebel_list : public listbox_element_base<movements_rebel_option, dcon::rebel_faction_id> {
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "rebel_window";
    }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto rebellion : state.world.nation_get_rebellion_within(nation_id)) {
			row_contents.push_back(rebellion.get_rebels().id);
		}
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
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
			auto ptr = make_element_by_type<movements_sort_order_button>(state, id);
			ptr->order = movements_sort_order::size;
			return ptr;
		} else if(name == "sortby_radical_button") {
			auto ptr = make_element_by_type<movements_sort_order_button>(state, id);
			ptr->order = movements_sort_order::radicalism;
			return ptr;
		} else if(name == "sortby_name_button") {
			auto ptr = make_element_by_type<movements_sort_order_button>(state, id);
			ptr->order = movements_sort_order::name;
			return ptr;
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
			switch(enum_val) {
				case movements_sort_order::size:
					std::sort(movements_listbox->row_contents.begin(), movements_listbox->row_contents.end(), [&](dcon::movement_id a, dcon::movement_id b) {
						auto a_size = state.world.movement_get_pop_support(a);
						auto b_size = state.world.movement_get_pop_support(b);
						return a_size > b_size;
					});
					movements_listbox->update(state);
					break;
				case movements_sort_order::radicalism:
					std::sort(movements_listbox->row_contents.begin(), movements_listbox->row_contents.end(), [&](dcon::movement_id a, dcon::movement_id b) {
						auto a_rad = state.world.movement_get_radicalism(a);
						auto b_rad = state.world.movement_get_radicalism(b);
						return a_rad > b_rad;
					});
					movements_listbox->update(state);
					break;
				case movements_sort_order::name:
					std::sort(movements_listbox->row_contents.begin(), movements_listbox->row_contents.end(), [&](dcon::movement_id a, dcon::movement_id b) {
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
					movements_listbox->update(state);
					break;
			}
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

}
