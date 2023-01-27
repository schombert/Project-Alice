#include "gui_element_types.hpp"

namespace ui {

inline message_result greater_result(message_result a, message_result b) {
	if(a == message_result::consumed || b == message_result::consumed)
		return message_result::consumed;
	if(a == message_result::seen || b == message_result::seen)
		return message_result::seen;
	return message_result::unseen;
}


element_base* container_base::impl_probe_mouse(sys::state& state, int32_t x, int32_t y) noexcept {
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			auto res = c->impl_probe_mouse(state, x - relative_location.x, y - relative_location.y);
			if(res)
				return res;
		}
	}
	return element_base::impl_probe_mouse(state, x, y);
}

message_result container_base::impl_on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			res = greater_result(res, c->impl_on_lbutton_down(state, x - relative_location.x, y - relative_location.y, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
	}
	return greater_result(res, element_base::impl_on_lbutton_down(state, x , y, mods));
}
message_result container_base::impl_on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			res = greater_result(res, c->impl_on_rbutton_down(state, x - relative_location.x, y - relative_location.y, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
	}
	return greater_result(res, element_base::impl_on_rbutton_down(state, x, y, mods));
}
message_result container_base::impl_on_key_down(sys::state& state, sys::virtual_key key, sys::key_modifiers mods) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		if(c->is_visible()) {
			res = greater_result(res, c->impl_on_key_down(state, key, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
	}
	return greater_result(res, element_base::impl_on_key_down(state, key, mods));
}
message_result container_base::impl_on_scroll(sys::state& state, int32_t x, int32_t y, float amount, sys::key_modifiers mods) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		if(c->is_visible()) {
			auto relative_location = child_relative_location(*this, *c);
			res = greater_result(res, c->impl_on_scroll(state, x - relative_location.x, y - relative_location.y, amount, mods));
			if(res == message_result::consumed)
				return message_result::consumed;
		}
	}
	return greater_result(res, element_base::impl_on_scroll(state, x, y, amount, mods));
}
void container_base::impl_on_update(sys::state& state) noexcept {
	for(auto& c : children) {
		if(c->is_visible()) {
			c->impl_on_update(state);
		}
	}
	on_update(state);
}
message_result container_base::impl_set(sys::state& state, Cyto::Any& payload) noexcept {
	message_result res = message_result::unseen;
	for(auto& c : children) {
		res = greater_result(res, c->impl_set(state, payload));
	}
	return greater_result(res, set(state, payload));
}

void container_base::impl_render(sys::state& state, int32_t x, int32_t y) noexcept {
	element_base::impl_render(state, x, y);

	for(size_t i = children.size(); i-- > 0; ) {
		if(children[i]->is_visible()) {
			auto relative_location = child_relative_location(*this, *(children[i]));
			children[i]->impl_render(state, x + relative_location.x, y + relative_location.y);
		}
	}
}


std::unique_ptr<element_base> container_base::remove_child(element_base* child) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
		if(it + 1 != children.end())
			std::rotate(it, it + 1, children.end());
		auto temp = std::move(children.back());
		children.pop_back();
		temp->parent = nullptr;
		return temp;
	}
	return std::unique_ptr<element_base>{};
}
void container_base::move_child_to_front(element_base* child) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
		if(it != children.begin())
			std::rotate(children.begin(), it, it + 1);
	}
}
void container_base::move_child_to_back(element_base* child) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[child](std::unique_ptr<element_base>& p) { return p.get() == child; }); it != children.end()) {
		if(it + 1 != children.end())
			std::rotate(it, it + 1, children.end());
	}
}
void container_base::add_child_to_front(std::unique_ptr<element_base> child) noexcept {
	child->parent = this;
	children.emplace_back(std::move(child));
	if(children.size() > 1) {
		std::rotate(children.begin(), children.end() - 1, children.end());
	}
}
void container_base::add_child_to_back(std::unique_ptr<element_base> child) noexcept {
	child->parent = this;
	children.emplace_back(std::move(child));
}
element_base* container_base::get_child_by_name(sys::state const& state, std::string_view name) noexcept {
	if(auto it = std::find_if(children.begin(), children.end(),
		[&state, name](std::unique_ptr<element_base>& p) { return state.to_string_view(p->base_data.name) == name; }); it != children.end()) {
		return it->get();
	}
	return nullptr;
}
element_base* container_base::get_child_by_index(sys::state const& state, int32_t index) noexcept {
	if(0 <= index && index < int32_t(children.size()))
		return children[index].get();
	return nullptr;
}

