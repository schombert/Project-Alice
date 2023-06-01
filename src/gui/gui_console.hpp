#pragma once

#include "gui_element_types.hpp"

namespace ui {

class console_edit : public edit_box_element_base {
  protected:
	// Vector list of last commands
	std::vector<std::string> command_history;
	std::string lhs_suggestion;
	std::string rhs_suggestion;
	// Index of the current command in the history
	int history_index = 0;

  public:
	void render(sys::state &state, int32_t x, int32_t y) noexcept override;
	void edit_box_update(sys::state &state, std::string_view s) noexcept override;
	void edit_box_tab(sys::state &state, std::string_view s) noexcept override;
	void edit_box_enter(sys::state &state, std::string_view s) noexcept override;
	void edit_box_esc(sys::state &state) noexcept override {
		state.ui_state.console_window->set_visible(state, false);
	}
	void edit_box_backtick(sys::state &state) noexcept override {
		state.ui_state.console_window->set_visible(state, false);
	}
	void edit_box_up(sys::state &state) noexcept override;
	void edit_box_down(sys::state &state) noexcept override;
	void add_to_history(sys::state &state, std::string s) noexcept {
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
		if (history_index < 0) {
			history_index = 0;
		} else if (history_index >= size) {
			history_index = size;
		}
		if (history_index > 0 && history_index <= size) {
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

class console_text : public simple_text_element_base {
  public:
	void render(sys::state &state, int32_t x, int32_t y) noexcept override;
};

class console_list_entry : public listbox_row_element_base<std::string> {
  private:
	simple_text_element_base *entry_text_box = nullptr;

  public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "console_text") {
			auto ptr = make_element_by_type<console_text>(state, id);
			entry_text_box = ptr.get();
			entry_text_box->set_text(state, "");
			return ptr;
		} else {
			return nullptr;
		}
	}

	void update(sys::state &state) noexcept override {
		entry_text_box->set_text(state, content);
	}
};

class console_list : public listbox_element_base<console_list_entry, std::string> {
  protected:
	std::string_view get_row_element_name() override {
		return "console_entry_wnd";
	}

	bool is_reversed() override {
		return true;
	}
};

class console_window : public window_element_base {
  private:
	console_list *console_output_list = nullptr;
	console_edit *edit_box = nullptr;

  public:
	std::unique_ptr<element_base> make_child(sys::state &state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if (name == "console_list") {
			auto ptr = make_element_by_type<console_list>(state, id);
			console_output_list = ptr.get();
			return ptr;
		} else if (name == "console_edit") {
			auto ptr = make_element_by_type<console_edit>(state, id);
			edit_box = ptr.get();
			return ptr;
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state &state, Cyto::Any &payload) noexcept override {
		if (payload.holds_type<std::string>()) {
			auto entry = any_cast<std::string>(payload);
			console_output_list->row_contents.push_back(entry);
			console_output_list->update(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}

	void clear_list(sys::state &state) noexcept {
		console_output_list->row_contents.clear();
		console_output_list->update(state);
	}

	static void show_toggle(sys::state &state);

	void on_visible(sys::state &state) noexcept override {
		state.ui_state.edit_target = edit_box;
	}
	void on_hide(sys::state &state) noexcept override {
		state.ui_state.edit_target = nullptr;
	}
};
} // namespace ui
