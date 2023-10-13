#pragma once

#include "dcon_generated.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_province_window.hpp"
#include "province.hpp"
#include "text.hpp"
#include <algorithm>

namespace ui {

class province_search_edit : public edit_box_element_base {
public:
	void edit_box_update(sys::state& state, std::string_view s) noexcept override {
		Cyto::Any input = s;
		parent->impl_get(state, input);
	}
};

class province_search_list_item : public listbox_row_button_base<dcon::province_id> {
public:
	void button_action(sys::state& state) noexcept override {
		auto map_prov_id = content;
		if(map_prov_id) {
			state.map_state.set_selected_province(map_prov_id);
			static_cast<province_view_window*>(state.ui_state.province_window)->set_active_province(state, map_prov_id);
			state.map_state.center_map_on_province(state, map_prov_id);
		}
	}

	void update(sys::state& state) noexcept override {
		dcon::province_fat_id fat_id = dcon::fatten(state.world, content);
		auto name = text::produce_simple_string(state, fat_id.get_name());
		set_button_text(state, name);
	}
};

class province_search_list : public listbox_element_base<province_search_list_item, dcon::province_id> {
protected:
	std::string_view get_row_element_name() override {
		return "goto_location_button";
	}
};

class province_search_window : public window_element_base {
private:
	province_search_list* search_listbox = nullptr;
	province_search_edit* edit_box = nullptr;

	std::vector<dcon::province_id> search_provinces(sys::state& state, std::string_view search_term) noexcept {
		std::vector<dcon::province_id> results{};
		std::string search_term_lower = parsers::lowercase_str(search_term);

		if(!search_term.empty()) {
			state.world.for_each_province([&](dcon::province_id prov_id) {
				dcon::province_fat_id fat_id = dcon::fatten(state.world, prov_id);
				auto name = parsers::lowercase_str(text::produce_simple_string(state, fat_id.get_name()));
				if(name.starts_with(search_term_lower)) {
					results.push_back(prov_id);
				}
			});
		}

		return results;
	}

public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		move_child_to_front(search_listbox);
		state.ui_state.edit_target = edit_box;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "cancel") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "goto_edit") {
			auto ptr = make_element_by_type<province_search_edit>(state, id);
			edit_box = ptr.get();
			return ptr;
		} else if(name == "provinces") {
			auto ptr = make_element_by_type<province_search_list>(state, id);
			search_listbox = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<std::string_view>()) {
			auto search_term = any_cast<std::string_view>(payload);
			search_listbox->row_contents = search_provinces(state, search_term);
			search_listbox->update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	void on_visible(sys::state& state) noexcept override {
		state.ui_state.edit_target = edit_box;
	}

	void on_hide(sys::state& state) noexcept override {
		state.ui_state.edit_target = nullptr;
	}
};

} // namespace ui
