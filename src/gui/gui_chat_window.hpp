#pragma once

#include "gui_element_types.hpp"

namespace ui {

class chat_message_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<chat_message>(state, parent);

		auto border = base_data.data.text.border_size;
		auto color = black_text ? text::text_color::black : text::text_color::white;
		if(!black_text && content.target) {
			color = text::text_color::orange;
		}
		auto container = text::create_endless_layout(
			internal_layout,
			text::layout_parameters{
				border.x,
				border.y,
				int16_t(base_data.size.x - border.x * 2),
				int16_t(base_data.size.y - border.y * 2),
				base_data.data.text.font_handle,
				0,
				text::alignment::left,
				color,
				false});
		
		std::string text_form_msg = std::string(content.body);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(state, container, box, text_form_msg, color);
		text::close_layout_box(container, box);
	}
};

class chat_message_entry : public listbox_row_element_base<chat_message> {
	flag_button* country_flag = nullptr;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag = ptr.get();
			return ptr;
		} else if(name == "text_shadow") {
			return make_element_by_type<chat_message_text>(state, id);
		} else if(name == "text") {
			return make_element_by_type<chat_message_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content.source);
			return message_result::consumed;
		} else if(payload.holds_type<chat_message>()) {
			payload.emplace<chat_message>(content);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}

	void update(sys::state& state) noexcept override {
		country_flag->set_visible(state, bool(content.source));
		if(bool(content.source))
			country_flag->on_update(state);
	}
};

class chat_message_listbox : public listbox_element_base<chat_message_entry, chat_message> {
protected:
	std::string_view get_row_element_name() override {
		return "chat_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(auto const& c : state.ui_state.chat_messages)
			row_contents.push_back(c);
		update(state);
	}
};

class chat_player_entry : public listbox_row_element_base<dcon::nation_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "name") {
			return make_element_by_type<generic_name_text<dcon::nation_id>>(state, id);
		} else if(name == "button_kick") {
			return make_element_by_type<button_element_base>(state, id);
		} else {
			return nullptr;
		}
	}
};

class chat_player_listbox : public listbox_element_base<chat_player_entry, dcon::nation_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ingame_multiplayer_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_nation([&](dcon::nation_id n) {
			if(state.world.nation_get_is_player_controlled(n))
				row_contents.push_back(n);
		});
		update(state);
	}
};

class chat_edit_box : public edit_box_element_base {
public:
	void edit_box_enter(sys::state& state, std::string_view s) noexcept override {
		dcon::nation_id target{};
		if(s.length() > 4 && s[0] == '@') {
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
				if(curr == s.substr(1, 3))
					target = state.world.national_identity_get_nation_from_identity_holder(id);
			});
		}
		
		char body[max_chat_message_len];
		size_t len = s.length() >= max_chat_message_len ? max_chat_message_len : s.length();
		memcpy(body, s.data(), len);
		body[len] = '\0';

		command::chat_message(state, state.local_player_nation, body, target);
	}
};

class chat_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "start_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "chatlog") {
			return make_element_by_type<chat_message_listbox>(state, id);
		} else if(name == "multiplayer_list") {
			return make_element_by_type<chat_player_listbox>(state, id);
		} else if(name == "lobby_chat_edit") {
			return make_element_by_type<chat_edit_box>(state, id);
		} else if(name == "back_button") {
			auto ptr = make_element_by_type<button_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else {
			return nullptr;
		}
	}
};

}
