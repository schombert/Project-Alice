#pragma once

#include "gui_element_types.hpp"
#include "gui_factory_buttons_window.hpp"
#include "gui_invest_brow_window.hpp"
#include "gui_invest_buttons_window.hpp"
#include "gui_pop_sort_buttons_window.hpp"
#include "gui_commodity_filters_window.hpp"
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

struct production_selection_wrapper {
	dcon::state_instance_id data{};
	bool is_build = false;
	xy_pair focus_pos{ 0, 0 };
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

	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);
			frame = economy::factory_priority(state, fid);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);
			switch (economy::factory_priority(state, fid)) {
				case 0:
					command::change_factory_settings(state, state.local_player_nation, fid, 1, fat.get_subsidized());
					break;
				case 1:
					command::change_factory_settings(state, state.local_player_nation, fid, 2, fat.get_subsidized());
					break;
				case 2:
					command::change_factory_settings(state, state.local_player_nation, fid, 3, fat.get_subsidized());
					break;
				case 3:
					command::change_factory_settings(state, state.local_player_nation, fid, 0, fat.get_subsidized());
					break;
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);
		auto fat = dcon::fatten(state.world, fid);

		auto box = text::open_layout_box(contents, 0);
		// TODO - check if we can change the priority,
		// 	- This appears to depend only on if the ruling party is kosher with it
		text::localised_format_box(state, contents, box, std::string_view("production_allowed_to_change_prio_tooltip"));
		// Why pdx, why must the diplomacy priority be used instead of having a seperate priority for prod. ?
		// just why
		switch (economy::factory_priority(state, fid)) {
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

		//text::localised_format_box(state, contents, box, std::string_view("production_not_allowed_to_change_prio_tooltip"));
		text::add_divider_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, std::string_view("production_prio_factory_desc_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_upgrade_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);
		auto fat = dcon::fatten(state.world, fid);

		Cyto::Any payload1 = dcon::state_instance_id{};
		parent->impl_get(state, payload1);
		auto sid = any_cast<dcon::state_instance_id>(payload1);

		command::begin_factory_building_construction(state, state.local_player_nation, sid, fat.get_building_type().id, true);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("production_expand_factory_tooltip"));
		// TODO - check if we can expand the factory
		// What we need to check:
		// 	- Ruling party opinion on expanding factories
		// 	- if we got enough money to expand
		// 	- if our level is < 99
		text::localised_format_box(state, contents, box, std::string_view("production_expand_factory_because_tooltip"));
		//text::localised_format_box(state, contents, box, std::string_view("production_not_expand_factory_because_tooltip"));
		text::close_layout_box(contents, box);
	}
};

class factory_shutdown_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
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

	void button_action(sys::state& state) noexcept override {
		/*if(parent) {	TODO - We want to open/close the factory if we press this button, depending on if its closed/opened respectively
			Cyto::Any payload = dcon::factory_id{};
			parent->impl_get(state, payload);
			auto fid = any_cast<dcon::factory_id>(payload);
			auto fat = dcon::fatten(state.world, fid);
		}*/
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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

class factory_subsidise_button : public button_element_base {	// Got a problem with mixed variants? too bad, Vic2 does same thing
public:
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);
		auto fat = dcon::fatten(state.world, fid);

		if(fat.get_subsidized()) {	// TODO - we want to check if the player can *even* subside the factory, the tooltip function details this afaik
			if(command::can_change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), false)) {
				command::change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), false);
				frame = 0;
			}
		} else {
			if(command::can_change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true)) {
				command::change_factory_settings(state, state.local_player_nation, fid, uint8_t(economy::factory_priority(state, fid)), true);
				frame = 1;
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
				//text::localised_format_box(state, contents, box, std::string_view("production_not_allowed_to_subsidise_tooltip"));
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
	void button_action(sys::state& state) noexcept override {
		Cyto::Any payload = dcon::factory_id{};
		parent->impl_get(state, payload);
		auto fid = any_cast<dcon::factory_id>(payload);

		command::delete_factory(state, state.local_player_nation, fid);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::close_layout_box(contents, box);
	}
};

struct production_factory_slot_data {
	dcon::factory_id fid{};
	size_t index = 0;
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
	factory_delete_button* delete_factory = nullptr;

