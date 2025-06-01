#pragma once

#include "gui_element_types.hpp"

namespace ui {

template<bool IsShadow>
class chat_message_text : public multiline_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<chat_message>(state, parent);
		auto border = base_data.data.text.border_size;
		auto color = IsShadow ? text::text_color::black : (content.target ? text::text_color::orange : text::text_color::white);
		auto container = text::create_endless_layout(state, 
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


		auto p = network::find_country_player(state, content.source);
		auto nickname = state.world.mp_player_get_nickname(p);

		std::string sender_name = sys::player_name{nickname }.to_string() + ": ";
		std::string text_form_msg = std::string(content.body);
		auto box = text::open_layout_box(container);
		text::add_to_layout_box(state, container, box, sender_name, IsShadow ? text::text_color::black : text::text_color::orange);
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
			return make_element_by_type<chat_message_text<true>>(state, id);
		} else if(name == "text") {
			return make_element_by_type<chat_message_text<false>>(state, id);
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

template<bool ShowFull>
class chat_message_listbox : public listbox_element_base<chat_message_entry, chat_message> {
protected:
	uint8_t prev_index = 0;
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
		if constexpr(!ShowFull) {
			if(!row_contents.empty()) {
				auto to_keep = std::min(row_contents.size(), row_windows.size());
				auto to_delete = row_contents.size() - to_keep;
				row_contents.erase(row_contents.begin(), row_contents.begin() + to_delete);
			}
		}
		update(state);
		if constexpr(ShowFull) {
			if(prev_index != state.ui_state.chat_messages_index)
				scroll_to_bottom(state);
			prev_index = state.ui_state.chat_messages_index;
		}
	}

	bool is_reversed() override {
		return false;
	}

	mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		// Showing full chat? Needs a functional scrollbar!
		if constexpr(ShowFull) {
			return listbox_element_base<chat_message_entry, chat_message>::impl_probe_mouse(state, x, y, type);
		} else {
			return mouse_probe{ nullptr, ui::xy_pair{0,0} };
		}
	}
};

class player_kick_button : public button_element_base {
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

class player_ban_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		disabled = !command::can_notify_player_ban(state, state.local_player_nation, n);
		if(state.network_mode != sys::network_mode_type::host)
			disabled = true;
	}

	void button_action(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);
		command::notify_player_ban(state, state.local_player_nation, n);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents, 0);
		text::localised_format_box(state, contents, box, std::string_view("tip_ban"));
		text::close_layout_box(contents, box);
	}
};

class player_name_text : public simple_text_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(state.network_mode == sys::network_mode_type::single_player) {
			set_text(state, text::produce_simple_string(state, "player"));
		} else {
			auto n = retrieve<dcon::nation_id>(state, parent);

			auto p = network::find_country_player(state, n);
			auto nickname = state.world.mp_player_get_nickname(p);
			set_text(state, sys::player_name{nickname }.to_string());
		}
	}
};

class chat_player_ready_state : public color_text_element {
	void on_update(sys::state& state) noexcept override {
		auto n = retrieve<dcon::nation_id>(state, parent);


		auto player = network::find_country_player(state, n);
		if(state.world.mp_player_get_fully_loaded(player)) {
			color = text::text_color::dark_green;
			set_text(state, text::produce_simple_string(state, "ready"));
		} else {
			color = text::text_color::yellow;
			set_text(state, text::produce_simple_string(state, "Loading"));
		}
	}

};

