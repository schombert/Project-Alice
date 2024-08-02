#pragma once

#include "gui_element_types.hpp"
#include "fif.hpp"

namespace ui {

class console_edit : public edit_box_element_base {
protected:
	// Vector list of last commands
	std::vector<std::string> command_history;
	// Index of the current command in the history
	int history_index = 0;

public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void edit_box_update(sys::state& state, std::string_view s) noexcept override;
	void edit_box_tab(sys::state& state, std::string_view s) noexcept override;
	void edit_box_enter(sys::state& state, std::string_view s) noexcept override;
	void edit_box_esc(sys::state& state) noexcept override;
	void edit_box_backtick(sys::state& state) noexcept override;
	void edit_box_back_slash(sys::state& state) noexcept override;
	void edit_box_up(sys::state& state) noexcept override;
	void edit_box_down(sys::state& state) noexcept override;
	void add_to_history(sys::state& state, std::string s) noexcept {
		// Add the command to the history, starting with the most recent command
		command_history.insert(command_history.begin(), s);
		// Reset the history index
		history_index = 0;
	}
	std::string navigate_history(int16_t step) noexcept {
		// Start with most recent command.
		// If user presses up/down, go to the older/newer command, etc.
		int16_t size = static_cast<int16_t>(command_history.size());
		history_index += step;
		if(history_index < 0) {
			history_index = 0;
		} else if(history_index >= size) {
			history_index = size;
		}
		if(history_index > 0 && history_index <= size) {
			return command_history[history_index - 1];
		}
		return "";
	}
	std::string down_history() noexcept {
		return navigate_history(-1);
	}
	std::string up_history() noexcept {
		return navigate_history(1);
	}
};

class console_list : public scrollable_text {
public:
	std::string raw_text;
	bool text_pending = false;

	void on_update(sys::state& state) noexcept override {
		std::string new_content;
		{
			std::lock_guard lg{ state.lock_console_strings };
			new_content = state.console_command_result;
			state.console_command_result.clear();
		}
		if(new_content.size() > 0) {
			raw_text += new_content;
			text_pending = true;
		}
		if(text_pending) {
			text_pending = false;
			auto contents = text::create_endless_layout(state, delegate->internal_layout,
				text::layout_parameters{ 10, 10, int16_t(base_data.size.x), int16_t(base_data.size.y),
				base_data.data.text.font_handle, 0, text::alignment::left,
				text::is_black_from_font_id(base_data.data.text.font_handle) ? text::text_color::black : text::text_color::white, false });
			auto box = text::open_layout_box(contents);
			text::add_unparsed_text_to_layout_box(state, contents, box, raw_text);
			text::close_layout_box(contents, box);
			calibrate_scrollbar(state);
		}
	}
};

class console_window : public window_element_base {
private:
	console_list* console_output_list = nullptr;
	console_edit* edit_box = nullptr;
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "console_list") {
			auto ptr = make_element_by_type<console_list>(state, id);
			console_output_list = ptr.get();
			return ptr;
		} else if(name == "console_edit") {
			auto ptr = make_element_by_type<console_edit>(state, id);
			edit_box = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<std::string>()) {
			auto entry = any_cast<std::string>(payload);
			console_output_list->raw_text += entry + "\\n";
			console_output_list->text_pending = true;
			console_output_list->impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<console_edit*>()) {
			//console_output_list->scroll_to_bottom(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	void clear_list(sys::state& state) noexcept {
		console_output_list->raw_text.clear();
		console_output_list->impl_on_update(state);
	}

	static void show_toggle(sys::state& state);

	void on_visible(sys::state& state) noexcept override {
		//console_output_list->scroll_to_bottom(state);
		state.ui_state.edit_target = edit_box;
	}
	void on_hide(sys::state& state) noexcept override {
		state.ui_state.edit_target = nullptr;
	}
};

void initialize_console_fif_environment(sys::state& state);
std::string format_fif_value(sys::state& state, int64_t data, int32_t type);

} // namespace ui
