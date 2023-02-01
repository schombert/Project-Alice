#pragma once

#include "system_state.hpp"
#include <glm/vec2.hpp>

namespace map {

class display_data {
private:
	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::system_clock> last_update_time{};

	glm::vec2 pos;
	glm::vec2 pos_velocity;
	glm::vec2 last_camera_drag_pos;
	bool is_dragging = false;

	void set_pos(glm::vec2 pos);
public:
	// Time in seconds, send to the map shader for animations
	float time_counter = 0;

	GLuint provinces_texture_handle = 0;
	GLuint terrain_texture_handle = 0;
	GLuint rivers_texture_handle = 0;
	GLuint terrainsheet_texture_handle = 0;
	GLuint water_normal = 0;
	GLuint colormap_water = 0;
	GLuint colormap_terrain = 0;
	GLuint overlay = 0;

	glm::vec2 size;
	float offset_x = 0.f;
	float offset_y = 0.f;
	float zoom = 1;

	void update();
	glm::vec2 screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size);
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_key_up(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, sys::key_modifiers mod, float amount);
	void on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);
};
void load_map(sys::state& state);
}
