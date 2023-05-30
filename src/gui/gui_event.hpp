#pragma once

#include <variant>
#include "gui_element_types.hpp"

namespace ui {

template<bool Left>
class event_lr_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = Left ? 0 : 1;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{ Left };
			parent->impl_get(state, payload);
		}
	}
};

struct option_taken_notification { int a = 0; };

//
// National events
//
typedef std::variant<
	event::pending_human_n_event,
	event::pending_human_f_n_event
> national_event_data_wrapper;
class national_event_option_button : public button_element_base {
public:
	uint8_t index = 0;
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
			dcon::text_sequence_id name{};
			if(std::holds_alternative<event::pending_human_n_event>(content))
				name = state.world.national_event_get_options(std::get<event::pending_human_n_event>(content).e)[index].name;
			else if(std::holds_alternative<event::pending_human_f_n_event>(content))
				name = state.world.free_national_event_get_options(std::get<event::pending_human_f_n_event>(content).e)[index].name;
			if(bool(name)) {
				set_button_text(state, text::produce_simple_string(state, name));
			} else {
				set_visible(state, false);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
			if(std::holds_alternative<event::pending_human_n_event>(content)) {
				auto phe = std::get<event::pending_human_n_event>(content);
				effect_description(state, contents, state.world.national_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.n), trigger::to_generic(phe.n), phe.from_slot, phe.r_lo, phe.r_hi);
			} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
				auto phe = std::get<event::pending_human_f_n_event>(content);
				effect_description(state, contents, state.world.free_national_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.n), trigger::to_generic(phe.n), -1, phe.r_lo, phe.r_hi);
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
			if(std::holds_alternative<event::pending_human_n_event>(content)) {
				auto phe = std::get<event::pending_human_n_event>(content);
				command::make_event_choice(state, phe, index);
			} else if(std::holds_alternative<event::pending_human_f_n_event>(content)) {
				auto phe = std::get<event::pending_human_f_n_event>(content);
				command::make_event_choice(state, phe, index);
			}
			Cyto::Any n_payload = option_taken_notification{};
			parent->impl_get(state, n_payload);
		}
	}
};
class national_event_image : public image_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);
			if(std::holds_alternative<event::pending_human_n_event>(content))
				base_data.data.image.gfx_object = state.world.national_event_get_image(std::get<event::pending_human_n_event>(content).e);
			else if(std::holds_alternative<event::pending_human_f_n_event>(content))
				base_data.data.image.gfx_object = state.world.free_national_event_get_image(std::get<event::pending_human_f_n_event>(content).e);
		}
	}
};
class national_event_desc_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);

			auto contents = text::create_endless_layout(
				internal_layout,
				text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
			);

			dcon::text_sequence_id description{};
			if(std::holds_alternative<event::pending_human_n_event>(content))
				description = state.world.national_event_get_description(std::get<event::pending_human_n_event>(content).e);
			else if(std::holds_alternative<event::pending_human_f_n_event>(content))
				description = state.world.free_national_event_get_description(std::get<event::pending_human_f_n_event>(content).e);
			
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(contents, state, box, description, text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};
class national_event_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = national_event_data_wrapper{};
			parent->impl_get(state, payload);
			national_event_data_wrapper content = any_cast<national_event_data_wrapper>(payload);

			auto contents = text::create_endless_layout(
				internal_layout,
				text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black }
			);

			dcon::text_sequence_id name{};
			if(std::holds_alternative<event::pending_human_n_event>(content))
				name = state.world.national_event_get_name(std::get<event::pending_human_n_event>(content).e);
			else if(std::holds_alternative<event::pending_human_f_n_event>(content))
				name = state.world.free_national_event_get_name(std::get<event::pending_human_f_n_event>(content).e);
			
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(contents, state, box, name, text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};
template<bool IsMajor>
class national_event_window : public window_element_base {
	element_base* option_buttons[sys::max_event_options];
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;
public:
	std::vector<national_event_data_wrapper> events;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		auto s1 = IsMajor ? "event_major_option_start" : "event_country_option_start";
		auto s2 = IsMajor ? "event_major_option_offset" : "event_country_option_offset";
		auto s3 = IsMajor ? "event_major_optionbutton" : "event_country_optionbutton";

		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(s1)->second.definition].position;
		xy_pair offset = state.ui_defs.gui[state.ui_state.defs_by_name.find(s2)->second.definition].position;
		for(size_t i = 0; i < size_t(sys::max_event_options); ++i) {
			auto ptr = make_element_by_type<national_event_option_button>(state, state.ui_state.defs_by_name.find(s3)->second.definition);
			ptr->base_data.position = cur_offset;
			ptr->index = uint8_t(i);
			option_buttons[i] = ptr.get();
			add_child_to_front(std::move(ptr));
			cur_offset.x += offset.x;
			cur_offset.y += offset.y;
		}
		set_visible(state, false);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto bg_ptr = make_element_by_type<draggable_target>(state, id);
			xy_pair cur_pos{ 0, 0 };
			{
				auto ptr = make_element_by_type<event_lr_button<false>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
				cur_pos.x = bg_ptr->base_data.size.x - (ptr->base_data.size.x * 2);
				cur_pos.y = ptr->base_data.size.y * 1;
				ptr->base_data.position = cur_pos;
				add_child_to_front(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<simple_text_element_base>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
				cur_pos.x -= ptr->base_data.size.x;
				ptr->base_data.position = cur_pos;
				count_text = ptr.get();
				add_child_to_front(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<event_lr_button<true>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
				cur_pos.x -= ptr->base_data.size.x;
				ptr->base_data.position = cur_pos;
				add_child_to_front(std::move(ptr));
			}
			return bg_ptr;
		} else if(name == "title") {
			return make_element_by_type<national_event_name_text>(state, id);
		} else if(name == "description") {
			return make_element_by_type<national_event_desc_text>(state, id);
		} else if(name == "image") {
			return make_element_by_type<national_event_image>(state, id);
		} else if(name == "date") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			// Handling for the case of "major national" flavour
			// TODO: correct rotation for this flag
			if(name == "country_flag1") {
				auto ptr = make_element_by_type<flag_button>(state, id);
				ptr->base_data.flags &= ui::element_data::rotation_mask;
				ptr->base_data.flags |= uint8_t(ui::rotation::r90_left);
				ptr->base_data.position.x += 21;
				ptr->base_data.position.y -= 23;
				ptr->base_data.size.x -= 32;
				ptr->base_data.size.y += 42;
				return ptr;
			} else if(name == "country_flag2") {
				auto ptr = make_element_by_type<flag_button>(state, id);
				ptr->base_data.position.x += 21;
				ptr->base_data.position.y -= 23;
				ptr->base_data.size.x -= 32;
				ptr->base_data.size.y += 42;
				return ptr;
			}
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		for(auto e : option_buttons)
			e->set_visible(state, true);

		auto it = std::remove_if(events.begin(), events.end(), [&](auto& e) {
			sys::date date{};
			if(std::holds_alternative<event::pending_human_n_event>(e))
				date = std::get<event::pending_human_n_event>(e).date;
			else if(std::holds_alternative<event::pending_human_f_n_event>(e))
				date = std::get<event::pending_human_f_n_event>(e).date;
			return date + event::expiration_in_days <= state.current_date;
		});
		auto r = std::distance(it, events.end());
		events.erase(it, events.end());

		if(events.empty()) {
			set_visible(state, false);
		}
		count_text->set_text(state, std::to_string(index + 1) + "/" + std::to_string(events.size()));
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(index >= int32_t(events.size()))
			index = 0;
		else if(index < 0)
			index = int32_t(events.size()) - 1;
		
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else if(payload.holds_type<national_event_data_wrapper>()) {
			if(events.empty()) {
				payload.emplace<national_event_data_wrapper>(national_event_data_wrapper{});
			} else {
				payload.emplace<national_event_data_wrapper>(events[index]);
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
			index += b ? -1 : +1;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<option_taken_notification>()) {
			if(!events.empty())
				events.erase(events.begin() + size_t(index));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

//
// Provincial events
//
typedef std::variant<
	event::pending_human_p_event,
	event::pending_human_f_p_event
> provincial_event_data_wrapper;
class provincial_event_option_button : public button_element_base {
public:
	uint8_t index = 0;
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = provincial_event_data_wrapper{};
			parent->impl_get(state, payload);
			provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
			dcon::text_sequence_id name{};
			if(std::holds_alternative<event::pending_human_p_event>(content))
				name = state.world.provincial_event_get_options(std::get<event::pending_human_p_event>(content).e)[index].name;
			else if(std::holds_alternative<event::pending_human_f_p_event>(content))
				name = state.world.free_provincial_event_get_options(std::get<event::pending_human_f_p_event>(content).e)[index].name;
			if(bool(name)) {
				set_button_text(state, text::produce_simple_string(state, name));
				set_visible(state, true);
			} else {
				set_visible(state, false);
			}
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(parent) {
			Cyto::Any payload = provincial_event_data_wrapper{};
			parent->impl_get(state, payload);
			provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
			if(std::holds_alternative<event::pending_human_p_event>(content)) {
				auto phe = std::get<event::pending_human_p_event>(content);
				effect_description(state, contents, state.world.provincial_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.p), trigger::to_generic(phe.p), phe.from_slot, phe.r_lo, phe.r_hi);
			} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
				auto phe = std::get<event::pending_human_f_p_event>(content);
				effect_description(state, contents, state.world.free_provincial_event_get_options(phe.e)[index].effect, trigger::to_generic(phe.p), trigger::to_generic(phe.p), -1, phe.r_lo, phe.r_hi);
			}
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = provincial_event_data_wrapper{};
			parent->impl_get(state, payload);
			provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);
			if(std::holds_alternative<event::pending_human_p_event>(content)) {
				auto phe = std::get<event::pending_human_p_event>(content);
				command::make_event_choice(state, phe, index);
			} else if(std::holds_alternative<event::pending_human_f_p_event>(content)) {
				auto phe = std::get<event::pending_human_f_p_event>(content);
				command::make_event_choice(state, phe, index);
			}
			Cyto::Any n_payload = option_taken_notification{};
			parent->impl_get(state, n_payload);
		}
	}
};
class provincial_event_desc_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = provincial_event_data_wrapper{};
			parent->impl_get(state, payload);
			provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);

			auto contents = text::create_endless_layout(
				internal_layout,
				text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::left, text::text_color::black }
			);

			dcon::text_sequence_id description{};
			if(std::holds_alternative<event::pending_human_p_event>(content))
				description = state.world.provincial_event_get_description(std::get<event::pending_human_p_event>(content).e);
			else if(std::holds_alternative<event::pending_human_f_p_event>(content))
				description = state.world.free_provincial_event_get_description(std::get<event::pending_human_f_p_event>(content).e);
			
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(contents, state, box, description, text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};
class provincial_event_name_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = provincial_event_data_wrapper{};
			parent->impl_get(state, payload);
			provincial_event_data_wrapper content = any_cast<provincial_event_data_wrapper>(payload);

			auto contents = text::create_endless_layout(
				internal_layout,
				text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::black }
			);

			dcon::text_sequence_id name{};
			if(std::holds_alternative<event::pending_human_p_event>(content))
				name = state.world.provincial_event_get_name(std::get<event::pending_human_p_event>(content).e);
			else if(std::holds_alternative<event::pending_human_f_p_event>(content))
				name = state.world.free_provincial_event_get_name(std::get<event::pending_human_f_p_event>(content).e);
			
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(contents, state, box, name, text::substitution_map{});
			text::close_layout_box(contents, box);
		}
	}
};
class provincial_event_window : public window_element_base {
	element_base* option_buttons[sys::max_event_options];
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;
public:
	std::vector<provincial_event_data_wrapper> events;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		xy_pair cur_offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_start")->second.definition].position;
		xy_pair offset = state.ui_defs.gui[state.ui_state.defs_by_name.find("event_province_option_offset")->second.definition].position;
		for(size_t i = 0; i < size_t(sys::max_event_options); ++i) {
			auto ptr = make_element_by_type<provincial_event_option_button>(state, state.ui_state.defs_by_name.find("event_province_optionbutton")->second.definition);
			ptr->base_data.position = cur_offset;
			ptr->base_data.position.y -= 150; // Omega nudge??
			ptr->index = uint8_t(i);
			option_buttons[i] = ptr.get();
			add_child_to_front(std::move(ptr));
			cur_offset.x += offset.x;
			cur_offset.y += offset.y;
		}
		set_visible(state, false);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "background") {
			auto bg_ptr = make_element_by_type<draggable_target>(state, id);
			xy_pair cur_pos{ 0, 0 };
			{
				auto ptr = make_element_by_type<event_lr_button<false>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
				cur_pos.x = bg_ptr->base_data.size.x - (ptr->base_data.size.x * 2);
				cur_pos.y = ptr->base_data.size.y * 1;
				ptr->base_data.position = cur_pos;
				add_child_to_front(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<simple_text_element_base>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
				cur_pos.x -= ptr->base_data.size.x;
				ptr->base_data.position = cur_pos;
				count_text = ptr.get();
				add_child_to_front(std::move(ptr));
			}
			{
				auto ptr = make_element_by_type<event_lr_button<true>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
				cur_pos.x -= ptr->base_data.size.x;
				ptr->base_data.position = cur_pos;
				add_child_to_front(std::move(ptr));
			}
			return bg_ptr;
		} else if(name == "title") {
			return make_element_by_type<provincial_event_name_text>(state, id);
		} else if(name == "description") {
			return make_element_by_type<provincial_event_desc_text>(state, id);
		} else if(name == "date") {
			return make_element_by_type<simple_text_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		for(auto e : option_buttons)
			e->set_visible(state, true);
		if(events.empty()) {
			set_visible(state, false);
		} else {
			for(auto it = events.begin(); it != events.end(); it++) {
				sys::date date{};
				if(std::holds_alternative<event::pending_human_p_event>(*it))
					date = std::get<event::pending_human_p_event>(*it).date;
				else if(std::holds_alternative<event::pending_human_f_p_event>(*it))
					date = std::get<event::pending_human_f_p_event>(*it).date;
				if(date + event::expiration_in_days <= state.current_date) {
					// Remove expired events
					events.erase(it);
					it--;
				}
			}
		}
		count_text->set_text(state, std::to_string(index + 1) + "/" + std::to_string(events.size()));
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(index >= int32_t(events.size()))
			index = 0;
		else if(index < 0)
			index = int32_t(events.size()) - 1;
		
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else if(payload.holds_type<provincial_event_data_wrapper>()) {
			if(events.empty()) {
				payload.emplace<provincial_event_data_wrapper>(provincial_event_data_wrapper{});
			} else {
				payload.emplace<provincial_event_data_wrapper>(events[index]);
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
			index += b ? -1 : +1;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<option_taken_notification>()) {
			if(!events.empty())
				events.erase(events.begin() + size_t(index));
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

}

