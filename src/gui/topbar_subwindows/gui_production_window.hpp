#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_commodity_filters_window.hpp"
#include "gui_projects_window.hpp"
#include "gui_build_factory_window.hpp"
#include "gui_project_investment_window.hpp"
#include "gui_foreign_investment_window.hpp"
#include <vector>

namespace ui {

enum class production_window_tab : uint8_t { factories = 0x0, investments = 0x1, projects = 0x2, goods = 0x3 };

struct production_selection_wrapper {
	dcon::state_instance_id data{};
	bool is_build = false;
	xy_pair focus_pos{0, 0};
};

class factory_employment_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::factory_id>(payload);
			frame = int32_t(state.world.factory_get_primary_employment(content) * 10.f);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);
		auto fat = dcon::fatten(state.world, fid);

		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_factory_employeecount_tooltip"));
		text::add_line_break_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, std::string_view(" -"));
		text::localised_format_box(state, contents, box, std::string_view("craftsmen"));
		text::add_to_layout_box(contents, state, box, int64_t(fat.get_primary_employment()));
		text::add_line_break_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, std::string_view(" -"));
		text::localised_format_box(state, contents, box, std::string_view("clerks"));
		text::add_to_layout_box(contents, state, box, int64_t(fat.get_secondary_employment()));
		text::close_layout_box(contents, box);
	}
};

class factory_priority_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			const dcon::factory_id fid = any_cast<dcon::factory_id>(payload);
			frame = economy::factory_priority(state, fid);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			const dcon::factory_id fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			switch(economy::factory_priority(state, fid)) {
			case 0:
				command::change_factory_settings(state, n, fid, 1, fat.get_subsidized());
				break;
			case 1:
				command::change_factory_settings(state, n, fid, 2, fat.get_subsidized());
				break;
			case 2:
				command::change_factory_settings(state, n, fid, 3, fat.get_subsidized());
				break;
			case 3:
				command::change_factory_settings(state, n, fid, 0, fat.get_subsidized());
				break;
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		const dcon::factory_id fid = any_cast<dcon::factory_id>(payload);

		auto box = text::open_layout_box(contents, 0);
		// TODO - check if we can change the priority,
		// 	- This appears to depend only on if the ruling party is kosher with it
		text::localised_format_box(state, contents, box, std::string_view("production_allowed_to_change_prio_tooltip"));
		// Why pdx, why must the diplomacy priority be used instead of having a separate priority for prod. ?
		// just why
		switch(economy::factory_priority(state, fid)) {
		case 0:
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_prio_none"));
			break;
		case 1:
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_prio_low"));
			break;
		case 2:
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_prio_middle"));
			break;
		case 3:
			text::localised_format_box(state, contents, box, std::string_view("diplomacy_prio_high"));
			break;
		}

		// text::localised_format_box(state, contents, box, std::string_view("production_not_allowed_to_change_prio_tooltip"));
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("production_prio_factory_desc_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_upgrade_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);

			Cyto::Any payload1 = dcon::state_instance_id{};
			parent->impl_get(state, payload1);
			auto sid = any_cast<dcon::state_instance_id>(payload1);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			disabled = !command::can_begin_factory_building_construction(state, n, sid, fat.get_building_type().id, true);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);

			Cyto::Any payload1 = dcon::state_instance_id{};
			parent->impl_get(state, payload1);
			auto sid = any_cast<dcon::state_instance_id>(payload1);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			command::begin_factory_building_construction(state, n, sid, fat.get_building_type().id, true);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_expand_factory_tooltip"));
		// TODO - check if we can expand the factory
		// What we need to check:
		// 	- Ruling party opinion on expanding factories
		// 	- if we got enough money to expand
		// 	- if our level is < 99
		text::localised_format_box(state, contents, box, std::string_view("production_expand_factory_because_tooltip"));
		// text::localised_format_box(state, contents, box, std::string_view("production_not_expand_factory_because_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_shutdown_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);

			if(fat.get_production_scale() < 0.05) {
				frame = 0;
			} else {
				frame = 1;
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		/*if(parent) {	TODO - We want to open/close the factory if we press this button, depending on if its closed/opened respectively
		    Cyto::Any payload = dcon::factory_id{};
		    parent->impl_get(state, payload);
		    auto fid = any_cast<dcon::factory_id>(payload);
		    auto fat = dcon::fatten(state.world, fid);
		}*/
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);
		auto fat = dcon::fatten(state.world, fid);

		auto box = text::open_layout_box(contents, 0);
		if(fat.get_production_scale() < 0.05) {
			text::localised_format_box(state, contents, box, std::string_view("production_open_factory_tooltip"));
		} else {
			text::localised_format_box(state, contents, box, std::string_view("production_close_factory_tooltip"));
		}

		text::close_layout_box(contents, box);
	}
};

