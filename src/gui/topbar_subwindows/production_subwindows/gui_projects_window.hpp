#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

typedef std::variant<
    dcon::province_building_construction_id,
    dcon::state_building_construction_id
> production_project_data;

template<std::size_t Index>
class production_project_input_item : public window_element_base {
    simple_text_element_base* amount_text = nullptr;
    commodity_factory_image* type_icon = nullptr;
public:
    void on_update(sys::state& state) noexcept override {
        if(parent) {
            Cyto::Any payload = production_project_data(dcon::province_building_construction_id{});
            parent->impl_get(state, payload);
            auto data = any_cast<production_project_data>(payload);

            auto needed_amount = 0.f;
            auto satisfied_amount = 0.f;
            dcon::commodity_id cid{};
            if(std::holds_alternative<dcon::province_building_construction_id>(data)) {
                auto id = std::get<dcon::province_building_construction_id>(data);
                auto fat_id = dcon::fatten(state.world, id);
                switch(economy::province_building_type(fat_id.get_type())) {
                case economy::province_building_type::railroad:
                    needed_amount = state.economy_definitions.railroad_definition.cost.commodity_amounts[Index];
                    break;
                case economy::province_building_type::fort:
                    needed_amount = state.economy_definitions.fort_definition.cost.commodity_amounts[Index];
                    break;
                case economy::province_building_type::naval_base:
                    needed_amount = state.economy_definitions.naval_base_definition.cost.commodity_amounts[Index];
                    break;
                }
                satisfied_amount = fat_id.get_purchased_goods().commodity_amounts[Index];
                cid = fat_id.get_purchased_goods().commodity_type[Index];
            } else if(std::holds_alternative<dcon::state_building_construction_id>(data)) {
                auto id = std::get<dcon::state_building_construction_id>(data);
                auto fat_id = dcon::fatten(state.world, id);
                needed_amount = fat_id.get_type().get_construction_costs().commodity_amounts[Index];
                satisfied_amount = fat_id.get_purchased_goods().commodity_amounts[Index];
                cid = fat_id.get_purchased_goods().commodity_type[Index];
            }

            if(bool(cid)) {
                amount_text->set_visible(state, true);
                type_icon->set_visible(state, true);

                amount_text->set_text(state, text::format_float(satisfied_amount, 1) + "/" + text::format_float(needed_amount, 1));
                Cyto::Any c_payload = cid;
                type_icon->impl_set(state, c_payload);
            } else {
                amount_text->set_visible(state, false);
                type_icon->set_visible(state, false);

            }
        }
    }

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "goods_type") {
			auto ptr = make_element_by_type<commodity_factory_image>(state, id);
            type_icon = ptr.get();
            return ptr;
        } else if(name == "goods_amount") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
            amount_text = ptr.get();
            return ptr;
        } else {
            return nullptr;
        }
    }
};

class production_project_inputs_needed : public window_element_base {
	template<std::size_t... Targs>
	void generate_commodity_need_template(sys::state& state, std::integer_sequence<std::size_t, Targs...> const &) {
		const xy_pair cell_offset = base_data.position;
		(([&]{
			auto win = make_element_by_type<production_project_input_item<Targs>>(state, state.ui_state.defs_by_name.find("goods_need_template")->second.definition);
			win->base_data.position.x = cell_offset.x + (Targs * win->base_data.size.x);
			win->base_data.position.y = cell_offset.y;
			add_child_to_front(std::move(win));
		})(), ...);
	}
public:
    void on_create(sys::state& state) noexcept override {
        window_element_base::on_create(state);
        generate_commodity_need_template(state, std::integer_sequence<std::size_t, 0, 1, 2, 3, 4, 5, 6>{});
    }
};

class production_project_info : public listbox_row_element_base<production_project_data> {
    image_element_base* building_icon = nullptr;
    image_element_base* factory_icon = nullptr;
    simple_text_element_base* name_text = nullptr;
    simple_text_element_base* cost_text = nullptr;

