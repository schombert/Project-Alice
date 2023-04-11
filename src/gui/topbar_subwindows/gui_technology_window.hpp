#pragma once

#include "gui_element_types.hpp"

namespace ui {

class technology_folder_tab_sub_button : public checkbox_button {
public:
	culture::tech_category target;
	bool is_active(sys::state& state) noexcept final;
	void button_action(sys::state& state) noexcept final;
};
class technology_folder_tab_button : public window_element_base {
	image_element_base* folder_icon = nullptr;
	simple_text_element_base* folder_name = nullptr;
	technology_folder_tab_sub_button* folder_button = nullptr;
public:
	culture::tech_category target;
	void set_target(sys::state& state, culture::tech_category new_target) {
		folder_button->target = target = new_target;

		folder_icon->frame = static_cast<int32_t>(target);

		std::string str{};
		switch(target) {
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
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "tech_folder_progress_overlay") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "folder_number_discovered") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class technology_admin_type_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto mod_id = state.world.nation_get_tech_school(state.local_player_nation);
		if(mod_id) {
			auto name = text::produce_simple_string(state, state.world.modifier_get_name(mod_id));
			set_text(state, name);
		} else {
			set_text(state, text::produce_simple_string(state, "traditional_academic"));
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

class technology_item_window : public window_element_base {
	simple_text_element_base* tech_name = nullptr;
	dcon::technology_id tech_id;
	culture::tech_category category;
public:
	void set_technology(sys::state& state, dcon::technology_id id) {
		tech_id = id;

		auto tech = dcon::fatten(state.world, tech_id);
		category = state.culture_definitions.tech_folders[tech.get_folder_index()].category;
		auto name = text::produce_simple_string(state, tech.get_name());
		tech_name->set_text(state, name);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "start_research") {
			return make_element_by_type<button_element_base>(state, id);
		} else if(name == "tech_name") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			tech_name = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override;
};

class technology_selected_tech_window : public window_element_base {
	image_element_base* tech_picture = nullptr;
	simple_text_element_base* tech_name = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "picture") {
			auto ptr = make_element_by_type<image_element_base>(state, id);
			tech_picture = ptr.get();
			return ptr;
		} else if(name == "title") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			tech_name = ptr.get();
			return ptr;
		} else if(name == "effect") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "desc") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "diff_icon") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "diff_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "diff") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "year_label") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "year") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else if(name == "start") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class technology_window : public generic_tabbed_window<culture::tech_category> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);

		xy_pair folder_offset{ 32, 55 };
		for(auto curr_folder = culture::tech_category::army;
			curr_folder != culture::tech_category::count;
			curr_folder = static_cast<culture::tech_category>(static_cast<uint8_t>(curr_folder) + 1))
		{	
			auto ptr = make_element_by_type<technology_folder_tab_button>(state, state.ui_state.defs_by_name.find("folder_window")->second.definition);
			ptr->set_target(state, curr_folder);
			ptr->base_data.position = folder_offset;
			folder_offset.x += ptr->base_data.size.x;
			add_child_to_front(std::move(ptr));
		}

		// Collect folders by category; the order in which we add technology groups and stuff
		// will be determined by this:
		// Order of category
		// **** Order of folders within category
		// ******** Order of appareance of technologies that have said folder?
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
			for(const auto& folder_index : folder_category)
				folder_x_offset[folder_index] = y_offset++;
		}
		// Technologies per folder (used for positioning!!!)
		std::vector<size_t> items_per_folder(state.culture_definitions.tech_folders.size(), 0);

		for(auto cat = culture::tech_category::army;
			cat != culture::tech_category::count;
			cat = static_cast<culture::tech_category>(static_cast<uint8_t>(cat) + 1))
		{
			state.world.for_each_technology([&](dcon::technology_id tech_id) {
				auto tech = dcon::fatten(state.world, tech_id);
				auto folder_id = tech.get_folder_index();
				const auto& folder = state.culture_definitions.tech_folders[folder_id];
				if(folder.category == cat) {
					auto ptr = make_element_by_type<technology_item_window>(state, state.ui_state.defs_by_name.find("tech_window")->second.definition);
					ptr->set_technology(state, tech_id);

					ptr->base_data.position.x = 28 + (folder_x_offset[folder_id] * ptr->base_data.size.x);
					// 16px spacing between tech items, 109+16 base offset
					ptr->base_data.position.y = 109 + 16 + (items_per_folder[folder_id] * ptr->base_data.size.y);
					items_per_folder[folder_id]++;

					add_child_to_front(std::move(ptr));
				}
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
			return make_element_by_type<technology_admin_type_text>(state, id);
		} else if(name == "research_progress_name") {
			return make_element_by_type<technology_research_progress_name_text>(state, id);
		} else if(name == "research_progress_category") {
			return make_element_by_type<technology_research_progress_category_text>(state, id);
		} else if(name == "selected_tech_window") {
			return make_element_by_type<technology_selected_tech_window>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<culture::tech_category>()) {
			auto enum_val = any_cast<culture::tech_category>(payload);
			active_tab = enum_val;
			for(auto& c : children)
				c->impl_set(state, payload);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}
