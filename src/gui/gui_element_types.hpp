#pragma once

#include "gui_graphics.hpp"

namespace ui {

template<typename T>
std::unique_ptr<element_base> make_element_by_type(sys::state& state, std::string_view name) { // also bypasses global creation hooks
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		auto res = std::make_unique<T>();
		std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.defintion]), sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		res->on_update(state);
		return res;
	}
	return std::unique_ptr<element_base>{};
}

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
	void impl_render(sys::state& state, int32_t x, int32_t y) noexcept override;

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
public:
	message_result test_mouse(sys::state& state, int32_t x, int32_t y) noexcept override {
		return message_result::consumed;
	}
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override {
		return message_result::consumed;
	}
	tooltip_behavior has_tooltip(sys::state& state, int32_t x, int32_t y) noexcept override {
		return tooltip_behavior::no_tooltip;
	}
};

class button_element_base : public opaque_element_base {
public:
	button_element_base() {
		interactable = true;
	}

	virtual void button_action(sys::state& state) noexcept { }
	message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept final {
		// TODO: button click
		button_action(state);
		return message_result::consumed;
	}
	message_result on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept final {
		if(base_data.get_element_type() == element_type::button && base_data.data.button.shortcut == key) {
			// TODO: button click
			button_action(state);
			return message_result::consumed;
		} else {
			return message_result::unseen;
		}
	}
};

class window_element_base : public container_base {
public:
	//std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name)
	virtual std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name) noexcept {
		return nullptr;
	}
	void on_create(sys::state& state) noexcept final {
		if(base_data.get_element_type() == element_type::window) {
			auto first_child = base_data.data.window.first_child;
			auto num_children = base_data.data.window.num_children;
			for(uint32_t i = 0; i < num_children; ++i) {
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i + first_child.index()));
				auto ch_res = make_child(state, state.to_string_view(state.ui_defs.gui[child_tag].name));
				if(!ch_res) {
					ch_res = ui::make_element_immediate(state, child_tag);
				}
				if(ch_res) {
					this->add_child_to_back(std::move(ch_res));
				}
			}
		}
	}
};

}
