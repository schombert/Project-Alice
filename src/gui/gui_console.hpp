#pragma once

#include "gui_element_types.hpp"
#include "system_state_forward.hpp"

namespace ui {

class console_edit : public edit_box_element_base {
protected:
	// Vector list of last commands
	std::vector<std::string> command_history;
	// Index of the current command in the history
	int history_index = 0;

public:
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void edit_box_update(sys::state& state, std::u16string_view s) noexcept override;
	void on_edit_command(sys::state& state, edit_command command, sys::key_modifiers mods) noexcept override;
	void on_text(sys::state& state, char32_t ch) noexcept override;
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

	void on_update(sys::state& state) noexcept override;
};

class console_window : public window_element_base {
private:
	console_list* console_output_list = nullptr;
	console_edit* edit_box = nullptr;
public:
	void on_create(sys::state& state) noexcept override;
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void clear_list(sys::state& state) noexcept;
	static void show_toggle(sys::state& state);
	void on_visible(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
};

void initialize_console_fif_environment(sys::state& state);
std::string format_fif_value(sys::state& state, int64_t data, int32_t type);

} // namespace ui