	dcon::factory_id get_factory(sys::state& state) noexcept {
		if(parent) {
			Cyto::Any payload = production_factory_slot_data{ dcon::factory_id{}, index };
			parent->impl_get(state, payload);
			auto content = any_cast<production_factory_slot_data>(payload);
			return content.fid;
		}
		return dcon::factory_id{};
	}
public:
	uint8_t index = 0; // from 0 to 8

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
			return make_element_by_type<factory_priority_button>(state, id);
		} else if(name == "upgrade") {
			return make_element_by_type<factory_upgrade_button>(state, id);
		} else if(name == "subsidise") {
			return make_element_by_type<factory_subsidise_button>(state, id);
		} else if(name == "delete_factory") {
			auto ptr = make_element_by_type<factory_delete_button>(state, id);
			delete_factory = ptr.get();
			return ptr;
		} else if(name == "open_close") {
			return make_element_by_type<factory_shutdown_button>(state, id);
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
		dcon::factory_id fid = get_factory(state);

		bool is_closed = dcon::fatten(state.world, fid).get_production_scale() < 0.05;
		closed_overlay->set_visible(state, is_closed);
		closed_text->set_visible(state, is_closed);
		is_closed ? delete_factory->set_visible(state, is_closed) : delete_factory->set_visible(state, is_closed);

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

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::factory_id>()) {
			payload.emplace<dcon::factory_id>(get_factory(state));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class production_factory_info_bounds_window : public window_element_base {
	std::vector<element_base*> infos;
	std::vector<dcon::factory_id> factories;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		// TODO: Unhardcode the factories
		factories.resize(8);
		// Create factory slots for each of the provinces
		for(uint8_t factory_index = 0; factory_index < 8; ++factory_index) {
			auto ptr = make_element_by_type<production_factory_info>(state, state.ui_state.defs_by_name.find("factory_info")->second.definition);
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

		for(const auto c : infos)
			c->set_visible(state, false);
		size_t index = 0;
		province::for_each_province_in_state_instance(state, state_id, [&](dcon::province_id pid) {
			auto fat_id = dcon::fatten(state.world, pid);
			fat_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
				auto fid = state.world.factory_location_get_factory(flid);

				// Pass to filters...
				Cyto::Any payload = commodity_filter_query_data{ state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false };
				parent->impl_get(state, payload);
				auto content = any_cast<commodity_filter_query_data>(payload);

				if(content.filter) {
					factories[index] = fid;
					infos[index]->set_visible(state, bool(fid));
					++index;
				}
			});
		});
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<production_factory_slot_data>()) {
			auto content = any_cast<production_factory_slot_data>(payload);
			payload.emplace<production_factory_slot_data>(production_factory_slot_data{ factories[content.index], content.index });
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

			bool can_build = false;
			state.world.for_each_factory_type([&](dcon::factory_type_id ftid) {
				can_build = can_build || command::can_begin_factory_building_construction(state, state.local_player_nation, sid, ftid, false);
			});
			disabled = !can_build;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			auto sid = any_cast<dcon::state_instance_id>(payload);
			Cyto::Any s_payload = production_selection_wrapper{ sid, true, xy_pair{ 0, 0 } };
			parent->impl_get(state, s_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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

			return bool(state.world.state_instance_get_owner_focus(content).id) ? state.world.state_instance_get_owner_focus(content).get_icon() - 1 : 0;
		}
		return 0;
	}
public:
	void on_update(sys::state& state) noexcept override {
		frame = get_icon_frame(state);
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::state_instance_id{};
			parent->impl_get(state, payload);
			Cyto::Any s_payload = production_selection_wrapper{ any_cast<dcon::state_instance_id>(payload), false, base_data.position };
			parent->impl_get(state, s_payload);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

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
	std::string_view get_row_element_name() override {
        return "state_info";
    }
public:
	void on_update(sys::state& state) noexcept override {
		Cyto::Any payload = bool{};
		parent->impl_get(state, payload);
		auto show_empty = any_cast<bool>(payload);

		row_contents.clear();
		for(const auto fat_id : state.world.nation_get_state_ownership(state.local_player_nation)) {
			if(show_empty) {
				row_contents.push_back(fat_id.get_state());
			} else if(economy::has_factory(state, fat_id.get_state().id)) {
				// Then account for factories **hidden** by the filter from goods...
				size_t count = 0;
				province::for_each_province_in_state_instance(state, fat_id.get_state(), [&](dcon::province_id pid) {
					auto ffact_id = dcon::fatten(state.world, pid);
					ffact_id.for_each_factory_location_as_province([&](dcon::factory_location_id flid) {
						auto fid = state.world.factory_location_get_factory(flid);
						Cyto::Any payload = commodity_filter_query_data{ state.world.factory_type_get_output(state.world.factory_get_building_type(fid)).id, false };
						parent->impl_get(state, payload);
						auto content = any_cast<commodity_filter_query_data>(payload);
						count += content.filter ? 1 : 0;
					});
				});

				if(count > 0)
					row_contents.push_back(fat_id.get_state());
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
	bool *show_output_commodity;

	production_state_listbox* state_listbox = nullptr;
	element_base* nf_win = nullptr;
	element_base* build_win = nullptr;

	sys::commodity_group curr_commodity_group{};
	dcon::state_instance_id focus_state{};
	xy_pair base_commodity_offset{ 33, 50 };
	xy_pair commodity_offset{ 33, 50 };

	std::vector<element_base*> factory_elements;
	std::vector<element_base*> investment_brow_elements;
	std::vector<element_base*> investment_button_elements;
	std::vector<element_base*> project_elements;
	std::vector<element_base*> good_elements;
	std::vector<bool> commodity_filters;

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

		// All filters enabled by default
		commodity_filters.resize(state.world.commodity_size(), true);

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

		{
			auto ptr = make_element_by_type<national_focus_window>(state, "state_focus_window");
			ptr->set_visible(state, false);
			nf_win = ptr.get();
			add_child_to_front(std::move(ptr));
		}

		auto win = make_element_by_type<factory_build_window>(state, state.ui_state.defs_by_name.find("build_factory")->second.definition);
		build_win = win.get();
		add_child_to_front(std::move(win));

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
		} else if(payload.holds_type<element_selection_wrapper<factory_all_actions>>()) {
			auto content = any_cast<element_selection_wrapper<factory_all_actions>>(payload).data;
			switch(content) {
				case factory_all_actions::subsidise_all:
					break;
				case factory_all_actions::unsubsidise_all:
					break;
				case factory_all_actions::filter_select_all:
					for(uint32_t i = 0; i < commodity_filters.size(); i++) {
						commodity_filters[i] = true;
					}
					break;
				case factory_all_actions::filter_deselect_all:
					for(uint32_t i = 0; i < commodity_filters.size(); i++) {
						commodity_filters[i] = false;
					}
					break;
				case factory_all_actions::open_all:
					break;
				case factory_all_actions::close_all:
					break;
				default:
					break;
			}
			impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