ogl::color_modification get_color_modification(bool is_under_mouse, bool is_disabled, bool is_interactable) {
	if(!is_under_mouse || !is_interactable) {
		if(is_disabled) {
			return ogl::color_modification::disabled;
		} else {
			return ogl::color_modification::none;
		}
	} else {
		if(is_disabled) {
			return ogl::color_modification::interactable_disabled;
		} else {
			return ogl::color_modification::interactable;
		}
	}
}

void image_element_base::render(sys::state& state, int32_t x, int32_t y) noexcept {
	dcon::gfx_object_id gid;
	if(base_data.get_element_type() == element_type::image) {
		gid = base_data.data.image.gfx_object;
	} else if(base_data.get_element_type() == element_type::button) {
		gid = base_data.data.button.button_image;
	}
	if(gid) {
		auto& gfx_def = state.ui_defs.gfx[gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					gfx_def.type_dependant,
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					frame,
					gfx_def.number_of_frames,
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			} else {
				ogl::render_textured_rect(
					state,
					get_color_modification(this == state.ui_state.under_mouse, disabled, interactable),
					float(x), float(y), float(base_data.size.x), float(base_data.size.y),
					ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()),
					base_data.get_rotation(),
					gfx_def.is_vertically_flipped()
				);
			}
		}
	}
}

void make_size_from_graphics(sys::state& state, ui::element_data& dat) {
	if(dat.size.x == 0 || dat.size.y == 0) {
		dcon::gfx_object_id gfx_handle;
		float scale = 1.0f;
		if(dat.get_element_type() == ui::element_type::image) {
			gfx_handle = dat.data.image.gfx_object;
			scale = dat.data.image.scale;
		} else if(dat.get_element_type() == ui::element_type::button) {
			gfx_handle = dat.data.button.button_image;
		}
		if(gfx_handle) {
			if(state.ui_defs.gfx[gfx_handle].size.x != 0) {
				dat.size = state.ui_defs.gfx[gfx_handle].size;
			} else {
				auto tex_handle = state.ui_defs.gfx[gfx_handle].primary_texture_handle;
				if(tex_handle) {
					ogl::get_texture_handle(state, tex_handle, state.ui_defs.gfx[gfx_handle].is_partially_transparent());
					dat.size.y = int16_t(state.open_gl.asset_textures[tex_handle].size_y);
					dat.size.x = int16_t(state.open_gl.asset_textures[tex_handle].size_x / state.ui_defs.gfx[gfx_handle].number_of_frames);
				}
			}
			if(scale != 1.0f) {
				dat.size.x = int16_t(dat.size.x * dat.data.image.scale);
				dat.size.y = int16_t(dat.size.y * dat.data.image.scale);
			}
		}
	}
}



std::unique_ptr<element_base> make_element(sys::state& state, std::string_view name) {
	auto it = state.ui_state.defs_by_name.find(name);
	if(it != state.ui_state.defs_by_name.end()) {
		if(it->second.generator) {
			auto res = it->second.generator(state, it->second.defintion);
			if(res) {
				std::memcpy(&(res->base_data), &(state.ui_defs.gui[it->second.defintion]), sizeof(ui::element_data));
				make_size_from_graphics(state, res->base_data);
				res->on_create(state);
				res->on_update(state);
				return res;
			}
		}
		return make_element_immediate(state, it->second.defintion);
	}
	return std::unique_ptr<element_base>{};
}

state::state() {
	root = std::make_unique<container_base>();
}


void window_element_base::on_create(sys::state& state) noexcept {
	if(base_data.get_element_type() == element_type::window) {
		auto first_child = base_data.data.window.first_child;
		auto num_children = base_data.data.window.num_children;
		for(uint32_t i = num_children; i-- > 0; ) {
			auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(i + first_child.index()));
			auto ch_res = make_child(state, state.to_string_view(state.ui_defs.gui[child_tag].name), child_tag);
			if(!ch_res) {
				ch_res = ui::make_element_immediate(state, child_tag);
			}
			if(ch_res) {
				this->add_child_to_back(std::move(ch_res));
			}
		}
	}
}

void window_element_base::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	auto location_abs = get_absolute_location(*this);
	if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
		xy_pair new_abs_pos = location_abs;
		new_abs_pos.x += int16_t(x - oldx);
		new_abs_pos.y += int16_t(y - oldy);

		if(ui_width(state) > base_data.size.x)
			new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui_width(state) - base_data.size.x));
		if(ui_height(state) > base_data.size.y)
			new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui_height(state) - base_data.size.y));

		base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
	}
}

message_result draggable_target::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	for(auto tmp = parent; tmp != nullptr; tmp = tmp->parent) {
		if(tmp->base_data.get_element_type() == element_type::window && tmp->base_data.data.window.is_moveable()) {
			state.ui_state.drag_target = tmp;
			return message_result::consumed;
		}
	}
	return message_result::consumed;
}

