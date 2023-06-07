#include "map_state.hpp"

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/polar_coordinates.hpp>

namespace map {

dcon::province_id map_state::get_selected_province() { return selected_province; }

// Called to load the terrain and province map data
void map_state::load_map_data(parsers::scenario_building_context& context) { map_data.load_map_data(context); }

// Called to load the map. Will load the texture and shaders from disk
void map_state::load_map(sys::state& state) { map_data.load_map(state); }

void map_state::set_selected_province(dcon::province_id prov_id) {
	unhandled_province_selection = selected_province != prov_id;
	selected_province = prov_id;
}

void map_state::render(sys::state& state, uint32_t screen_x, uint32_t screen_y) {
	update(state);
	glm::vec2 offset = glm::vec2(glm::mod(pos.x, 1.f) - 0.5f, pos.y - 0.5f);
	map_data.render(glm::vec2(screen_x, screen_y), offset, zoom, state.user_settings.map_is_globe ? map_view::globe : map_view::flat,
		active_map_mode, globe_rotation, time_counter);
}

void map_state::update(sys::state& state) {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	// Set the last_update_time if it hasn't been set yet
	if(last_update_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_update_time = now;
	if(last_zoom_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_zoom_time = now;

	auto microseconds_since_last_update = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time);
	float seconds_since_last_update = (float)(microseconds_since_last_update.count() / 1e6);
	last_update_time = now;

	time_counter += seconds_since_last_update;
	time_counter = (float)std::fmod(time_counter, 600.f); // Reset it after every 10 minutes

	glm::vec2 velocity;

	velocity = (pos_velocity + scroll_pos_velocity) * (seconds_since_last_update / zoom);
	velocity.x *= float(map_data.size_y) / float(map_data.size_x);
	pos += velocity;

	globe_rotation = glm::rotate(glm::mat4(1.f), (0.25f - pos.x) * 2 * glm::pi<float>(), glm::vec3(0, 0, 1));
	// Rotation axis
	glm::vec3 axis = glm::vec3(globe_rotation * glm::vec4(1, 0, 0, 0));
	axis.z = 0;
	axis = glm::normalize(axis);
	axis.y *= -1;
	globe_rotation = glm::rotate(globe_rotation, (-pos.y + 0.5f) * glm::pi<float>(), axis);

	if(has_zoom_changed) {
		last_zoom_time = now;
		has_zoom_changed = false;
	}
	auto microseconds_since_last_zoom = std::chrono::duration_cast<std::chrono::microseconds>(now - last_zoom_time);
	float seconds_since_last_zoom = (float)(microseconds_since_last_zoom.count() / 1e6);

	zoom += (zoom_change * seconds_since_last_update) / (1 / zoom);
	zoom_change *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;
	zoom = glm::clamp(zoom, 1.f, 75.f);
	scroll_pos_velocity *= std::max(0.1f - seconds_since_last_zoom, 0.f) * 9.5f;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.f);

	if(unhandled_province_selection) {
		map_mode::update_map_mode(state);
		map_data.set_selected_province(state, selected_province);
		unhandled_province_selection = false;
	}

	map_data.update_borders(state);
}

void map_state::set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode new_map_mode) {
	active_map_mode = new_map_mode;
	map_data.set_province_color(prov_color);
}

void map_state::set_terrain_map_mode() { active_map_mode = map_mode::mode::terrain; }

void map_state::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		pos_velocity.x = -1.f;
		left_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::RIGHT) {
		pos_velocity.x = +1.f;
		right_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::UP) {
		pos_velocity.y = -1.f;
		up_arrow_key_down = true;
	} else if(keycode == sys::virtual_key::DOWN) {
		pos_velocity.y = +1.f;
		down_arrow_key_down = true;
	}
}

void map_state::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	if(keycode == sys::virtual_key::LEFT) {
		if(pos_velocity.x < 0) {
			if(right_arrow_key_down == false) {
				pos_velocity.x = 0;
			} else {
				pos_velocity.x *= -1;
			}
		}
		left_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::RIGHT) {
		if(pos_velocity.x > 0) {
			if(left_arrow_key_down == false) {
				pos_velocity.x = 0;
			} else {
				pos_velocity.x *= -1;
			}
		}
		right_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::UP) {
		if(pos_velocity.y < 0) {
			if(down_arrow_key_down == false) {
				pos_velocity.y = 0;
			} else {
				pos_velocity.y *= -1;
			}
		}
		up_arrow_key_down = false;
	} else if(keycode == sys::virtual_key::DOWN) {
		if(pos_velocity.y > 0) {
			if(up_arrow_key_down == false) {
				pos_velocity.y = 0;
			} else {
				pos_velocity.y *= -1;
			}
		}
		down_arrow_key_down = false;
	}
}

void map_state::set_pos(glm::vec2 new_pos) {
	pos.x = glm::mod(new_pos.x, 1.f);
	pos.y = glm::clamp(new_pos.y, 0.f, 1.0f);
}

void map_state::on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod, float amount) {
	constexpr auto zoom_speed_factor = 15.f;

	zoom_change = std::copysign(((amount / 5.f) * zoom_speed_factor), amount);
	has_zoom_changed = true;

	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	scroll_pos_velocity = mouse_pos - screen_size * .5f;
	scroll_pos_velocity /= screen_size;
	scroll_pos_velocity *= zoom_speed_factor;
	if(amount > 0) {
		scroll_pos_velocity /= 3.f;
	} else if(amount < 0) {
		scroll_pos_velocity /= 6.f;
	}
}

