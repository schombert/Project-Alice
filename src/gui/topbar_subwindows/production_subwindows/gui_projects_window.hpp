#pragma once

#include <variant>
#include "gui_element_types.hpp"
#include "gui_project_investment_window.hpp"
#include "gui_production_enum.hpp"

namespace ui {

typedef std::variant< dcon::province_building_construction_id, dcon::state_building_construction_id> production_project_data;

struct production_project_input_data {
	dcon::commodity_id cid{};
	float satisfied = 0.f;
	float needed = 0.f;
};

class production_project_input_item : public listbox_row_element_base<production_project_input_data> {
	simple_text_element_base* amount_text = nullptr;
	element_base* commodity_icon = nullptr;

public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<production_project_input_data>::on_create(state);
		amount_text->base_data.position.y = commodity_icon->base_data.position.y + commodity_icon->base_data.size.y - 4;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			auto ptr = make_element_by_type<commodity_factory_image>(state, id);
			commodity_icon = ptr.get();
			return ptr;
		} else if(name == "goods_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			amount_text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		amount_text->set_text(state, text::format_float(content.satisfied, 1) + "/" + text::format_float(content.needed, 1));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(content.cid);
			return message_result::consumed;
		}
		return listbox_row_element_base<production_project_input_data>::get(state, payload);
	}
};

class production_project_input_listbox
    : public overlapping_listbox_element_base<production_project_input_item, production_project_input_data> {
protected:
	std::string_view get_row_element_name() override { return "goods_need_template"; }
};

class production_project_invest_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<production_action>{production_action{production_action::investment_window}};
			parent->impl_get(state, payload);
		}
	}
};

class production_project_info : public listbox_row_element_base<production_project_data> {
	image_element_base* building_icon = nullptr;
	image_element_base* factory_icon = nullptr;
	simple_text_element_base* name_text = nullptr;
	simple_text_element_base* cost_text = nullptr;
	production_project_input_listbox* input_listbox = nullptr;

	float get_cost(sys::state& state, economy::commodity_set const& cset) {
		float total = 0.f;
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
			dcon::commodity_id cid = cset.commodity_type[i];
			if(bool(cid))
				total += state.world.commodity_get_current_price(cid) * cset.commodity_amounts[i];
		}
		return total;
	}

	dcon::state_instance_id get_state_instance_id(sys::state& state) {
		if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
			auto fat_id = dcon::fatten(state.world, std::get<dcon::province_building_construction_id>(content));
			auto sdef = state.world.province_get_state_from_abstract_state_membership(fat_id.get_province());
			dcon::state_instance_id state_id{};
			state.world.for_each_state_instance([&](dcon::state_instance_id sid) {
				if(state.world.state_instance_get_definition(sid) == sdef)
					state_id = sid;
			});
			return state_id;
		} else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
			auto fat_id = dcon::fatten(state.world, std::get<dcon::state_building_construction_id>(content));
			return fat_id.get_state();
		}
		return dcon::state_instance_id{};
	}

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "project_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			factory_icon = ptr.get();
			return ptr;
		} else if(name == "infra") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			building_icon = ptr.get();
			return ptr;
		} else if(name == "project_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			name_text = ptr.get();
			return ptr;
		} else if(name == "project_cost") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			cost_text = ptr.get();
			return ptr;
		} else if(name == "pop_icon") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "pop_amount") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "invest_project") {
			return make_element_by_type<production_project_invest_button>(state, id);
		} else if(name == "input_goods") {
			auto ptr = make_element_by_type<production_project_input_listbox>(state, id);
			input_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		economy::commodity_set satisfied_commodities{};
		economy::commodity_set needed_commodities{};
		if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
			factory_icon->set_visible(state, false);
			building_icon->set_visible(state, true);
			auto fat_id = dcon::fatten(state.world, std::get<dcon::province_building_construction_id>(content));
			factory_icon->frame = uint16_t(fat_id.get_type());
			name_text->set_text(state, text::produce_simple_string(
			                               state, province_building_type_get_name(economy::province_building_type(fat_id.get_type()))));
			switch(economy::province_building_type(fat_id.get_type())) {
			case economy::province_building_type::railroad:
				needed_commodities = state.economy_definitions.railroad_definition.cost;
				break;
			case economy::province_building_type::fort:
				needed_commodities = state.economy_definitions.fort_definition.cost;
				break;
			case economy::province_building_type::naval_base:
				needed_commodities = state.economy_definitions.naval_base_definition.cost;
				break;
			}
			satisfied_commodities = fat_id.get_purchased_goods();
		} else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
			factory_icon->set_visible(state, true);
			building_icon->set_visible(state, false);
			auto fat_id = dcon::fatten(state.world, std::get<dcon::state_building_construction_id>(content));
			factory_icon->frame = uint16_t(fat_id.get_type().get_output().get_icon());
			name_text->set_text(state, text::produce_simple_string(state, fat_id.get_type().get_name()));
			needed_commodities = fat_id.get_type().get_construction_costs();
			satisfied_commodities = fat_id.get_purchased_goods();
		}

		if(input_listbox) {
			input_listbox->row_contents.clear();
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i)
				if(bool(needed_commodities.commodity_type[i]))
					input_listbox->row_contents.push_back(production_project_input_data{
					    needed_commodities.commodity_type[i],       // cid
					    satisfied_commodities.commodity_amounts[i], // satisfied
					    needed_commodities.commodity_amounts[i]     // needed
					});
			input_listbox->update(state);
		}

		float purchased_cost = get_cost(state, satisfied_commodities);
		float total_cost = get_cost(state, needed_commodities);
		cost_text->set_text(state, text::format_money(purchased_cost) + "/" + text::format_money(total_cost));
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(get_state_instance_id(state));
			return message_result::consumed;
		}
		return listbox_row_element_base<production_project_data>::get(state, payload);
	}
};

class production_project_listbox : public listbox_element_base<production_project_info, production_project_data> {
protected:
	std::string_view get_row_element_name() override { return "project_info"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.nation_for_each_state_building_construction_as_nation(state.local_player_nation,
		                                                                  [&](dcon::state_building_construction_id id) {
			                                                                  auto fat_id = dcon::fatten(state.world, id);
			                                                                  if(fat_id.get_is_pop_project())
				                                                                  row_contents.push_back(production_project_data(id));
		                                                                  });
		state.world.nation_for_each_province_building_construction_as_nation(state.local_player_nation,
		                                                                     [&](dcon::province_building_construction_id id) {
			                                                                     auto fat_id = dcon::fatten(state.world, id);
			                                                                     if(fat_id.get_is_pop_project())
				                                                                     row_contents.push_back(production_project_data(id));
		                                                                     });
		update(state);
	}
};
} // namespace ui
