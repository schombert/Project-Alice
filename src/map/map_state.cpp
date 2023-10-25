#include "map_state.hpp"

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/transform.hpp>

#include "system_state.hpp"
#include "military.hpp"
#include "parsers_declarations.hpp"
#include "gui_graphics.hpp"
#include "gui_element_base.hpp"

namespace map {

dcon::province_id map_state::get_selected_province() {
	return selected_province;
}

// Called to load the map. Will load the texture and shaders from disk
void map_state::load_map(sys::state& state) {
	map_data.load_map(state);
}

void map_state::set_selected_province(dcon::province_id prov_id) {
	unhandled_province_selection = selected_province != prov_id;
	selected_province = prov_id;
}

void map_state::render(sys::state& state, uint32_t screen_x, uint32_t screen_y) {
	update(state);
	glm::vec2 offset = glm::vec2(glm::mod(pos.x, 1.f) - 0.5f, pos.y - 0.5f);
	map_data.render(state, glm::vec2(screen_x, screen_y), offset, zoom,
			state.user_settings.map_is_globe ? map_view::globe : map_view::flat, active_map_mode, globe_rotation, time_counter);
}

glm::vec2 get_port_location(sys::state& state, dcon::province_id p) {
	auto adj = state.world.get_province_adjacency_by_province_pair(p, state.world.province_get_port_to(p));
	assert(adj);
	auto id = adj.index();
	auto& map_data = state.map_state.map_data;
	auto& border = map_data.borders[id];
	auto& vertex = map_data.border_vertices[border.start_index + border.count / 2];
	glm::vec2 map_size = glm::vec2(map_data.size_x, map_data.size_y);

	return vertex.position_ * map_size;
}

bool is_sea_province(sys::state& state, dcon::province_id prov_id) {
	return prov_id.index() >= state.province_definitions.first_sea_province.index();
}

glm::vec2 get_navy_location(sys::state& state, dcon::province_id prov_id) {
	if(is_sea_province(state, prov_id))
		return state.world.province_get_mid_point(prov_id);
	else
		return get_port_location(state, prov_id);
}

glm::vec2 get_army_location(sys::state& state, dcon::province_id prov_id) {
	return state.world.province_get_mid_point(prov_id);
}

void update_unit_arrows(sys::state& state, display_data& map_data) {
	std::vector<std::vector<glm::vec2>> arrows;
	std::vector<float> progresses;
	for(auto& selected_army : state.selected_armies) {
		progresses.push_back(military::fractional_distance_covered(state, selected_army));

		auto current_pos = state.world.army_get_location_from_army_location(selected_army);
		auto path = state.world.army_get_path(selected_army);
		arrows.push_back(std::vector<glm::vec2>());
		arrows.back().push_back(get_army_location(state, current_pos));
		for(auto i = path.size(); i-- > 0;) {
			auto army_pos = get_army_location(state, path[i]);
			arrows.back().push_back(army_pos);
		}
	}
	for(auto& selected_navy : state.selected_navies) {
		progresses.push_back(military::fractional_distance_covered(state, selected_navy));

		auto current_pos = state.world.navy_get_location_from_navy_location(selected_navy);
		auto path = state.world.navy_get_path(selected_navy);
		arrows.push_back(std::vector<glm::vec2>());
		arrows.back().push_back(get_navy_location(state, current_pos));
		for(auto i = path.size(); i-- > 0;) {
			auto navy_pos = get_navy_location(state, path[i]);
			arrows.back().push_back(navy_pos);
		}
	}
	map_data.set_unit_arrows(arrows, progresses);
}

void update_text_lines(sys::state& state, display_data& map_data) {
	// retroscipt
	std::vector<text_line_generator_data> text_data;
	std::vector<bool> visited(65536, false);
	for(auto p : state.world.in_province) {
		auto rid = p.get_connected_region_id();
		if(visited[uint16_t(rid)])
			continue;
		visited[uint16_t(rid)] = true;
		//
		auto n = p.get_nation_from_province_control();
		if(!n || !n.get_name())
			continue;
		std::array<glm::vec2, 5> key_provs{
			p.get_mid_point(), //capital
			p.get_mid_point(), //min x
			p.get_mid_point(), //min y
			p.get_mid_point(), //max x
			p.get_mid_point() //max y
		};
		for(auto p2 : state.world.in_province) {
			if(p2.get_connected_region_id() == rid) {
				if(p2.get_mid_point().x <= key_provs[1].x) {
					key_provs[1] = p2.get_mid_point();
				} if(p2.get_mid_point().y <= key_provs[2].y) {
					key_provs[2] = p2.get_mid_point();
				} if(p2.get_mid_point().x >= key_provs[3].x) {
					key_provs[3] = p2.get_mid_point();
				} if(p2.get_mid_point().y >= key_provs[4].y) {
					key_provs[4] = p2.get_mid_point();
				}
			}
		}
		
		glm::vec2 map_size{ float(state.map_state.map_data.size_x), float(state.map_state.map_data.size_y) };
		glm::vec2 basis{ key_provs[1].x, key_provs[2].y };
		glm::vec2 ratio{ key_provs[3].x - key_provs[1].x, key_provs[4].y - key_provs[2].y };

		// Populate common dataset points
		std::vector<float> my;
		std::vector<float> w;
		for(auto p2 : state.world.in_province) {
			if(p2.get_connected_region_id() == rid) {
				auto e = p2.get_mid_point();
				e -= basis;
				e /= ratio;
				my.push_back(e.y);
				w.push_back(float(map_data.province_area[province::to_map_id(p2)]));
			}
		}

		std::string name = text::produce_simple_string(state, n.get_name());
		if(n.get_capital().get_connected_region_id() != rid) {
			// Adjective + " " + Continent
			name = text::produce_simple_string(state, n.get_adjective()) + " " + text::produce_simple_string(state, p.get_continent().get_name());
		}

		bool use_cuadratic = false;
		// We will try cubic regression first, if that results in very
		// weird lines, for example, lines that go to the infinite
		// we will "fallback" to using a quadratic instead
		{
			// Columns -> n
			// Rows -> fixed size of 4
			// [ x0^0 x0^1 x0^2 x0^3 ]
			// [ x1^0 x1^1 x1^2 x1^3 ]
			// [ ...  ...  ...  ...  ]
			// [ xn^0 xn^1 xn^2 xn^3 ]
			std::vector<std::array<float, 4>> mx;
			for(auto p2 : state.world.in_province) {
				if(p2.get_connected_region_id() == rid) {
					auto e = p2.get_mid_point();
					e -= basis;
					e /= ratio;
					mx.push_back(std::array<float, 4>{ 1.f, e.x, e.x* e.x, e.x* e.x* e.x });
				}
			}
			// [AB]i,j = sum(n, r=1, a_(i,r) * b(r,j))
			// [ x0^0 x0^1 x0^2 x0^3 ] * [ x0^0 x1^0 ... xn^0 ] = [ a0 a1 a2 ... an ]
			// [ x1^0 x1^1 x1^2 x1^3 ] * [ x0^1 x1^1 ... xn^1 ] = [ b0 b1 b2 ... bn ]
			// [ ...  ...  ...  ...  ] * [ x0^2 x1^2 ... xn^2 ] = [ c0 c1 c2 ... cn ]
			// [ xn^0 xn^1 xn^2 xn^3 ] * [ x0^3 x1^3 ... xn^3 ] = [ d0 d1 d2 ... dn ]
			glm::mat4x4 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
						m0[i][j] += mx[r][j] * w[r] * mx[r][i];
			m0 = glm::inverse(m0); // m0 = (T(X)*X)^-1
			glm::vec4 m1(0.f); // m1 = T(X)*Y
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
					m1[i] += mx[r][i] * w[r] * my[r];
			glm::vec4 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];
			// y = a + bx + cx^2 + dx^3
			// y = mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x
			auto poly_fn = [&](float x) {
				return mo[0] + mo[1] * x + mo[2] * x * x + mo[3] * x * x * x;
			};
			auto dx_fn = [&](float x) {
				return 1.f + 2.f * mo[2] * x + 3.f * mo[3] * x * x;
			};
			float xstep = (1.f / float(name.length() * 4.f));
			for(float x = 0.f; x <= 1.f; x += xstep) {
				float y = poly_fn(x);
				if(y < 0.f || y > 1.f) {
					use_cuadratic = true;
					break;
				}
				// Steep change in curve => use cuadratic
				float dx = glm::abs(dx_fn(x) - dx_fn(x - xstep));
				if(dx >= 0.45f) {
					use_cuadratic = true;
					break;
				}
			}
			if(!use_cuadratic)
				text_data.emplace_back(name, mo, basis, ratio);
		}
		bool use_linear = false;
		if(use_cuadratic) {
			// Now lets try quadratic
			std::vector<std::array<float, 3>> mx;
			for(auto p2 : state.world.in_province) {
				if(p2.get_connected_region_id() == rid) {
					auto e = p2.get_mid_point();
					e -= basis;
					e /= ratio;
					mx.push_back(std::array<float, 3>{ 1.f, e.x, e.x* e.x });
				}
			}
			glm::mat3x3 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
						m0[i][j] += mx[r][j] * w[r] * mx[r][i];
			m0 = glm::inverse(m0); // m0 = (T(X)*X)^-1
			glm::vec3 m1(0.f); // m1 = T(X)*Y
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
					m1[i] += mx[r][i] * w[r] * my[r];
			glm::vec3 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];
			// y = a + bx + cx^2
			// y = mo[0] + mo[1] * x + mo[2] * x * x
			auto poly_fn = [&](float x) {
				return mo[0] + mo[1] * x + mo[2] * x * x;
			};
			auto dx_fn = [&](float x) {
				return 1.f + 2.f * mo[2] * x;
			};
			float xstep = (1.f / float(name.length() * 4.f));
			for(float x = 0.f; x <= 1.f; x += xstep) {
				float y = poly_fn(x);
				if(y < 0.f || y > 1.f) {
					use_linear = true;
					break;
				}
				// Steep change in curve => use cuadratic
				float dx = glm::abs(dx_fn(x) - dx_fn(x - xstep));
				if(dx >= 0.45f) {
					use_linear = true;
					break;
				}
			}
			if(!use_linear)
				text_data.emplace_back(name, glm::vec4(mo, 0.f), basis, ratio);
		}
		if(use_linear) {
			// Now lets try linear
			std::vector<std::array<float, 2>> mx;
			for(auto p2 : state.world.in_province) {
				if(p2.get_connected_region_id() == rid) {
					auto e = p2.get_mid_point();
					e -= basis;
					e /= ratio;
					mx.push_back(std::array<float, 2>{ 1.f, e.x });
				}
			}
			glm::mat2x2 m0(0.f);
			for(glm::length_t i = 0; i < m0.length(); i++)
				for(glm::length_t j = 0; j < m0.length(); j++)
					for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
						m0[i][j] += mx[r][j] * w[r] * mx[r][i];
			m0 = glm::inverse(m0); // m0 = (T(X)*X)^-1
			glm::vec2 m1(0.f); // m1 = T(X)*Y
			for(glm::length_t i = 0; i < m1.length(); i++)
				for(glm::length_t r = 0; r < glm::length_t(mx.size()); r++)
					m1[i] += mx[r][i] * w[r] * my[r];
			glm::vec2 mo(0.f); // mo = m1 * m0
			for(glm::length_t i = 0; i < mo.length(); i++)
				for(glm::length_t j = 0; j < mo.length(); j++)
					mo[i] += m0[i][j] * m1[j];
			// y = a + bx
			// y = mo[0] + mo[1] * x
			text_data.emplace_back(name, glm::vec4(mo, 0.f, 0.f), basis, ratio);
		}
	}
	map_data.set_text_lines(state, text_data);
}

