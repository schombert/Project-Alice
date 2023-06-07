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

class message_desc_text : public scrollable_text {
public:
	void on_create(sys::state& state) noexcept override {
		base_data.size.x = 500 - (base_data.position.x * 2) - 8;
		base_data.size.y = 18 * 6;
		scrollable_text::on_create(state);
	}
};

class message_flag_button : public nation_player_flag {
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
			ogl::render_masked_rect(state, get_color_modification(this == state.ui_state.under_mouse, disabled, interactable), float(x),
			                        float(y), float(base_data.size.x), float(base_data.size.y), flag_texture_handle, mask_handle,
			                        base_data.get_rotation(), gfx_def.is_vertically_flipped());
		}
		image_element_base::render(state, x, y);
	}
};

class message_window : public window_element_base {
	simple_text_element_base* count_text = nullptr;
	int32_t index = 0;

	multiline_text_element_base* title_text = nullptr;
	message_desc_text* desc_text = nullptr;

public:
	std::vector<notification::message> messages;

	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		xy_pair cur_pos{0, 0};
		{
			auto ptr = make_element_by_type<message_lr_button<false>>(
			    state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
			cur_pos.x = base_data.size.x - (ptr->base_data.size.x * 2);
			cur_pos.y = ptr->base_data.size.y * 1;
			ptr->base_data.position = cur_pos;
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr =
			    make_element_by_type<message_count_text>(state, state.ui_state.defs_by_name.find("alice_page_count")->second.definition);
			cur_pos.x -= ptr->base_data.size.x;
			ptr->base_data.position = cur_pos;
			count_text = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<message_lr_button<true>>(
			    state, state.ui_state.defs_by_name.find("alice_left_right_button")->second.definition);
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
			auto ptr = make_element_by_type<multiline_text_element_base>(state, id);
			ptr->base_data.size.x = base_data.size.x - (ptr->base_data.position.x * 2);
			ptr->base_data.size.y = 22 * 2;
			title_text = ptr.get();
			return ptr;
		} else if(name == "line3") {
			auto ptr = make_element_by_type<message_desc_text>(state, id);
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
			set_visible(state, false);
		} else {
			if(index >= int32_t(messages.size()))
				index = 0;
			else if(index < 0)
				index = int32_t(messages.size()) - 1;

			count_text->set_text(state, std::to_string(int32_t(index) + 1) + "/" + std::to_string(int32_t(messages.size())));

			auto const& m = messages[index];

			auto title_container = text::create_endless_layout(
			    title_text->internal_layout, text::layout_parameters{0, 0, title_text->base_data.size.x, title_text->base_data.size.y,
			                                                         title_text->base_data.data.text.font_handle, -6,
			                                                         text::alignment::center, text::text_color::black});
			m.title(state, title_container);
			auto desc_container = text::create_endless_layout(
			    desc_text->delegate->internal_layout,
			    text::layout_parameters{0, 0, desc_text->base_data.size.x, desc_text->base_data.size.y,
			                            desc_text->base_data.data.text.font_handle, 0, text::alignment::center, text::text_color::white});
			m.body(state, desc_container);
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
				payload.emplace<dcon::nation_id>(messages[index].primary);
			}
			return message_result::consumed;
		} else if(payload.holds_type<element_selection_wrapper<bool>>()) {
			bool b = any_cast<element_selection_wrapper<bool>>(payload).data;
			index += b ? -1 : +1;
			impl_on_update(state);
			return message_result::consumed;
		} else if(payload.holds_type<message_dismiss_notification>()) {
			if(!messages.empty()) {
				messages.erase(messages.begin() + size_t(index));
				impl_on_update(state);
			}
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

} // namespace ui
