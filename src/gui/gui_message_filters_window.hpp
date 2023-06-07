#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"

namespace ui {

class message_filters_country_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, "");
	}

	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);
			frame = state.world.nation_get_is_interesting(content) ? 1 : 0;
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::nation_id{};
			parent->impl_get(state, payload);
			auto content = any_cast<dcon::nation_id>(payload);

			auto fat_id = dcon::fatten(state.world, content);
			fat_id.set_is_interesting(!fat_id.get_is_interesting());
			parent->impl_on_update(state);
		}
	}
};

class message_filters_country_item : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "entry_bg") {
			return make_element_by_type<message_filters_country_button>(state, id);
		} else if(name == "country_name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::nation_id>::get(state, payload);
	}
};

class message_filters_country_listbox : public listbox_element_base<message_filters_country_item, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "message_filters_entry";
	}

public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base::on_create(state);
		on_update(state);
	}

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id id) {
			if(state.world.nation_get_owned_province_count(id) != 0)
				row_contents.push_back(id);
		});
		update(state);
	}
};

class message_filters_window : public window_element_base {
	message_filters_country_listbox* country_listbox = nullptr;

public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "countries_listbox") {
			auto ptr = make_element_by_type<message_filters_country_listbox>(state, id);
			country_listbox = ptr.get();
			return ptr;
		} else if(name == "filter_deselect") {
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::deselect_all;
			return ptr;
		} else if(name == "filter_bestguess") {
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::best_guess;
			return ptr;
		} else if(name == "filter_enemies") {
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::enemies;
			return ptr;
		} else if(name == "filter_allies") {
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::allies;
			return ptr;
		} else if(name == "filter_neighbours") {
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::neighbors;
			return ptr;
		} else if(name == "filter_sphere") {
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<country_list_filter>>(state, id);
			ptr->target = country_list_filter::sphere;
			return ptr;
		} else if(name.length() >= 7 && name.substr(0, 7) == "filter_") {
			auto const filter_name = name.substr(7);
			state.ui_defs.gui[id].data.button.font_handle = text::name_into_font_id(state, "vic_18_black"); // Nudge font
			auto ptr = make_element_by_type<generic_tab_button<dcon::modifier_id>>(state, id);
			ptr->target = ([&]() {
				dcon::modifier_id filter_mod_id{0};
				auto it = state.key_to_text_sequence.find(parsers::lowercase_str(filter_name));
				if(it != state.key_to_text_sequence.end())
					state.world.for_each_modifier([&](dcon::modifier_id mod_id) {
						auto fat_id = dcon::fatten(state.world, mod_id);
						if(it->second == fat_id.get_name())
							filter_mod_id = mod_id;
					});
				return filter_mod_id;
			})();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<country_list_filter>()) {
			auto filter = any_cast<country_list_filter>(payload);
			switch(filter) {
			case country_list_filter::best_guess:
				state.world.for_each_nation([&](dcon::nation_id id) {
					auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(id, state.local_player_nation);
					if(state.world.diplomatic_relation_get_are_allied(rel) ||
						military::are_allied_in_war(state, state.local_player_nation, id) ||
						military::are_at_war(state, state.local_player_nation, id) ||
						state.world.nation_get_in_sphere_of(id) == state.local_player_nation ||
						state.world.get_nation_adjacency_by_nation_adjacency_pair(state.local_player_nation, id))
						state.world.nation_set_is_interesting(id, true);
				});
				break;
			case country_list_filter::deselect_all:
				state.world.for_each_nation([&](dcon::nation_id id) { state.world.nation_set_is_interesting(id, false); });
				break;
			case country_list_filter::allies:
				state.world.for_each_nation([&](dcon::nation_id id) {
					auto rel = state.world.get_diplomatic_relation_by_diplomatic_pair(id, state.local_player_nation);
					if(state.world.diplomatic_relation_get_are_allied(rel) ||
						military::are_allied_in_war(state, state.local_player_nation, id))
						state.world.nation_set_is_interesting(id, true);
				});
				break;
			case country_list_filter::enemies:
				state.world.for_each_nation([&](dcon::nation_id id) {
					if(military::are_at_war(state, state.local_player_nation, id))
						state.world.nation_set_is_interesting(id, true);
				});
				break;
			case country_list_filter::sphere:
				state.world.for_each_nation([&](dcon::nation_id id) {
					if(state.world.nation_get_in_sphere_of(id) == state.local_player_nation)
						state.world.nation_set_is_interesting(id, true);
				});
				break;
			case country_list_filter::neighbors:
				state.world.for_each_nation([&](dcon::nation_id id) {
					if(state.world.get_nation_adjacency_by_nation_adjacency_pair(state.local_player_nation, id))
						state.world.nation_set_is_interesting(id, true);
				});
				break;
			default:
				break;
			}
			country_listbox->on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::modifier_id>()) {
			auto mod_id = any_cast<dcon::modifier_id>(payload);
			state.world.for_each_nation([&](dcon::nation_id id) {
				if(state.world.nation_get_owned_province_count(id) != 0) {
					dcon::nation_fat_id fat_id = dcon::fatten(state.world, id);
					if(mod_id == fat_id.get_capital().get_continent().id)
						state.world.nation_set_is_interesting(id, true);
				}
			});
			country_listbox->on_update(state);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