void map_state::update(sys::state& state) {
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	// Set the last_update_time if it hasn't been set yet
	if(last_update_time == std::chrono::time_point<std::chrono::system_clock>{})
		last_update_time = now;

	update_unit_arrows(state, map_data);

	auto microseconds_since_last_update = std::chrono::duration_cast<std::chrono::microseconds>(now - last_update_time);
	float seconds_since_last_update = (float)(microseconds_since_last_update.count() / 1e6);
	last_update_time = now;

	time_counter += seconds_since_last_update;
	time_counter = (float)std::fmod(time_counter, 600.f); // Reset it after every 10 minutes

	if((left_arrow_key_down xor right_arrow_key_down) or (up_arrow_key_down xor down_arrow_key_down)) {
		glm::vec2 arrow_key_velocity_vector{};
		if (left_arrow_key_down) {
			arrow_key_velocity_vector.x -= 1.f;
		}
		if (right_arrow_key_down) {
			arrow_key_velocity_vector.x += 1.f;
		}
		if (up_arrow_key_down) {
			arrow_key_velocity_vector.y -= 1.f;
		}
		if (down_arrow_key_down) {
			arrow_key_velocity_vector.y += 1.f;
		}
		arrow_key_velocity_vector = glm::normalize(arrow_key_velocity_vector);
		arrow_key_velocity_vector *= 0.175f;
		pos_velocity += arrow_key_velocity_vector;
	}
	pos_velocity /= 1.125;

	glm::vec2 velocity;

	velocity = pos_velocity * (seconds_since_last_update / zoom);
	velocity.x *= float(map_data.size_y) / float(map_data.size_x);
	pos += velocity;

	pos.x = glm::mod(pos.x, 1.f);
	pos.y = glm::clamp(pos.y, 0.f, 1.f);

	if(pgup_key_down) {
		zoom_change += 0.1f;
	}
	if(pgdn_key_down) {
		zoom_change -= 0.1f;
	}

	glm::vec2 mouse_pos{ state.mouse_x_position, state.mouse_y_position };
	glm::vec2 screen_size{ state.x_size, state.y_size };
	auto view_mode = state.user_settings.map_is_globe ? map_view::globe : map_view::flat;
	glm::vec2 pos_before_zoom;
	bool valid_pos = screen_to_map(mouse_pos, screen_size, view_mode, pos_before_zoom);

	auto zoom_diff = (zoom_change * seconds_since_last_update) / (1 / zoom);
	zoom += zoom_diff;
	zoom_change *= std::exp(-seconds_since_last_update * 20);
	zoom = glm::clamp(zoom, min_zoom, max_zoom);

	glm::vec2 pos_after_zoom;
	if(valid_pos && screen_to_map(mouse_pos, screen_size, view_mode, pos_after_zoom)) {
		pos += pos_before_zoom - pos_after_zoom;
	}


	globe_rotation = glm::rotate(glm::mat4(1.f), (0.25f - pos.x) * 2 * glm::pi<float>(), glm::vec3(0, 0, 1));
	// Rotation axis
	glm::vec3 axis = glm::vec3(globe_rotation * glm::vec4(1, 0, 0, 0));
	axis.z = 0;
	axis = glm::normalize(axis);
	axis.y *= -1;
	globe_rotation = glm::rotate(globe_rotation, (-pos.y + 0.5f) * glm::pi<float>(), axis);


	if(unhandled_province_selection) {
		map_mode::update_map_mode(state);
		map_data.set_selected_province(state, selected_province);
		unhandled_province_selection = false;
	}
}

