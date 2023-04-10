#pragma once

#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "system_state.hpp"

namespace ui {

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
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "sortby_size_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_radical_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "sortby_name_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			return ptr;
		} else if(name == "rebel_listbox") {
			return make_element_by_type<movements_rebel_list>(state, id);
		} else if(name == "suppression_value") {
			return make_element_by_type<nation_suppression_points_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
