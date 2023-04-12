#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_invest_buttons_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_goods_filter_window.hpp"
#include <vector>

namespace ui {

enum class production_window_tab : uint8_t {
	factories = 0x0,
	investments = 0x1,
	projects = 0x2,
	goods = 0x3
};

class production_state_info : public listbox_row_element_base<dcon::state_instance_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_focus") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_count") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "build_new_factory") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "avg_infra_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class production_state_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override {
        return "state_info";
    }
public:
	void on_create(sys::state& state) noexcept override {
		// Clear "center" property of object...
		state.ui_defs.gui[state.ui_state.defs_by_name.find("state_info")->second.definition].flags &= ~element_data::orientation_mask;
		listbox_element_base::on_create(state);
	}

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_state_instance([&](dcon::state_instance_id id) {
			auto nation_id = state.world.state_instance_get_nation_from_state_ownership(id);
			if(state.local_player_nation == nation_id)
				row_contents.push_back(id);
		});
		update(state);
	}
};


class production_goods_category_name : public window_element_base {
	simple_text_element_base* goods_cat_name = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cat_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			goods_cat_name = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<sys::commodity_group>()) {
			auto group = any_cast<sys::commodity_group>(payload);
			switch(group) {
			case sys::commodity_group::military_goods:
				goods_cat_name->set_text(state, text::produce_simple_string(state, "military_goods"));
				break;
			case sys::commodity_group::raw_material_goods:
				goods_cat_name->set_text(state, text::produce_simple_string(state, "raw_material_goods"));
				break;
			case sys::commodity_group::industrial_goods:
				goods_cat_name->set_text(state, text::produce_simple_string(state, "industrial_goods"));
				break;
			case sys::commodity_group::consumer_goods:
				goods_cat_name->set_text(state, text::produce_simple_string(state, "consumer_goods"));
				break;
			default:
				break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class commodity_output_total_text : public simple_text_element_base {
	dcon::commodity_id commodity_id{};
public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, text::format_float(economy::commodity_daily_production_amount(state, commodity_id), 1));
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class commodity_factory_image : public image_element_base {
	dcon::commodity_id commodity_id{};
public:
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			frame = static_cast<int32_t>(commodity_id.index());
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_good_info : public window_element_base {
	dcon::commodity_id commodity_id{};
	commodity_output_total_text* good_output_total = nullptr;
	image_element_base* good_not_producing_overlay = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "output_factory") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "output_total") {
			auto ptr = make_element_by_type<commodity_output_total_text>(state, id);
			good_output_total = ptr.get();
			return ptr;
		} else if(name == "prod_producing_not_total") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			good_not_producing_overlay = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		bool is_producing = economy::commodity_daily_production_amount(state, commodity_id) > 0.f;
		// Display red-overlay if not producing
		good_not_producing_overlay->set_visible(state, !is_producing);
		good_output_total->set_visible(state, is_producing);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			commodity_id = any_cast<dcon::commodity_id>(payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_window : public generic_tabbed_window<production_window_tab> {
	production_state_listbox* state_listbox = nullptr;

	sys::commodity_group curr_commodity_group{};
	xy_pair base_commodity_offset{ 33, 65 };
	xy_pair commodity_offset{ 33, 65 };
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		{
			auto ptr = make_child(state, "goods_filter_template", state.ui_state.defs_by_name.find("goods_filter_template")->second.definition);
			add_child_to_front(std::move(ptr));
		}

		// Clear "center" property so they don't look messed up!
		state.ui_defs.gui[state.ui_state.defs_by_name.find("production_goods_name")->second.definition].flags &= ~element_data::orientation_mask;
		for(curr_commodity_group = sys::commodity_group::military_goods;
			curr_commodity_group != sys::commodity_group::count;
			curr_commodity_group = static_cast<sys::commodity_group>(uint8_t(curr_commodity_group) + 1))
		{
			commodity_offset.x = base_commodity_offset.x;

			// Place legend for this category...
			auto ptr = make_child(state, "production_goods_name", state.ui_state.defs_by_name.find("production_goods_name")->second.definition);
			commodity_offset.y += ptr->base_data.size.y;
			add_child_to_front(std::move(ptr));

			int16_t cell_height = 0;
			// Place infoboxes for each of the goods...
			state.world.for_each_commodity([&](dcon::commodity_id id) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != curr_commodity_group
				|| !bool(id))
					return;

				auto info_ptr = make_child(state, "production_info", state.ui_state.defs_by_name.find("production_info")->second.definition);

				int16_t cell_width = info_ptr->base_data.size.x;
				cell_height = info_ptr->base_data.size.y;

				commodity_offset.x += cell_width;
				if(commodity_offset.x + cell_width >= base_data.size.x) {
					commodity_offset.x = base_commodity_offset.x;
					commodity_offset.y += cell_height;
				}

				Cyto::Any payload = id;
				info_ptr->impl_set(state, payload);
				
				add_child_to_front(std::move(info_ptr));
			});
			// Has atleast 1 good on this row? skip to next row then...
			if(commodity_offset.x > base_commodity_offset.x)
				commodity_offset.y += cell_height;
		}

		set_visible(state, false);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "tab_factories") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::factories;
			return ptr;
		} else if(name == "tab_invest") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::investments;
			return ptr;
		} else if(name == "tab_popprojects") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::projects;
			return ptr;
		} else if(name == "tab_goodsproduction") {
			auto ptr = make_element_by_type<generic_tab_button<production_window_tab>>(state, id);
			ptr->target = production_window_tab::goods;
			return ptr;
		} else if(name == "tab_factories_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_invest_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_goodsproduction_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "tab_popprojects_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "factory_buttons") {
			auto ptr = make_element_by_type<factory_buttons_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "state_listbox") {
			auto ptr = make_element_by_type<production_state_listbox>(state, id);
			state_listbox = ptr.get();
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "investment_browser") {
			auto ptr = make_element_by_type<invest_brow_window>(state, id);
			investment_brow_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "invest_buttons") {
			auto ptr = make_element_by_type<invest_buttons_window>(state, id);
			investment_button_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_state") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projects") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_completion") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "sort_by_projecteers") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "pop_sort_buttons") {
			auto ptr = make_element_by_type<pop_sort_buttons_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "goods_filter_template") {
			auto ptr = make_element_by_type<goods_filter_window>(state, id);
			factory_elements.push_back(ptr.get());
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "production_goods_name") {
			auto ptr = make_element_by_type<production_goods_category_name>(state, id);
			ptr->base_data.position = commodity_offset;
			
			Cyto::Any payload = curr_commodity_group;
			ptr->impl_set(state, payload);

			good_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "production_info") {
			auto ptr = make_element_by_type<production_good_info>(state, id);
			ptr->base_data.position = commodity_offset;
			good_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	void hide_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, false);
		}
	}

	void show_vector_elements(sys::state& state, std::vector<element_base*>& elements) {
		for(auto element : elements) {
			element->set_visible(state, true);
		}
	}

	void hide_sub_windows(sys::state& state) {
		hide_vector_elements(state, factory_elements);
		hide_vector_elements(state, investment_brow_elements);
		hide_vector_elements(state, investment_button_elements);
		hide_vector_elements(state, project_elements);
		hide_vector_elements(state, good_elements);
	}
	
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_window_tab>()) {
			auto enum_val = any_cast<production_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case production_window_tab::factories:
					show_vector_elements(state, factory_elements);
					break;
				case production_window_tab::investments:
					show_vector_elements(state, investment_brow_elements);
					break;
				case production_window_tab::projects:
					show_vector_elements(state, project_elements);
					break;
				case production_window_tab::goods:
					show_vector_elements(state, good_elements);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	
	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> investment_button_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
};

}
