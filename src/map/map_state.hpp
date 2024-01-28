#pragma once

#include "map_modes.hpp"
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "map.hpp"

namespace sys {
struct state;
};
namespace parsers {
struct scenario_building_context;
};

namespace map {

enum class map_view { globe, globe_perspect, flat };
class map_state {
public:
	map_state(){};

	// Called to load the terrain and province map data
	void load_map_data(parsers::scenario_building_context& context);
	// Called to load the map. Will load the texture and shaders from disk
	void load_map(sys::state& state);

	map_view current_view(sys::state& state);

	void render(sys::state& state, uint32_t screen_x, uint32_t screen_y);
	void set_province_color(std::vector<uint32_t> const& prov_color, map_mode::mode map_mode);
	void set_terrain_map_mode();
	void update_borders(sys::state& state);

	glm::vec2 normalize_map_coord(glm::vec2 pos);
	bool map_to_screen(sys::state& state, glm::vec2 map_pos, glm::vec2 screen_size, glm::vec2& screen_pos);

	// Set the position of camera. Position relative from 0-1
	void set_pos(glm::vec2 pos);

	void center_map_on_province(sys::state& state, dcon::province_id);

	// Input methods
	void on_key_down(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_key_up(sys::virtual_key keycode, sys::key_modifiers mod);
	void on_mouse_wheel(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod, float amount);
	void on_mouse_move(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_down(int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_mbuttom_up(int32_t x, int32_t y, sys::key_modifiers mod);
	void on_lbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_lbutton_up(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	void on_rbutton_down(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y, sys::key_modifiers mod);
	dcon::province_id get_province_under_mouse(sys::state& state, int32_t x, int32_t y, int32_t screen_size_x, int32_t screen_size_y);

	dcon::province_id get_selected_province();
	void set_selected_province(dcon::province_id prov_id);

	map_mode::mode active_map_mode = map_mode::mode::terrain;
	dcon::province_id selected_province = dcon::province_id{};

	display_data map_data;
	bool is_dragging = false;

	// Last update time, used for smooth map movement
	std::chrono::time_point<std::chrono::steady_clock> last_update_time{};

	// Time in seconds, send to the map shader for animations
	float time_counter = 0;

	// interaction
	bool unhandled_province_selection = false;

	// Position and movement
	glm::vec2 pos = glm::vec2(0.5f, 0.5f);
	glm::vec2 pos_velocity = glm::vec2(0.f);
	glm::vec2 last_camera_drag_pos = glm::vec2(0.5f, 0.5f);
	glm::mat4 globe_rotation = glm::mat4(1.0f);
	glm::vec2 last_unit_box_drag_pos = glm::vec2(0, 0);

	float zoom = 1.f;
	float zoom_change = 1.f;
	bool has_zoom_changed = false;
	bool pgup_key_down = false;
	bool pgdn_key_down = false;
	bool left_arrow_key_down = false;
	bool right_arrow_key_down = false;
	bool up_arrow_key_down = false;
	bool down_arrow_key_down = false;
	bool left_mouse_down = false;
	glm::vec2 scroll_pos_velocity = glm::vec2(0.f);
	std::vector<bool> visible_provinces;

	void update(sys::state& state);

	bool screen_to_map(glm::vec2 screen_pos, glm::vec2 screen_size, map_view view_mode, glm::vec2& map_pos);

	float get_zoom() {
		return zoom;
	}
};

void update_text_lines(sys::state& state, display_data& map_data);

} // namespace map
