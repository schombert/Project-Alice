#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_invest_buttons_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_goods_filter_window.hpp"
#include "gui_projects_window.hpp"
#include "gui_build_factory_window.hpp"
#include <vector>

namespace ui {

enum class production_window_tab : uint8_t {
	factories = 0x0,
	investments = 0x1,
	projects = 0x2,
	goods = 0x3
};

class production_factory_info : public window_element_base {
	image_element_base* output_icon = nullptr;
	image_element_base* input_icons[economy::commodity_set::set_size] = { nullptr };
	image_element_base* input_lack_icons[economy::commodity_set::set_size] = { nullptr };
	image_element_base* closed_overlay = nullptr;
	simple_text_element_base* closed_text = nullptr;
	progress_bar* inprogress_bar = nullptr;
	progress_bar* building_bar = nullptr;
	button_element_base* cancel_progress_btn = nullptr;
	progress_bar* upgrade_bar = nullptr;
	image_element_base* upgrade_overlay = nullptr;

	dcon::factory_id get_factory(sys::state& state) noexcept {
		dcon::factory_id fid{};
		uint8_t count = index;
		province::for_each_province_in_state_instance(state, state_id, [&](dcon::province_id pid) {
			auto fat_id = dcon::fatten(state.world, pid);
			fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				if(count == 0)
					fid = state.world.factory_location_get_factory(flid);
				--count;
			});
		});
		return fid;
	}
public:
	dcon::state_instance_id state_id{};
	uint8_t index = 0; // from 0 to 8

	void on_update(sys::state& state) noexcept override {
		dcon::factory_id fid = get_factory(state);
		set_visible(state, bool(fid));
		if(!bool(fid))
			return;

		bool is_closed = false;
		closed_overlay->set_visible(state, is_closed);
		closed_text->set_visible(state, is_closed);

		bool is_building = false;
		inprogress_bar->set_visible(state, is_building);
		building_bar->set_visible(state, is_building);
		cancel_progress_btn->set_visible(state, is_building);

		bool is_upgrade = false;
		upgrade_bar->set_visible(state, is_upgrade);
		upgrade_overlay->set_visible(state, is_upgrade);

		auto fat_btid = state.world.factory_get_building_type(fid);
		{
			auto cid = fat_btid.get_output().id;
			output_icon->frame = int32_t(state.world.commodity_get_icon(cid));
		}
		// Commodity set
		auto cset = fat_btid.get_inputs();
		for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i)
			if(input_icons[size_t(i)]) {
				auto cid = cset.commodity_type[size_t(i)];
				input_icons[size_t(i)]->frame = int32_t(state.world.commodity_get_icon(cid));
				bool is_lack = cid != dcon::commodity_id{} ? state.world.nation_get_demand_satisfaction(state.local_player_nation, cid) < 0.5f : false;
				input_lack_icons[size_t(i)]->set_visible(state, is_lack);
			}
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "level") {
			return make_element_by_type<factory_level_text>(state, id);
		} else if(name == "income") {
			return make_element_by_type<factory_profit_text>(state, id);
		} else if(name == "output") {
			auto ptr = make_element_by_type<commodity_factory_image>(state, id);
			output_icon = ptr.get();
			return ptr;
		} else if(name == "closed_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			closed_overlay = ptr.get();
			return ptr;
		} else if(name == "factory_closed_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			closed_text = ptr.get();
			return ptr;
		} else if(name == "prod_factory_inprogress_bg") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			inprogress_bar = ptr.get();
			return ptr;
		} else if(name == "build_factory_progress") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			building_bar = ptr.get();
			return ptr;
		} else if(name == "prod_cancel_progress") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			cancel_progress_btn = ptr.get();
			return ptr;
		} else if(name == "upgrade_factory_progress") {
			auto ptr = make_element_by_type<progress_bar>(state, id);
			upgrade_bar = ptr.get();
			return ptr;
		} else if(name == "progress_overlay_16_64") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			upgrade_overlay = ptr.get();
			return ptr;
		} else if(name == "employment_ratio") {
			return make_element_by_type<factory_employment_image>(state, id);
		} else if(name == "priority") {
			return make_element_by_type<factory_priority_image>(state, id);
		} else if(name.substr(0, 6) == "input_") {
			auto input_index = size_t(std::stoi(std::string(name.substr(6))));
			if(name.ends_with("_lack2")) {
				auto ptr = make_element_by_type<image_element_base>(state, id);
				input_lack_icons[input_index] = ptr.get();
				return ptr;
			} else {
				auto ptr = make_element_by_type<commodity_factory_image>(state, id);
				input_icons[input_index] = ptr.get();
				return ptr;
			}
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::factory_id>()) {
			payload.emplace<dcon::factory_id>(get_factory(state));
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			state_id = any_cast<dcon::state_instance_id>(payload);
			on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_factory_info_bounds_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		// Create factory slots for each of the provinces
		for(uint8_t factory_index = 0; factory_index < 8; ++factory_index) {
			auto ptr = make_element_by_type<production_factory_info>(state, state.ui_state.defs_by_name.find("factory_info")->second.definition);
			ptr->index = factory_index;
			ptr->base_data.position.x = factory_index * ptr->base_data.size.x;
			add_child_to_front(std::move(ptr));
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::state_instance_id>()) {
			for(auto& child : children)
				child->impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_build_new_factory : public button_element_base {
public:
	/*void button_action(sys::state& state) noexcept override {
		make_element_by_type<factory_build_new_factory_window>(state, id);
	}*/
};
/*factory_build_new_factory_window*/
class production_state_info : public listbox_row_element_base<dcon::state_instance_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_focus") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "state_name") {
			return make_element_by_type<state_name_text>(state, id);
		} else if(name == "factory_count") {
			return make_element_by_type<state_factory_count_text>(state, id);
		} else if(name == "build_new_factory") {
			return make_element_by_type<production_build_new_factory>(state, id);
		} else if(name == "avg_infra_text") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "factory_info_bounds") {
			return make_element_by_type<production_factory_info_bounds_window>(state, id);
		} else {
			return nullptr;
		}
	}

	void update(sys::state& state) noexcept override {
		Cyto::Any payload = content;
		impl_set(state, payload);
	}
};

