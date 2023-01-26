#pragma once

#include "gui_graphics.hpp"

namespace ui {

inline message_result greater_result(message_result a, message_result b) {
	if(a == message_result::consumed || b == message_result::consumed)
		return message_result::consumed;
	if(a == message_result::seen || b == message_result::seen)
		return message_result::seen;
	return message_result::unseen
}

class container_base : public element_base {
public:
	std::vector<std::unique_ptr<element_base>> children;


	virtual message_result impl_test_mouse(sys::state& state, int32_t x, int32_t y) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_test_mouse(state, x, y));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, test_mouse(state, x, y));
	}
	virtual message_result impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_lbutton_down(state, x, y, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_lbutton_down(state, x, y, mods));
	}
	virtual message_result impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_rbutton_down(state, x, y, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_rbutton_down(state, x, y, mods));
	}
	virtual message_result impl_on_drag(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_drag(state, x, y, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_drag(state, x, y, mods));
	}
	virtual message_result impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_key_down(state, key, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_key_down(state, key, mods));
	}
	virtual message_result impl_on_text(sys::state& state, char ch) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_text(state, ch));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_text(state, ch));
	}
	virtual message_result impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_on_scroll(state, x, y, amount, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
		return greater_result(res, on_scroll(state, x, y, amount, mods));
	}
	virtual void impl_on_update(sys::state& state) final noexcept {
		for(auto& c : children) {
			c->impl_on_update(state);
		}
		on_update(state);
	}
	virtual message_result impl_set(Cyto::Any& payload) final noexcept {
		message_result res = message_result::unseen;
		for(auto& c : children) {
			res = greater_result(res, c->impl_set(payload));
		}
		reutrn greater_result(res, set(payload));
	}

	virtual message_result on_resize(sys::state& state, int32_t x, int32_t y) final noexcept { } // used to get a parent to reposition its children

	virtual std::unique_ptr<element_base> remove_child(element_base* child) final noexcept {
		return std::unique_ptr<element_base>{};
	}
	virtual void move_child_to_front(element_base* child) final noexcept { }
	virtual void move_child_to_back(element_base* child) final noexcept { }
	virtual void add_child_to_front(std::unique_ptr<element_base> child) final noexcept { }
	virtual void add_child_to_back(std::unique_ptr<element_base> child) final noexcept { }
};

}
