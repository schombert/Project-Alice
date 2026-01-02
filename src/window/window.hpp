#pragma once

#include <chrono>

#include "constants.hpp"
#include "system_state_forward.hpp"

namespace ui {
class element_base;
}


struct ITfThreadMgr;

namespace window {
struct text_services_object;

struct win32_text_services {
private:
	ITfThreadMgr* manager_ptr = nullptr;
	unsigned long client_id = 0;
	bool send_notifications = true;
public:
	win32_text_services();
	~win32_text_services();
	void start_text_services();
	void end_text_services();
	text_services_object* create_text_service_object(sys::state&, ui::element_base& ei);
	void on_text_change(text_services_object*, uint32_t old_start, uint32_t old_end, uint32_t new_end);
	void on_selection_change(text_services_object*);
	void set_focus(sys::state& win, text_services_object*);
	void suspend_keystroke_handling();
	void resume_keystroke_handling();
	bool send_mouse_event_to_tso(text_services_object* ts, int32_t x, int32_t y, uint32_t buttons);
	friend struct text_services_object;
};
}

#ifdef _WIN64

#ifndef UNICODE
#define UNICODE
#endif
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

typedef struct HWND__* HWND;
typedef struct HDC__* HDC;

namespace window {
class window_data_impl {
public:
	win32_text_services text_services;
	HWND hwnd = nullptr;
	HDC opengl_window_dc = nullptr;
	HCURSOR cursors[9] = { HCURSOR(NULL) };
	std::chrono::time_point<std::chrono::steady_clock> last_dbl_click;

	int32_t creation_x_size = 600;
	int32_t creation_y_size = 400;

	bool in_fullscreen = false;
	bool left_mouse_down = false;
};
} // namespace window
#else
struct GLFWwindow;

namespace window {
class window_data_impl {
public:
	win32_text_services text_services;
	GLFWwindow* window = nullptr;

	int32_t creation_x_size = 600;
	int32_t creation_y_size = 400;

	bool in_fullscreen = false;
	bool left_mouse_down = false;
};
} // namespace window
#endif

namespace sys {
struct state;
}

namespace window {
enum class window_state : uint8_t { normal, maximized, minimized };
struct creation_parameters {
	int32_t size_x = 1024;
	int32_t size_y = 768;
	window_state initial_state = window_state::maximized;
	bool borderless_fullscreen = false;
};

void create_window(sys::state& game_state,
		creation_parameters const& params);		 // this function will not return until the window is closed or otherwise destroyed
void close_window(sys::state& game_state); // close the main window
void set_borderless_full_screen(sys::state& game_state, bool fullscreen);
bool is_in_fullscreen(sys::state const& game_state);
bool is_key_depressed(sys::state const& game_state, sys::virtual_key key); // why not cheer it up then?

enum class cursor_type : uint8_t {
	normal,
	busy,
	drag_select,
	hostile_move,
	friendly_move,
	no_move,
	text,
	normal_cancel_busy
};
void change_cursor(sys::state& state, cursor_type type);

void get_window_size(sys::state const& game_state, int& width, int& height);
int32_t cursor_blink_ms();
int32_t double_click_ms();
void release_text_services_object(text_services_object* ptr);

void emit_error_message(std::string const& content, bool fatal); // also terminates the program if fatal
} // namespace window