void map_state::set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode new_map_mode) {
	active_map_mode = new_map_mode;
	map_data.set_province_color(prov_color);
}

void map_state::set_terrain_map_mode() {
	active_map_mode = map_mode::mode::terrain;
}

void map_state::on_key_down(sys::virtual_key keycode, sys::key_modifiers mod) {
	switch (keycode) {
	case sys::virtual_key::LEFT:
		left_arrow_key_down = true;
		break;
	case sys::virtual_key::RIGHT:
		right_arrow_key_down = true;
		break;
	case sys::virtual_key::UP:
		up_arrow_key_down = true;
		break;
	case sys::virtual_key::DOWN:
		down_arrow_key_down = true;
		break;
	case sys::virtual_key::PRIOR:
		pgup_key_down = true;
		break;
	case sys::virtual_key::NEXT:
		pgdn_key_down = true;
		break;
	default:
		break;
	}
}

void map_state::on_key_up(sys::virtual_key keycode, sys::key_modifiers mod) {
	switch(keycode) {
	case sys::virtual_key::LEFT:
		left_arrow_key_down = false;
		break;
	case sys::virtual_key::RIGHT:
		right_arrow_key_down = false;
		break;
	case sys::virtual_key::UP:
		up_arrow_key_down = false;
		break;
	case sys::virtual_key::DOWN:
		down_arrow_key_down = false;
		break;
	case sys::virtual_key::PRIOR:
		pgup_key_down = false;
		break;
	case sys::virtual_key::NEXT:
		pgdn_key_down = false;
		break;
	default:
		break;
	}
}