std::unique_ptr<element_base> make_element_immediate(sys::state& state, dcon::gui_def_id id) {
	auto& def = state.ui_defs.gui[id];
	if(def.get_element_type() == ui::element_type::image) {
		auto res = std::make_unique<image_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::button) {
		auto res = std::make_unique<button_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		make_size_from_graphics(state, res->base_data);
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::window) {
		auto res = std::make_unique<window_element_base>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		res->on_create(state);
		return res;
	} else if(def.get_element_type() == ui::element_type::scrollbar) {
		auto res = std::make_unique<scrollbar>();
		std::memcpy(&(res->base_data), &def, sizeof(ui::element_data));
		res->on_create(state);
		return res;
	}
	// TODO: other defaults

	return nullptr;
}


void scrollbar_left::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = value_change{ -step_size, true, true };
		parent->impl_get(state, payload);
	}
}
void scrollbar_right::button_action(sys::state& state) noexcept {
	if(parent) {
		Cyto::Any payload = value_change{ step_size, true, true };
		parent->impl_get(state, payload);
	}
}

message_result scrollbar_track::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(parent) {
		Cyto::Any payload = scrollbar_settings{};
		if(parent->impl_get(state, payload) == message_result::consumed) {
			scrollbar_settings parent_state = any_cast<scrollbar_settings>(payload);
			int32_t pos_in_track = parent_state.vertical ? y : x;
			int32_t clamped_pos = std::clamp(pos_in_track, parent_state.buttons_size / 2, parent_state.track_size - parent_state.buttons_size / 2);
			float fp_pos = float(clamped_pos - parent_state.buttons_size / 2) / float(parent_state.track_size - parent_state.buttons_size);

			Cyto::Any adjustment_payload = value_change{
				int32_t(parent_state.lower_value + fp_pos * (parent_state.upper_value - parent_state.lower_value)),
				true, false };
			parent->impl_get(state, adjustment_payload);
		}
	}
	return message_result::consumed;
}

message_result scrollbar_slider::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return message_result::consumed;
}
void scrollbar_slider::on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	if(!parent)
		return;

	auto location_abs = get_absolute_location(*this);

	scrollbar_settings parent_settings = [&]() {
		Cyto::Any payload = scrollbar_settings{};
		if(parent->impl_get(state, payload) == message_result::consumed) {
			return any_cast<scrollbar_settings>(payload);
		}
		return scrollbar_settings{};
	}();

	if(parent_settings.vertical) {
		if(!(location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y)) {
			return; // drag has left slider behind
		}
	} else {
		if(!(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x)) {
			return; // drag has left slider behind
		}
	}

	int32_t pos_in_track = 0;

	// TODO: take care of case where there are partial range limits

	if(parent_settings.vertical) {
		base_data.position.y += int16_t(y - oldy);
		base_data.position.y = int16_t(std::clamp(int32_t(base_data.position.y), parent_settings.buttons_size, parent_settings.track_size));
		pos_in_track = base_data.position.y - parent_settings.buttons_size / 2;
	} else {
		base_data.position.x += int16_t(x - oldx);
		base_data.position.x = int16_t(std::clamp(int32_t(base_data.position.x), parent_settings.buttons_size, parent_settings.track_size));
		pos_in_track = base_data.position.x - parent_settings.buttons_size / 2;
	}
	float fp_pos = float(pos_in_track - parent_settings.buttons_size / 2) / float(parent_settings.track_size - parent_settings.buttons_size);

	Cyto::Any adjustment_payload = value_change{
		int32_t(parent_settings.lower_value + fp_pos * (parent_settings.upper_value - parent_settings.lower_value)),
		false, false };
	parent->impl_get(state, adjustment_payload);
}


void scrollbar::update_raw_value(int32_t v) {
	// TODO: adjust to limits if using limits
	stored_value = std::clamp(v, settings.lower_value, settings.upper_value);
	float percentage = float(stored_value - settings.lower_value) / float(settings.upper_value - settings.lower_value);
	auto offset = settings.buttons_size + int32_t((settings.track_size - settings.buttons_size) * percentage);
	if(slider) {
		if(settings.vertical)
			slider->base_data.position.y = int16_t(offset);
		else
			slider->base_data.position.x = int16_t(offset);
	}
}
void scrollbar::update_scaled_value(float v) {
	int32_t rv = std::clamp(int32_t(v * settings.scaling_factor), settings.lower_value, settings.upper_value);
	update_raw_value(rv);
}
float scrollbar::scaled_value() const {
	return float(stored_value) / float(settings.scaling_factor);
}

