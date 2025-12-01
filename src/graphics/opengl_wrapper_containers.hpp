#pragma once

namespace ogl {

class animation;

class render_capture {
private:
	GLuint framebuffer = 0;
	GLuint texture_handle = 0;
	int32_t max_x = 0;
	int32_t max_y = 0;
public:
	void ready(sys::state& state);
	void finish(sys::state& state);
	GLuint get();
	~render_capture();

	friend class animation;
};

class animation {
public:
	enum class type {
		page_flip_left,
		page_flip_right,
		page_flip_up,
		page_flip_left_rev,
		page_flip_right_rev,
		page_flip_up_rev,
		page_flip_mid,
		page_flip_mid_rev
	};
private:
	render_capture start_state;
	render_capture end_state;
	decltype(std::chrono::steady_clock::now()) start_time;
	int32_t ms_run_time = 0;
	int32_t x_pos = 0;
	int32_t y_pos = 0;
	int32_t x_size = 0;
	int32_t y_size = 0;
	type ani_type;
	bool running = false;
public:
	void start_animation(sys::state& state, int32_t x, int32_t y, int32_t w, int32_t h, type t, int32_t runtime);
	void post_update_frame(sys::state& state);
	void render(sys::state& state);
};
}
