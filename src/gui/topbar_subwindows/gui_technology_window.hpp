#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "triggers.hpp"

namespace ui {

void invention_description(sys::state& state, text::layout_base& contents, dcon::invention_id inv_id, int32_t indent) noexcept;
void technology_description(sys::state& state, text::layout_base& contents, dcon::technology_id tech_id) noexcept;

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
			bool can_research = command::can_start_research(state, state.local_player_nation, id);
			bool is_current = state.world.nation_get_current_research(state.local_player_nation) == id;
			auto color = discovered
				? text::text_color::green
				: (is_current
					? text::text_color::light_blue
					: (can_research
						? text::text_color::red
						: text::text_color::light_grey));
			auto name = fat_id.get_name();
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), color);
			text::close_layout_box(contents, box);
		});
		switch(category) {
		case culture::tech_category::army:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::army_tech_research_bonus, true);
			break;
		case culture::tech_category::commerce:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::commerce_tech_research_bonus, true);
			break;
		case culture::tech_category::culture:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::culture_tech_research_bonus, true);
			break;
		case culture::tech_category::industry:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::industry_tech_research_bonus, true);
			break;
		case culture::tech_category::navy:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::navy_tech_research_bonus, true);
			break;
		case culture::tech_category::military_theory:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::military_theory_tech_research_bonus, true);
			break;
		case culture::tech_category::population:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::population_tech_research_bonus, true);
			break;
		case culture::tech_category::diplomacy:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::diplomacy_tech_research_bonus, true);
			break;
		case culture::tech_category::flavor:
			active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::flavor_tech_research_bonus, true);
			break;
		case culture::tech_category::unknown:
			break;
		}
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
		folder_name->set_text(state, text::produce_simple_string(state, culture::get_tech_category_name(category)));

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

class technology_research_progress_category_text : public simple_text_element_base {
	std::string get_text(sys::state& state) noexcept {
		auto tech_id = nations::current_research(state, state.local_player_nation);
		if(tech_id) {
			auto tech = dcon::fatten(state.world, tech_id);
			auto const& folder = state.culture_definitions.tech_folders[tech.get_folder_index()];
			std::string str{};
			str += text::produce_simple_string(state, folder.name);
			str += ", ";
			str += text::produce_simple_string(state, culture::get_tech_category_name(folder.category));
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
		auto content = retrieve<dcon::technology_id>(state, parent);
		if(state.ui_state.technology_subwindow) {
			Cyto::Any sl_payload = technology_select_tech{content};
			state.ui_state.technology_subwindow->impl_set(state, sl_payload);
		}
	}

	void button_right_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		auto it = std::find(state.ui_state.tech_queue.begin(), state.ui_state.tech_queue.end(), content);
		if(it != state.ui_state.tech_queue.end()) {
			state.ui_state.tech_queue.erase(it);
			state.game_state_updated.store(true, std::memory_order::release);
		}
	}

	void button_shift_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		if(!content)
			return;

		auto it = std::find(state.ui_state.tech_queue.begin(), state.ui_state.tech_queue.end(), content);
		if(it == state.ui_state.tech_queue.end()) {
			if(content != state.world.nation_get_current_research(state.local_player_nation) && !state.world.nation_get_active_technologies(state.local_player_nation, content)) { // don't add already researched or researching
				state.ui_state.tech_queue.push_back(content);
				parent->impl_on_update(state);
			}
		} else {
			state.ui_state.tech_queue.erase(it);
			state.ui_state.tech_queue.push_back(content);
			parent->impl_on_update(state);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);
		if(auto name = fat_id.get_name(); state.key_is_localized(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(auto desc = fat_id.get_desc(); state.key_is_localized(desc)) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, desc);
			text::close_layout_box(contents, box);
		}
		technology_description(state, contents, content);
		{
			auto box = text::open_layout_box(contents, 0);
			text::localised_format_box(state, contents, box, "tech_queue_explain");
			text::close_layout_box(contents, box);
		}
	}
};

class technology_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, content);

		auto it = std::find(state.ui_state.tech_queue.begin(), state.ui_state.tech_queue.end(), content);
		if(it == state.ui_state.tech_queue.end()) {
			// not queued
			simple_text_element_base::set_text(state, text::get_name_as_string(state, fat_id));
		} else {
			// queued
			simple_text_element_base::set_text(state, "(" + std::to_string(std::distance(state.ui_state.tech_queue.begin(), it) + 1) + ") " + text::get_name_as_string(state, fat_id));
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
			return make_element_by_type<technology_name_text>(state, id);
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(!tech_id)
			return;

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
		auto content = retrieve<dcon::invention_id>(state, parent);
		frame = int32_t(state.world.invention_get_technology_type(content));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::invention_id>(state, parent);
		auto category = static_cast<culture::tech_category>(state.world.invention_get_technology_type(content));
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, culture::get_tech_category_name(category)), text::text_color::white);
		text::close_layout_box(contents, box);
	}
};