void map_state::set_pos(glm::vec2 new_pos) {
	pos.x = glm::mod(new_pos.x, 1.f);
	pos.y = glm::clamp(new_pos.y, 0.f, 1.0f);
}

void map_state::center_map_on_province(sys::state& state, dcon::province_id p) {
	if(!p)
		return;

	auto map_pos = state.world.province_get_mid_point(p);
	map_pos.x /= float(map_data.size_x);
	map_pos.y /= float(map_data.size_y);
	map_pos.y = 1.0f - map_pos.y;
	set_pos(map_pos);
}

void map_state::on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod,
		float amount) {
	constexpr auto zoom_speed_factor = 15.f;

	zoom_change = (amount / 5.f) * zoom_speed_factor;
}

void map_state::on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	if(is_dragging) { // Drag the map with middlemouse
		glm::vec2 map_pos;
		screen_to_map(mouse_pos, screen_size, map_view::flat, map_pos);

		set_pos(pos + last_camera_drag_pos - glm::vec2(map_pos));
	}
	glm::vec2 mouse_diff = glm::abs(last_unit_box_drag_pos - mouse_pos);
	if((mouse_diff.x > std::ceil(screen_size_x * 0.0025f) || mouse_diff.y > std::ceil(screen_size_y * 0.0025f))
		&& left_mouse_down)
	{
		auto pos1 = last_unit_box_drag_pos / screen_size;
		auto pos2 = mouse_pos / screen_size;
		auto pixel_size = glm::vec2(1) / screen_size;
		map_data.set_drag_box(true, pos1, pos2, pixel_size);
	} else {
		map_data.set_drag_box(false, {}, {}, {});
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
		float sphere_radius = zoom / glm::pi<float>();

		glm::vec3 intersection_pos;
		glm::vec3 intersection_normal;

		if(glm::intersectRaySphere(cursor_pos, cursor_direction, sphere_center, sphere_radius, intersection_pos,
			intersection_normal)) {
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

void map_state::on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod) {
	is_dragging = false;
}

void map_state::on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
		sys::key_modifiers mod) {
	left_mouse_down = true;
	map_data.set_drag_box(false, {}, {}, {});
	last_unit_box_drag_pos = glm::vec2(x, y);
}