void scrollbar::change_settings(sys::state& state, mutable_scrollbar_settings const& settings_s) {
	settings.lower_limit = settings_s.lower_limit * settings.scaling_factor;
	settings.lower_value = settings_s.lower_value * settings.scaling_factor;
	settings.upper_limit = settings_s.upper_limit * settings.scaling_factor;
	settings.upper_value = settings_s.upper_value * settings.scaling_factor;
	settings.using_limits = settings_s.using_limits;

	settings.upper_value = std::min(settings.upper_value, settings.lower_value + 1); // ensure the scrollbar is never of range zero

	// TODO: adjust to limits if using limits
	if(stored_value < settings.lower_value || stored_value > settings.upper_value) {
		stored_value = std::clamp(stored_value, settings.lower_value, settings.upper_value);
		on_value_change(state, stored_value);
	}
}

void scrollbar::on_create(sys::state& state) noexcept {
	// create & position sub elements
	// fill out settings data
	// set initial slider pos

	if(base_data.get_element_type() == element_type::scrollbar) {
		auto step = base_data.data.scrollbar.get_step_size();
		settings.scaling_factor = 1;
		switch(step) {
			case step_size::one:
				break;
			case step_size::two:
				break;
			case step_size::one_tenth:
				settings.scaling_factor = 10;
				break;
			case step_size::one_hundredth:
				settings.scaling_factor = 100;
				break;
			case step_size::one_thousandth:
				settings.scaling_factor = 1000;
				break;
		}
		settings.lower_value = 0;
		settings.upper_value = base_data.data.scrollbar.max_value * settings.scaling_factor;
		settings.lower_limit = 0;
		settings.upper_limit = settings.upper_value;

		settings.vertical = !base_data.data.scrollbar.is_horizontal();
		stored_value = settings.lower_value;

		auto first_child = base_data.data.scrollbar.first_child;
		auto num_children = base_data.data.scrollbar.num_children;
		if(num_children >= 4) {
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(2 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_slider>(state, child_tag);
				slider = ch_res.get();
				add_child_to_back(std::move(ch_res));
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(0 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_left>(state, child_tag);
				left = ch_res.get();
				add_child_to_back(std::move(ch_res));

				settings.buttons_size = settings.vertical ? left->base_data.size.y : left->base_data.size.x;
				if(step_size::two == step)
					left->step_size = 2;
				else
					left->step_size = 1;
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(1 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_right>(state, child_tag);
				right = ch_res.get();
				add_child_to_back(std::move(ch_res));

				if(step_size::two == step)
					right->step_size = 2;
				else
					right->step_size = 1;
			}
			{
				auto child_tag = dcon::gui_def_id(dcon::gui_def_id::value_base_t(3 + first_child.index()));
				auto ch_res = make_element_by_type<scrollbar_track>(state, child_tag);
				track = ch_res.get();
				add_child_to_back(std::move(ch_res));

				settings.track_size = settings.vertical ? track->base_data.size.y : track->base_data.size.x;
			}
			left->base_data.position.x = 0;
			left->base_data.position.y = 0;
			if(settings.vertical) {
				track->base_data.position.y = int16_t(settings.buttons_size);
				slider->base_data.position.y = int16_t(settings.buttons_size);
				right->base_data.position.y = int16_t(settings.track_size + settings.buttons_size);
				track->base_data.position.x = 0;
				slider->base_data.position.x = 0;
				right->base_data.position.x = 0;
			} else {
				track->base_data.position.x = int16_t(settings.buttons_size);
				slider->base_data.position.x = int16_t(settings.buttons_size);
				right->base_data.position.x = int16_t(settings.track_size + settings.buttons_size);
				track->base_data.position.y = 0;
				slider->base_data.position.y = 0;
				right->base_data.position.y = 0;
			}
		}
	}
}
message_result scrollbar::get(sys::state& state, Cyto::Any& payload) noexcept {
	if(payload.holds_type<scrollbar_settings>()) {
		payload = settings;
		return message_result::consumed;
	} else if(payload.holds_type<value_change>()) {
		auto adjustments = any_cast<value_change>(payload);

		if(adjustments.is_relative)
			stored_value += adjustments.new_value;
		else
			stored_value = adjustments.new_value;

		if(adjustments.move_slider) {
			update_raw_value(stored_value);
		} else {
			// TODO: adjust to limits if using limits
			stored_value = std::clamp(stored_value, settings.lower_value, settings.upper_value);
		}

		on_value_change(state, stored_value);
		return message_result::unseen;
	} else {
		return message_result::unseen;
	}
}

}
