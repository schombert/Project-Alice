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
				false });

		std::string text_form_msg = std::string(content.body);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(state, container, box, text_form_msg, color);
		text::close_layout_box(container, box);
	}
};

class chat_message_entry : public listbox_row_element_base<chat_message> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "shield") {
			return make_element_by_type<flag_button>(state, id);
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
};

class chat_message_listbox : public listbox_element_base<chat_message_entry, chat_message> {
protected:
	std::string_view get_row_element_name() override {
		return "chat_entry";
	}
public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		for(uint8_t i = state.ui_state.chat_messages_index; i < uint8_t(state.ui_state.chat_messages.size()); i++) {
			auto const& c = state.ui_state.chat_messages[i];
			if(bool(c.source))
				row_contents.push_back(c);
		}
		for(uint8_t i = 0; i < state.ui_state.chat_messages_index; i++) {
			auto const& c = state.ui_state.chat_messages[i];
			if(bool(c.source))
				row_contents.push_back(c);
		}
		update(state);
	}

	bool is_reversed() override {
		return true;
	}
};

class chat_player_kick_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_notify_player_kick(state, state.local_player_nation, n);
		if(state.network_mode != sys::network_mode_type::host)
			disabled = true;
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		command::notify_player_kick(state, state.local_player_nation, n);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("tip_kick"));
		text::close_layout_box(contents, box);
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
			return make_element_by_type<chat_player_kick_button>(state, id);
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
	void on_create(sys::state& state) noexcept override {
		edit_box_element_base::on_create(state);
		base_data.size.y += 4;
		base_data.data.text.border_size.y += 8;
	}

	void edit_box_enter(sys::state& state, std::string_view s) noexcept override {
		dcon::nation_id target{};
		if(s.length() > 4 && s[0] == '@') {
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
				if(curr == s.substr(1, 3))
					target = state.world.national_identity_get_nation_from_identity_holder(id);
			});
		}

		char body[max_chat_message_len + 1];
		size_t len = s.length() >= max_chat_message_len ? max_chat_message_len : s.length();
		memcpy(body, s.data(), len);
		body[len] = '\0';

		command::chat_message(state, state.local_player_nation, body, target);
	}
};

class chat_return_to_lobby_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "back"));
	}

	void on_update(sys::state& state) noexcept override {
		disabled = (state.network_mode != sys::network_mode_type::host) || (state.mode == sys::game_mode_type::pick_nation);
	}

	void button_action(sys::state& state) noexcept override {
		command::stop_game(state, state.local_player_nation);
	}
};

class chat_window : public window_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		state.ui_state.chat_window = this; // This is required to dynamically switch between the lobby and in-game chat!
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
			return make_element_by_type<chat_return_to_lobby_button>(state, id);
		} else {
			return nullptr;
		}
	}
};

}