class invention_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::invention_id>(state, parent);
		set_text(state, text::produce_simple_string(state, dcon::fatten(state.world, content).get_name()));
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto content = retrieve<dcon::invention_id>(state, parent);

		if(auto name = state.world.invention_get_name(content); name) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, name), text::text_color::yellow);
			text::close_layout_box(contents, box);
		}
		if(auto desc = state.world.invention_get_name(content); desc) {
			auto box = text::open_layout_box(contents, 0);
			text::add_to_layout_box(state, contents, box, desc);
			text::close_layout_box(contents, box);
		}
		text::add_line_break_to_layout(state, contents);
		invention_description(state, contents, content, 0);
		text::add_line_break_to_layout(state, contents);
	}
};

class invention_chance_percent_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(auto content = retrieve<dcon::invention_id>(state, parent); content) {
			auto mod_k = state.world.invention_get_chance(content);
			auto chances = trigger::evaluate_additive_modifier(state, mod_k, trigger::to_generic(state.local_player_nation),
					trigger::to_generic(state.local_player_nation), 0);
			set_text(state, text::format_percentage(chances / 100.f, 0));
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(auto content = retrieve<dcon::invention_id>(state, parent); content) {
			text::add_line(state, contents, "alice_invention_chance");
			auto mod_k = state.world.invention_get_chance(content);
			additive_value_modifier_description(state, contents, mod_k, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), 0);
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

enum class invention_sort_type {
	name, type, chance
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
			if(!state.world.nation_get_active_inventions(state.local_player_nation, id) && trigger::evaluate(state, lim_trigger_k, trigger::to_generic(state.local_player_nation),
						 trigger::to_generic(state.local_player_nation), -1))
				row_contents.push_back(id);
		});

		auto sort_order = retrieve< invention_sort_type>(state, parent);
		switch(sort_order) {
		case invention_sort_type::name:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::invention_id a, dcon::invention_id b) {
				auto a_name = text::produce_simple_string(state, dcon::fatten(state.world, a).get_name());
				auto b_name = text::produce_simple_string(state, dcon::fatten(state.world, b).get_name());
				return a_name < b_name;
			});
			break;
		case invention_sort_type::type:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::invention_id a, dcon::invention_id b) {
				return state.world.invention_get_technology_type(a) < state.world.invention_get_technology_type(b);
			});
			break;
		case invention_sort_type::chance:
			std::sort(row_contents.begin(), row_contents.end(), [&](dcon::invention_id a, dcon::invention_id b) {
				auto mod_a = state.world.invention_get_chance(a);
				auto chances_a = trigger::evaluate_additive_modifier(state, mod_a, trigger::to_generic(state.local_player_nation),
						trigger::to_generic(state.local_player_nation), 0);
				auto mod_b = state.world.invention_get_chance(b);
				auto chances_b = trigger::evaluate_additive_modifier(state, mod_b, trigger::to_generic(state.local_player_nation),
						trigger::to_generic(state.local_player_nation), 0);

				return chances_a > chances_b;
			});
			break;
		}
		update(state);
	}
};