    float get_cost(sys::state& state, economy::commodity_set& cset) {
        auto total = 0.f;
        for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i) {
            auto cid = cset.commodity_type[i];
            if(bool(cid))
                total += state.world.commodity_get_current_price(cid) * cset.commodity_amounts[i];
        }
        return total;
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
            return make_element_by_type<button_element_base>(state, id);
		} else if(name == "input_goods") {
            return make_element_by_type<production_project_inputs_needed>(state, id);
		} else {
			return nullptr;
		}
	}
    
    void on_update(sys::state& state) noexcept override {
        dcon::state_instance_id state_id{};
        if(std::holds_alternative<dcon::province_building_construction_id>(content)) {
            factory_icon->set_visible(state, false);
            building_icon->set_visible(state, true);

            auto fat_id = dcon::fatten(state.world, std::get<dcon::province_building_construction_id>(content));
            factory_icon->frame = uint16_t(fat_id.get_type());

            auto total_cost = 0.f;
            switch(economy::province_building_type(fat_id.get_type())) {
            case economy::province_building_type::railroad:
                name_text->set_text(state, text::produce_simple_string(state, "railroad"));
                total_cost = get_cost(state, state.economy_definitions.railroad_definition.cost);
                break;
            case economy::province_building_type::fort:
                name_text->set_text(state, text::produce_simple_string(state, "fort"));
                total_cost = get_cost(state, state.economy_definitions.fort_definition.cost);
                break;
            case economy::province_building_type::naval_base:
                name_text->set_text(state, text::produce_simple_string(state, "naval_base"));
                total_cost = get_cost(state, state.economy_definitions.naval_base_definition.cost);
                break;
            }

            // TODO: better way to get definition from instance
            auto sdef = state.world.province_get_state_from_abstract_state_membership(fat_id.get_province());
            state.world.for_each_state_instance([&](dcon::state_instance_id sid) {
                if(state.world.state_instance_get_definition(sid) == sdef)
                    state_id = sid;
            });

            auto cost = get_cost(state, fat_id.get_purchased_goods());
            cost_text->set_text(state, text::format_money(cost) + "/" + text::format_money(total_cost));
        } else if(std::holds_alternative<dcon::state_building_construction_id>(content)) {
            factory_icon->set_visible(state, true);
            building_icon->set_visible(state, false);

            auto fat_id = dcon::fatten(state.world, std::get<dcon::state_building_construction_id>(content));
            factory_icon->frame = uint16_t(fat_id.get_type().id.index());
            name_text->set_text(state, text::produce_simple_string(state, fat_id.get_type().get_name()));
            state_id = fat_id.get_state();

            auto cost = get_cost(state, fat_id.get_purchased_goods());
            auto total_cost = get_cost(state, fat_id.get_type().get_construction_costs());
            cost_text->set_text(state, text::format_money(cost) + "/" + text::format_money(total_cost));
        }

        Cyto::Any payload = state_id;
        impl_set(state, payload);
    }

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_project_data>()) {
			payload.emplace<production_project_data>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base<production_project_data>::get(state, payload);
	}
};

class production_project_listbox : public listbox_element_base<production_project_info, production_project_data> {
protected:
	std::string_view get_row_element_name() override {
        return "project_info";
    }
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
        state.world.nation_for_each_province_building_construction_as_nation(state.local_player_nation, [&](dcon::province_building_construction_id id) {
            auto fat_id = dcon::fatten(state.world, id);
            if(fat_id.get_is_pop_project())
                row_contents.push_back(production_project_data(id));
        });
        state.world.nation_for_each_state_building_construction_as_nation(state.local_player_nation, [&](dcon::state_building_construction_id id) {
            auto fat_id = dcon::fatten(state.world, id);
            if(fat_id.get_is_pop_project())
                row_contents.push_back(production_project_data(id));
        });
		update(state);
	}
};
}
