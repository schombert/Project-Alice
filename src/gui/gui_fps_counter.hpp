#pragma once

#include "gui_element_types.hpp"
namespace ui {

class fps_counter_text_box : public simple_text_element_base {
private:
	std::chrono::time_point<std::chrono::steady_clock> last_render_time{};
	std::chrono::time_point<std::chrono::steady_clock> last_compute_time{};

public:
	void on_create(sys::state& state) noexcept override {
		simple_text_element_base::on_create(state);
		set_text(state, "0");
	}

	void render(sys::state& state, int32_t x, int32_t y) noexcept override {
		std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();
		if(last_render_time == std::chrono::time_point<std::chrono::steady_clock>{}) {
			last_render_time = now;
		}
		auto microseconds_since_last_render = std::chrono::duration_cast<std::chrono::microseconds>(now - last_render_time);
		auto microseconds_since_last_compute = std::chrono::duration_cast<std::chrono::microseconds>(now - last_compute_time);
		if(microseconds_since_last_render.count() > 0.f && microseconds_since_last_compute.count() > 1e5) {
			auto frames_per_second = 1.f / float(microseconds_since_last_render.count() / 1e6);
			set_text(state, std::to_string(int32_t(frames_per_second)));
			last_compute_time = now;
		}
		last_render_time = now;

		simple_text_element_base::render(state, x, y);
	}
};

} // namespace ui