class factory_subsidise_button : public button_element_base { // Got a problem with mixed variants? too bad, Vic2 does same thing
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			if(fat.get_subsidized()) { // TODO - we want to check if the player can *even* subside the factory, the tooltip function
				                       // details this afaik
				if(command::can_change_factory_settings(state, n, fid, uint8_t(economy::factory_priority(state, fid)), false)) {
					command::change_factory_settings(state, n, fid, uint8_t(economy::factory_priority(state, fid)), false);
					frame = 0;
				}
			} else {
				if(command::can_change_factory_settings(state, n, fid, uint8_t(economy::factory_priority(state, fid)), true)) {
					command::change_factory_settings(state, n, fid, uint8_t(economy::factory_priority(state, fid)), true);
					frame = 1;
				}
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			if(dcon::fatten(state.world, fid).get_subsidized()) {
				text::localised_format_box(state, contents, box, std::string_view("production_cancel_subsidies"));
			} else {
				// TODO - check if the ruling party likes subsidising parties, if it does then allow subisdising otherwise dont
				// text::localised_format_box(state, contents, box, std::string_view("production_not_allowed_to_subsidise_tooltip"));
				text::localised_format_box(state, contents, box, std::string_view("production_allowed_to_subsidise_tooltip"));
			}
			text::add_divider_to_layout_box(state, contents, box);
			text::localised_format_box(state, contents, box, std::string_view("production_subsidies_desc"));
			text::close_layout_box(contents, box);
		}
	}
};

class factory_delete_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			disabled = !command::can_delete_factory(state, n, fid);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			command::delete_factory(state, n, fid);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::close_layout_box(contents, box);
	}
};

struct production_factory_slot_data {
	std::variant<dcon::factory_id, economy::upgraded_factory, economy::new_factory> data;
	size_t index = 0;
};

class factory_build_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = economy::new_factory{};
			parent->impl_get(state, payload);
			progress = any_cast<economy::new_factory>(payload).progress;
		}
	}
};

class factory_upgrade_progress_bar : public progress_bar {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = economy::upgraded_factory{};
			parent->impl_get(state, payload);
			progress = any_cast<economy::upgraded_factory>(payload).progress;
		}
	}
};

