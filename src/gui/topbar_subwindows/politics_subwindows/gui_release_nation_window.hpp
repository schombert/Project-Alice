#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

class politics_release_nation_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	// TODO - this window doesnt appear!?!?
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			return make_element_by_type<button_element_base>(state, id);
		} else
		if(name == "default_popup_banner") {
			return make_element_by_type<image_element_base>(state, id);
		} else
		if(name == "title") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "description") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else
		if(name == "agreebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else
		if(name == "declinebutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else
		if(name == "playasbutton") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class release_nation_description_text : public generic_multiline_text<dcon::national_identity_id> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::national_identity_id id) noexcept override {
		int64_t province_count = 0;
		std::string provinces = "";
		state.world.national_identity_for_each_core(id, [&](dcon::core_id core) {
			auto province = state.world.core_get_province(core);
			if(state.world.province_get_nation_from_province_ownership(province) == state.local_player_nation && province_count++ < 5) {
				if(!provinces.empty()) {
					provinces += ", ";
				}
				provinces += text::produce_simple_string(state, state.world.province_get_name(province));
			}
		});
		auto box = text::open_layout_box(contents);
		text::substitution_map sub;
		text::add_to_substitution_map(sub, text::variable_type::tag, id);
		text::add_to_substitution_map(sub, text::variable_type::num, province_count);
		text::add_to_substitution_map(sub, text::variable_type::provinces, std::string_view(provinces));
		text::localised_format_box(state, contents, box, std::string_view("politics_release_vassal_desc"), sub);
		if(province_count > 5) {
			text::add_to_layout_box(contents, state, box, std::string(" and others."), text::text_color::black);
		} else {
			text::add_to_layout_box(contents, state, box, std::string("."), text::text_color::black);
		}
		text::close_layout_box(contents, box);
	}
};

class release_nation_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		// TODO: Release nation command
		Cyto::Any payload = dcon::national_identity_id{};
		parent->impl_get(state, payload);
		state.ui_state.politics_subwindow->show_release_window(state, payload);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::add_to_layout_box(contents, state, box, std::string_view("Rewease me"));
		text::close_layout_box(contents, box);
	}
};

class release_nation_option : public listbox_row_element_base<dcon::national_identity_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			return make_element_by_type<generic_name_text<dcon::national_identity_id>>(state, id);
		} else if(name == "country_flag") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "type") {
			return make_element_by_type<national_identity_vassal_type_text>(state, id);
		} else if(name == "desc") {
			return make_element_by_type<release_nation_description_text>(state, id);
		} else if(name == "country_release_vassal") {
			return make_element_by_type<release_nation_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

class release_nation_listbox : public listbox_element_base<release_nation_option, dcon::national_identity_id> {
protected:
	std::string_view get_row_element_name() override {
		return "vassal_nation";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_national_identity([&](dcon::national_identity_id ident) {
			if(nations::can_release_as_vassal(state, state.local_player_nation, ident)) {
				row_contents.push_back(ident);
			}
		});
		update(state);
	}
};

class release_nation_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "nations") {
			return make_element_by_type<release_nation_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