class technology_selected_invention_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::invention_id>(state, parent);
		frame = 0; // inactive
		if(content && state.world.nation_get_active_inventions(state.local_player_nation, content)) {
			frame = 1; // This invention's been discovered
		} else {
			auto tech_id = retrieve<dcon::technology_id>(state, parent);
			if(tech_id && state.world.nation_get_active_technologies(state.local_player_nation, tech_id))
				frame = 2; // Active technology but not invention
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto inv = retrieve<dcon::invention_id>(state, parent);
		auto tech_id = retrieve<dcon::technology_id>(state, parent);
		if(inv && state.world.nation_get_active_inventions(state.local_player_nation, inv)) {
			text::add_line(state, contents, "invention_bulb_3");
		} else if(tech_id && state.world.nation_get_active_technologies(state.local_player_nation, tech_id)) {
			text::add_line(state, contents, "invention_bulb_2");
		} else {
			text::add_line(state, contents, "invention_bulb_1");
		}
	}
};
class technology_selected_invention : public listbox_row_element_base<dcon::invention_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "invention_icon") {
			return make_element_by_type<technology_selected_invention_image>(state, id);
		} else if(name == "i_invention_name") {
			return make_element_by_type<invention_name_text>(state, id);
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
		auto content = retrieve<dcon::technology_id>(state, parent);
		row_contents.clear();
		state.world.for_each_invention([&](dcon::invention_id id) {
			auto lim_trigger_k = state.world.invention_get_limit(id);
			bool activable_by_this_tech = false;
			trigger::recurse_over_triggers(state.trigger_data.data() + state.trigger_data_indices[lim_trigger_k.index() + 1],
					[&](uint16_t* tval) {
						if((tval[0] & trigger::code_mask) == trigger::technology && trigger::payload(tval[1]).tech_id == content)
							activable_by_this_tech = true;
					});
			if(activable_by_this_tech)
				row_contents.push_back(id);
		});
		update(state);
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
		auto t = retrieve<dcon::technology_id>(state, parent);
		if(t) {
			set_text(state, std::to_string(int64_t(
				culture::effective_technology_cost(state, state.ui_date.to_ymd(state.start_date).year, state.local_player_nation, t)
			)));
		} else {
			set_text(state, "");
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto tech_id = retrieve<dcon::technology_id>(state, parent);
		if(!tech_id)
			return;

		auto base_cost = state.world.technology_get_cost(tech_id);
		auto availability_year = state.world.technology_get_year(tech_id);
		auto folder = state.world.technology_get_folder_index(tech_id);
		auto category = state.culture_definitions.tech_folders[folder].category;

		{
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, "base_value");
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, int64_t(base_cost));
			text::close_layout_box(contents, box);
		}

		switch(category) {
			case culture::tech_category::army:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::army_tech_research_bonus, true);
				break;
			case culture::tech_category::navy:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::navy_tech_research_bonus, true);
				break;
			case culture::tech_category::commerce:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::commerce_tech_research_bonus, true);
				break;
			case culture::tech_category::culture:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::culture_tech_research_bonus, true);
				break;
			case culture::tech_category::industry:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::industry_tech_research_bonus, true);
				break;
			//non vanilla
			case culture::tech_category::military_theory:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::military_theory_tech_research_bonus, true);
				break;
			case culture::tech_category::population:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::population_tech_research_bonus, true);
				break;
			case culture::tech_category::diplomacy:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::diplomacy_tech_research_bonus, true);
				break;
			case culture::tech_category::flavor:
				ui::active_modifiers_description(state, contents, state.local_player_nation, 0, sys::national_mod_offsets::flavor_tech_research_bonus, true);
				break;
			default:
				break;
		}

		auto ol_mod = state.world.nation_get_active_technologies( state.world.overlord_get_ruler(state.world.nation_get_overlord_as_subject(state.local_player_nation)), tech_id) ? state.defines.tech_factor_vassal : 1.0f;

		if(ol_mod != 1.0f) {
			text::add_line(state, contents, "tech_ol_tt", text::variable_type::x, text::fp_percentage{ ol_mod });
		}

		auto year_mod = (1.0f - std::max(0.0f, float(state.ui_date.to_ymd(state.start_date).year - availability_year) / state.defines.tech_year_span));
		if(year_mod != 1.0f) {
			text::add_line(state, contents, "tech_year_tt", text::variable_type::x, text::fp_percentage{ year_mod });
		}
	}
};

class technology_selected_effect_text : public scrollable_text {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.y *= 2; // Nudge fix for technology descriptions
		base_data.size.y -= 24;
		base_data.size.x -= 10;
		scrollable_text::on_create(state);
	}
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		auto layout = text::create_endless_layout(state, delegate->internal_layout,
				text::layout_parameters{0, 0, int16_t(base_data.size.x), int16_t(base_data.size.y),
			base_data.data.text.font_handle, 0, text::alignment::left,
			text::is_black_from_font_id(base_data.data.text.font_handle) ? text::text_color::black : text::text_color::white, false});
		technology_description(state, layout, content);
		calibrate_scrollbar(state);
	}
};

class technology_start_research : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		disabled = !command::can_start_research(state, state.local_player_nation, content);
	}

	void button_action(sys::state& state) noexcept override {
		auto content = retrieve<dcon::technology_id>(state, parent);
		command::start_research(state, state.local_player_nation, content);
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
			return make_element_by_type<simple_body_text>(state, id);
		} else if(name == "diff") {
			return make_element_by_type<technology_research_points_text>(state, id);
		} else if(name == "year_label") {
			return make_element_by_type<simple_body_text>(state, id);
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
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "technologyview_sort_by_type_tooltip"),
				text::text_color::white);
		text::close_layout_box(contents, box);
	}

	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<invention_sort_type>{invention_sort_type::type});
	}
};

