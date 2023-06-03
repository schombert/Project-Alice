#pragma once

#include "gui_graphics.hpp"
#include "text.hpp"

namespace ui {

enum class mouse_probe_type {
	click,
	tooltip,
	scroll
};

class element_base {
public:
	static constexpr uint8_t is_invisible_mask = 0x01;

	element_data base_data;
	element_base* parent = nullptr;
	uint8_t flags = 0;

	bool is_visible() const {
		return (flags & is_invisible_mask) == 0;
	}
	void set_visible(sys::state& state, bool vis) {
		auto old_visibility = is_visible();
		flags = uint8_t((flags & ~is_invisible_mask) | (vis ? 0 : is_invisible_mask));
		if(vis && !old_visibility) {
			impl_on_update(state);
			on_visible(state);
		} else if(!vis && old_visibility) {
			on_hide(state);
		}
	}

	element_base() { }

	// impl members: to be overridden only for the very basic container / not a container distinction
	//       - are responsible for propagating messages and responses
	//       - should be called in general when something happens
	virtual mouse_probe impl_probe_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept; // tests which element is under the cursor
	virtual message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept;
	virtual message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept;
	virtual message_result impl_on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual void impl_on_update(sys::state& state) noexcept;
	message_result impl_get(sys::state& state, Cyto::Any& payload) noexcept;
	virtual message_result impl_set(sys::state& state, Cyto::Any& payload) noexcept;
	virtual void impl_render(sys::state& state, int32_t x, int32_t y) noexcept;
	virtual void impl_on_reset_text(sys::state& state) noexcept;
	virtual void impl_on_drag_finish(sys::state& state) noexcept {
		on_drag_finish(state);
	}

	virtual tooltip_behavior has_tooltip(sys::state& state) noexcept { // used to test whether a tooltip is possible
		return tooltip_behavior::no_tooltip;
	}
	virtual void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept { }

	// these message handlers can be overridden by basically anyone
	//        - generally *should not* be called directly
protected:
	virtual message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept; // asks whether the mouse would be intercepted here, but without taking an action
	virtual message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept;
	virtual message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept;
	virtual message_result on_mouse_move(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept;
	virtual message_result get(sys::state& state, Cyto::Any& payload) noexcept;
	virtual message_result set(sys::state& state, Cyto::Any& payload) noexcept;
	virtual void render(sys::state& state, int32_t x, int32_t y) noexcept { }
	virtual void on_update(sys::state& state) noexcept;
	virtual void on_create(sys::state& state) noexcept { }                                                                       // called automatically after the element has been created by the system
	virtual void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept; // as drag events are generated
	virtual void on_text(sys::state& state, char ch) noexcept { }
	virtual void on_visible(sys::state& state) noexcept { }
	virtual void on_hide(sys::state& state) noexcept { }
	virtual void on_reset_text(sys::state& state) noexcept { }

	virtual focus_result on_get_focus(sys::state& state) noexcept { // used to both react to getting the focus and to accept or reject it
		return focus_result::ignored;
	}
	virtual void on_lose_focus(sys::state& state) noexcept { }  // called when the focus is taken away
	virtual void on_drag_finish(sys::state& state) noexcept { } // when the mouse is released, and drag ends
private:
	uint8_t get_pixel_opacity(sys::state& state, int32_t x, int32_t y, dcon::texture_id tid);

public:
	// these commands are meaningful only if the element has children
	virtual std::unique_ptr<element_base> remove_child(element_base* child) noexcept {
		return std::unique_ptr<element_base>{};
	}
	virtual void move_child_to_front(element_base* child) noexcept { }
	virtual void move_child_to_back(element_base* child) noexcept { }
	virtual void add_child_to_front(std::unique_ptr<element_base> child) noexcept {
		std::abort();
	}
	virtual void add_child_to_back(std::unique_ptr<element_base> child) noexcept {
		std::abort();
	}
	virtual element_base* get_child_by_name(sys::state const & state, std::string_view name) noexcept {
		return nullptr;
	}
	virtual element_base* get_child_by_index(sys::state const & state, int32_t index) noexcept {
		return nullptr;
	}

	virtual ~element_base() { }

	friend std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name);
	friend std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id);
	friend void sys::state::on_mouse_drag(int32_t x, int32_t y, sys::key_modifiers mod);
	friend void sys::state::on_text(char c);
	friend void sys::state::on_drag_finished(int32_t x, int32_t y, key_modifiers mod);
	template<typename T>
	friend std::unique_ptr<T> make_element_by_type(sys::state& state, dcon::gui_def_id id);
	template<typename T>
	friend std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name);
};

} // namespace ui
