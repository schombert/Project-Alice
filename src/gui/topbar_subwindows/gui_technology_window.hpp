#pragma once

#include "gui_element_types.hpp"
#include "triggers.hpp"

namespace ui {

static void technology_description(element_base& element, sys::state& state, text::layout_base& contents, dcon::technology_id tech_id) noexcept {
	auto tech_fat_id = dcon::fatten(state.world, tech_id);
	auto mod_id = tech_fat_id.get_modifier().id;
	if(bool(mod_id))
		modifier_description(state, contents, mod_id);

	auto increase_naval_base = tech_fat_id.get_increase_naval_base();
	if(increase_naval_base) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "naval_base"), text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "tech_max_level"), text::text_color::white);
		text::add_to_layout_box(contents, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "+1"), text::text_color::green);
		text::close_layout_box(contents, box);
	}

	auto increase_railroad = tech_fat_id.get_increase_railroad();
	if(increase_railroad) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "railroad"), text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "tech_max_level"), text::text_color::white);
		text::add_to_layout_box(contents, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "+1"), text::text_color::green);
		text::close_layout_box(contents, box);
	}

	auto increase_fort = tech_fat_id.get_increase_fort();
	if(increase_fort) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "fort"), text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "tech_max_level"), text::text_color::white);
		text::add_to_layout_box(contents, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "+1"), text::text_color::green);
		text::close_layout_box(contents, box);
	}

	auto activate_unit_description = [&](dcon::unit_type_id id) {
		if(tech_fat_id.get_activate_unit(id)) {
			auto unit_type_name = state.military_definitions.unit_base_definitions[id].name;

			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "enable_unit_tech"), text::text_color::white);
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, unit_type_name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
	};
	for(uint8_t unit_type_i = 0; unit_type_i < tech_fat_id.get_activate_unit_size(); ++unit_type_i) {
		dcon::unit_type_id id(unit_type_i);
		activate_unit_description(id);
	}

	auto activate_factory_description = [&](dcon::factory_type_id id) {
		if(tech_fat_id.get_activate_building(id)) {
			auto factory_type_fat_id = dcon::fatten(state.world, id);

			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "enable_building_tech"), text::text_color::white);
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, factory_type_fat_id.get_name()), text::text_color::yellow);
			text::close_layout_box(contents, box);

			box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "activate_goods"), text::text_color::white);
			text::add_space_to_layout_box(contents, state, box);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, factory_type_fat_id.get_output().get_name()), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
	};
	for(uint8_t building_type_i = 0; building_type_i < tech_fat_id.get_activate_building_size(); ++building_type_i) {
		dcon::factory_type_id id(building_type_i);
		activate_factory_description(id);
	}

	auto commodity_mod_description = [&](const auto& list, std::string_view locale_base_name, std::string_view locale_farm_base_name) {
		for(const auto mod : list) {
			auto box = text::open_layout_box(contents, 0);
			auto name = state.world.commodity_get_name(mod.type);
			if(bool(name)) {
				text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), text::text_color::white);
				text::add_space_to_layout_box(contents, state, box);
			}
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, state.world.commodity_get_is_mine(mod.type) ? locale_base_name : locale_farm_base_name), text::text_color::white);
			text::add_to_layout_box(contents, state, box, std::string{":"}, text::text_color::white);
			text::add_space_to_layout_box(contents, state, box);
			auto color = mod.amount > 0.f ? text::text_color::green : text::text_color::red;
			text::add_to_layout_box(contents, state, box, (mod.amount > 0.f ? "+" : "") + text::format_percentage(mod.amount, 1), color);
			text::close_layout_box(contents, box);
		}
	};
	commodity_mod_description(tech_fat_id.get_factory_goods_output(), "tech_output", "tech_output");
	commodity_mod_description(tech_fat_id.get_rgo_goods_output(), "tech_mine_output", "tech_farm_output");
	commodity_mod_description(tech_fat_id.get_rgo_size(), "tech_mine_size", "tech_farm_size");

	auto colonial_points = tech_fat_id.get_colonial_points();
	if(colonial_points != 0) {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "colonial_points_tech"), text::text_color::white);
		text::add_to_layout_box(contents, state, box, std::string_view{":"}, text::text_color::white);
		text::add_space_to_layout_box(contents, state, box);
		auto color = colonial_points > 0.f ? text::text_color::green : text::text_color::red;
		text::add_to_layout_box(contents, state, box, (colonial_points > 0.f ? "+" : "") + text::prettify(int64_t(colonial_points)), color);
		text::close_layout_box(contents, box);
	}
}