class technology_sort_by_name_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "technologyview_sort_by_name_tooltip"),
				text::text_color::white);
		text::close_layout_box(contents, box);
	}

	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<invention_sort_type>{invention_sort_type::name});
	}
};

class technology_sort_by_percent_button : public button_element_base {
public:
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(state, contents, box, text::produce_simple_string(state, "technologyview_sort_by_percent_tooltip"),
				text::text_color::white);
		text::close_layout_box(contents, box);
	}

	void button_action(sys::state& state) noexcept override {
		send(state, parent, element_selection_wrapper<invention_sort_type>{invention_sort_type::chance});
	}
};

class technology_window : public generic_tabbed_window<culture::tech_category> {
	technology_selected_tech_window* selected_tech_win = nullptr;
	dcon::technology_id tech_id{};
	invention_sort_type invention_sort = invention_sort_type::type;
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		xy_pair folder_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("folder_offset"))->second.definition].position;
		for(auto curr_folder = culture::tech_category::army; curr_folder != culture::tech_category(5);
				curr_folder = static_cast<culture::tech_category>(static_cast<uint8_t>(curr_folder) + 1)) {
			auto ptr = make_element_by_type<technology_folder_tab_button>(state,
					state.ui_state.defs_by_name.find(state.lookup_key("folder_window"))->second.definition);
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
		std::vector<std::vector<size_t>> folders_by_category(static_cast<size_t>(5));
		for(size_t i = 0; i < state.culture_definitions.tech_folders.size(); i++) {
			auto const& folder = state.culture_definitions.tech_folders[i];
			folders_by_category[static_cast<size_t>(folder.category)].push_back(i);
		}
		// Now obtain the x-offsets of each folder (remember only one category of folders
		// is ever shown at a time)
		std::vector<size_t> folder_x_offset(state.culture_definitions.tech_folders.size(), 0);
		for(auto const& folder_category : folders_by_category) {
			size_t y_offset = 0;
			for(auto const folder_index : folder_category)
				folder_x_offset[folder_index] = y_offset++;
		}
		// Technologies per folder (used for positioning!!!)
		std::vector<size_t> items_per_folder(state.culture_definitions.tech_folders.size(), 0);

		xy_pair base_group_offset =
			state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("tech_group_offset"))->second.definition].position;
		xy_pair base_tech_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("tech_offset"))->second.definition].position;

		for(auto cat = culture::tech_category::army; cat != culture::tech_category(5);
				cat = static_cast<culture::tech_category>(static_cast<uint8_t>(cat) + 1)) {
			// Add tech group names
			int16_t group_count = 0;
			for(auto const& folder : state.culture_definitions.tech_folders) {
				if(folder.category != cat)
					continue;

				auto ptr = make_element_by_type<technology_tech_group_window>(state,
						state.ui_state.defs_by_name.find(state.lookup_key("tech_group"))->second.definition);

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

				auto ptr = make_element_by_type<technology_item_window>(state,
						state.ui_state.defs_by_name.find(state.lookup_key("tech_window"))->second.definition);

				Cyto::Any payload = tid;
				ptr->impl_set(state, payload);

				ptr->base_data.position.x =
					static_cast<int16_t>(base_group_offset.x + (folder_x_offset[folder_id] * ptr->base_data.size.x));
				// 16px spacing between tech items, 109+16 base offset
				ptr->base_data.position.y =
					static_cast<int16_t>(base_group_offset.y + base_tech_offset.y +
															 (static_cast<int16_t>(items_per_folder[folder_id]) * ptr->base_data.size.y));
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
		if(name == "main_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "bg_tech") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "administration") {
			return make_element_by_type<simple_body_text>(state, id);
		} else if(name == "current_research") {
			return make_element_by_type<simple_body_text>(state, id);
		} else if(name == "administration_type") {
			return make_element_by_type<national_tech_school>(state, id);
		} else if(name == "research_progress") {
			return make_element_by_type<nation_technology_research_progress>(state, id);
		} else if(name == "research_progress_name") {
			return make_element_by_type<nation_current_research_text>(state, id);
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
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<invention_sort_type>>()) {
			invention_sort = any_cast<element_selection_wrapper<invention_sort_type>>(payload).data;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<invention_sort_type>()) {
			payload = invention_sort;
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