class production_factory_info : public window_element_base {
	image_element_base* output_icon = nullptr;
	image_element_base* input_icons[economy::commodity_set::set_size] = {nullptr};
	image_element_base* input_lack_icons[economy::commodity_set::set_size] = {nullptr};
	std::vector<element_base*> factory_elements;
	std::vector<element_base*> upgrade_elements;
	std::vector<element_base*> build_elements;
	std::vector<element_base*> closed_elements;

public:
	uint8_t index = 0; // from 0 to int32_t(state.defines.factories_per_state)

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "prod_factory_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "level") {
			auto ptr = make_element_by_type<factory_level_text>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "income") {
			auto ptr = make_element_by_type<factory_profit_text>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "income_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "output") {
			auto ptr = make_element_by_type<commodity_factory_image>(state, id);
			output_icon = ptr.get();
			return ptr;
		} else if(name == "closed_overlay") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			closed_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "factory_closed_text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			closed_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "prod_factory_inprogress_bg") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			build_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "build_factory_progress") {
			auto ptr = make_element_by_type<factory_build_progress_bar>(state, id);
			build_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "prod_cancel_progress") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			build_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "upgrade_factory_progress") {
			auto ptr = make_element_by_type<factory_upgrade_progress_bar>(state, id);
			upgrade_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "progress_overlay_16_64") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			upgrade_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "employment_ratio") {
			auto ptr = make_element_by_type<factory_employment_image>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "priority") {
			auto ptr = make_element_by_type<factory_priority_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "upgrade") {
			auto ptr = make_element_by_type<factory_upgrade_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "subsidise") {
			auto ptr = make_element_by_type<factory_subsidise_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "delete_factory") {
			auto ptr = make_element_by_type<factory_delete_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
		} else if(name == "open_close") {
			auto ptr = make_element_by_type<factory_shutdown_button>(state, id);
			factory_elements.push_back(ptr.get());
			return ptr;
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

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = production_factory_slot_data{dcon::factory_id{}, index};
			parent->impl_get(state, payload);
			auto content = any_cast<production_factory_slot_data>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			dcon::factory_type_fat_id fat_btid(state.world, dcon::factory_type_id{});
			if(std::holds_alternative<economy::new_factory>(content.data)) {
				// New factory
				economy::new_factory nf = std::get<economy::new_factory>(content.data);
				fat_btid = dcon::fatten(state.world, nf.type);

				for(auto const& e : factory_elements)
					e->set_visible(state, false);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, false);
				for(auto const& e : build_elements)
					e->set_visible(state, true);
				for(auto const& e : closed_elements)
					e->set_visible(state, false);
			} else if(std::holds_alternative<economy::upgraded_factory>(content.data)) {
				// Upgrade
				economy::upgraded_factory uf = std::get<economy::upgraded_factory>(content.data);
				fat_btid = dcon::fatten(state.world, uf.type);

				for(auto const& e : factory_elements)
					e->set_visible(state, true);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, true);
				for(auto const& e : build_elements)
					e->set_visible(state, false);
				for(auto const& e : closed_elements)
					e->set_visible(state, false);
			} else if(std::holds_alternative<dcon::factory_id>(content.data)) {
				// "Normal" factory, not being upgraded or built
				dcon::factory_id fid = std::get<dcon::factory_id>(content.data);
				fat_btid = state.world.factory_get_building_type(fid);

				bool is_closed = dcon::fatten(state.world, fid).get_production_scale() < 0.05;
				for(auto const& e : factory_elements)
					e->set_visible(state, true);
				for(auto const& e : upgrade_elements)
					e->set_visible(state, false);
				for(auto const& e : build_elements)
					e->set_visible(state, false);
				for(auto const& e : closed_elements)
					e->set_visible(state, is_closed);
			}

			{
				dcon::commodity_id cid = fat_btid.get_output().id;
				output_icon->frame = int32_t(state.world.commodity_get_icon(cid));
			}
			// Commodity set
			auto cset = fat_btid.get_inputs();
			for(uint32_t i = 0; i < economy::commodity_set::set_size; ++i)
				if(input_icons[size_t(i)]) {
					dcon::commodity_id cid = cset.commodity_type[size_t(i)];
					input_icons[size_t(i)]->frame = int32_t(state.world.commodity_get_icon(cid));
					bool is_lack = cid != dcon::commodity_id{} ? state.world.nation_get_demand_satisfaction(n, cid) < 0.5f : false;
					input_lack_icons[size_t(i)]->set_visible(state, is_lack);
				}
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(parent) {
			Cyto::Any p_payload = production_factory_slot_data{dcon::factory_id{}, index};
			parent->impl_get(state, p_payload);
			auto content = any_cast<production_factory_slot_data>(p_payload);
			if(payload.holds_type<dcon::factory_id>()) {
				if(std::holds_alternative<dcon::factory_id>(content.data))
					payload.emplace<dcon::factory_id>(std::get<dcon::factory_id>(content.data));
				return message_result::consumed;
			} else if(payload.holds_type<economy::upgraded_factory>()) {
				if(std::holds_alternative<economy::upgraded_factory>(content.data))
					payload.emplace<economy::upgraded_factory>(std::get<economy::upgraded_factory>(content.data));
				return message_result::consumed;
			} else if(payload.holds_type<economy::new_factory>()) {
				if(std::holds_alternative<economy::new_factory>(content.data))
					payload.emplace<economy::new_factory>(std::get<economy::new_factory>(content.data));
				return message_result::consumed;
			}
		}
		return message_result::unseen;
	}
};

