#include "gui_element_types.hpp"
#include "gui_context_window.hpp"

namespace ui {

class context_menu_entry_logic {
public:
	virtual dcon::text_key get_name(sys::state& state, context_menu_context context) {
		return state.lookup_key("null");
	}

	virtual bool is_available(sys::state& state, context_menu_context context) {
		return false;
	}

	virtual void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) {

	}

	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) {

	}
	virtual ~context_menu_entry_logic() { };
};

class context_menu_build_factory : public context_menu_entry_logic {
public:
	dcon::text_key get_name(sys::state& state, context_menu_context context) noexcept override {
		return state.lookup_key("build_factory");
	}

	bool is_available(sys::state& state, context_menu_context context) noexcept override  {
		return true;
	}

	void button_action(sys::state& state, context_menu_context context, ui::element_base* parent) noexcept override {

	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents, context_menu_context context) noexcept override  {

	}
};

inline static context_menu_build_factory context_menu_build_factory_logic;

class context_window_entry : public button_element_base {
public:
	int ind = 0;
	context_menu_entry_logic* entry_logic;
	context_menu_context context;

	void on_update(sys::state& state) noexcept override {
		if (entry_logic)
			this->set_button_text(state, text::produce_simple_string(state, entry_logic->get_name(state, context)));
	}
	void button_action(sys::state& state) noexcept override {
		if (entry_logic)
			entry_logic->button_action(state, context, parent);
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		if(entry_logic)
			entry_logic->update_tooltip(state, x, y, contents, context);
	}
};

class context_menu_window : public window_element_base {
public:
	std::vector<context_window_entry*> entries = std::vector<context_window_entry*>(16);
	std::vector<context_menu_entry_logic*> logics = std::vector<context_menu_entry_logic*>(16);
	context_menu_context context;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);

		for(size_t i = 0; i < entries.size(); i++) {
			auto ptr = make_element_by_type<context_window_entry>(state, "context_menu_item");
			ptr->base_data.position.y += int16_t((ptr->base_data.size.y) * i);
			ptr->set_visible(state, false);

			entries[i] = ptr.get();
			entries[i]->ind = (int)i;

			add_child_to_front(std::move(ptr));
		}
	}

	void on_update(sys::state& state) noexcept override {
		window_element_base::on_update(state);

		for(size_t i = 0; i < entries.size(); i++) {
			if(logics[i]) {
				entries[i]->set_visible(state, true);
				entries[i]->entry_logic = logics[i];
				entries[i]->context = context;
			} else {
				entries[i]->set_visible(state, false);
			}
		}
	}
};

void hide_context_menu(sys::state& state, context_menu_context context) {
	
	if(!state.ui_state.context_menu) {
		return;
	}
		
	state.ui_state.context_menu->set_visible(state, false);
}

void show_context_menu(sys::state& state, context_menu_context context) {
	std::vector<context_menu_entry_logic*> logics = std::vector<context_menu_entry_logic*>(16);

	if(context.province) {;
		logics[0] = &context_menu_build_factory_logic;
	}

	if(!state.ui_state.context_menu) {
		auto new_mm = make_element_by_type<context_menu_window>(state, "context_menu_window");
		state.ui_state.context_menu = new_mm.get();
		state.ui_state.context_menu->logics = logics;
		state.ui_state.context_menu->context = context;
		state.ui_state.context_menu->base_data.position.x = (int16_t) (state.mouse_x_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->base_data.position.y = (int16_t) (state.mouse_y_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->set_visible(state, true);
		state.ui_state.context_menu->impl_on_update(state);
		state.ui_state.root->add_child_to_front(std::move(new_mm));
	} else {
		state.ui_state.context_menu->logics = logics;
		state.ui_state.context_menu->context = context;
		state.ui_state.context_menu->base_data.position.x = (int16_t)(state.mouse_x_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->base_data.position.y = (int16_t)(state.mouse_y_position / state.user_settings.ui_scale);
		state.ui_state.context_menu->set_visible(state, true);
		state.ui_state.context_menu->impl_on_update(state);
		state.ui_state.root->move_child_to_front(state.ui_state.context_menu);
	}
}

}
