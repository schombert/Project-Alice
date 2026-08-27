#pragma once

#include "system_state.hpp"

namespace ui {

template<class RowWinT, class RowConT>
class standard_listbox_scrollbar : public autoscaling_scrollbar {
public:
	void on_value_change(sys::state& state, int32_t v) noexcept override;
};

template<class RowConT>
class listbox_row_element_base : public window_element_base {
protected:
	RowConT content{};

public:
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

template<class RowConT>
class listbox_row_button_base : public button_element_base {
protected:
	RowConT content{};

public:
	virtual void update(sys::state& state) noexcept {
	}
	message_result get(sys::state& state, Cyto::Any& payload) noexcept override;
};

template<class RowWinT, class RowConT>
class listbox_element_base : public container_base {
protected:
	std::vector<RowWinT*> row_windows{};

	virtual std::string_view get_row_element_name() {
		return std::string_view{};
	}
	virtual bool is_reversed() {
		return false;
	}

public:
	standard_listbox_scrollbar<RowWinT, RowConT>* list_scrollbar = nullptr;
	std::vector<RowConT> row_contents{};

	void update(sys::state& state);
	message_result on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state& state, int32_t x, int32_t y) noexcept override;
	void scroll_to_bottom(sys::state& state);
	message_result test_mouse(sys::state& state, int32_t x, int32_t y, mouse_probe_type type) noexcept override {
		return (type == mouse_probe_type::scroll && row_contents.size() > row_windows.size()) ? message_result::consumed : message_result::unseen;
	}
};

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::update(sys::state& state) {
	auto content_off_screen = int32_t(row_contents.size() - row_windows.size());
	int32_t scroll_pos = list_scrollbar->raw_value();
	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		scroll_pos = 0;
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{ 0, content_off_screen, 0, 0, false });
		list_scrollbar->set_visible(state, true);
		scroll_pos = std::min(scroll_pos, content_off_screen);
	}

	if(is_reversed()) {
		auto i = int32_t(row_contents.size()) - scroll_pos - 1;
		for(int32_t rw_i = int32_t(row_windows.size()) - 1; rw_i >= 0; rw_i--) {
			RowWinT* row_window = row_windows[size_t(rw_i)];
			if(i >= 0) {
				row_window->set_visible(state, true);
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i--];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	} else {
		auto i = size_t(scroll_pos);
		for(RowWinT* row_window : row_windows) {
			if(i < row_contents.size()) {
				auto prior_content = retrieve<RowConT>(state, row_window);
				auto new_content = row_contents[i++];

				if(prior_content != new_content) {
					send(state, row_window, wrapped_listbox_row_content<RowConT>{ new_content });
					if(!row_window->is_visible()) {
						row_window->set_visible(state, true);
					} else {
						row_window->impl_on_update(state);
					}
				} else {
					row_window->set_visible(state, true);
				}
			} else {
				row_window->set_visible(state, false);
			}
		}
	}
}

template<class RowWinT, class RowConT>
message_result listbox_element_base<RowWinT, RowConT>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(row_contents.size() > row_windows.size()) {
		amount = is_reversed() ? -amount : amount;
		list_scrollbar->update_raw_value(state, list_scrollbar->raw_value() + (amount < 0 ? 1 : -1));
		state.ui_state.last_tooltip = nullptr; //force update of tooltip
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::scroll_to_bottom(sys::state& state) {
	uint32_t list_size = 0;
	for(auto rc : row_contents) {
		list_size++;
	}
	list_scrollbar->update_raw_value(state, list_size);
	state.ui_state.last_tooltip = nullptr; //force update of tooltip
	update(state);
}


template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::on_create(sys::state& state) noexcept {
	int16_t current_y = 0;
	int16_t subwindow_y_size = 0;
	while(current_y + subwindow_y_size <= base_data.size.y) {
		auto ptr = make_element_by_type<RowWinT>(state, get_row_element_name());
		row_windows.push_back(static_cast<RowWinT*>(ptr.get()));
		int16_t offset = ptr->base_data.position.y;
		ptr->base_data.position.y += current_y;
		subwindow_y_size = ptr->base_data.size.y;
		current_y += ptr->base_data.size.y + offset;
		add_child_to_front(std::move(ptr));
	}
	auto ptr = make_element_by_type<standard_listbox_scrollbar<RowWinT, RowConT>>(state, "standardlistbox_slider");
	list_scrollbar = static_cast<standard_listbox_scrollbar<RowWinT, RowConT>*>(ptr.get());
	add_child_to_front(std::move(ptr));
	list_scrollbar->scale_to_parent();

	update(state);
}

template<class RowWinT, class RowConT>
void listbox_element_base<RowWinT, RowConT>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid = base_data.data.list_box.background_image;
	if(gid) {
		auto const& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(false, false, true), gfx_def.type_dependent, float(x), float(y),
					float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			} else {
				ogl::render_textured_rect(state, get_color_modification(false, false, true), float(x), float(y), float(base_data.size.x),
					float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			}
		}
	}
	container_base::render(state, x, y);
}