void map_state::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	if(is_dragging) { // Drag the map with middlemouse
		auto mouse_pos = glm::vec2(x, y);
		auto screen_size = glm::vec2(screen_size_x, screen_size_y);
		glm::vec2 map_pos;
		screen_to_map(mouse_pos, screen_size, map_view::flat, map_pos);

		set_pos(pos + last_camera_drag_pos - glm::vec2(map_pos));
	}
}

bool map_state::screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size, map_view view_mode, glm::vec2& map_pos) {
	if(view_mode == map_view::globe) {
		screen_pos -= screen_size * 0.5f;
		screen_pos /= screen_size;
		screen_pos.x *= screen_size.x / screen_size.y;

		float cursor_radius = glm::length(screen_pos);
		glm::vec3 cursor_pos = glm::vec3(screen_pos.x, -10 * zoom, -screen_pos.y);
		glm::vec3 cursor_direction = glm::vec3(0, 1, 0);
		glm::vec3 sphere_center = glm::vec3(0, 0, 0);
		float sphere_radius = 0.2f * zoom;

		glm::vec3 intersection_pos;
		glm::vec3 intersection_normal;

		if(glm::intersectRaySphere(cursor_pos, cursor_direction, sphere_center, sphere_radius, intersection_pos, intersection_normal)) {
			intersection_pos = glm::mat3(glm::inverse(globe_rotation)) * intersection_pos;
			float theta = std::acos(std::clamp(intersection_pos.z / glm::length(intersection_pos), -1.f, 1.f));
			float phi = std::atan2(intersection_pos.y, intersection_pos.x);
			float pi = glm::pi<float>();
			map_pos = glm::vec2((phi / (2 * pi)) + 0.5f, theta / pi);
			return true;
		}
		return false;
	} else {
		screen_pos -= screen_size * 0.5f;
		screen_pos /= screen_size;
		screen_pos.x *= screen_size.x / screen_size.y;
		screen_pos.x *= float(map_data.size_y) / float(map_data.size_x);

		screen_pos /= zoom;
		screen_pos += pos;
		map_pos = screen_pos;
		return (map_pos.x >= 0 && map_pos.y >= 0 && map_pos.x <= map_data.size_x && map_pos.y <= map_data.size_y);
	}
}

void map_state::on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);

	glm::vec2 map_pos;
	screen_to_map(mouse_pos, screen_size, map_view::flat, map_pos);

	last_camera_drag_pos = map_pos;
	is_dragging = true;
	pos_velocity = glm::vec2(0);
}

void map_state::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod) { is_dragging = false; }

void map_state::on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
	sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	glm::vec2 map_pos;
	if(!screen_to_map(mouse_pos, screen_size, state.user_settings.map_is_globe ? map_view::globe : map_view::flat, map_pos)) {
		return;
	}
	map_pos *= glm::vec2(float(map_data.size_x), float(map_data.size_y));
	auto idx = int32_t(map_data.size_y - map_pos.y) * int32_t(map_data.size_x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
		sound::play_interface_sound(state, sound::get_click_sound(state),
			state.user_settings.interface_volume * state.user_settings.master_volume);
		auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
		if(map_data.province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
			set_selected_province(province::from_map_id(map_data.province_id_map[idx]));
		} else {
			set_selected_province(dcon::province_id{});
		}
	} else {
		set_selected_province(dcon::province_id{});
	}
}

bool map_state::map_to_screen(sys::state& state, glm::vec2 map_pos, glm::vec2 screen_size, glm::vec2& screen_pos) {
	if(state.user_settings.map_is_globe) {
		glm::vec3 cartesian_coords;
		float section = float(map_data.size_x / 256);
		float pi = glm::pi<float>();
		float angle_x1 = 2 * pi * std::floor(map_pos.x * section) / section;
		float angle_x2 = 2 * pi * std::floor(map_pos.x * section + 1) / section;
		cartesian_coords.x = std::lerp(std::cos(angle_x1), std::cos(angle_x2), std::fmod(map_pos.x * section, 1.f));
		cartesian_coords.y = std::lerp(std::sin(angle_x1), std::sin(angle_x2), std::fmod(map_pos.x * section, 1.f));

		float angle_y = (1.f - map_pos.y) * pi;
		cartesian_coords.x *= std::sin(angle_y);
		cartesian_coords.y *= std::sin(angle_y);
		cartesian_coords.z = std::cos(angle_y);
		cartesian_coords = glm::mat3(globe_rotation) * cartesian_coords;
		cartesian_coords *= 0.2;
		cartesian_coords.x *= -1;
		cartesian_coords.y *= -1;
		if(cartesian_coords.y > 0) {
			return false;
		}
		cartesian_coords += glm::vec3(0.5);

		screen_pos = glm::vec2(cartesian_coords.x, cartesian_coords.z);
		screen_pos = (2.f * screen_pos - glm::vec2(1.f));
		screen_pos *= zoom;
		screen_pos.x *= screen_size.y / screen_size.x;
		screen_pos = ((screen_pos + glm::vec2(1.f)) * 0.5f);
		screen_pos *= screen_size;
		return true;
	} else {

		map_pos -= pos;

		if(map_pos.x >= 0.5f)
			map_pos.x -= 1.0f;
		if(map_pos.x < -0.5f)
			map_pos.x += 1.0f;

		map_pos *= zoom;

		map_pos.x *= float(map_data.size_x) / float(map_data.size_y);
		map_pos.x *= screen_size.y / screen_size.x;
		map_pos *= screen_size;
		map_pos += screen_size * 0.5f;
		screen_pos = map_pos;
		return true;
	}
}

glm::vec2 map_state::normalize_map_coord(glm::vec2 p) {
	auto new_pos = p / glm::vec2{float(map_data.size_x), float(map_data.size_y)};
	new_pos.y = 1.f - new_pos.y;
	return new_pos;
}

} // namespace map