void map_state::on_lbutton_up(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
		sys::key_modifiers mod) {
	left_mouse_down = false;
	map_data.set_drag_box(false, {}, {}, {});
	auto mouse_pos = glm::vec2(x, y);
	glm::vec2 mouse_diff = glm::abs(last_unit_box_drag_pos - mouse_pos);
	if(mouse_diff.x <= std::ceil(screen_size_x * 0.0025f) && mouse_diff.y <= std::ceil(screen_size_y * 0.0025f)) {
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
}


void map_state::on_rbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y,
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

	} else {
		set_selected_province(dcon::province_id{});
	}
}

dcon::province_id map_state::get_province_under_mouse(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y) {
	auto mouse_pos = glm::vec2(x, y);
	auto screen_size = glm::vec2(screen_size_x, screen_size_y);
	glm::vec2 map_pos;
	if(!map_state::screen_to_map(mouse_pos, screen_size, state.user_settings.map_is_globe ? map_view::globe : map_view::flat, map_pos)) {
		return dcon::province_id{};
	}
	map_pos *= glm::vec2(float(map_data.size_x), float(map_data.size_y));
	auto idx = int32_t(map_data.size_y - map_pos.y) * int32_t(map_data.size_x) + int32_t(map_pos.x);
	if(0 <= idx && size_t(idx) < map_data.province_id_map.size()) {
		auto fat_id = dcon::fatten(state.world, province::from_map_id(map_data.province_id_map[idx]));
		//if(map_data.province_id_map[idx] < province::to_map_id(state.province_definitions.first_sea_province)) {
		return province::from_map_id(map_data.province_id_map[idx]);
		/*} else {
			return dcon::province_id{};
		}*/
	} else {
		return dcon::province_id{};
	}
}