class technology_folder_tab_sub_button : public checkbox_button {
public:
	culture::tech_category category{};
	bool is_active(sys::state& state) noexcept final;
	void button_action(sys::state& state) noexcept final;

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		state.world.for_each_technology([&](dcon::technology_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			if(state.culture_definitions.tech_folders[fat_id.get_folder_index()].category != category)
				return;
			bool discovered = state.world.nation_get_active_technologies(state.local_player_nation, id);
			auto color = discovered ? text::text_color::green : text::text_color::red;
			auto name = fat_id.get_name();
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), color);
			text::close_layout_box(contents, box);
		});
	}
};

class technology_tab_progress : public progress_bar {
public:
	culture::tech_category category{};
	void on_update(sys::state& state) noexcept override {
		auto discovered = 0;
		auto total = 0;
		state.world.for_each_technology([&](dcon::technology_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			if(state.culture_definitions.tech_folders[fat_id.get_folder_index()].category == category) {
				discovered += state.world.nation_get_active_technologies(state.local_player_nation, id) ? 1 : 0;
				++total;
			}
		});
		progress = bool(total) && bool(discovered) ? float(discovered) / float(total) : 0.f;
	}
};

class technology_num_discovered_text : public simple_text_element_base {
	std::string get_text(sys::state& state) noexcept {
		auto discovered = 0;
		auto total = 0;
		state.world.for_each_technology([&](dcon::technology_id id) {
			auto fat_id = dcon::fatten(state.world, id);
			if(state.culture_definitions.tech_folders[fat_id.get_folder_index()].category == category) {
				discovered += state.world.nation_get_active_technologies(state.local_player_nation, id) ? 1 : 0;
				++total;
			}
		});
		return text::produce_simple_string(state, std::to_string(discovered) + "/" + std::to_string(total));
	}

public:
	culture::tech_category category{};
	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

class technology_folder_tab_button : public window_element_base {
	image_element_base* folder_icon = nullptr;
	simple_text_element_base* folder_name = nullptr;
	technology_folder_tab_sub_button* folder_button = nullptr;
	technology_tab_progress* folder_progress = nullptr;
	technology_num_discovered_text* folder_num_discovered = nullptr;

public:
	culture::tech_category category{};
	void set_category(sys::state& state, culture::tech_category new_category) {
		folder_button->category = category = new_category;

		folder_icon->frame = static_cast<int32_t>(category);

		std::string str{};
		switch(category) {
		case culture::tech_category::army:
			str += text::produce_simple_string(state, "army_tech");
			break;
		case culture::tech_category::navy:
			str += text::produce_simple_string(state, "navy_tech");
			break;
		case culture::tech_category::commerce:
			str += text::produce_simple_string(state, "commerce_tech");
			break;
		case culture::tech_category::culture:
			str += text::produce_simple_string(state, "culture_tech");
			break;
		case culture::tech_category::industry:
			str += text::produce_simple_string(state, "industry_tech");
			break;
		default:
			break;
		}
		folder_name->set_text(state, str);

		folder_num_discovered->category = folder_progress->category = category;
		folder_progress->on_update(state);
		folder_num_discovered->on_update(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "folder_button") {
			auto ptr = make_element_by_type<technology_folder_tab_sub_button>(state, id);
			folder_button = ptr.get();
			return ptr;
		} else if(name == "folder_icon") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			folder_icon = ptr.get();
			return ptr;
		} else if(name == "folder_category") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			folder_name = ptr.get();
			return ptr;
		} else if(name == "folder_progress") {
			auto ptr = make_element_by_type<technology_tab_progress>(state, id);
			folder_progress = ptr.get();
			return ptr;
		} else if(name == "folder_number_discovered") {
			auto ptr = make_element_by_type<technology_num_discovered_text>(state, id);
			folder_num_discovered = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}
};

