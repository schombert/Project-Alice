#pragma once

#include "gui_graphics.hpp"

namespace ui {

class container_base : public element_base {
public:
	std::vector<std::unique_ptr<element_base>> children;


	element_base* impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept final;
	message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	message_result impl_on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final;
	message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final;
	message_result impl_on_text(sys::state& state, char ch) noexcept final;
	message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept final;
	void impl_on_update(sys::state& state) noexcept final;
	message_result impl_set(Cyto::Any& payload) noexcept final;
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept final;

	std::unique_ptr<element_base> remove_child(element_base* child) noexcept final;
	void move_child_to_front(element_base* child) noexcept final;
	void move_child_to_back(element_base* child) noexcept final;
	void add_child_to_front(std::unique_ptr<element_base> child) noexcept final;
	void add_child_to_back(std::unique_ptr<element_base> child) noexcept final;
	element_base* get_child_by_name(sys::state const& state, std::string_view name) noexcept final;
	element_base* get_child_by_index(sys::state const& state, int32_t index) noexcept final;
};

class image_element_base : public element_base {
public:
	int32_t frame = 0; // if it is from an icon strip
	bool disabled = false;
	bool interactable = false;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
};

class opaque_element_base : public image_element_base {
	virtual message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept {
		return message_result::consumed;
	}
	virtual message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
		return message_result::consumed;
	}
	virtual message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
		return message_result::consumed;
	}
	virtual message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	virtual tooltip_behavior has_tooltip(sys::state& state, int32_t x, int32_t y) noexcept override {
		return tooltip_behavior::no_tooltip;
	}
};

}