template<class RowConT>
message_result listbox_row_element_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<RowConT>()) {
		payload.emplace<RowConT>(content);
		return message_result::consumed;
	} else if(payload.holds_type<wrapped_listbox_row_content<RowConT>>()) {
		content = any_cast<wrapped_listbox_row_content<RowConT>>(payload).content;
		impl_on_update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<class RowWinT, class RowConT>
void standard_listbox_scrollbar<RowWinT, RowConT>::on_value_change(sys::state& state, int32_t v) noexcept {
	static_cast<listbox_element_base<RowWinT, RowConT>*>(parent)->update(state);
}


template<class RowConT>
message_result listbox_row_button_base<RowConT>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<RowConT>()) {
		payload.emplace<RowConT>(content);
		return message_result::consumed;
	} else if(payload.holds_type<wrapped_listbox_row_content<RowConT>>()) {
		content = any_cast<wrapped_listbox_row_content<RowConT>>(payload).content;
		update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<typename contents_type>
message_result listbox2_base<contents_type>::on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	if(int32_t(row_contents.size()) > visible_row_count) {
		//amount = is_reversed() ? -amount : amount;
		list_scrollbar->update_raw_value(state, list_scrollbar->raw_value() + (amount < 0 ? 1 : -1));
		impl_on_update(state);
		return message_result::consumed;
	}
	return message_result::unseen;
}

template<typename contents_type>
void listbox2_base<contents_type>::on_update(sys::state& state) noexcept {
	auto content_off_screen = int32_t(row_contents.size()) - visible_row_count;
	int32_t scroll_pos = list_scrollbar->raw_value();

	if(content_off_screen <= 0) {
		list_scrollbar->set_visible(state, false);
		list_scrollbar->update_raw_value(state, 0);

		int32_t i = 0;
		for(; i < int32_t(row_contents.size()); ++i) {
			row_windows[i]->set_visible(state, true);
		}
		for(; i < int32_t(row_windows.size()); ++i) {
			row_windows[i]->set_visible(state, false);
		}
	} else {
		list_scrollbar->change_settings(state, mutable_scrollbar_settings{ 0, content_off_screen, 0, 0, false });
		list_scrollbar->set_visible(state, true);
		scroll_pos = std::min(scroll_pos, content_off_screen);

		int32_t i = 0;
		for(; i < visible_row_count; ++i) {
			row_windows[i]->set_visible(state, true);
		}
		for(; i < int32_t(row_windows.size()); ++i) {
			row_windows[i]->set_visible(state, false);
		}
	}
}

template<typename contents_type>
message_result listbox2_base<contents_type>::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<listbox2_scroll_event>()) {
		impl_on_update(state);
		return message_result::consumed;
	} else if(payload.holds_type<listbox2_row_view>()) {
		listbox2_row_view ptr = any_cast<listbox2_row_view>(payload);
		for(int32_t index = 0; index < int32_t(row_windows.size()); ++index) {
			if(row_windows[index] == ptr.row) {
				stored_index = index + list_scrollbar->raw_value();
				return message_result::consumed;
			}
		}
		stored_index = -1;
		return message_result::consumed;
	} else if(payload.holds_type<contents_type>()) {
		if(0 <= stored_index && stored_index < int32_t(row_contents.size())) {
			payload = row_contents[stored_index];
		}
		return message_result::consumed;
	} else {
		return message_result::unseen;
	}
}

template<typename contents_type>
void listbox2_base<contents_type>::resize(sys::state& state, int32_t height) {
	int32_t row_height = row_windows[0]->base_data.position.y + row_windows[0]->base_data.size.y;
	int32_t height_covered = int32_t(row_windows.size()) * row_height;
	int32_t required_rows = (height - height_covered) / row_height;

	while(required_rows > 0) {
		auto new_row = make_row(state);
		row_windows.push_back(new_row.get());
		new_row->base_data.position.y += int16_t(height_covered);
		add_child_to_back(std::move(new_row));

		height_covered += row_height;
		--required_rows;
	}

	visible_row_count = height / row_height;
	base_data.size.y = int16_t(row_height * visible_row_count);

	if(visible_row_count != 0) {
		if(scrollbar_is_internal)
			base_data.size.x -= 16;
		list_scrollbar->scale_to_parent();
		if(scrollbar_is_internal)
			base_data.size.x += 16;
	}
}

template<typename contents_type>
void listbox2_base<contents_type>::on_create(sys::state& state) noexcept {
	auto ptr = make_element_by_type<listbox2_scrollbar>(state, "standardlistbox_slider");
	list_scrollbar = static_cast<listbox2_scrollbar*>(ptr.get());
	add_child_to_back(std::move(ptr));

	auto base_row = make_row(state);
	row_windows.push_back(base_row.get());
	add_child_to_back(std::move(base_row));

	resize(state, base_data.size.y);
}
template<typename contents_type>
void listbox2_base<contents_type>::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid = base_data.data.list_box.background_image;
	if(gid) {
		auto const& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, get_color_modification(false, false, true), gfx_def.type_dependent, float(x), float(y),
					float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			} else {
				ogl::render_textured_rect(state, get_color_modification(false, false, true), float(x), float(y), float(base_data.size.x),
					float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(), gfx_def.is_vertically_flipped(),
					state.world.locale_get_native_rtl(state.font_collection.get_current_locale()));
			}
		}
	}
	container_base::render(state, x, y);
}


}