class technology_research_progress_name_text : public simple_text_element_base {
	std::string get_text(sys::state& state) noexcept {
		auto tech_id = nations::current_research(state, state.local_player_nation);
		if(tech_id) {
			return text::get_name_as_string(state, dcon::fatten(state.world, tech_id));
		} else {
			return text::produce_simple_string(state, "tb_tech_no_current");
		}
	}

public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

class technology_research_progress_category_text : public simple_text_element_base {
	std::string get_text(sys::state& state) noexcept {
		auto tech_id = nations::current_research(state, state.local_player_nation);
		if(tech_id) {
			auto tech = dcon::fatten(state.world, tech_id);
			const auto& folder = state.culture_definitions.tech_folders[tech.get_folder_index()];

			std::string str{};
			str += text::produce_simple_string(state, folder.name);
			str += ", ";
			switch(folder.category) {
			case culture::tech_category::army:
				str += text::produce_simple_string(state, "army_tech");
				break;
			case culture::tech_category::navy:
				str += text::produce_simple_string(state, "navy_tech");
				break;
			case culture::tech_category::commerce:
				str += text::produce_simple_string(state, "commerce_tech");
				break;
			case culture::tech_category::culture:
				str += text::produce_simple_string(state, "culture_tech");
				break;
			case culture::tech_category::industry:
				str += text::produce_simple_string(state, "industry_tech");
				break;
			default:
				break;
			}
			return str;
		} else {
			return "";
		}
	}

public:
	void on_update(sys::state& state) noexcept override {
		set_text(state, get_text(state));
	}
};

struct technology_select_tech {
	dcon::technology_id tech_id;
};
class technology_item_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);
			if(state.ui_state.technology_subwindow) {
				Cyto::Any sl_payload = technology_select_tech{content};
				state.ui_state.technology_subwindow->impl_set(state, sl_payload);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			auto fat_id = dcon::fatten(state.world, content);
			auto name = fat_id.get_name();
			if(bool(name)) {
				auto box = text::open_layout_box(contents, 0);
				text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, name), text::text_color::yellow);
				text::close_layout_box(contents, box);
			}
			technology_description(*this, state, contents, content);
		}
	}
};

class technology_item_window : public window_element_base {
	technology_item_button* tech_button = nullptr;
	culture::tech_category category;

public:
	dcon::technology_id tech_id{};

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "start_research") {
			auto ptr = make_element_by_type<technology_item_button>(state, id);
			tech_button = ptr.get();
			return ptr;
		} else if(name == "tech_name") {
			return make_element_by_type<generic_name_text<dcon::technology_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		auto tech_fat_id = dcon::fatten(state.world, tech_id);
		if(state.world.nation_get_active_technologies(state.local_player_nation, tech_id)) {
			// Fully researched.
			tech_button->frame = 1;
		} else if(nations::current_research(state, state.local_player_nation) == tech_id) {
			// Research in progress.
			tech_button->frame = 0;
		} else if(command::can_start_research(state, state.local_player_nation, tech_id)) {
			// Can be researched.
			tech_button->frame = 2;
		} else {
			// Can not be researched yet.
			tech_button->frame = 3;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::technology_id>()) {
			payload.emplace<dcon::technology_id>(tech_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
	message_result set(sys::state& state, Cyto::Any& payload) noexcept override;
};

class invention_image : public opaque_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			frame = int32_t(state.world.invention_get_technology_type(content));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			auto category = static_cast<culture::tech_category>(state.world.invention_get_technology_type(content));
			std::string category_name{};
			switch(category) {
			case culture::tech_category::army:
				category_name = "army_tech";
				break;
			case culture::tech_category::navy:
				category_name = "navy_tech";
				break;
			case culture::tech_category::commerce:
				category_name = "commerce_tech";
				break;
			case culture::tech_category::culture:
				category_name = "culture_tech";
				break;
			case culture::tech_category::industry:
				category_name = "industry_tech";
				break;
			default:
				break;
			}
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, category_name), text::text_color::white);
			text::close_layout_box(contents, box);
		}
	}
};