class chat_player_entry : public listbox_row_element_base<dcon::nation_id> {
public:
	void on_create(sys::state& state) noexcept override {
		listbox_row_element_base<dcon::nation_id>::on_create(state);
		base_data.size.y -= 6; //nudge
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "player_shield") {
			return make_element_by_type<flag_button>(state, id);
		} else if(name == "name") {
			return make_element_by_type<player_name_text>(state, id);
		} else if(name == "button_kick") {
			return make_element_by_type<player_kick_button>(state, id);
		} else if(name == "button_ban") {
			return make_element_by_type<player_ban_button>(state, id);
		} else if(name == "ready_state") {
			return make_element_by_type<chat_player_ready_state>(state, id);
		}
		else {
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

	void edit_box_enter(sys::state& state, std::string_view str) noexcept override {
		auto s = parsers::remove_surrounding_whitespace(str);
		if(s.empty())
			return;

		dcon::nation_id target{};
		if(s.length() > 4 && s[0] == '@') {
			state.world.for_each_national_identity([&](dcon::national_identity_id id) {
				auto curr = nations::int_to_tag(state.world.national_identity_get_identifying_int(id));
				if(curr == std::string(s).substr(1, 3))
					target = state.world.national_identity_get_nation_from_identity_holder(id);
			});
		}

		char body[max_chat_message_len + 1];
		size_t len = s.length() >= max_chat_message_len ? max_chat_message_len : s.length();
		memcpy(body, s.data(), len);
		body[len] = '\0';

		command::chat_message(state, state.local_player_nation, body, target);

		Cyto::Any payload = this;
		impl_get(state, payload);
	}

	void edit_box_tab(sys::state& state, std::string_view s) noexcept override;
};

class chat_return_to_lobby_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "alice_lobby_back"));
	}
	void on_update(sys::state& state) noexcept override {
		disabled = (state.network_mode == sys::network_mode_type::client) || (state.current_scene.is_lobby) || (state.network_state.num_client_loading != 0);
	}
	void button_action(sys::state& state) noexcept override {
		map_mode::set_map_mode(state, map_mode::mode::political);
		command::notify_stop_game(state, state.local_player_nation);
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.network_state.num_client_loading != 0) {
			text::add_line(state, contents, "alice_lobby_back_player_loading");
		}
		if(state.current_scene.is_lobby) {
			text::add_line(state, contents, "alice_lobby_back_tt_1");
		}
		if(state.network_mode == sys::network_mode_type::client) {
			text::add_line(state, contents, "alice_lobby_back_tt_2");
		}
	}
};




class chat_resync_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		//set_button_text(state, text::produce_simple_string(state, "alice_lobby_resync"));
	}
	void on_update(sys::state& state) noexcept override {
		disabled = true;
		if(state.network_state.num_client_loading != 0 || state.network_mode != sys::network_mode_type::host) {
			return;
		}
		disabled = !network::any_player_oos(state);
	}
	void button_action(sys::state& state) noexcept override {
		if(state.network_mode == sys::network_mode_type::host) {
			disabled = true;
			network::full_reset_after_oos(state);
		}
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(state.network_state.num_client_loading != 0) {
			text::add_line(state, contents, "alice_lobby_resync_players_loading");
		}
		if(!network::any_player_oos(state)) {
			text::add_line(state, contents, "alice_lobby_resync_no_oos");
		}
		if(state.network_mode != sys::network_mode_type::host) {
			text::add_line(state, contents, "alice_lobby_resync_not_host");
		}
	}
};








class chat_close_button : public generic_close_button {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		set_button_text(state, text::produce_simple_string(state, "close"));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "reopen_with_tab");
	}
};

class chat_window : public window_element_base {
private:
	chat_message_listbox<true>* chat_message_box = nullptr;
public:
	chat_edit_box* chat_edit = nullptr;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "start_button") {
			return make_element_by_type<chat_close_button>(state, id);
		} else if(name == "background") {
			return make_element_by_type<draggable_target>(state, id);
		} else if(name == "chatlog") {
			auto ptr = make_element_by_type<chat_message_listbox<true>>(state, id);
			chat_message_box = ptr.get();
			return ptr;
		} else if(name == "multiplayer_list") {
			return make_element_by_type<chat_player_listbox>(state, id);
		} else if(name == "lobby_chat_edit") {
			auto ptr = make_element_by_type<chat_edit_box>(state, id);
			chat_edit = ptr.get();
			return ptr;
		} else if(name == "back_button") {
			return make_element_by_type<chat_return_to_lobby_button>(state, id);
		} else if(name == "resync_button") {
			return make_element_by_type<chat_resync_button>(state, id);
		} else {
			return nullptr;
		}
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<chat_edit_box*>()) {
			chat_message_box->scroll_to_bottom(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

inline void open_chat_before_game(sys::state& state) {
	state.ui_state.r_chat_window->set_visible(state, !state.ui_state.r_chat_window->is_visible());
	state.ui_state.edit_target = static_cast<ui::chat_window*>(state.ui_state.r_chat_window)->chat_edit;
	state.ui_state.root->move_child_to_front(state.ui_state.r_chat_window);
}

inline void open_chat_during_game(sys::state& state) {
	state.ui_state.chat_window->set_visible(state, !state.ui_state.chat_window->is_visible());
	state.ui_state.edit_target = static_cast<ui::chat_window*>(state.ui_state.chat_window)->chat_edit;
	state.ui_state.root->move_child_to_front(state.ui_state.chat_window);
}

inline void open_chat_window(sys::state& state) {
	state.current_scene.open_chat(state);
}

inline void chat_edit_box::edit_box_tab(sys::state& state, std::string_view s) noexcept {
	ui::open_chat_window(state); //close/open like if tab was pressed!
}

}