class production_factory_info_bounds_window : public window_element_base {
	std::vector<element_base*> infos;
	std::vector<std::variant<dcon::factory_id, economy::upgraded_factory, economy::new_factory>> factories;

	bool get_filter(sys::state& state, dcon::commodity_id cid) {
		Cyto::Any payload = commodity_filter_query_data{cid, false};
		parent->impl_get(state, payload);
		auto content = any_cast<commodity_filter_query_data>(payload);
		return content.filter;
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		factories.resize(size_t(state.defines.factories_per_state));
		// Create factory slots for each of the provinces
		for(uint8_t factory_index = 0; factory_index < uint8_t(state.defines.factories_per_state); ++factory_index) {
			auto ptr =
			    make_element_by_type<production_factory_info>(state, state.ui_state.defs_by_name.find("factory_info")->second.definition);
			ptr->index = factory_index;
			ptr->base_data.position.x = factory_index * ptr->base_data.size.x;
			infos.push_back(ptr.get());
			add_child_to_front(std::move(ptr));
		}
	}

	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::state_instance_id{};
		parent->impl_get(state, payload);
		auto state_id = any_cast<dcon::state_instance_id>(payload);

		for(auto const c : infos)
			c->set_visible(state, false);

		std::vector<bool> visited_types(state.world.factory_type_size(), false);
		size_t index = 0;
		// First, the new factories are taken into account
		economy::for_each_new_factory(state, state_id, [&](economy::new_factory const& nf) {
			dcon::commodity_id cid = state.world.factory_type_get_output(nf.type).id;
			if(!visited_types[nf.type.index()] && get_filter(state, cid)) {
				factories[index] = nf;
				visited_types[nf.type.index()] = true;
				infos[index]->set_visible(state, true);
				++index;
			}
		});
		// Then, the factories being upgraded
		economy::for_each_upgraded_factory(state, state_id, [&](economy::upgraded_factory const& uf) {
			dcon::commodity_id cid = state.world.factory_type_get_output(uf.type).id;
			if(!visited_types[uf.type.index()] && get_filter(state, cid)) {
				factories[index] = uf;
				visited_types[uf.type.index()] = true;
				infos[index]->set_visible(state, true);
				++index;
			}
		});
		// Finally, factories "doing nothing" are accounted for
		province::for_each_province_in_state_instance(state, state_id, [&](dcon::province_id pid) {
			dcon::fatten(state.world, pid).for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				dcon::factory_id fid = state.world.factory_location_get_factory(flid);
				dcon::factory_type_id ftid = state.world.factory_get_building_type(fid);
				dcon::commodity_id cid = state.world.factory_type_get_output(ftid).id;
				if(!visited_types[ftid.index()] && get_filter(state, cid)) {
					factories[index] = fid;
					visited_types[ftid.index()] = true;
					infos[index]->set_visible(state, true);
					++index;
				}
			});
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_factory_slot_data>()) {
			auto content = any_cast<production_factory_slot_data>(payload);
			content.data = factories[content.index];
			payload.emplace<production_factory_slot_data>(content);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_build_new_factory : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto sid = any_cast<dcon::state_instance_id>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			bool can_build = false;
			state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
				can_build = can_build || command::can_begin_factory_building_construction(state, n, sid, ftid, false);
			});
			disabled = !can_build;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto sid = any_cast<dcon::state_instance_id>(payload);
			Cyto::Any s_payload = production_selection_wrapper{sid, true, xy_pair{0, 0}};
			parent->impl_get(state, s_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, std::string_view("OwO *notices you building new factory*"));
		text::close_layout_box(contents, box);
	}
};