class invention_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);
			set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, stored_text), text::text_color::yellow);
			text::close_layout_box(contents, box);

			auto invention_fat_id = dcon::fatten(state.world, content);
			auto mod_id = invention_fat_id.get_modifier().id;
			if(bool(mod_id))
				modifier_description(state, contents, mod_id);
		}
	}
};

class invention_chance_percent_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			auto mod_k = state.world.invention_get_chance(content);
			auto chances = trigger::evaluate_additive_modifier(state, mod_k, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), 0);
			set_text(state, text::format_percentage(chances / 100.f, 0));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			auto mod_k = state.world.invention_get_chance(content);
			auto mod_d = state.value_modifiers[mod_k];

			auto box = text::open_layout_box(contents, 0);
			text::substitution_map sub_map;
			text::add_to_substitution_map(sub_map, text::variable_type::chance, static_cast<int32_t>(mod_d.base_factor));
			text::localised_format_box(state, contents, box, "base_chance", sub_map);
			text::close_layout_box(contents, box);

			for(uint32_t i = 0; i < mod_d.segments_count; ++i) {
				auto seg = state.value_modifier_segments[mod_d.first_segment_offset + i];
				if(seg.condition) {
					trigger_description(state, contents, seg.condition, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1);
				}
			}
		}
	}
};

class technology_possible_invention : public listbox_row_element_base<dcon::invention_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "folder_icon") {
			return make_element_by_type<invention_image>(state, id);
		} else if(name == "invention_name") {
			return make_element_by_type<invention_name_text>(state, id);
		} else if(name == "invention_percent") {
			return make_element_by_type<invention_chance_percent_text>(state, id);
		} else {
			return nullptr;
		}
	}
};
class technology_possible_invention_listbox : public listbox_element_base<technology_possible_invention, dcon::invention_id> {
protected:
	std::string_view get_row_element_name() override {
		return "invention_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_invention([&](dcon::invention_id id) {
			auto lim_trigger_k = state.world.invention_get_limit(id);
			if(trigger::evaluate(state, lim_trigger_k, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1))
				row_contents.push_back(id);
		});
		update(state);
	}
};

class technology_selected_invention_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::invention_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::invention_id>(payload);

			frame = 0; // inactive
			if(state.world.nation_get_active_inventions(state.local_player_nation, content))
				frame = 2; // This invention's been discovered
			else {
				Cyto::Any tech_payload = dcon::technology_id{};
				parent->impl_get(state, tech_payload);
				auto tech_id = any_cast<dcon::technology_id>(tech_payload);
				if(state.world.nation_get_active_technologies(state.local_player_nation, tech_id))
					frame = 1; // Active technology but not invention
			}
		}
	}
};
class technology_selected_invention : public listbox_row_element_base<dcon::invention_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "invention_icon") {
			return make_element_by_type<technology_selected_invention_image>(state, id);
		} else if(name == "i_invention_name") {
			return make_element_by_type<generic_name_text<dcon::invention_id>>(state, id);
		} else {
			return nullptr;
		}
	}
};
class technology_selected_inventions_listbox : public listbox_element_base<technology_selected_invention, dcon::invention_id> {
protected:
	std::string_view get_row_element_name() override {
		return "invention_icon_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			row_contents.clear();
			state.world.for_each_invention([&](dcon::invention_id id) {
				auto lim_trigger_k = state.world.invention_get_limit(id);
				bool activable_by_this_tech = false;
				trigger::recurse_over_triggers(state.trigger_data.data() + state.trigger_data_indices[lim_trigger_k.index()], [&](uint16_t* tval) {
					if((tval[0] & trigger::code_mask) == trigger::technology && trigger::payload(tval[1]).tech_id == content)
						activable_by_this_tech = true;
				});
				if(activable_by_this_tech)
					row_contents.push_back(id);
			});
			update(state);
		}
	}
};

