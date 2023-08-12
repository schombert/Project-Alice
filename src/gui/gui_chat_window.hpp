#pragma once

#include "gui_element_types.hpp"

namespace ui {

class chat_message_entry : public listbox_row_element_base<chat_message> {
	flag_button* country_flag = nullptr;
	simple_text_element_base* text_shadow = nullptr;
	simple_text_element_base* text = nullptr;
	std::string text_form_msg;
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			auto ptr = make_element_by_type<flag_button>(state, id);
			country_flag = ptr.get();
			return ptr;
		} else if(name == "text_shadow") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			text_shadow = ptr.get();
			return ptr;
		} else if(name == "text") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			text = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(content.source);
			return message_result::consumed;
		}
		return listbox_row_element_base::get(state, payload);
	}

	void update(sys::state& state) noexcept override {
		country_flag->on_update(state);
		text_form_msg = std::string(content.body);
		text_shadow->set_text(state, text_form_msg);
		text->set_text(state, text_form_msg);
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
		char body[max_chat_message_len];
		size_t len = s.length() >= max_chat_message_len ? max_chat_message_len : s.length();
		memcpy(body, s.data(), len);
		body[len] = '\0';
		command::chat_message(state, state.local_player_nation, body);
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
