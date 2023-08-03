#pragma once

#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "nations.hpp"
#include "system_state.hpp"
#include "text.hpp"

namespace ui {

struct release_query_wrapper {
	dcon::nation_id content;
};
struct release_emplace_wrapper {
	dcon::nation_id content;
};

class release_play_as_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			const dcon::national_identity_id niid = any_cast<dcon::national_identity_id>(payload);
			disabled = !command::can_release_and_play_as(state, state.local_player_nation, niid);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			const dcon::national_identity_id niid = any_cast<dcon::national_identity_id>(payload);
			command::release_and_play_as(state, state.local_player_nation, niid);
			parent->set_visible(state, false);
		}
	}
};

class release_agree_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			const dcon::national_identity_id niid = any_cast<dcon::national_identity_id>(payload);
			disabled = !command::can_make_vassal(state, state.local_player_nation, niid);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			const dcon::national_identity_id niid = any_cast<dcon::national_identity_id>(payload);
			command::make_vassal(state, state.local_player_nation, niid);
			parent->set_visible(state, false); // Close parent window automatically
		}
	}
};

class politics_release_nation_window_title : public simple_text_element_base {
public:
	std::string get_text(sys::state& state) noexcept {
		if(auto k = state.key_to_text_sequence.find(std::string_view("politics_release_vassal")); k != state.key_to_text_sequence.end()) {
			return text::produce_simple_string(state, k->second);
		}
		return "";
	}

	void on_update(sys::state& state) noexcept override { set_text(state, get_text(state)); }
};

class release_nation_description_text : public generic_multiline_text<dcon::national_identity_id> {
public:
	void on_create(sys::state& state) noexcept override {
		generic_multiline_text<dcon::national_identity_id>::on_create(state);
		black_text = false;
	}
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
			text::add_to_layout_box(state, contents, box, std::string(" and others."), text::text_color::black);
		} else {
			text::add_to_layout_box(state, contents, box, std::string("."), text::text_color::black);
		}
		text::close_layout_box(contents, box);
	}
};

class release_nation_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = dcon::national_identity_id{};
			parent->impl_get(state, payload);
			const dcon::national_identity_id niid = any_cast<dcon::national_identity_id>(payload);
			Cyto::Any e_payload = release_emplace_wrapper{state.world.national_identity_get_nation_from_identity_holder(niid)};
			parent->impl_get(state, e_payload);
		}
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
	std::string_view get_row_element_name() override { return "vassal_nation"; }

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

// NOTE FOR OTHERS - THIS CODE IS NOT INTERCHANGEABLE WITH ITS SIMIARLY NAMED VERSION ABOVE DO NOT REMOVE THIS
class release_nation_window_description_text : public generic_multiline_text<dcon::national_identity_id> {
protected:
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
			text::add_to_layout_box(state, contents, box, std::string(" and others."));
		} else {
			text::add_to_layout_box(state, contents, box, std::string("."));
		}
		text::close_layout_box(contents, box);
	}

public:
	void on_create(sys::state& state) noexcept override {
		generic_multiline_text<dcon::national_identity_id>::on_create(state);
		black_text = false; // Nudge force to white text
	}
};

class politics_release_nation_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
		base_data.position.x = base_data.position.y = 0;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size; // Nudge
			return ptr;
		} else if(name == "default_popup_banner")
			return make_element_by_type<image_element_base>(state, id); // Not used in Vic2?
		else if(name == "title")
			return make_element_by_type<politics_release_nation_window_title>(state, id);
		else if(name == "description")
			return make_element_by_type<release_nation_window_description_text>(state, id);
		else if(name == "agreebutton")
			return make_element_by_type<release_agree_button>(state, id);
		else if(name == "declinebutton")
			return make_element_by_type<generic_close_button>(state, id);
		else if(name == "playasbutton")
			return make_element_by_type<release_play_as_button>(state, id);
		else
			return nullptr;
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::national_identity_id>()) {
			Cyto::Any n_payload = release_query_wrapper{};
			parent->impl_get(state, n_payload);
			auto content = state.world.nation_get_identity_from_identity_holder(any_cast<dcon::nation_id>(n_payload));
			payload.emplace<dcon::national_identity_id>(content);
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			Cyto::Any n_payload = release_query_wrapper{};
			parent->impl_get(state, n_payload);
			auto content = any_cast<release_query_wrapper>(n_payload).content;
			payload.emplace<dcon::nation_id>(content);
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