bool map_state::map_to_screen(sys::state& state, glm::vec2 map_pos, glm::vec2 screen_size, glm::vec2& screen_pos) {
	if(state.user_settings.map_is_globe) {
		glm::vec3 cartesian_coords;
		float section = 200;
		float pi = glm::pi<float>();
		float angle_x1 = 2 * pi * std::floor(map_pos.x * section) / section;
		float angle_x2 = 2 * pi * std::floor(map_pos.x * section + 1) / section;
		if(!std::isfinite(angle_x1)) {
			assert(false);
			angle_x1 = 0.0f;
		}
		if(!std::isfinite(angle_x2)) {
			assert(false);
			angle_x2 = 0.0f;
		}
		if(!std::isfinite(map_pos.x)) {
			assert(false);
			map_pos.x = 0.0f;
		}
		if(!std::isfinite(map_pos.y)) {
			assert(false);
			map_pos.y = 0.0f;
		}
		cartesian_coords.x = std::lerp(std::cos(angle_x1), std::cos(angle_x2), std::fmod(map_pos.x * section, 1.f));
		cartesian_coords.y = std::lerp(std::sin(angle_x1), std::sin(angle_x2), std::fmod(map_pos.x * section, 1.f));

		float angle_y = (1.f - map_pos.y) * pi;
		cartesian_coords.x *= std::sin(angle_y);
		cartesian_coords.y *= std::sin(angle_y);
		cartesian_coords.z = std::cos(angle_y);
		cartesian_coords = glm::mat3(globe_rotation) * cartesian_coords;
		cartesian_coords /= glm::pi<float>();
		cartesian_coords.x *= -1;
		cartesian_coords.y *= -1;
		if(cartesian_coords.y > 0) {
			return false;
		}
		cartesian_coords += glm::vec3(0.5f);

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
		if(screen_pos.x >= float(std::numeric_limits<int16_t>::max() / 2))
			return false;
		if(screen_pos.x <= float(std::numeric_limits<int16_t>::min() / 2))
			return false;
		if(screen_pos.y >= float(std::numeric_limits<int16_t>::max() / 2))
			return false;
		if(screen_pos.y <= float(std::numeric_limits<int16_t>::min() / 2))
			return false;
		return true;
	}
}

glm::vec2 map_state::normalize_map_coord(glm::vec2 p) {
	auto new_pos = p / glm::vec2{ float(map_data.size_x), float(map_data.size_y) };
	new_pos.y = 1.f - new_pos.y;
	return new_pos;
}

} // namespace map