class production_national_focus_button : public button_element_base {
	int32_t get_icon_frame(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			return bool(state.world.state_instance_get_owner_focus(content).id)
			           ? state.world.state_instance_get_owner_focus(content).get_icon() - 1
			           : 0;
		}
		return 0;
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			disabled = true;
			state.world.for_each_national_focus([&](dcon::national_focus_id nfid) {
				disabled = command::can_set_national_focus(state, n, content, nfid) ? false : disabled;
			});
			frame = get_icon_frame(state);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			Cyto::Any s_payload = production_selection_wrapper{any_cast<dcon::state_instance_id>(payload), false, base_data.position};
			parent->impl_get(state, s_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override { return tooltip_behavior::variable_tooltip; }

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::state_instance_id>(payload);

			dcon::national_focus_fat_id focus = state.world.state_instance_get_owner_focus(content);
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, focus.get_name());
			text::close_layout_box(contents, box);
		}
	}
};

class production_state_info : public listbox_row_element_base<dcon::state_instance_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "state_focus") {
			return make_element_by_type<production_national_focus_button>(state, id);
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
};

class production_state_listbox : public listbox_element_base<production_state_info, dcon::state_instance_id> {
protected:
	std::string_view get_row_element_name() override { return "state_info"; }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		if(parent) {
			Cyto::Any payload = bool{};
			parent->impl_get(state, payload);
			auto show_empty = any_cast<bool>(payload);

			Cyto::Any n_payload = dcon::nation_id{};
			parent->impl_get(state, n_payload);
			dcon::nation_id n = any_cast<dcon::nation_id>(n_payload);

			for(auto const fat_id : state.world.nation_get_state_ownership(n)) {
				if(show_empty) {
					row_contents.push_back(fat_id.get_state());
				} else if(economy::has_factory(state, fat_id.get_state().id)) {
					// Then account for factories **hidden** by the filter from goods...
					size_t count = 0;
					province::for_each_province_in_state_instance(state, fat_id.get_state(), [&](dcon::province_id pid) {
						auto ffact_id = dcon::fatten(state.world, pid);
						ffact_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
							auto fid = state.world.factory_location_get_factory(flid);
							Cyto::Any payload = commodity_filter_query_data{
							    state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false};
							parent->impl_get(state, payload);
							auto content = any_cast<commodity_filter_query_data>(payload);
							count += content.filter ? 1 : 0;
						});
					});

					if(count > 0)
						row_contents.push_back(fat_id.get_state());
				}
			}
		}
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
	commodity_player_production_text* good_output_total = nullptr;
	image_element_base* good_not_producing_overlay = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "output_factory") {
			return make_element_by_type<commodity_factory_image>(state, id);
		} else if(name == "output_total") {
			auto ptr = make_element_by_type<commodity_player_production_text>(state, id);
			good_output_total = ptr.get();
			return ptr;
		} else if(name == "prod_producing_not_total") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			good_not_producing_overlay = ptr.get();
			return ptr;
		} else if(name == "pop_factory") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = int32_t(dcon::fatten(state.world, state.culture_definitions.primary_factory_worker).get_sprite() - 1);
			return ptr;
		} else if(name == "pop_factory2") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			ptr->frame = int32_t(dcon::fatten(state.world, state.culture_definitions.secondary_factory_worker).get_sprite() - 1);
			return ptr;
		} else if(name == "output") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "output2") {
			return make_element_by_type<simple_text_element_base>(state, id);
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
	bool show_empty_states = true;
	bool* show_output_commodity;

	production_state_listbox* state_listbox = nullptr;
	element_base* nf_win = nullptr;
	element_base* build_win = nullptr;
	element_base* project_window = nullptr;
	element_base* foreign_invest_win = nullptr;

	sys::commodity_group curr_commodity_group{};
	dcon::state_instance_id focus_state{};
	xy_pair base_commodity_offset{33, 50};
	xy_pair commodity_offset{33, 50};

	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
	std::vector<bool> commodity_filters;
	bool open_foreign_invest = false;

	void set_visible_vector_elements(sys::state& state, std::vector<element_base*>& elements, bool v) noexcept {
		for(auto element : elements)
			element->set_visible(state, v);
	}

	void hide_sub_windows(sys::state& state) noexcept {
		set_visible_vector_elements(state, factory_elements, false);
		set_visible_vector_elements(state, investment_brow_elements, false);
		set_visible_vector_elements(state, project_elements, false);
		set_visible_vector_elements(state, good_elements, false);
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		// All filters enabled by default
		commodity_filters.resize(state.world.commodity_size(), true);

		for(curr_commodity_group = sys::commodity_group::military_goods; curr_commodity_group != sys::commodity_group::count;
		    curr_commodity_group = static_cast<sys::commodity_group>(uint8_t(curr_commodity_group) + 1)) {
			commodity_offset.x = base_commodity_offset.x;

			// Place legend for this category...
			auto ptr = make_element_by_type<production_goods_category_name>(
			    state, state.ui_state.defs_by_name.find("production_goods_name")->second.definition);
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
				if(sys::commodity_group(state.world.commodity_get_commodity_group(id)) != curr_commodity_group || !bool(id))
					return;

				auto info_ptr = make_element_by_type<production_good_info>(
				    state, state.ui_state.defs_by_name.find("production_info")->second.definition);
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

		{
			auto ptr = make_element_by_type<national_focus_window>(state, "state_focus_window");
			ptr->set_visible(state, false);
			nf_win = ptr.get();
			add_child_to_front(std::move(ptr));
		}

		auto win =
		    make_element_by_type<factory_build_window>(state, state.ui_state.defs_by_name.find("build_factory")->second.definition);
		build_win = win.get();
		add_child_to_front(std::move(win));

		auto win2 = make_element_by_type<project_investment_window>(
		    state, state.ui_state.defs_by_name.find("invest_project_window")->second.definition);
		win2->set_visible(state, false);
		project_window = win2.get();
		add_child_to_front(std::move(win2));

		show_output_commodity = new bool[state.world.commodity_size()];

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
		} else if(name == "invest_buttons") {
			auto ptr = make_element_by_type<production_foreign_investment_window>(state, id);
			foreign_invest_win = ptr.get();
			investment_brow_elements.push_back(ptr.get());
			ptr->set_visible(state, false);
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
				foreign_invest_win->set_visible(state, open_foreign_invest);
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
		} else if(payload.holds_type<dcon::state_instance_id>()) {
			payload.emplace<dcon::state_instance_id>(focus_state);
			return message_result::consumed;
		} else if(payload.holds_type<production_selection_wrapper>()) {
			auto data = any_cast<production_selection_wrapper>(payload);
			focus_state = data.data;
			if(data.is_build) {
				build_win->set_visible(state, true);
				move_child_to_front(build_win);
			} else {
				nf_win->set_visible(state, true);
				nf_win->base_data.position = data.focus_pos;
				move_child_to_front(nf_win);
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<bool>()) {
			payload.emplace<bool>(show_empty_states);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			show_empty_states = any_cast<element_selection_wrapper<bool>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<commodity_filter_query_data>()) {
			auto content = any_cast<commodity_filter_query_data>(payload);
			content.filter = commodity_filters[content.cid.index()];
			payload.emplace<commodity_filter_query_data>(content);
			return message_result::consumed;
		} else if(payload.holds_type<commodity_filter_toggle_data>()) {
			auto content = any_cast<commodity_filter_toggle_data>(payload);
			commodity_filters[content.data.index()] = !commodity_filters[content.data.index()];
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<production_action>>()) {
			auto content = any_cast<element_selection_wrapper<production_action>>(payload).data;
			switch(content) {
			case production_action::subsidise_all:
				break;
			case production_action::unsubsidise_all:
				break;
			case production_action::filter_select_all:
				for(uint32_t i = 0; i < commodity_filters.size(); i++) {
					commodity_filters[i] = true;
				}
				break;
			case production_action::filter_deselect_all:
				for(uint32_t i = 0; i < commodity_filters.size(); i++) {
					commodity_filters[i] = false;
				}
				break;
			case production_action::open_all:
				break;
			case production_action::close_all:
				break;
			case production_action::investment_window:
				project_window->is_visible() ? project_window->set_visible(state, false) : project_window->set_visible(state, true);
				break;
			default:
				break;
			}
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<dcon::nation_id>>()) {
			open_foreign_invest = true;
			foreign_invest_win->set_visible(state, true);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