class production_state_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override {
        return "state_info";
    }
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(const auto fat_id : state.world.nation_get_state_ownership(state.local_player_nation))
			row_contents.push_back(fat_id.get_state());
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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::commodity_id>()) {
			payload.emplace<dcon::commodity_id>(commodity_id);
			return message_result::consumed;
		}
		return message_result::unseen;
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
	xy_pair base_commodity_offset{ 33, 50 };
	xy_pair commodity_offset{ 33, 50 };

	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> investment_button_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;

	void set_visible_vector_elements(sys::state& state, std::vector<element_base*>& elements, bool v) noexcept {
		for(auto element : elements)
			element->set_visible(state, v);
	}

	void hide_sub_windows(sys::state& state) noexcept {
		set_visible_vector_elements(state, factory_elements, false);
		set_visible_vector_elements(state, investment_brow_elements, false);
		set_visible_vector_elements(state, investment_button_elements, false);
		set_visible_vector_elements(state, project_elements, false);
		set_visible_vector_elements(state, good_elements, false);
	}
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		{
			auto ptr = make_element_by_type<goods_filter_window>(state, state.ui_state.defs_by_name.find("goods_filter_template")->second.definition);
			ptr->set_visible(state, true);
			factory_elements.push_back(ptr.get());
			add_child_to_front(std::move(ptr));
		}

		for(curr_commodity_group = sys::commodity_group::military_goods;
			curr_commodity_group != sys::commodity_group::count;
			curr_commodity_group = static_cast<sys::commodity_group>(uint8_t(curr_commodity_group) + 1))
		{
			commodity_offset.x = base_commodity_offset.x;

			// Place legend for this category...
			auto ptr = make_element_by_type<production_goods_category_name>(state, state.ui_state.defs_by_name.find("production_goods_name")->second.definition);
			ptr->base_data.position = commodity_offset;
			Cyto::Any payload = curr_commodity_group;
			ptr->impl_set(state, payload);
			ptr->set_visible(state, false);
			commodity_offset.y += ptr->base_data.size.y;
			good_elements.push_back(ptr.get());
			add_child_to_front(std::move(ptr));

			int16_t cell_height = 0;
			// Place infoboxes for each of the goods...
			state.world.for_each_commodity([&](dcon::commodity_id id) {
				if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != curr_commodity_group
				|| !bool(id))
					return;

				auto info_ptr = make_element_by_type<production_good_info>(state, state.ui_state.defs_by_name.find("production_info")->second.definition);
				info_ptr->base_data.position = commodity_offset;
				info_ptr->set_visible(state, false);

				int16_t cell_width = info_ptr->base_data.size.x;
				cell_height = info_ptr->base_data.size.y;

				commodity_offset.x += cell_width;
				if(commodity_offset.x + cell_width >= base_data.size.x) {
					commodity_offset.x = base_commodity_offset.x;
					commodity_offset.y += cell_height;
				}

				Cyto::Any payload = id;
				info_ptr->impl_set(state, payload);

				good_elements.push_back(info_ptr.get());
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
		} else if(name == "project_listbox") {
			auto ptr = make_element_by_type<production_project_listbox>(state, id);
			project_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_window_tab>()) {
			auto enum_val = any_cast<production_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
				case production_window_tab::factories:
					set_visible_vector_elements(state, factory_elements, true);
					break;
				case production_window_tab::investments:
					set_visible_vector_elements(state, investment_brow_elements, true);
					break;
				case production_window_tab::projects:
					set_visible_vector_elements(state, project_elements, true);
					break;
				case production_window_tab::goods:
					set_visible_vector_elements(state, good_elements, true);
					break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
