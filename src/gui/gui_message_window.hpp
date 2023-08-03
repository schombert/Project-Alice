#pragma once

#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "notifications.hpp"

namespace ui {

template<bool Left> class message_lr_button : public button_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		button_element_base::on_create(state);
		frame = Left ? 0 : 1;
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = element_selection_wrapper<bool>{Left};
			parent->impl_get(state, payload);
		}
	}
};

struct message_dismiss_notification {
	int dummy = 0;
};

class message_dismiss_button : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		if(parent) {
			Cyto::Any payload = message_dismiss_notification{};
			parent->impl_get(state, payload);
		}
	}
};

class message_count_text : public simple_text_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		black_text = false;
	}
};

class message_body_text_internal : public simple_multiline_body_text {
	virtual void populate_layout(sys::state& state, text::endless_layout& contents) noexcept override {
		auto msg = retrieve< notification::message*>(state, parent);
		if(msg) {
			msg->body(state, contents);
		}
	}
};

class message_body_text : public window_element_base {
protected:
	multiline_text_scrollbar* text_scrollbar = nullptr;
	message_body_text_internal* delegate = nullptr;

public:
	void on_create(sys::state& state) noexcept override;
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
	void on_update(sys::state& state) noexcept override;
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override { return message_result::consumed; }
};

void message_body_text::on_create(sys::state& state) noexcept {
	base_data.size.x = 500 - (base_data.position.x * 2) - 8;
	base_data.size.y = 18 * 6;

	auto res = std::make_unique<message_body_text_internal>();
	std::memcpy(&(res->base_data), &(base_data), sizeof(ui::element_data));
	make_size_from_graphics(state, res->base_data);
	res->base_data.position.x = 0;
	res->base_data.position.y = 0;
	res->on_create(state);
	delegate = res.get();
	add_child_to_front(std::move(res));

	auto ptr = make_element_by_type<multiline_text_scrollbar>(state, "standardlistbox_slider");
	text_scrollbar = static_cast<multiline_text_scrollbar*>(ptr.get());
	add_child_to_front(std::move(ptr));
	text_scrollbar->scale_to_parent();
}

void message_body_text::on_update(sys::state& state) noexcept {
	if(delegate->internal_layout.number_of_lines > delegate->visible_lines) {
		text_scrollbar->set_visible(state, true);
		text_scrollbar->change_settings(state, mutable_scrollbar_settings{0, delegate->internal_layout.number_of_lines - delegate->visible_lines, 0, 0, false});
	} else {
		text_scrollbar->set_visible(state, false);
		delegate->current_line = 0;
	}
}

message_result message_body_text::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(delegate->internal_layout.number_of_lines > delegate->visible_lines) {
		text_scrollbar->update_scaled_value(state, text_scrollbar->scaled_value() + std::clamp(-amount, -1.f, 1.f));
		delegate->current_line = int32_t(text_scrollbar->scaled_value());
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

message_result message_body_text::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<multiline_text_scroll_event>()) {
		auto event = any_cast<multiline_text_scroll_event>(payload);
		delegate->current_line = event.new_value;
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

class message_flag_button : public flag_button {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.position.y -= 6;
		base_data.size.y += 32;

		base_data.position.x += 8;
		base_data.size.x -= 16;
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		dcon::gfx_object_id gid;
		if(base_data.get_element_type() == element_type::image) {
			gid = base_data.data.image.gfx_object;
		} else if(base_data.get_element_type() == element_type::button) {
			gid = base_data.data.button.button_image;
		}
		if(gid && flag_texture_handle > 0) {
			auto& gfx_def = state.ui_defs.gfx[gid];
			auto mask_handle = ogl::get_texture_handle(state, dcon::texture_id(gfx_def.type_dependent - 1), true);
			auto& mask_tex = state.open_gl.asset_textures[dcon::texture_id(gfx_def.type_dependent - 1)];
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x), float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle, base_data.get_rotation(), gfx_def.is_vertically_flipped());
		}
		image_element_base::render(state, x, y);
	}
};

class message_window : public window_element_base {
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;

	simple_text_element_base* title_text = nullptr;
	message_body_text* desc_text = nullptr;

public:
	std::vector<notification::message> messages;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		xy_pair cur_pos{0, 0};
		{
			auto ptr = make_element_by_type<message_lr_button<false>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x = base_data.size.x - (ptr->base_data.size.x * 2);
			cur_pos.y = ptr->base_data.size.y * 1;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<message_count_text>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			count_text = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<message_lr_button<true>>(state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		set_visible(state, false);
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "header") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "line1") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->base_data.size.x = base_data.size.x - (ptr->base_data.position.x * 2);
			ptr->base_data.size.y = 22;
			title_text = ptr.get();
			return ptr;
		} else if(name == "line3") {
			auto ptr = make_element_by_type<message_body_text>(state, id);
			desc_text = ptr.get();
			return ptr;
		} else if(name.substr(0, 4) == "line") {
			auto ptr = make_element_by_type<simple_text_element_base>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "agreebutton") {
			auto ptr = make_element_by_type<message_dismiss_button>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "declinebutton") {
			auto ptr = make_element_by_type<message_dismiss_button>(state, id);
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "centerok") {
			return make_element_by_type<message_dismiss_button>(state, id);
		} else if(name == "leftshield") {
			return make_element_by_type<message_flag_button>(state, id);
		} else if(name == "rightshield") {
			return make_element_by_type<message_flag_button>(state, id);
		} else if(name == "background") {
			auto ptr = make_element_by_type<draggable_target>(state, id);
			ptr->base_data.size = base_data.size;
			return ptr;
		} else {
			return nullptr;
		}
	}

	void on_update(sys::state& state) noexcept override {
		if(messages.empty()) {
			state.ui_pause.store(false, std::memory_order_release);
			set_visible(state, false);
		} else {
			if(index >= int32_t(messages.size()))
				index = 0;
			else if(index < 0)
				index = int32_t(messages.size()) - 1;

			count_text->set_text(state, std::to_string(int32_t(index) + 1) + "/" + std::to_string(int32_t(messages.size())));

			auto const& m = messages[index];
			title_text->set_text(state, text::produce_simple_string(state, m.title));
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(index >= int32_t(messages.size()))
			index = 0;
		else if(index < 0)
			index = int32_t(messages.size()) - 1;

		if(payload.holds_type<dcon::nation_id>()) {
			if(messages.empty()) {
				payload.emplace<dcon::nation_id>(dcon::nation_id{});
			} else {
				payload.emplace<dcon::nation_id>(messages[index].about);
			}
			return message_result::consumed;
		} else if(payload.holds_type<dcon::national_identity_id>()) {
			if(messages.empty()) {
				payload.emplace<dcon::national_identity_id>(dcon::national_identity_id{});
			} else {
				payload.emplace<dcon::national_identity_id>(state.world.nation_get_identity_from_identity_holder(messages[index].about));
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
			index += b ? -1 : +1;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<message_dismiss_notification>()) {
			messages.clear();
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<notification::message*>()) {
			if(messages.empty()) {
				payload.emplace<notification::message*>(nullptr);
			} else {
				payload.emplace<notification::message*>(&(messages[index]));
			}
		}
		return window_element_base::get(state, payload);
	}
};

} // namespace ui
