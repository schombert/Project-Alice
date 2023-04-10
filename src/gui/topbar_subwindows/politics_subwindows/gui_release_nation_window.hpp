#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

class release_nation_description_text : public standard_nation_national_identity_multiline_text {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		if(auto k = state.key_to_text_sequence.find(std::string_view("politics_release_vassal_desc")); k != state.key_to_text_sequence.end()) {
			int64_t province_count = 0;
			std::string provinces = "";
			state.world.national_identity_for_each_core(national_identity_id, [&](dcon::core_id core) {
				auto province = state.world.core_get_province(core);
				if(state.world.province_get_nation_from_province_ownership(province) == nation_id && province_count++ < 5) {
					if(!provinces.empty()) {
						provinces += ", ";
					}
					provinces += text::produce_simple_string(state, state.world.province_get_name(province));
				}
			});
			auto box = text::open_layout_box(contents);
			text::substitution_map sub;
			text::add_to_substitution_map(sub, text::variable_type::tag, national_identity_id);
			text::add_to_substitution_map(sub, text::variable_type::num, province_count);
			text::add_to_substitution_map(sub, text::variable_type::provinces, std::string_view(provinces));
			text::add_to_layout_box(contents, state, box, k->second, sub);
			if(province_count > 5) {
				text::add_to_layout_box(contents, state, box, std::string(" and others."), text::text_color::black);
			} else {
				text::add_to_layout_box(contents, state, box, std::string("."), text::text_color::black);
			}
			text::close_layout_box(contents, box);
		}
	}
};

class release_nation_option : public listbox_row_element_base<dcon::national_identity_id> {
private:
	flag_button* country_flag = nullptr;

public:
	void update(sys::state& state) noexcept override {
		country_flag->set_current_nation(state, content);
		Cyto::Any payload = content;
		impl_set(state, payload);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "name") {
			return make_element_by_type<generic_name_text<dcon::national_identity_id>>(state, id);
		} else if(name == "country_flag") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag = ptr.get();
			ptr->set_current_nation(state, content);
			return ptr;
		} else if(name == "type") {
			return make_element_by_type<national_identity_vassal_type_text>(state, id);
		} else if(name == "desc") {
			return make_element_by_type<release_nation_description_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class release_nation_listbox : public listbox_element_base<release_nation_option, dcon::national_identity_id> {
private:
	dcon::nation_id nation_id{};

protected:
	std::string_view get_row_element_name() override {
        return "vassal_nation";
    }

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_national_identity([&](dcon::national_identity_id ident) {
			if(nations::can_release_as_vassal(state, nation_id, ident)) {
				row_contents.push_back(ident);
			}
		});
		update(state);
	}

	message_result set(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			nation_id = any_cast<dcon::nation_id>(payload);
			on_update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
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