class technology_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			base_data.data.image.gfx_object = state.world.technology_get_image(content);
		}
	}
};

class technology_year_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			set_text(state, std::to_string(state.world.technology_get_year(content)));
		}
	}
};

class technology_research_points_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			set_text(state, std::to_string(state.world.technology_get_cost(content)));
		}
	}
};

class technology_selected_effect_text : public multiline_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		multiline_text_element_base::on_create(state);
		base_data.size.y *= 2; // Nudge fix for technology descriptions
		base_data.size.y -= 24;
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);

			auto layout = text::create_endless_layout(
			    internal_layout,
			    text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black});
			technology_description(*this, state, layout, content);
		}
	}

	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return message_result::consumed;
	}
};

class technology_start_research : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);
			disabled = !command::can_start_research(state, state.local_player_nation, content);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::technology_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::technology_id>(payload);
			command::start_research(state, state.local_player_nation, content);
		}
	}
};

class technology_selected_tech_window : public window_element_base {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "picture") {
			return make_element_by_type<technology_image>(state, id);
		} else if(name == "title") {
			return make_element_by_type<generic_name_text<dcon::technology_id>>(state, id);
		} else if(name == "effect") {
			return make_element_by_type<technology_selected_effect_text>(state, id);
		} else if(name == "diff_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "diff_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "diff") {
			return make_element_by_type<technology_research_points_text>(state, id);
		} else if(name == "year_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "year") {
			return make_element_by_type<technology_year_text>(state, id);
		} else if(name == "start") {
			return make_element_by_type<technology_start_research>(state, id);
		} else if(name == "inventions") {
			return make_element_by_type<technology_selected_inventions_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class technology_tech_group_window : public window_element_base {
	simple_text_element_base* group_name = nullptr;

public:
	culture::tech_category category{};

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "group_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			group_name = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<culture::folder_info>()) {
			auto folder = any_cast<culture::folder_info>(payload);
			group_name->set_text(state, text::produce_simple_string(state, folder.name));
			return message_result::consumed;
		} else if(payload.holds_type<culture::tech_category>()) {
			auto enum_val = any_cast<culture::tech_category>(payload);
			set_visible(state, category == enum_val);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

class technology_sort_by_type_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "technologyview_sort_by_type_tooltip"), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class technology_sort_by_name_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "technologyview_sort_by_name_tooltip"), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class technology_sort_by_percent_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, text::produce_simple_string(state, "technologyview_sort_by_percent_tooltip"), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class technology_window : public generic_tabbed_window<culture::tech_category> {
	technology_selected_tech_window* selected_tech_win = nullptr;
	dcon::technology_id tech_id{};

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		xy_pair folder_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("folder_offset")->second.definition].position;
		for(auto curr_folder = culture::tech_category::army;
		    curr_folder != culture::tech_category::count;
		    curr_folder = static_cast<culture::tech_category>(static_cast<uint8_t>(curr_folder) + 1)) {
			auto ptr = make_element_by_type<technology_folder_tab_button>(state, state.ui_state.defs_by_name.find("folder_window")->second.definition);
			ptr->set_category(state, curr_folder);
			ptr->base_data.position = folder_offset;
			folder_offset.x += ptr->base_data.size.x;
			add_child_to_front(std::move(ptr));
		}

		// Collect folders by category; the order in which we add technology groups and stuff
		// will be determined by this:
		// Order of category
		// **** Order of folders within category
		// ******** Order of appearance of technologies that have said folder?
		std::vector<std::vector<size_t>> folders_by_category(static_cast<size_t>(culture::tech_category::count));
		for(size_t i = 0; i < state.culture_definitions.tech_folders.size(); i++) {
			const auto& folder = state.culture_definitions.tech_folders[i];
			folders_by_category[static_cast<size_t>(folder.category)].push_back(i);
		}
		// Now obtain the x-offsets of each folder (remember only one category of folders
		// is ever shown at a time)
		std::vector<size_t> folder_x_offset(state.culture_definitions.tech_folders.size(), 0);
		for(const auto& folder_category : folders_by_category) {
			size_t y_offset = 0;
			for(const auto folder_index : folder_category)
				folder_x_offset[folder_index] = y_offset++;
		}
		// Technologies per folder (used for positioning!!!)
		std::vector<size_t> items_per_folder(state.culture_definitions.tech_folders.size(), 0);

		xy_pair base_group_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("tech_group_offset")->second.definition].position;
		xy_pair base_tech_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("tech_offset")->second.definition].position;

		for(auto cat = culture::tech_category::army;
		    cat != culture::tech_category::count;
		    cat = static_cast<culture::tech_category>(static_cast<uint8_t>(cat) + 1)) {
			// Add tech group names
			int16_t group_count = 0;
			for(const auto& folder : state.culture_definitions.tech_folders) {
				if(folder.category != cat)
					continue;

				auto ptr = make_element_by_type<technology_tech_group_window>(state, state.ui_state.defs_by_name.find("tech_group")->second.definition);

				ptr->category = cat;
				Cyto::Any payload = culture::folder_info(folder);
				ptr->impl_set(state, payload);

				ptr->base_data.position.x = static_cast<int16_t>(base_group_offset.x + (group_count * ptr->base_data.size.x));
				ptr->base_data.position.y = base_group_offset.y;
				++group_count;
				add_child_to_front(std::move(ptr));
			}

			// Add technologies
			state.world.for_each_technology([&](dcon::technology_id tid) {
				auto tech = dcon::fatten(state.world, tid);
				size_t folder_id = static_cast<size_t>(tech.get_folder_index());
				const auto& folder = state.culture_definitions.tech_folders[folder_id];
				if(folder.category != cat)
					return;

				auto ptr = make_element_by_type<technology_item_window>(state, state.ui_state.defs_by_name.find("tech_window")->second.definition);

				Cyto::Any payload = tid;
				ptr->impl_set(state, payload);

				ptr->base_data.position.x = static_cast<int16_t>(base_group_offset.x + (folder_x_offset[folder_id] * ptr->base_data.size.x));
				// 16px spacing between tech items, 109+16 base offset
				ptr->base_data.position.y = static_cast<int16_t>(base_group_offset.y + base_tech_offset.y + (static_cast<int16_t>(items_per_folder[folder_id]) * ptr->base_data.size.y));
				items_per_folder[folder_id]++;
				add_child_to_front(std::move(ptr));
			});
		}

		// Properly setup technology displays...
		Cyto::Any payload = active_tab;
		impl_set(state, payload);

		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "administration_type") {
			return make_element_by_type<nation_technology_admin_type_text>(state, id);
		} else if(name == "research_progress") {
			return make_element_by_type<nation_technology_research_progress>(state, id);
		} else if(name == "research_progress_name") {
			return make_element_by_type<technology_research_progress_name_text>(state, id);
		} else if(name == "research_progress_category") {
			return make_element_by_type<technology_research_progress_category_text>(state, id);
		} else if(name == "selected_tech_window") {
			auto ptr = make_element_by_type<technology_selected_tech_window>(state, id);
			selected_tech_win = ptr.get();
			return ptr;
		} else if(name == "sort_by_type") {
			auto ptr = make_element_by_type<technology_sort_by_type_button>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "sort_by_name") {
			return make_element_by_type<technology_sort_by_name_button>(state, id);
		} else if(name == "sort_by_percent") {
			auto ptr = make_element_by_type<technology_sort_by_percent_button>(state, id);
			ptr->base_data.position.y -= 1; // Nudge
			return ptr;
		} else if(name == "inventions") {
			return make_element_by_type<technology_possible_invention_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<culture::tech_category>()) {
			active_tab = any_cast<culture::tech_category>(payload);
			for(auto& c : children)
				c->impl_set(state, payload);
			return message_result::consumed;
		} else if(payload.holds_type<technology_select_tech>()) {
			tech_id = any_cast<technology_select_tech>(payload).tech_id;
			selected_tech_win->impl_on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::technology_id>()) {
			payload.emplace<dcon::technology_id>(tech_id);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
